import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync,rmSync,openSync,closeSync} from 'node:fs';
import {resolve,relative,dirname,isAbsolute} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

// Resource-free player-shot lookup and actual update_shots exactness suites.
// The lookup lane uses explicit motion/timer doubles; the full lane uses real
// game arithmetic, movement, timers and registry plus an original-body oracle.
// Usage: WASI_SDK_PATH=/path/to/wasi-sdk node portable/check-player-shot-lookup.mjs
// TH10_TEST_UBSAN=1 instruments C++ only. Vendored SoftFloat is excluded;
// the pre-existing SoftFloat defect and full-engine ECL limitation remain open.
const root=resolve(dirname(fileURLToPath(import.meta.url)),'..');
const sdk=process.env.WASI_SDK_PATH;
if(!sdk)throw Error('Set WASI_SDK_PATH to an installed WASI SDK directory.');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw Error(`WASI compiler not found: ${compiler}`);
const sanitized=process.env.TH10_TEST_UBSAN==='1';
const out=resolve(root,'artifacts/player-shot-lookup'+(sanitized?'-ubsan':''));mkdirSync(out,{recursive:true});
const metadataPath=resolve(out,'build.json');rmSync(metadataPath,{force:true});
const flags=['--target=wasm32-wasip1','-O2','-std=c++17','-Wall','-Wextra','-Werror',
    '-DTH_NATIVE_PLATFORM=1','-fno-exceptions','-fno-rtti','-ffp-contract=off','-fno-strict-aliasing',
    '-I'+resolve(root,'th10_web/cpp/game')];
if(sanitized)flags.push('-fsanitize=undefined','-fsanitize-trap=undefined');
const softfloatFlags=['--target=wasm32-wasip1','-O2','-x','c','-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5'];
const linkFlags=['--target=wasm32-wasip1','-Wl,-z,stack-size=1048576'];
const sanitizer={enabled:sanitized,softfloatIncluded:false,scope:sanitized?'C++ tests/production code only; vendored SoftFloat excluded':'none'};
console.log(JSON.stringify({suite:'player-shot-lookup',sanitizer}));
const dependencies=new Set([resolve(root,'portable/check-player-shot-lookup.mjs')]),runs=[];
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
async function run(name,objects){
    const wasm=resolve(out,name+'.wasm'),log=resolve(out,name+'.log');
    execFileSync(compiler,[...linkFlags,...objects,'-o',wasm],{cwd:root,stdio:'inherit'});
    const stdout=openSync(log,'w');
    try{
        const wasi=new WASI({version:'preview1',args:[name],env:{},stdout,returnOnExit:true});
        const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
        const code=wasi.start(instance);if(code!==0)throw Error(`${name} failed: ${code}`);
    }finally{closeSync(stdout);}
    const text=readFileSync(log,'utf8');console.log(`${name}: ${text.trim()}`);
    runs.push({name,passed:true,output:text,wasmSha256:createHash('sha256').update(readFileSync(wasm)).digest('hex')});
    return text;
}
const softfloat=compile('th10_web/cpp/rebuild/third_party/softfloat.c','softfloat',softfloatFlags);
const registry=compile('th10_web/cpp/game/AnmRegistry.cpp','AnmRegistry');
const numeric=['Arithmetic','Movement','GameMath','Timer'].map(name=>compile(`th10_web/cpp/game/${name}.cpp`,name));
let originalTrace;
for(const multiplayer of [false,true]){
    const variant=multiplayer?'multiplayer':'ordinary';
    const variantFlags=[...flags,...(multiplayer?['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']:[])];
    const shooting=compile('th10_web/cpp/game/PlayerShooting.cpp',variant+'-PlayerShooting',variantFlags);
    const lookup=compile('tests/player-shot-lookup-test.cpp',variant+'-lookup',variantFlags);
    const trace=await run(variant+'-lookup',[lookup,registry,shooting]);
    if(!multiplayer)originalTrace=trace;else if(trace!==originalTrace)throw Error('Ordinary/multiplayer lookup trace differs');
    const full=compile('tests/player-shot-update-test.cpp',variant+'-update',variantFlags);
    await run(variant+'-update',[full,shooting,registry,...numeric,softfloat]);
}
writeFileSync(metadataPath,JSON.stringify({passed:true,sanitizer,runtime:'wasm32-wasip1',node:process.version,
    compiler:execFileSync(compiler,['--version'],{encoding:'utf8'}).trim(),compileFlags:flags,softfloatFlags,linkFlags,sanitized,
    variants:['ordinary','multiplayer'],runs,
    dependencyScope:'Clang -MMD non-system source/header closure plus this driver; SDK system headers/libraries excluded',
    sources:Object.fromEntries([...dependencies].sort().map(path=>[relative(root,path).replaceAll('\\','/'),createHash('sha256').update(readFileSync(path)).digest('hex')]))},null,2)+'\n');
