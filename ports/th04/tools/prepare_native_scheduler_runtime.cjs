// Adapt a source-built full-Asyncify core for isolated scheduler QA.
// No deployed assets are changed by this tool.
const fs=require('node:fs'),path=require('node:path'),cp=require('node:child_process');
const root=path.resolve(__dirname,'..'),build=path.join(root,'build/native-scheduler'),out=path.join(build,'runtime');
fs.mkdirSync(out,{recursive:true});
let source=fs.readFileSync(path.join(build,'out-native/wdosbox-x-jspi.js'),'utf8');
if(source.includes('var Module = moduleArg')){
 const sdk=path.join(build,'toolchain/emsdk-4.0.15');
 const minified=cp.spawnSync(path.join(sdk,'node/22.16.0_64bit/bin/node.exe'),[path.join(sdk,'upstream/emscripten/tools/acorn-optimizer.mjs'),path.join(build,'out-native/wdosbox-x-jspi.js'),'--minify-whitespace'],{encoding:'utf8',maxBuffer:4*1024*1024});
 if(minified.error||minified.status)throw minified.error||Error(minified.stderr);
 source=minified.stdout;
}
// Keep native code and glue from the same source build. Replace only JSPI's
// host continuation implementation, as in the existing full-rollback builder.
const legacy=fs.readFileSync(path.join(root,'vendor-source/emulators-8.5.2/wdosbox-x.js'),'utf8');
const asyncStart=legacy.indexOf('var Asyncify='),asyncEnd=legacy.indexOf('var FS_createPath=',asyncStart);
source=source.slice(0,source.indexOf('var Asyncify='))+legacy.slice(asyncStart,asyncEnd)+source.slice(source.indexOf('var FS_createPath=',source.indexOf('var Asyncify=')));
const tableStart=source.indexOf('var getWasmTableEntry='),tableEnd=source.indexOf('var ___call_sighandler=',tableStart);
if([asyncStart,asyncEnd,tableStart,tableEnd].some(x=>x<0))throw Error('Continuation implementation anchors changed');
// Native callbacks are synchronous; only Asyncify's outer runtime export owns
// a saved continuation. Do not wrap callback table entries in JSPI promises.
source=source.slice(0,tableStart)+'var getWasmTableEntry=index=>wasmTable.get(index);'+source.slice(tableEnd);
function replace(a,b){if(source.split(a).length!==2)throw Error('Source build glue anchor changed: '+a.slice(0,100));source=source.replace(a,b);}
replace('var Asyncify=', 'var runAndAbortIfError=func=>{try{return func()}catch(e){abort(e)}};var Asyncify=');
replace('var Module=moduleArg;', 'var Module=moduleArg;const Date=Module.virtualDate||globalThis.Date;var _asyncify_start_unwind,_asyncify_stop_unwind,_asyncify_start_rewind,_asyncify_stop_rewind;');
replace('StackSize:4096','StackSize:131072');
replace('function assignWasmExports(wasmExports){','function assignWasmExports(wasmExports){_asyncify_start_unwind=wasmExports.asyncify_start_unwind;_asyncify_stop_unwind=wasmExports.asyncify_stop_unwind;_asyncify_start_rewind=wasmExports.asyncify_start_rewind;_asyncify_stop_rewind=wasmExports.asyncify_stop_rewind;');
replace('var _emscripten_get_now=()=>performance.now();', 'var _emscripten_get_now=()=>Module.virtualNow;');
const begin=source.indexOf('function __asyncjs__syncSleep('),end=source.indexOf('__asyncjs__syncSleep.sig=',begin);
if(begin<0||end<0)throw Error('syncSleep boundary changed');
source=source.slice(0,begin)+`function __asyncjs__syncSleep(ms,nonSkippable){Module.assertNativeCanSuspend();return Asyncify.handleSleep(wake=>{Module.pendingWake=wake;Module.wakeAt=Module.virtualNow+(nonSkippable?Math.max(1,ms):1);});}`+source.slice(end);
replace('var _emscripten_sleep=ms=>Asyncify.handleSleep(wakeUp=>safeSetTimeout(wakeUp,ms));','var _emscripten_sleep=ms=>{Module.assertNativeCanSuspend();return Asyncify.handleSleep(wake=>{if(Module.deterministic){Module.pendingWake=wake;Module.wakeAt=Module.virtualNow+Math.max(1,ms);}else safeSetTimeout(wake,ms);});};');
replace('function initTimeoutSyncSleep(){','function initTimeoutSyncSleep(){if(Module.deterministic){Module.alive=true;return true;}');
replace('function initMessageSyncSleep(worker){','function initMessageSyncSleep(worker){if(Module.deterministic)return initTimeoutSyncSleep();');
replace('Module["FS"]=FS;',`Module["FS"]=FS;Module.MEMFS=MEMFS;Module.TTY=TTY;Module.nativeExports=()=>wasmExports;
Module.assertNativeCanSuspend=()=>{if(wasmExports.thCpuState&&Module.HEAPU32[(wasmExports.thCpuState(0)>>>2)+3])throw Error('Suspend reached guarded synchronous CPU lane');};
Module.bindNativeScheduler=()=>{
 if(wasmExports.thCpuState){const cpu=wasmExports.thCpuState(0);Module.thDirtyMark?.(cpu,16);Module.HEAPU32[cpu>>>2]=Module.nativeCpu===false?0:1;}
 if(!Module.deterministic)return;
 const ptr=wasmExports.thSchedulerState();
 const now=Module.virtualNow,deadline=Module.targetNow;
 const read=offset=>new DataView(Module.HEAPU8.buffer).getFloat64(ptr+offset,true);
 const write=(offset,value)=>{Module.thDirtyMark?.(ptr+offset,8);new DataView(Module.HEAPU8.buffer).setFloat64(ptr+offset,value,true);};
 write(0,now);write(8,deadline);write(16,Module.nativeScheduling===false?1:2);
 Object.defineProperties(Module,{virtualNow:{configurable:true,get:()=>read(0),set:value=>write(0,value)},targetNow:{configurable:true,get:()=>read(8),set:value=>write(8,value)}});
 Module.nativeScheduler={pointer:ptr,mode:()=>read(16)};
};
Module.preRun=[...(Module.preRun||[]),()=>Module.bindNativeScheduler()];
Module.captureContinuation=()=>{if(Asyncify.state!==0||!Asyncify.currData||Asyncify.exportCallStack.length)throw Error('DOSBox-X is not at a suspended boundary');return {currData:Asyncify.currData,returnValue:Asyncify.handleSleepReturnValue,keepalive:runtimeKeepaliveCounter,pendingWake:Module.pendingWake,virtualNow:Module.virtualNow,targetNow:Module.targetNow,wakeAt:Module.wakeAt};};
Module.restoreContinuation=s=>{Asyncify.state=0;Asyncify.currData=s.currData;Asyncify.handleSleepReturnValue=s.returnValue;runtimeKeepaliveCounter=s.keepalive;Module.pendingWake=s.pendingWake;Module.virtualNow=s.virtualNow;Module.targetNow=s.targetNow;Module.wakeAt=s.wakeAt;};
Module.advanceNative=()=>{if(!Module.pendingWake)throw Error('DOSBox-X continuation missing');const wake=Module.pendingWake;Module.pendingWake=null;wake();};`);
source=require('./patch_frame_presenter.cjs').patch(source);
source+='\nexport default WDOSBOXXJSPI;\n';
fs.writeFileSync(path.join(out,'dosbox-rollback.mjs'),source);
const prepared=cp.spawnSync(process.execPath,[path.join(__dirname,'prepare_core_candidate.cjs'),'build/native-scheduler/full-asyncify.wasm','build/native-scheduler/runtime/wdosbox-x-rollback.wasm'],{encoding:'utf8'});
if(prepared.error||prepared.status)throw prepared.error||Error(prepared.stderr);
console.log(prepared.stdout.trim());
