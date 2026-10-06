import {parseMakeDependencies} from './make-dependencies.mjs';
import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync,rmSync} from 'node:fs';
import {resolve,relative,dirname,isAbsolute} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

// Resource-free regression of Presentation::submit against real 16-bit
// GameInput edges, including a two-byte-aligned/four-byte-unaligned key word.
// Usage: WASI_SDK_PATH=/path/to/wasi-sdk node portable/check-presentation-input.mjs
// Optional TH10_TEST_UBSAN=1 enables trapping undefined-behavior checks.
const root=resolve(dirname(fileURLToPath(import.meta.url)),'..');
const sdk=process.env.WASI_SDK_PATH;
if(!sdk)throw Error('Set WASI_SDK_PATH to an installed WASI SDK directory.');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw Error(`WASI compiler not found: ${compiler}`);
const sanitized=process.env.TH10_TEST_UBSAN==='1';
const out=resolve(root,'artifacts/presentation-input'+(sanitized?'-ubsan':''));mkdirSync(out,{recursive:true});
const metadataPath=resolve(out,'build.json');rmSync(metadataPath,{force:true});
const flags=['--target=wasm32-wasip1','-O2','-std=c++17','-Wall','-Wextra','-Werror',
    '-DTH_NATIVE_PLATFORM=1','-fno-exceptions','-fno-rtti','-ffp-contract=off','-fno-strict-aliasing'];
if(sanitized)flags.push('-fsanitize=undefined','-fsanitize-trap=undefined');
const linkFlags=['--target=wasm32-wasip1','-Wl,-z,stack-size=1048576'];
const binding=resolve(root,'th10_web/cpp/platform/ApplicationPresentation.cpp');
if(!readFileSync(binding,'utf8').replace(/\s+/g,'').includes('pressed_keys=&a.input.player_profiles[0].input.raw_pressed;'))
    throw Error('AppPresentation must bind the real 16-bit raw_pressed word directly.');
const dependencies=new Set([resolve(root,'portable/check-presentation-input.mjs'),resolve(root,'portable/make-dependencies.mjs'),binding]);
function compile(source,name,compileFlags){
    const object=resolve(out,name+'.o'),depfile=object+'.d';
    execFileSync(compiler,[...compileFlags,'-MMD','-MF',depfile,'-MT','source','-c',resolve(root,source),'-o',object],{cwd:root,stdio:'inherit'});
    for(const token of parseMakeDependencies(readFileSync(depfile,'utf8'))){
        const path=resolve(root,token),name=relative(root,path);
        if(name.startsWith('..')||isAbsolute(name))throw Error(`Project dependency outside checkout: ${path}`);
        dependencies.add(path);
    }
    return object;
}
for(const multiplayer of [false,true]){
    const variant=multiplayer?'multiplayer':'ordinary';
    const variantFlags=[...flags,...(multiplayer?['-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1']:[])];
    // Compile the real owner bindings as well as executing the small fixture.
    // Existing platform sources are not warning-clean, so keep errors strict
    // for fixture/game sources without upgrading legacy platform warnings.
    const bindingFlags=[...variantFlags.filter(flag=>flag!=='-Werror'),'-I',resolve(root,'third_party/eagler-common/include')];
    for(const source of ["th10_web/cpp/platform/ApplicationPresentation.cpp"])
        compile(source,variant+'-binding-'+source.split('/').at(-1).replace('.cpp',''),bindingFlags);
    const objects=[compile('tests/presentation-input-test.cpp',variant+'-test',variantFlags),
        ...['GameInput','Presentation'].map(name=>compile(`th10_web/cpp/game/${name}.cpp`,variant+'-'+name,variantFlags))];
    const wasm=resolve(out,variant+'.wasm');
    execFileSync(compiler,[...linkFlags,...objects,'-o',wasm],{cwd:root,stdio:'inherit'});
    const wasi=new WASI({version:'preview1',args:[variant],env:{},returnOnExit:true});
    const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
    const code=wasi.start(instance);if(code!==0)throw Error(`${variant} failed: ${code}`);
}
writeFileSync(metadataPath,JSON.stringify({passed:true,runtime:'wasm32-wasip1',node:process.version,
    compiler:execFileSync(compiler,['--version'],{encoding:'utf8'}).trim(),compileFlags:flags,linkFlags,sanitized,
    bindingCompileFlags:[...flags.filter(flag=>flag!=='-Werror'),'-I','third_party/eagler-common/include'],
    variants:['ordinary','multiplayer'],productionExecution:['GameInput::update_raw','Presentation::submit'],
    dependencyScope:'Clang -MMD non-system source/header closure plus this driver; SDK system headers/libraries excluded',
    sources:Object.fromEntries([...dependencies].sort().map(path=>[relative(root,path).replaceAll('\\','/'),createHash('sha256').update(readFileSync(path)).digest('hex')]))},null,2)+'\n');
