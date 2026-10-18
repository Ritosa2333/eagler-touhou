import {parseMakeDependencies} from './make-dependencies.mjs';
import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve,relative,dirname,isAbsolute} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

// Real StageRenderer/Timer/Camera/AnmRenderer component with production math.
// The fixture records the external graphics boundary and empty ANM layers.
// It proves fade-before/after semantics, not Application hooks, pixels, or speed.
// Usage: node portable/check-network-tail-presentation.mjs --output-dir NEW_DIRECTORY
// Set WASI_SDK_PATH to the installed SDK; every run preserves its own output.
// TH10_TEST_UBSAN=1 instruments C++ only; vendored SoftFloat stays excluded,
// matching check-anm-submit.mjs and its documented SoftFloat scope limitation.
const root=resolve(dirname(fileURLToPath(import.meta.url)),'..');
const sdk=process.env.WASI_SDK_PATH;
if(!sdk)throw Error('Set WASI_SDK_PATH to an installed WASI SDK directory.');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw Error(`WASI compiler not found: ${compiler}`);
const sanitized=process.env.TH10_TEST_UBSAN==='1';
const args=process.argv.slice(2);
if(args.length!==2||args[0]!=='--output-dir'||!args[1])
    throw Error('Usage: node portable/check-network-tail-presentation.mjs --output-dir NEW_DIRECTORY');
const out=resolve(root,args[1]);
mkdirSync(dirname(out),{recursive:true});
mkdirSync(out); // Preserve failed and successful runs; every invocation is fresh.
const metadataPath=resolve(out,'build.json');
const flags=['--target=wasm32-wasip1','-O2','-std=c++17','-Wall','-Wextra','-Werror',
    '-DTH_NATIVE_PLATFORM=1','-fno-exceptions','-fno-rtti','-ffp-contract=off','-fno-strict-aliasing',
    '-I'+resolve(root,'th10_web/cpp/game')];
if(sanitized)flags.push('-fsanitize=undefined','-fsanitize-trap=undefined');
const softfloatFlags=['--target=wasm32-wasip1','-O2','-x','c','-std=c11',
    '-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5'];
const linkFlags=['--target=wasm32-wasip1','-Wl,-z,stack-size=1048576'];
const dependencies=new Map();
const sha256=path=>createHash('sha256').update(readFileSync(path)).digest('hex');
function remember(path){
    const name=relative(root,path);
    if(name.startsWith('..')||isAbsolute(name))throw Error(`Project dependency outside checkout: ${path}`);
    const digest=sha256(path);
    if(dependencies.has(path)&&dependencies.get(path)!==digest)
        throw Error(`Project dependency changed after first observation: ${name}`);
    dependencies.set(path,digest);
}
remember(resolve(root,'portable/check-network-tail-presentation.mjs'));
remember(resolve(root,'portable/make-dependencies.mjs'));
function compile(source,name,compileFlags=flags){
    const sourcePath=resolve(root,source),object=resolve(out,name+'.o'),depfile=object+'.d';
    remember(sourcePath);
    execFileSync(compiler,[...compileFlags,'-MMD','-MF',depfile,'-MT','source',
        '-c',sourcePath,'-o',object],{cwd:root,stdio:'inherit'});
    for(const token of parseMakeDependencies(readFileSync(depfile,'utf8')))
        remember(resolve(root,token));
    return object;
}
const common=[compile('th10_web/cpp/rebuild/third_party/softfloat.c','softfloat',softfloatFlags),
    ...['Arithmetic','GameMath'].map(name=>compile(`th10_web/cpp/game/${name}.cpp`,name))];
const sanitizer={enabled:sanitized,softfloatIncluded:false,
    scope:sanitized?'C++ test/production code only; third-party SoftFloat excluded':'none'};
const runs=[];
console.log(JSON.stringify({suite:'network-tail-presentation-stage-boundary',sanitizer}));
for(const multiplayer of [false,true]){
    const variant=multiplayer?'multiplayer':'ordinary';
    const variantFlags=[...flags,...(multiplayer?['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']:[])];
    const production=['StageRenderer','Timer','Camera','GraphicsMath','AnmRenderer']
        .map(name=>compile(`th10_web/cpp/game/${name}.cpp`,variant+'-'+name,variantFlags));
    // Capture remains disabled. Any unexpected live-motion callback aborts.
    const observer=multiplayer
        ?[compile('tests/live-motion-disabled-stub.cpp',variant+'-observer-disabled',variantFlags)]:[];
    const test=compile('portable/check-network-tail-presentation.cpp',variant+'-test',variantFlags);
    const wasm=resolve(out,variant+'.wasm');
    execFileSync(compiler,[...linkFlags,test,...production,...observer,...common,'-o',wasm],
        {cwd:root,stdio:'inherit'});
    const wasi=new WASI({version:'preview1',args:[variant+'-stage-boundary'],env:{},returnOnExit:true});
    const {instance}=await WebAssembly.instantiate(readFileSync(wasm),
        {wasi_snapshot_preview1:wasi.wasiImport});
    console.log(JSON.stringify({variant,suite:'stage-fade-boundary'}));
    const exitCode=wasi.start(instance);
    if(exitCode!==0)throw Error(`${variant}/stage-fade-boundary failed: ${exitCode}`);
    runs.push({variant,exitCode,wasmSha256:sha256(wasm)});
}
// Headers are first hashed when their actual Clang depfile is available.
// This guard detects later edits; it is not a pre-build snapshot of every header.
for(const path of dependencies.keys())remember(path);
writeFileSync(metadataPath,JSON.stringify({passed:true,
    suite:'network-tail-presentation-stage-boundary',
    scope:'Real WASI32 StageRenderer, Timer, Camera, GraphicsMath, AnmRenderer and shared Stage predicate; recorded external graphics boundary; no Application lifecycle, browser, pixels, audio or performance acceptance',
    sanitizer,runtime:'wasm32-wasip1',node:process.version,
    compiler:execFileSync(compiler,['--version'],{encoding:'utf8'}).trim(),
    compileFlags:flags,softfloatFlags,linkFlags,sanitized,runs,
    dependencyScope:'Clang -MMD non-system source/header closure plus driver and make-dependencies; hashes stable since first observation; SDK system headers/libraries excluded',
    sources:Object.fromEntries([...dependencies.entries()].sort(([a],[b])=>a.localeCompare(b))
        .map(([path,hash])=>[relative(root,path).replaceAll('\\','/'),hash]))},null,2)+'\n');
