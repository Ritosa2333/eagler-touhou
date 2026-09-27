import {execFileSync} from 'node:child_process';
import {mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'..'),common=resolve(root,'third_party/eagler-common');
const sdk=process.env.WASI_SDK_PATH??resolve(root,'../../toolchains/wasi-sdk-34.0-x86_64-windows');
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const title=resolve(root,'th08_web/cpp/multiplayer');
let connection=resolve(title,'NetworkConnection.cpp');
if(process.argv.includes('--baseline')){
  connection=resolve(out,'capture-boundary-baseline.cpp');
  writeFileSync(connection,execFileSync('git',['show','HEAD:th08_web/cpp/multiplayer/NetworkConnection.cpp'],{cwd:root}));
}
const wasm=resolve(out,process.argv.includes('--baseline')?'capture-boundary-baseline.wasm':'capture-boundary.wasm');
execFileSync(resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++'),['--target=wasm32-wasip1','-O2','-std=c++17','-fno-exceptions','-fno-rtti',
  '-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-I'+title,'-I'+resolve(root,'tests/support'),'-I'+resolve(common,'include'),
  '-Wl,-z,stack-size=1048576',connection,...['SessionSetup','NetplayRuntime'].map(n=>resolve(title,n+'.cpp')),
  ...['NetplayProtocol','NetplayCore','NetplaySession','SessionChannel'].map(n=>resolve(common,'src/netplay',n+'.cpp')),
  resolve(root,'tests/network-capture-boundary-test.cpp'),'-o',wasm],{stdio:'inherit',windowsHide:true});
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
const code=wasi.start(instance);if(code)throw Error('Capture boundary regression failed: '+code);
