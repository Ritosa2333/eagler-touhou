import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync,rmSync} from 'node:fs';
import {resolve,relative,dirname,isAbsolute} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

// Resource-free scalar oracle. The existing ANM/native-owner diagnostics own
// full-vertex and rollback checks; this does not freeze a duplicate renderer.
// Usage: WASI_SDK_PATH=/path/to/wasi-sdk node portable/check-anm-trig-input.mjs
// Optional TH10_TEST_UBSAN=1 enables trapping undefined-behavior checks.
const root=resolve(dirname(fileURLToPath(import.meta.url)),'..');
const sdk=process.env.WASI_SDK_PATH;
if(!sdk)throw Error('Set WASI_SDK_PATH to an installed WASI SDK directory.');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw Error(`WASI compiler not found: ${compiler}`);
const sanitized=process.env.TH10_TEST_UBSAN==='1';
const out=resolve(root,'artifacts/anm-trig-input'+(sanitized?'-ubsan':''));mkdirSync(out,{recursive:true});
const metadataPath=resolve(out,'build.json');rmSync(metadataPath,{force:true});
const flags=['--target=wasm32-wasip1','-O2','-std=c++17','-Wall','-Wextra','-Werror',
    '-DTH_NATIVE_PLATFORM=1','-fno-exceptions','-fno-rtti','-ffp-contract=off','-fno-strict-aliasing'];
if(sanitized)flags.push('-fsanitize=undefined','-fsanitize-trap=undefined');
const softfloatFlags=['--target=wasm32-wasip1','-O2','-x','c','-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5'];
const linkFlags=['--target=wasm32-wasip1','-Wl,-z,stack-size=1048576'];
const dependencies=new Set([resolve(root,'portable/check-anm-trig-input.mjs')]);
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
    ...['Arithmetic','GameMath'].map(name=>compile(`th10_web/cpp/game/${name}.cpp`,name))];
for(const multiplayer of [false,true]){
    const variant=multiplayer?'multiplayer':'ordinary';
    const test=compile('tests/anm-trig-input-test.cpp',variant,[...flags,...(multiplayer?['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']:[])]);
    const wasm=resolve(out,variant+'.wasm');
    execFileSync(compiler,[...linkFlags,test,...common,'-o',wasm],{cwd:root,stdio:'inherit'});
    const wasi=new WASI({version:'preview1',args:[variant],env:{},returnOnExit:true});
    const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
    const code=wasi.start(instance);if(code!==0)throw Error(`${variant} failed: ${code}`);
}
writeFileSync(metadataPath,JSON.stringify({passed:true,runtime:'wasm32-wasip1',node:process.version,
    compiler:execFileSync(compiler,['--version'],{encoding:'utf8'}).trim(),compileFlags:flags,softfloatFlags,linkFlags,sanitized,
    variants:['ordinary','multiplayer'],dependencyScope:'Clang -MMD non-system source/header closure plus this driver; SDK system headers/libraries excluded',
    sources:Object.fromEntries([...dependencies].sort().map(path=>[relative(root,path).replaceAll('\\','/'),createHash('sha256').update(readFileSync(path)).digest('hex')]))},null,2)+'\n');
