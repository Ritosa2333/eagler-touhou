// Native protocol gate; requires a WASI SDK, but no private game DATA.
import {readFileSync, mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {execFileSync} from 'node:child_process';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'../..');
const common=resolve(process.env.EAGLER_COMMON_ROOT||resolve(root,'../../common'));
const compiler=process.env.EAGLER_WASI_CXX;
if(!compiler)throw Error('Set EAGLER_WASI_CXX to the WASI SDK clang++ executable');
const output=resolve(root,'web/artifacts/multiplayer-tests');mkdirSync(output,{recursive:true});
const wasm=resolve(output,'delay-session-test.wasm');
execFileSync(compiler,['-O2','-std=c++17','-fno-exceptions','-fno-rtti',
  '-I'+resolve(root,'web/cpp'),'-I'+resolve(common,'include'),
  resolve(root,'web/tests/multiplayer/delay-session-test.cpp'),
  ...['NetplayProtocol.cpp','NetplayCore.cpp','NetplaySession.cpp','SessionChannel.cpp'].map(n=>resolve(common,'src/netplay',n)),
  '-Wl,-z,stack-size=1048576','-o',wasm],{stdio:'inherit'});
const wasi=new WASI({version:'preview1',args:[],env:{},preopens:{}});
const module=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
wasi.start(module.instance);
