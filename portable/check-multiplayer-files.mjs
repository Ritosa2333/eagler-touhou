import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'..');
const sdk=process.env.WASI_SDK_PATH??resolve(root,'../../toolchains/wasi-sdk-34.0-x86_64-windows');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw Error('Set WASI_SDK_PATH to the installed WASI SDK');
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const wasm=resolve(out,'file-events.wasm');
execFileSync(compiler,['--target=wasm32-wasip1','-O2','-std=c++17','-fno-exceptions','-fno-rtti',
 '-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-Wl,-z,stack-size=1048576',resolve(root,'th08_web/cpp/multiplayer/FileEvents.cpp'),
 resolve(root,'tests/file-events-test.cpp'),'-o',wasm],{windowsHide:true,stdio:'inherit'});
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
if(wasi.start(instance)!==0)throw Error('File events failed');
