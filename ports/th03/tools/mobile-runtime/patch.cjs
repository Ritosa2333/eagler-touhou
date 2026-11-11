const fs=require('node:fs');
const mode=process.argv[2];let source=fs.readFileSync(0,'utf8');
function replace(old,value){if(source.split(old).length!==2)throw Error('Adapter anchor changed: '+old);source=source.replace(old,value);}
const net=mode==='net',original=net?'np21-rollback.wasm':'np21.wasm',fast=net?'np21-rollback-mobile.wasm':'np21-mobile.wasm';
replace('var Module=moduleArg;',`var Module=moduleArg;const mobileNative=!Module.disableMobileOptimization&&!!WebAssembly.JSTag&&!!WebAssembly.Exception;const mobileWasm=mobileNative?'${fast}':'${original}';Module.mobileRuntime=mobileWasm;`);
replace(`var f="${original}"`,'var f=mobileWasm');
replace(`new URL("${original}",import.meta.url)`,'new URL(mobileWasm,import.meta.url)');
replace('var wasmImports={',(net?'':require('./present565.cjs').injection())+'var wasmImports={'+(net?'':'__present565:mobilePresent565,')+'__js_tag:WebAssembly.JSTag,__check_exception:e=>{if(e!==e+0)throw e;},');
// A WASM trap cannot be caught by the native EH bridge. Stop the instance
// instead of letting a damaged stack continue into the next emulated frame.
replace('var iterFunc=getWasmTableEntry(func);','var iterFunc=getWasmTableEntry(func);if(mobileNative){const nativeFrame=iterFunc;iterFunc=()=>{const sp=stackSave();try{return nativeFrame();}catch(e){stackRestore(sp);if(e instanceof WebAssembly.RuntimeError){ABORT=true;MainLoop.pause();}throw e;}};}');
if(!net){
  replace('181622:($0,$1,$2,$3)=>{','181622:($0,$1,$2,$3)=>{if(Module.nativeSoloAudio){openBufferedSoloAudio(Module,$0,$1,()=>dynCall("vi",$2,[$3]));return;}');
  source=require('./solo-audio.cjs').injection()+source;

  // Keep elapsed-time emulation; only yield to the browser after two frames.
  source=source.replaceAll('},fps,simulateInfiniteLoop)','},mobileNative?0:fps,simulateInfiniteLoop)').replaceAll('setMainLoop(iterFunc,fps,simulateInfiniteLoop)','setMainLoop(iterFunc,mobileNative?0:fps,simulateInfiniteLoop)');
  replace('if(interval==0)_emscripten_set_main_loop_timing(0,0);else _emscripten_set_main_loop_timing(1,interval)','if(mobileNative)_emscripten_set_main_loop_timing(1,1);else if(interval==0)_emscripten_set_main_loop_timing(0,0);else _emscripten_set_main_loop_timing(1,interval)');
}
process.stdout.write(source);
