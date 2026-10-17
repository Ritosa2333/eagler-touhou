import {execFileSync} from 'node:child_process';
import {mkdirSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'..');
const sdk=process.env.WASI_SDK_PATH??resolve(root,'../../toolchains/wasi-sdk-34.0-x86_64-windows');
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const wasm=resolve(out,'live-bullets.wasm');
execFileSync(resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++'),[
  '--target=wasm32-wasip1','-O2','-std=c++17','-fno-exceptions','-fno-rtti',
  '-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-I'+resolve(root,'third_party/eagler-common/include'),
  '-Wl,-z,stack-size=1048576',resolve(root,'tests/live-bullet-snapshot-test.cpp'),'-o',wasm],{stdio:'inherit',windowsHide:true});
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
const code=wasi.start(instance);if(code)throw Error('Live Bullet regression failed: '+code);
