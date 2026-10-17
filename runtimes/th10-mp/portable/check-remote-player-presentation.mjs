import {parseMakeDependencies} from './make-dependencies.mjs';
import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync,rmSync} from 'node:fs';
import {resolve,relative,dirname,isAbsolute,sep} from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

// Resource-free execution of the real remote display-frontier helper.
// Usage: WASI_SDK_PATH=/path/to/wasi-sdk node portable/check-remote-player-presentation.mjs
// Optional TH10_TEST_UBSAN=1 enables trapping undefined-behavior checks.
// This runner does not build, replace, or load a browser game artifact.
const driver=fileURLToPath(import.meta.url);
const root=resolve(dirname(driver),'..');
const sdk=process.env.WASI_SDK_PATH;
if(!sdk) throw Error('Set WASI_SDK_PATH to an installed WASI SDK directory.');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler)) throw Error('WASI compiler not found: '+compiler);
const sanitized=process.env.TH10_TEST_UBSAN==='1';
const out=resolve(root,'artifacts/remote-player-presentation'+(sanitized?'-ubsan':''));
mkdirSync(out,{recursive:true});
const metadataPath=resolve(out,'build.json');
rmSync(metadataPath,{force:true});
const flags=[
    '--target=wasm32-wasip1','-O2','-std=c++17','-Wall','-Wextra','-Werror',
    '-DTH_NATIVE_PLATFORM=1','-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1',
    '-fno-exceptions','-fno-rtti','-ffp-contract=off','-fno-strict-aliasing',
    '-I',resolve(root,'third_party/eagler-common/include')
];
if(sanitized) flags.push('-fsanitize=undefined','-fsanitize-trap=undefined');
const linkFlags=['--target=wasm32-wasip1','-Wl,-z,stack-size=1048576'];
const source=resolve(root,'tests/remote-player-presentation.cpp');
const object=resolve(out,'remote-player-presentation.o');
const wasm=resolve(out,'remote-player-presentation.wasm');
const discovery=resolve(out,'remote-player-presentation.before.d');
const depfile=object+'.d';
const compilerOptions={cwd:root,stdio:'inherit',windowsHide:true};
const hash=bytes=>createHash('sha256').update(bytes).digest('hex');

function dependencyPaths(path) {
    const result=new Set([driver,resolve(root,'portable/make-dependencies.mjs')]);
    for(const token of parseMakeDependencies(readFileSync(path,'utf8'))) {
        const dependency=resolve(root,token);
        const name=relative(root,dependency);
        if(name==='..'||name.startsWith('..'+sep)||isAbsolute(name))
            throw Error('Project dependency outside checkout: '+dependency);
        result.add(dependency);
    }
    return [...result].sort();
}
function inventory(paths) {
    return Object.fromEntries(paths.map(path=>[
        relative(root,path).replaceAll('\\','/'),hash(readFileSync(path))
    ]));
}

// Discover the same non-system closure before compiling, then verify it again
// after execution. A concurrent source edit cannot receive a misleading PASS
// manifest for different bytes. Reuse the Windows-safe Make depfile parser.
execFileSync(compiler,[...flags,'-MM','-MF',discovery,'-MT','source',source],compilerOptions);
const before=inventory(dependencyPaths(discovery));
execFileSync(compiler,[...flags,'-MMD','-MF',depfile,'-MT','source',
    '-c',source,'-o',object],compilerOptions);
const compiled=inventory(dependencyPaths(depfile));
if(JSON.stringify(before)!==JSON.stringify(compiled))
    throw Error('Source dependency closure or bytes changed during compilation.');
execFileSync(compiler,[...linkFlags,object,'-o',wasm],compilerOptions);

const wasmBytes=readFileSync(wasm);
const wasi=new WASI({
    version:'preview1',args:['remote-player-presentation'],env:{},returnOnExit:true
});
const {instance}=await WebAssembly.instantiate(wasmBytes,{
    wasi_snapshot_preview1:wasi.wasiImport
});
const exitCode=wasi.start(instance);
if(exitCode!==0) throw Error('Remote player presentation test failed: '+exitCode);
const sources=inventory(dependencyPaths(depfile));
if(JSON.stringify(before)!==JSON.stringify(sources))
    throw Error('Source dependency closure or bytes changed before verification completed.');

writeFileSync(metadataPath,JSON.stringify({
    passed:true,
    suite:'remote-player-presentation',
    runtime:'wasm32-wasip1',
    node:process.version,
    compiler:execFileSync(compiler,['--version'],{
        encoding:'utf8',windowsHide:true
    }).trim(),
    compileFlags:flags,
    linkFlags,
    sanitized,
    source:'tests/remote-player-presentation.cpp',
    productionExecution:[
        'RemotePlayerPresentation::Freeze','RemotePlayerPresentation::Publish',
        'RemotePlayerPresentation::Get','RemotePlayerPresentation::Consume',
        'RemotePlayerPresentation::Invalidate'
    ],
    coverage:[
        'stable and historical guards, nested recovery, warm and cold freeze',
        'repeat publication and unchanged source object bytes',
        'no-forward snap, unshown correction anchor and real Present acknowledgement',
        'already-presented endpoint, stale Consume and multiple forward frontiers',
        'owner reuse, death, state, lifecycle identity and invalid lifecycle',
        '128-pixel discontinuity, non-finite coordinates and exact native target bits',
        'local-seat exclusion, unusable session, frame regression and reset'
    ],
    dependencyScope:'Clang -MM/-MMD non-system source/header closure plus this driver and Make dependency parser; SDK system headers/libraries excluded',
    sourceStability:'Identical source inventory before compilation, after compilation and after execution',
    sources,
    sourceSha256:hash(JSON.stringify(sources)),
    wasmSha256:hash(wasmBytes)
},null,2)+'\n');
console.log('Remote player presentation manifest: '+metadataPath);
