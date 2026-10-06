import {parseMakeDependencies} from './make-dependencies.mjs';
import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync,rmSync} from 'node:fs';
import {resolve,relative,dirname,isAbsolute} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

// Resource-free original-body submit oracle. Retain every manager/VM/sprite
// byte, quad, callback/pipeline trace, return, numeric state and errno.
// Usage: WASI_SDK_PATH=/path/to/wasi-sdk node portable/check-anm-submit.mjs
// TH10_TEST_UBSAN=1 instruments C++ only; vendored SoftFloat remains excluded.
// Its independently documented zero-significand shift defect is not a pass.
const root=resolve(dirname(fileURLToPath(import.meta.url)),'..');
const sdk=process.env.WASI_SDK_PATH;
if(!sdk)throw Error('Set WASI_SDK_PATH to an installed WASI SDK directory.');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw Error(`WASI compiler not found: ${compiler}`);
const sanitized=process.env.TH10_TEST_UBSAN==='1';
const out=resolve(root,'artifacts/anm-submit'+(sanitized?'-ubsan':''));mkdirSync(out,{recursive:true});
const metadataPath=resolve(out,'build.json');rmSync(metadataPath,{force:true});
const flags=['--target=wasm32-wasip1','-O2','-std=c++17','-Wall','-Wextra','-Werror',
    '-DTH_NATIVE_PLATFORM=1','-fno-exceptions','-fno-rtti','-ffp-contract=off','-fno-strict-aliasing',
    '-I'+resolve(root,'th10_web/cpp/game')];
if(sanitized)flags.push('-fsanitize=undefined','-fsanitize-trap=undefined');
const softfloatFlags=['--target=wasm32-wasip1','-O2','-x','c','-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5'];
const linkFlags=['--target=wasm32-wasip1','-Wl,-z,stack-size=1048576'];
const dependencies=new Set([resolve(root,'portable/check-anm-submit.mjs'),resolve(root,'portable/make-dependencies.mjs')]);
function compile(source,name,compileFlags=flags){
    const object=resolve(out,name+'.o'),depfile=object+'.d';
    execFileSync(compiler,[...compileFlags,'-MMD','-MF',depfile,'-MT','source','-c',resolve(root,source),'-o',object],{cwd:root,stdio:'inherit'});
    for(const token of parseMakeDependencies(readFileSync(depfile,'utf8'))){
        const path=resolve(root,token),name=relative(root,path);
        if(name.startsWith('..')||isAbsolute(name))throw Error(`Project dependency outside checkout: ${path}`);
        dependencies.add(path);
    }
    return object;
}
const common=[compile('th10_web/cpp/rebuild/third_party/softfloat.c','softfloat',softfloatFlags),
    ...['Arithmetic','GameMath'].map(name=>compile(`th10_web/cpp/game/${name}.cpp`,name))];
const sanitizer={enabled:sanitized,softfloatIncluded:false,scope:sanitized?'C++ test/production code only; third-party SoftFloat excluded':'none'};
console.log(JSON.stringify({suite:'anm-submit',sanitizer}));
for(const multiplayer of [false,true]){
    const variant=multiplayer?'multiplayer':'ordinary';
    const variantFlags=[...flags,...(multiplayer?['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']:[])];
    const renderer=compile('th10_web/cpp/game/AnmRenderer.cpp',variant+'-renderer',variantFlags);
    for(const suite of ['helper','body']){
        const test=compile(suite==='helper'?'tests/anm-submit-helper-test.cpp':'tests/anm-submit-test.cpp',variant+'-'+suite,variantFlags);
        const wasm=resolve(out,variant+'-'+suite+'.wasm');
        execFileSync(compiler,[...linkFlags,test,renderer,...common,'-o',wasm],{cwd:root,stdio:'inherit'});
        const wasi=new WASI({version:'preview1',args:[variant+'-'+suite],env:{},returnOnExit:true});
        const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
        console.log(JSON.stringify({variant,suite}));
        const code=wasi.start(instance);if(code!==0)throw Error(`${variant}/${suite} failed: ${code}`);
    }
}
writeFileSync(metadataPath,JSON.stringify({passed:true,sanitizer,runtime:'wasm32-wasip1',node:process.version,
    compiler:execFileSync(compiler,['--version'],{encoding:'utf8'}).trim(),compileFlags:flags,softfloatFlags,linkFlags,sanitized,
    variants:['ordinary','multiplayer'],dependencyScope:'Clang -MMD non-system source/header closure plus this driver; SDK system headers/libraries excluded',
    sources:Object.fromEntries([...dependencies].sort().map(path=>[relative(root,path).replaceAll('\\','/'),createHash('sha256').update(readFileSync(path)).digest('hex')]))},null,2)+'\n');
