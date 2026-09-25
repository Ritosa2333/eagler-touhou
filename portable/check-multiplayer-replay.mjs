import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'..');
const sdk=process.env.WASI_SDK_PATH??resolve(root,'../../toolchains/wasi-sdk-34.0-x86_64-windows');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
if(!existsSync(compiler))throw Error('WASI SDK not found: '+compiler);
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const common=resolve(root,'third_party/eagler-common'),title=resolve(root,'th08_web/cpp/multiplayer');
const wasm=resolve(out,'replay-archive.wasm');
const sources=[...['SessionSetup','NetplayRuntime','ReplayArchive'].map(name=>resolve(title,name+'.cpp')),
 ...['NetplayProtocol','NetplayCore','NetplaySession','InputReplay'].map(name=>resolve(common,'src/netplay',name+'.cpp')),
 resolve(root,'tests/replay-archive-test.cpp')];
execFileSync(compiler,['--target=wasm32-wasip1','-O2','-std=c++17','-fno-exceptions','-fno-rtti',
 '-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-I'+title,'-I'+resolve(common,'include'),
 '-Wl,-z,stack-size=1048576',...sources,'-o',wasm],{windowsHide:true,stdio:'inherit'});
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
if(wasi.start(instance)!==0)throw Error('TH08 Replay archive tests failed');
console.log('TH08 Replay archive: PASS');
