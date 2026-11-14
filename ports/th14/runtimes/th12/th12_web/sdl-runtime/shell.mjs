// Platform shell for the upstream eagler-touhou/1 Launcher contract.
// Game construction, input, timing, rendering, text and sound belong to C++.
// Mirrors th10/th20 shell.mjs and imports the shared eagler-host transport.
import {createBrowserKeyboard} from './directory-keyboard.mjs';
import createModule from './th11-sdl.mjs';
import {scanCodes} from './keyboard.mjs';
import {validateMotionReplay} from './motion-replay.mjs';
import {runtimeEpochContract} from './runtime-epoch.mjs';
import {bindOutsideTouches,normalizeOptions,applyTouchOptions,touchControls,suspendRuntimeAudio,resumeRuntimeAudio,directTouch,installResources as installHostResources,observeMusicWrites,mountManagedData,isSupersededRuntimeError,createReplayViewerHostTh14} from './eagler-host.mjs';
const runtimeBuild=/*TH11_BUILD_INFO*/{version:'development-incomplete',completeGame:false};
const protocol='eagler-touhou/1',game='th11',query=new URLSearchParams(location.search),canvas=document.querySelector('canvas');
const navigation=runtimeEpochContract(game,query.get('runtimeEpoch'));
const emit=(event,fields={})=>parent.postMessage({protocol,game,...navigation.fields,event,...fields},location.origin);
const $=s=>document.querySelector(s);
let th14PageLeaving=false;
let replayViewer=null;
let Module,core,app=0,launched=false,first=false,stopping=false,language=query.get('language')==='lang_zh-hans'?'chs':'jp',options={},music=true,musicMode='none';
const keyboard=createBrowserKeyboard({
 accept:code=>!!scanCodes[code],
 send:(code,down)=>core?.th11_key(scanCodes[code],+down),
});
function clearKeyboard(){keyboard.clear();core?.th11_keys_clear();}
let frames=0,lastHealth=0,lastFrame=0,maxGap=0,lastPresented=0,saveTimer=null,storageSync=Promise.resolve();
const cancelTouches=bindOutsideTouches(document,canvas,()=>core,()=>launched&&options.touchEnabled);
const error=reason=>{const message=reason?.stack||String(reason);const node=$('#error');if(node)node.textContent=message;emit('error',{message,error:message});console.error(reason);};
const u32=(ptr,count)=>new Int32Array(Module.HEAPU8.buffer,ptr,count);
const sync=(populate=false)=>{const current=storageSync.then(()=>new Promise((r,j)=>Module.FS.syncfs(populate,e=>e?j(e):r())));storageSync=current.catch(()=>{});return current;};
const coreError=()=>{const p=core.th11_error(),end=Module.HEAPU8.indexOf(0,p);return new TextDecoder().decode(Module.HEAPU8.slice(p,end<0?p+256:end));};
async function callCore(name,...args){
 const call=()=>Module.ccall?Module.ccall(name,'number',args.map(()=> 'number'),args,{async:true}):core[name](...args);
 const runtime=Module.th14AsyncRuntime??Module.th14WebGpuRuntime;return runtime?runtime.run(call):call();
}
const save=async()=>{if(launched&&!await callCore('th11_save_scores'))throw Error(coreError());return sync(false);};
const practiceKeys={Tab:9,Enter:13,NumpadEnter:13,Escape:27,Backspace:8,Space:32,ShiftLeft:16,ShiftRight:16,ControlLeft:17,ControlRight:17,KeyA:65,KeyS:83,KeyD:68,KeyF:70,Backquote:192,ArrowLeft:37,ArrowUp:38,ArrowRight:39,ArrowDown:40,KeyU:85,KeyX:88,KeyZ:90,F1:112,F2:113,F3:114,F4:115,F5:116,F6:117,F7:118,F12:123};
function practiceKey(event,down){
 const key=practiceKeys[event.code];
 return key!==undefined&&!!core?.th14_practice_key?.(key,+down);
}
function configurePractice(){
 const locale=String(options.thpracLocale??'ja');
 return core.th14_practice_state?.(0,options.thpracEnabled?(locale.startsWith('zh')?1:locale.startsWith('en')?2:3):0);
}
function practiceTouchControls(message){
 const c=message.controls??message;
 core.th14_practice_state?.(5,(c.thpracMobilePanelsEnabled?1:0)|(c.thpracMobilePanelsVisible?2:0));
}
async function installResources(resources=[]){return installHostResources(Module,resources,{game,emit});}
// The Launcher has already verified the ZIP identity. Match TH10's Runtime
// manifest/path/size checks before exposing any file to the native game.
let runtimePackFiles=[];
function assertRuntimePackManifest(manifest,pack){
 if(manifest?.schema!=='eagler-touhou/thcrap-static-pack/1'||manifest.game!==game||
    manifest.language!==pack.language||typeof manifest.runtimeVersion!=='string'||
    !Array.isArray(manifest.files)||manifest.files.length>256)throw Error('Invalid TH14 language pack manifest');
 for(const file of manifest.files)
  if(typeof file?.path!=='string'||!file.path.startsWith('/thcrap/'+game+'/')||file.path.includes('\\')||file.path.includes('..')||
     !Number.isInteger(file.bytes)||file.bytes<0)throw Error('Invalid TH14 language pack file');
}
async function installRuntimePack(pack){
 if(launched)throw Error('Runtime resources cannot be changed after launch');
 if(typeof pack?.url!=='string'||typeof pack.language!=='string'||
    !Number.isInteger(pack.bytes)||pack.bytes<=0||!pack.manifest||!Array.isArray(pack.files))throw Error('Invalid TH14 language pack');
 if(new URL(pack.url,location.href).origin!==location.origin)throw Error('Cross-origin TH14 language pack');
 assertRuntimePackManifest(pack.manifest,pack);
 const expected=new Map(pack.manifest.files.map(file=>[file.path,file]));
 if(pack.files.length!==expected.size)throw Error('TH14 language pack file count mismatch');
 const verified=[];
 for(const file of pack.files){
  if(typeof file?.path!=='string'||!file.path.startsWith('/thcrap/'+game+'/')||file.path.includes('\\')||file.path.includes('..')||
     !(file.bytes instanceof Uint8Array))throw Error('Invalid TH14 language pack path');
  const declaration=expected.get(file.path);
  if(!declaration||file.bytes.length!==declaration.bytes)throw Error(file.path+': size mismatch');
  verified.push({path:file.path,bytes:file.bytes});
 }
 for(const path of runtimePackFiles){try{Module.FS.unlink(path);}catch{}}
 runtimePackFiles=[];
 for(const file of verified){
  Module.FS.mkdirTree(file.path.slice(0,file.path.lastIndexOf('/')));
  Module.FS.writeFile(file.path,file.bytes,{canOwn:true});runtimePackFiles.push(file.path);
 }
}
function apply(){applyTouchOptions(core,options);core.th11_music_enabled(+music);}
async function resumeForegroundAudio(forcePause=false){
 if(replayViewer?.state&&(replayViewer.state.paused||replayViewer.state.seeking||replayViewer.state.ended)){await replayViewer.foreground();return false;}
 if(!Module||!core||!launched||document.hidden)return false;
 if(forcePause)core.sdl_loop_pause(1);
 return resumeRuntimeAudio(Module,core,()=>!!core&&launched&&!document.hidden);
}
async function closeAudio(){
 // The launcher owns the shared context. SDL owns and must disconnect its
 // stream, but closing that stream must not close the launcher's context.
 const close=()=>{const s=Module.SDL3,context=s?.audioContext,borrowed=parent!==window&&context===parent.__touhouAudioContext;
  if(borrowed)s.audioContext=undefined;
  try{core.th11_audio_close();}finally{if(borrowed)s.audioContext=context;}};
 const runtime=Module.th14AsyncRuntime??Module.th14WebGpuRuntime;if(runtime)await runtime.run(close);else close();
}
async function stop(){if(stopping)return;stopping=true;core?.th14_practice_state?.(10,0);clearKeyboard();try{core.th11_loop_stop();if(replayViewer)await replayViewer.close();await save();await closeAudio();launched=false;emit('exit',{code:0,status:'success'});}finally{stopping=false;}}
function path(value){const name=String(value).replaceAll('\\','/').toLowerCase().replace(/^\/savesth11\//,'').replace(/^\//,'');if(!/^(?:scoreth11\.dat|th11\.cfg|replay\/th11_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?)$/.test(name))throw Error('存档路径无效');return name;}
async function launch(){
 if(launched)return;clearKeyboard();
 if(!await callCore('th11_initialize'))throw Error(coreError());
 const runtime=Module.th14AsyncRuntime??Module.th14WebGpuRuntime;
 if(runtime){core=runtime.bindCore(core);window.core=core;}
 if(core.th11_phase()===4&&!await callCore('th11_return_title'))throw Error(coreError());
 launched=true;apply();first=false;lastPresented=0;lastHealth=performance.now();lastFrame=0;frames=0;maxGap=0;
 const loading=$('#loading');if(loading)loading.textContent='';
 canvas.focus({preventScroll:true});core.sdl_loop_pause(1);if(!document.hidden)void resumeForegroundAudio();core.th11_loop_start();
 emit('runtime-info',{renderer:Module.th14WebGpuRuntime?'SDL3 / WebGPU / C++':'SDL3 / WebGL2 / C++',architecture:protocol,version:runtimeBuild.version});
}
function replayPlayback(){
 if(replayViewer)return replayViewer;
 const withBytes=async(bytes,name,...args)=>{
  const pointer=core.malloc(bytes.length);if(!pointer)throw Error('录像内存不足');
  try{Module.HEAPU8.set(bytes,pointer);return await callCore(name,pointer,bytes.length,...args);}finally{core.free(pointer);}
 };
 replayViewer=createReplayViewerHostTh14({
  read:value=>{const target=path(value);if(!target.startsWith('replay/'))throw Error('录像路径无效');return {path:target,bytes:Module.FS.readFile('/savesth11/'+target)};},
  describe:async bytes=>{
   const result=await withBytes(bytes,'th14_replay_description');if(!result)throw Error('录像数据无效');
   const end=Module.HEAPU8.indexOf(0,result);if(end<result)throw Error('录像信息无效');
   return JSON.parse(new TextDecoder().decode(new Uint8Array(Module.HEAPU8.subarray(result,end))));
  },
  open:async(bytes,stage)=>{
   await launch();const until=performance.now()+60000;
   while(!await callCore('th14_replay_open_ready')){
    if(performance.now()>until)throw Error('等待录像加载交接超时');
    await callCore('th11_loop_pause',0);await new Promise(resolve=>setTimeout(resolve,16));
   }
   await callCore('th11_loop_pause',1);if(!await withBytes(bytes,'th14_open_replay',stage))throw Error(coreError());
  },
  status:async()=>{const p=await callCore('th14_replay_status'),w=Array.from(u32(p,6));return {stage:w[0],frame:w[1],frames:w[2],ended:!!w[3],phase:w[4],active:!!w[5]};},
  step:(target,budget)=>callCore('th14_replay_seek_step',target,budget),
  rate:async value=>{if(!await callCore('th14_replay_rate',value))throw Error('录像倍率无效');},
  pause:async on=>{
   if(!launched)return;
   if(!on&&!document.hidden)await resumeRuntimeAudio(Module,core,()=>launched&&!document.hidden);
   await callCore('th11_loop_pause',on||document.hidden?1:0);
  },
  close:async()=>{if(core)await callCore('th14_replay_close');},
  visible:()=>!document.hidden&&!th14PageLeaving,
  emit:playback=>emit('runtime-info',{playback}),onError:error,
 });return replayViewer;
}
async function command(m){switch(m.command){
 case 'configure':if(launched)throw Error('不能配置正在运行的游戏');language=m.language==='lang_zh-hans'?'chs':'jp';options=normalizeOptions(m.options);music=m.music!=='none';musicMode=m.music;await installResources(m.sharedResources);await installResources(m.runtimeResources);await installResources(m.resources);if(m.runtimePack)await installRuntimePack(m.runtimePack);configurePractice();apply();return {};
 case 'resources':await installResources(m.resources);return {};
 case 'keyboard':if(launched&&!document.hidden&&!stopping){
  const down=!!m.down, captured=practiceKey(m,down);
  if(!down||!captured)keyboard.event(m,down,'hosted');
 }return {};
 case 'keyboard-clear':core?.th14_practice_state?.(9,0);clearKeyboard();return {};
 case 'touch-cancel':cancelTouches();return {};
 case 'direct-touch':directTouch(core,canvas,m,{width:innerWidth,height:innerHeight});return {};
 case 'touch-controls':practiceTouchControls(m);touchControls(core,options,m);return {};
 case 'launch':await launch();return {};
 case 'playback':if(game!=='th14')throw Error('不支持的录像操作');return replayPlayback().command(m);
 case 'sync':await save();return {};
 case 'list':{const files=[];for(const dir of ['','/replay'])for(const name of Module.FS.readdir('/savesth11'+dir)){const n=(dir+'/'+name).replace(/^\//,'');try{path(n);}catch{continue;}const s=Module.FS.stat('/savesth11/'+n);if(Module.FS.isFile(s.mode)){files.push({path:n,size:s.size});}}return {files};}
 case 'read':return {bytes:Array.from(Module.FS.readFile('/savesth11/'+path(m.path)))};
 case 'write':{
  const target=path(m.path),bytes=new Uint8Array(m.bytes||[]);
  // A new replay does not replace the active viewer's owned byte buffer.
  const viewerImport=game==='th14'&&options.replayViewer===true&&target.startsWith('replay/');
  if(launched&&!viewerImport)throw Error('请先退出游戏');
  if(launched&&Module.FS.analyzePath('/savesth11/'+target).exists)throw Error('录像文件已存在，请使用其他文件名');
  if(!bytes.length||bytes.length>64*1024*1024)throw Error('文件大小无效');
  const p=core.malloc(bytes.length);if(!p)throw Error('文件导入内存不足');let valid=false;try{Module.HEAPU8.set(bytes,p);valid=!!await callCore('th11_validate_file',target==='scoreth11.dat'?0:target==='th11.cfg'?2:1,p,bytes.length);}finally{core.free(p);}
  if(!valid)throw Error('文件不是有效的地灵殿存档或录像');
  if(target.startsWith('replay/')&&!validateMotionReplay(bytes))throw Error('录像数据无效');
  Module.FS.writeFile('/savesth11/'+target,bytes);await sync(false);return {};
 }
 case 'remove':{if(launched)throw Error('请先退出游戏');Module.FS.unlink('/savesth11/'+path(m.path));await sync(false);return {};}
 default:throw Error('不支持的操作');}}
let queue=Promise.resolve();
async function dispatchCommand(m){
 try{const result=await command(m);if(typeof m.request==='string')parent.postMessage({protocol,game,...navigation.fields,request:m.request,ok:true,...result},location.origin);}
 catch(e){if(typeof m.request==='string')parent.postMessage({protocol,game,...navigation.fields,request:m.request,ok:false,error:String(e),errno:e?.errno},location.origin);else error(e);}
}
window.addEventListener('message',event=>{
 const m=event.data;if(event.source!==parent||event.origin!==location.origin||m?.protocol!==protocol||m.game!==game||!navigation.accepts(m)||typeof m.command!=='string')return;
 if(m.command==='keyboard'||m.command==='keyboard-clear'){void dispatchCommand(m);return;}
 queue=queue.then(async()=>{if(await initialized===false)return;await dispatchCommand(m);}).catch(error);
});
document.addEventListener('visibilitychange',()=>{if(!core||!launched)return;clearKeyboard();cancelTouches();if(document.hidden){suspendRuntimeAudio(Module,core);queue=queue.then(save).catch(error);}else void resumeForegroundAudio(true);});
canvas.addEventListener('wheel',event=>{
 if(core?.th14_practice_wheel?.(-Math.sign(event.deltaY)))event.preventDefault();
},{passive:false});
window.addEventListener('blur',()=>{if(core){core?.th14_practice_state?.(9,0);clearKeyboard();cancelTouches();}});
window.addEventListener('pagehide',()=>{if(game==='th14')th14PageLeaving=true;clearKeyboard();cancelTouches();if(core&&launched){suspendRuntimeAudio(Module,core);void save().catch(console.error);}});
window.addEventListener('pageshow',()=>{if(game==='th14')th14PageLeaving=false;if(core&&launched&&!document.hidden)void resumeForegroundAudio(true);});
canvas.addEventListener('webglcontextlost',event=>{event.preventDefault();clearKeyboard();core?.sdl_loop_pause(1);error('图形环境已失效，请退出后重新开始。');});
for(const name of ['pointerdown','keydown'])window.addEventListener(name,()=>{if(Module?.SDL3?.audioContext?.state!=='running')void resumeForegroundAudio(true);},{capture:true});
for(const name of ['keydown','keyup'])window.addEventListener(name,event=>{
 if(!core||!launched||stopping||document.hidden)return;
 const down=name==='keydown', captured=practiceKey(event,down);
 if(captured)event.preventDefault();
 // A key pressed by the native menu may be released over the practice panel.
 if(!down||!captured)keyboard.event(event,down);
},{capture:true});
// A mobile browser may terminate a hidden page before pagehide's IDB callback.
// Persist during play as well, with writes serialized by the sync chain.
setInterval(()=>{if(launched&&!document.hidden&&core)try{queue=queue.then(()=>save()).catch(error);}catch(e){error(e);}},30000);
const initialized=(async()=>{
 let audioContext;try{audioContext=parent.__touhouAudioContext;}catch{}
 Module=await createModule({canvas,noInitialRun:true,resetBrowserKeyboard:()=>keyboard.clear(),...(audioContext?{SDL3:{audioContext}}:{}),print:console.log,printErr:console.error,
  instantiateWasm(imports,ready){return WebAssembly.instantiateStreaming(fetch('./th11-sdl.wasm'),imports).then(({instance,module})=>{core=instance.exports;ready(instance,module);return core;});}});
 window.Module=Module;window.FS=Module.FS;window.core=core;
 Module.onWebGpuError=error;
 Module.onAsyncError=error;
 if(typeof Module._th14_async_frame==='function'){
  const {Th14AsyncRuntime}=await import('./Th14AsyncRuntime.mjs');
  Module.th14AsyncRuntime=new Th14AsyncRuntime(Module);
 }
 observeMusicWrites(Module,core,game);
 Module.FS.mkdirTree('/savesth11');Module.FS.mount(Module.IDBFS,{},'/savesth11');await sync(true);
 Module.FS.mkdirTree('/savesth11/replay');Module.FS.symlink('/savesth11','/save');
 await mountData();
 let last=performance.now(),ticks=0;
 // TH14 normal menu retirement stops tick_once with phase=finished.
 // Complete host persistence/cleanup before classifying other false frames.
 Module.onGameFrame=(ok,ms,count)=>{if(!ok){if(game==='th14'&&core.th11_phase()===4){void stop().catch(error);return;}error(Error(coreError()));return;}ticks+=count;if(!first){first=true;last=performance.now();ticks=0;emit('first-frame');}const now=performance.now();if(now-last>=1000){emit('frame-health',{fps:ticks*1000/(now-last),maxGapMs:ms});const a=u32(core.th11_audio_statistics(),9);emit('audio-health',{queuedMs:a[4]*1000/44100,minQueuedMs:0,backend:'script',underruns:a[3]});last=now;ticks=0;}if(core.th11_phase()===4)void stop().catch(error);};
 emit('ready');
})().catch(e=>{if((game==='th14'&&th14PageLeaving)||isSupersededRuntimeError(e)){console.debug('Runtime navigation superseded');return false;}error(e);throw e;});
async function mountData(){
 // eagler-touhou managed package: the retail archive arrives through the parent
 // (written to /th11.dat, which the C++ archive layer opens) and the baked font
 // tables through the sibling resources.json.
 await mountManagedData(Module,{game,parentWindow:parent,query,emit});
}
