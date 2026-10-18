import {execFileSync} from 'node:child_process';
import {mkdirSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';
const root=resolve(import.meta.dirname,'..');
const sdk=process.env.WASI_SDK_PATH??resolve(root,'../../toolchains/wasi-sdk-34.0-x86_64-windows');
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const soft=resolve(out,'familiar-presentation-softfloat.o'),wasm=resolve(out,'familiar-presentation.wasm');
execFileSync(resolve(sdk,'bin/clang.exe'),['--target=wasm32-wasip1','-O2','-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5',
  '-c',resolve(root,'th08_web/cpp/third_party/softfloat.c'),'-o',soft],{windowsHide:true,stdio:'inherit'});
execFileSync(resolve(sdk,'bin/clang++.exe'),['--target=wasm32-wasip1','-O2','-std=c++17','-fno-exceptions','-fno-rtti',
  '-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-I'+resolve(root,'th08_web/cpp/game'),'-Wl,-z,stack-size=1048576',
  ...['Arithmetic','GameMath','Rng','Timer','AnmExecutor','EnemyDrawing'].map(name=>resolve(root,'th08_web/cpp/game',name+'.cpp')),
  resolve(root,'tests/familiar-presentation-test.cpp'),soft,'-o',wasm],{windowsHide:true,stdio:'inherit'});
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
const code=wasi.start(instance);if(code!==0)throw Error('Familiar presentation tests failed: '+code);
