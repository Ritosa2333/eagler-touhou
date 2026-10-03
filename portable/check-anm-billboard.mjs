import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync,rmSync} from 'node:fs';
import {resolve,relative,dirname,isAbsolute} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

// Resource-free billboard geometry oracle. Compare every vertex byte, project
// call/input, return value, sticky arithmetic flags and errno with the original
// expression. No captured assets, duplicate renderer or timing threshold.
// Usage: WASI_SDK_PATH=/path/to/wasi-sdk node portable/check-anm-billboard.mjs
// TH10_TEST_UBSAN=1 instruments C++ only; third-party SoftFloat is excluded.
// See docs/anm-billboard-check.md for the independently retained baseline fault.
// TH10_TEST_SOFTFLOAT_UBSAN=1 additionally instruments that backend and records
// failed results. It requires TH10_TEST_UBSAN=1; it is not a default pass lane.
const root=resolve(dirname(fileURLToPath(import.meta.url)),'..');
const sdk=process.env.WASI_SDK_PATH;
if(!sdk)throw Error('Set WASI_SDK_PATH to an installed WASI SDK directory.');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw Error(`WASI compiler not found: ${compiler}`);
const sanitized=process.env.TH10_TEST_UBSAN==='1';
const backendSanitized=process.env.TH10_TEST_SOFTFLOAT_UBSAN==='1';
if(backendSanitized&&!sanitized)throw Error('TH10_TEST_SOFTFLOAT_UBSAN requires TH10_TEST_UBSAN=1.');
const out=resolve(root,'artifacts/anm-billboard'+(backendSanitized?'-backend-ubsan':sanitized?'-ubsan':''));mkdirSync(out,{recursive:true});
const metadataPath=resolve(out,'build.json');rmSync(metadataPath,{force:true});
const flags=['--target=wasm32-wasip1','-O2','-std=c++17','-Wall','-Wextra','-Werror',
    '-DTH_NATIVE_PLATFORM=1','-fno-exceptions','-fno-rtti','-ffp-contract=off','-fno-strict-aliasing'];
if(sanitized)flags.push('-fsanitize=undefined','-fsanitize-trap=undefined');
const softfloatFlags=['--target=wasm32-wasip1','-O2','-x','c','-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5'];
const linkFlags=['--target=wasm32-wasip1','-Wl,-z,stack-size=1048576'];
const dependencies=new Set([resolve(root,'portable/check-anm-billboard.mjs')]);
function compile(source,name,compileFlags=flags){
    const object=resolve(out,name+'.o'),depfile=object+'.d';
    execFileSync(compiler,[...compileFlags,'-MMD','-MF',depfile,'-MT','source','-c',resolve(root,source),'-o',object],{cwd:root,stdio:'inherit'});
    const make=readFileSync(depfile,'utf8').replace(/\\\r?\n/g,'').replace(/^source:\s*/,'');
    for(const token of make.match(/(?:\\.|[^\s])+/g)??[]){
        const path=resolve(root,token.replace(/\\(.)/g,'$1')),name=relative(root,path);
        if(name.startsWith('..')||isAbsolute(name))throw Error(`Project dependency outside checkout: ${path}`);
        dependencies.add(path);
    }
    return object;
}
if(backendSanitized)softfloatFlags.push('-fsanitize=undefined','-fsanitize-trap=undefined');
const common=[compile('th10_web/cpp/rebuild/third_party/softfloat.c','softfloat',softfloatFlags),
    ...['Arithmetic','GameMath'].map(name=>compile(`th10_web/cpp/game/${name}.cpp`,name))];
const backendHash=createHash('sha256').update(readFileSync(resolve(root,'th10_web/cpp/rebuild/third_party/softfloat.c'))).digest('hex');
const baselineBackendFinding=backendHash==='2d7fed55bea49ff38635f93770e99345ef7f86721b9fa5b9eeec2d829fe5d12b'
    ?{passed:false,kind:'known-independent-baseline-observation',sourceSha256:backendHash,
        location:'softfloat.c:8692',reason:'zero significand shifts by 64 in softfloat_normSubnormalExtF80Sig',
        documentation:'docs/anm-billboard-check.md'}:null;
const sanitizer={enabled:sanitized,softfloatIncluded:backendSanitized,
    scope:backendSanitized?'C++ test/production arithmetic and third-party SoftFloat':sanitized?'C++ test/production arithmetic only; third-party SoftFloat excluded':'none',
    baselineBackendFinding};
console.log(JSON.stringify({suite:'anm-billboard',sanitizer}));
const results=[];
for(const multiplayer of [false,true]){
    const variant=multiplayer?'multiplayer':'ordinary';
    const variantFlags=[...flags,...(multiplayer?['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']:[])];
    const test=compile('tests/anm-billboard-test.cpp',variant,variantFlags);
    const projection=compile('th10_web/cpp/game/AnmProjection.cpp',variant+'-projection',variantFlags);
    const wasm=resolve(out,variant+'.wasm');
    execFileSync(compiler,[...linkFlags,test,projection,...common,'-o',wasm],{cwd:root,stdio:'inherit'});
    const wasi=new WASI({version:'preview1',args:[variant],env:{},returnOnExit:true});
    const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
    try{
        const code=wasi.start(instance);if(code!==0)throw Error(`${variant} failed: ${code}`);
        results.push({variant,passed:true});
    }catch(error){results.push({variant,passed:false,error:error.stack??String(error)});console.error(`${variant}: ${error.stack??error}`);}
}
writeFileSync(metadataPath,JSON.stringify({passed:results.every(result=>result.passed),results,sanitizer,runtime:'wasm32-wasip1',node:process.version,
    compiler:execFileSync(compiler,['--version'],{encoding:'utf8'}).trim(),compileFlags:flags,softfloatFlags,linkFlags,sanitized,
    variants:['ordinary','multiplayer'],dependencyScope:'Clang -MMD non-system source/header closure plus this driver; SDK system headers/libraries excluded',
    sources:Object.fromEntries([...dependencies].sort().map(path=>[relative(root,path).replaceAll('\\','/'),createHash('sha256').update(readFileSync(path)).digest('hex')]))},null,2)+'\n');
if(results.some(result=>!result.passed))process.exitCode=1;
