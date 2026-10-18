import {spawnSync} from 'node:child_process';
import {readFileSync,mkdirSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'../..'),out=resolve(root,'artifacts/multiplayer-tests/fresh-touch-native');
mkdirSync(out,{recursive:true});
const compiler=resolve(process.env.WASI_SDK_BIN||resolve(root,'../../toolchains/wasi-sdk-34.0-x86_64-windows/bin'),'clang++.exe');
const sources=['portable/multiplayer/fresh-touch-check.cpp','th08_web/cpp/multiplayer/AnalogMovement.cpp','th08_web/cpp/game/PlayerMovement.cpp','th08_web/cpp/game/Arithmetic.cpp',
  'third_party/eagler-common/src/netplay/NetplayCore.cpp','third_party/eagler-common/src/netplay/NetplayProtocol.cpp'];
const wasm=resolve(out,'test.wasm');
const soft=resolve(out,'softfloat.o');
const arithmetic=spawnSync(compiler,['-x','c','-std=c11','-O2','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5','-c',resolve(root,'th08_web/cpp/third_party/softfloat.c'),'-o',soft],{encoding:'utf8',windowsHide:true});
if(arithmetic.status||arithmetic.error)throw arithmetic.error||Error(arithmetic.stdout+arithmetic.stderr);
const built=spawnSync(compiler,['-O2','-std=c++17','-fno-exceptions','-fno-rtti','-ffp-contract=off','-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1',
  '-Wl,-z,stack-size=1048576','-I'+resolve(root,'third_party/eagler-common/include'),'-I'+resolve(root,'th08_web/cpp/multiplayer'),...sources.map(x=>resolve(root,x)),soft,'-o',wasm],{encoding:'utf8',windowsHide:true});
writeFileSync(resolve(out,'compile.log'),built.stdout+built.stderr);
if(built.status||built.error)throw built.error||Error(built.stdout+built.stderr);
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
const code=wasi.start(instance);if(code)throw Error('Fresh-touch verification failed: '+code);
