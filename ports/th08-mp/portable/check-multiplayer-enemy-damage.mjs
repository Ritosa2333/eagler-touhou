import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {WASI} from 'node:wasi';

const root=resolve(import.meta.dirname,'..');
const sdk=process.env.WASI_SDK_PATH??[
  resolve(root,'../toolchains/wasi-sdk-34.0-x86_64-windows'),
  resolve(root,'../../toolchains/wasi-sdk-34.0-x86_64-windows'),
].find(path=>existsSync(resolve(path,'bin/clang++.exe')));
if(!sdk)throw Error('Set WASI_SDK_PATH to the installed WASI SDK directory.');
const compiler=resolve(sdk,'bin',process.platform==='win32'?'clang++.exe':'clang++');
const cCompiler=resolve(sdk,'bin',process.platform==='win32'?'clang.exe':'clang');
const out=resolve(root,'artifacts/multiplayer-tests');mkdirSync(out,{recursive:true});
const object=resolve(out,'enemy-damage-softfloat.o'),wasm=resolve(out,'enemy-damage.wasm');
execFileSync(cCompiler,['--target=wasm32-wasip1','-O2','-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5','-c',resolve(root,'th08_web/cpp/third_party/softfloat.c'),'-o',object],{windowsHide:true,stdio:'inherit'});
execFileSync(compiler,[
  '--target=wasm32-wasip1','-O2','-std=c++17','-fno-exceptions','-fno-rtti',
  '-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-Wl,-z,stack-size=1048576',
  '-I'+resolve(root,'th08_web/cpp/game'),
  resolve(root,'th08_web/cpp/game/Arithmetic.cpp'),
  resolve(root,'th08_web/cpp/game/GameValues.cpp'),
  resolve(root,'th08_web/cpp/game/Rng.cpp'),
  resolve(root,'th08_web/cpp/game/EnemyContact.cpp'),
  resolve(root,'th08_web/cpp/game/EnemyDamage.cpp'),
  resolve(root,'tests/enemy-multiplayer-damage-test.cpp'),object,'-o',wasm,
],{windowsHide:true,stdio:'inherit'});
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
const {instance}=await WebAssembly.instantiate(readFileSync(wasm),{wasi_snapshot_preview1:wasi.wasiImport});
const code=wasi.start(instance);if(code!==0)throw Error(`Enemy damage tests failed: ${code}`);
console.log('enemy-multiplayer-damage: PASS');
