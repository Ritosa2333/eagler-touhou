import {execFileSync} from 'node:child_process';
import {existsSync,mkdirSync,readFileSync,writeFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {WASI} from 'node:wasi';

const root=resolve(import.meta.dirname,'../..');
const compilerName=process.platform==='win32'?'clang++.exe':'clang++';
const sdk=process.env.WASI_SDK_PATH??[
  resolve(root,'../toolchains/wasi-sdk-34.0-x86_64-windows'),
  resolve(root,'../../toolchains/wasi-sdk-34.0-x86_64-windows'),
].find(path=>existsSync(resolve(path,'bin',compilerName)));
if(!sdk)throw Error('Set WASI_SDK_PATH to the installed WASI SDK directory.');
const compiler=resolve(sdk,'bin',compilerName);
const cCompiler=resolve(sdk,'bin',process.platform==='win32'?'clang.exe':'clang');
const out=resolve(root,'artifacts/camera-cache-tests');mkdirSync(out,{recursive:true});
const object=resolve(out,'camera-cache-softfloat.o'),wasm=resolve(out,'camera-cache.wasm');
const reportPath=resolve(out,'report.json');
writeFileSync(reportPath,JSON.stringify({passed:false,phase:'not-completed',scope:'Camera cache build/oracle pending'},null,2)+'\n');
execFileSync(cCompiler,[
  '--target=wasm32-wasip1','-O2','-std=c11','-DSOFTFLOAT_FAST_INT64','-DINLINE_LEVEL=5',
  '-c',resolve(root,'th08_web/cpp/third_party/softfloat.c'),'-o',object,
],{windowsHide:true,stdio:'inherit'});
execFileSync(compiler,[
  '--target=wasm32-wasip1','-O2','-std=c++17','-fno-exceptions','-fno-rtti','-ffp-contract=off',
  '-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1','-DTH_CAMERA_CACHE_TEST=1','-Wl,-z,stack-size=1048576',
  '-I'+resolve(root,'th08_web/cpp/game'),
  resolve(root,'th08_web/cpp/game/Arithmetic.cpp'),
  resolve(root,'th08_web/cpp/game/GameMath.cpp'),
  resolve(root,'th08_web/cpp/game/GraphicsMath.cpp'),
  resolve(root,'th08_web/cpp/game/Camera.cpp'),
  resolve(root,'portable/multiplayer/camera-cache-test.cpp'),object,'-o',wasm,
],{windowsHide:true,stdio:'inherit'});
const bytes=readFileSync(wasm);
const wasi=new WASI({version:'preview1',args:[],env:{},returnOnExit:true});
const {instance}=await WebAssembly.instantiate(bytes,{wasi_snapshot_preview1:wasi.wasiImport});
const code=wasi.start(instance);
const report={
  passed:code===0,
  scope:'WASI bitwise camera oracle, cache-key and fallback correctness; not a frame-time benchmark',
  oracle:'Independent pre-cache Camera formulas with unchanged low-level GraphicsMath helpers',
  arithmeticCombinations:24,
  wasmSha256:createHash('sha256').update(bytes).digest('hex'),
  exitCode:code,
};
writeFileSync(reportPath,JSON.stringify(report,null,2)+'\n');
if(code!==0)throw Error('Camera cache tests failed: '+code);
console.log(JSON.stringify(report,null,2));

