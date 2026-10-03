import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync,rmSync} from 'node:fs';
import {resolve,relative,dirname,isAbsolute} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

// Resource-free stationary-UV oracle. Compare the unchanged original scroll
// expression against the guarded identity, including modes and sticky flags.
// Also compile the real interpreter and compare full VM bytes with original
// equations under aliased environment-rate inputs. Frozen-tape rollback is separate.
// Usage: WASI_SDK_PATH=/path/to/wasi-sdk node portable/check-anm-uv-scroll.mjs
// TH10_TEST_UBSAN=1 instruments C++ only. Third-party SoftFloat is excluded;
// its independently documented zero-significand shift defect is not a pass.
const root=resolve(dirname(fileURLToPath(import.meta.url)),'..');
const sdk=process.env.WASI_SDK_PATH;
if(!sdk)throw Error('Set WASI_SDK_PATH to an installed WASI SDK directory.');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw Error(`WASI compiler not found: ${compiler}`);
const sanitized=process.env.TH10_TEST_UBSAN==='1';
const out=resolve(root,'artifacts/anm-uv-scroll'+(sanitized?'-ubsan':''));mkdirSync(out,{recursive:true});
const metadataPath=resolve(out,'build.json');rmSync(metadataPath,{force:true});
const flags=['--target=wasm32-wasip1','-O2','-std=c++17','-Wall','-Wextra','-Werror',
    '-DTH_NATIVE_PLATFORM=1','-fno-exceptions','-fno-rtti','-ffp-contract=off','-fno-strict-aliasing'];
if(sanitized)flags.push('-fsanitize=undefined','-fsanitize-trap=undefined');
const softfloatFlags=['--target=wasm32-wasip1','-O2','-x','c','-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5'];
const linkFlags=['--target=wasm32-wasip1','-Wl,-z,stack-size=1048576'];
const interpreterPath=resolve(root,'th10_web/cpp/game/AnmInterpreter.cpp');
const interpreter=readFileSync(interpreterPath,'utf8');
const animate=interpreter.slice(interpreter.indexOf('void animate('),interpreter.indexOf('// 0x43ee30.'))
    .replace(/#else\s*const auto rate=number\(\*env\.rate\);\s*#endif/g,'#endif');
const capture=animate.match(/const float (\w+)=\*env\.rate;\s*const auto rate=number\(\1\);/);
if(!capture||(animate.match(/\*env\.rate/g)??[]).length!==1||!animate.includes(`anm_stationary_uv_mode(${capture[1]})`))
    throw Error('ANM UV guard must use the original single entry-point rate sample, never a late environment reread.');
const sanitizer={enabled:sanitized,softfloatIncluded:false,scope:sanitized?'C++ test/production code only; third-party SoftFloat excluded':'none'};
console.log(JSON.stringify({suite:'anm-uv-scroll',sanitizer,rateSamplingSourceCheck:true}));
const dependencies=new Set([resolve(root,'portable/check-anm-uv-scroll.mjs'),interpreterPath]);
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
const common=[compile('th10_web/cpp/rebuild/third_party/softfloat.c','softfloat',softfloatFlags),
    ...['Arithmetic'].map(name=>compile(`th10_web/cpp/game/${name}.cpp`,name))];
for(const multiplayer of [false,true]){
    const variant=multiplayer?'multiplayer':'ordinary';
    const variantFlags=[...flags,...(multiplayer?['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']:[])];
    const test=compile('tests/anm-uv-scroll-test.cpp',variant,variantFlags);
    const wasm=resolve(out,variant+'.wasm');
    execFileSync(compiler,[...linkFlags,test,...common,'-o',wasm],{cwd:root,stdio:'inherit'});
    const wasi=new WASI({version:'preview1',args:[variant],env:{},returnOnExit:true});
    const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
    const code=wasi.start(instance);if(code!==0)throw Error(`${variant} failed: ${code}`);
    const integration=compile('tests/anm-uv-integration-test.cpp',variant+'-integration',variantFlags);
    const engine=['AnmVm','AnmInterpreter','AnmVariables','AnmFile','Timer','Interpolation','Rng','GameMath']
        .map(name=>compile(`th10_web/cpp/game/${name}.cpp`,variant+'-'+name,variantFlags));
    const integrationWasm=resolve(out,variant+'-integration.wasm');
    execFileSync(compiler,[...linkFlags,integration,...engine,...common,'-o',integrationWasm],{cwd:root,stdio:'inherit'});
    const integrationWasi=new WASI({version:'preview1',args:[variant+'-integration'],env:{},returnOnExit:true});
    const integrationInstance=(await WebAssembly.instantiate(readFileSync(integrationWasm),{wasi_snapshot_preview1:integrationWasi.wasiImport})).instance;
    const integrationCode=integrationWasi.start(integrationInstance);if(integrationCode!==0)throw Error(`${variant} integration failed: ${integrationCode}`);
}
writeFileSync(metadataPath,JSON.stringify({passed:true,rateSamplingSourceCheck:true,sanitizer,runtime:'wasm32-wasip1',node:process.version,
    compiler:execFileSync(compiler,['--version'],{encoding:'utf8'}).trim(),compileFlags:flags,softfloatFlags,linkFlags,sanitized,
    variants:['ordinary','multiplayer'],dependencyScope:'Clang -MMD non-system source/header closure plus this driver; SDK system headers/libraries excluded',
    sources:Object.fromEntries([...dependencies].sort().map(path=>[relative(root,path).replaceAll('\\','/'),createHash('sha256').update(readFileSync(path)).digest('hex')]))},null,2)+'\n');
