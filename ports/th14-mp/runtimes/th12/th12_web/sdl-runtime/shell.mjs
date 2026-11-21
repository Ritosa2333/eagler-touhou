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
const emit=(event,fields={})=>parent.postMessage({protocol,game,...navigation.fields,event,
 ...((event==='first-frame'||event==='room-menu')?{launchSerial}:{}),...fields},location.origin);
const $=s=>document.querySelector(s);
const crashReporter=globalThis.__eaglerCrashReporterV1;
let th14PageLeaving=false;
let replayViewer=null,roomRuntime=false,roomBuild=[];
let roomPreview=false,roomOwned=false,roomPresented=null,launchSerial=0;
const saveRoot=()=>roomRuntime?'/savesth11mp':'/savesth11';
let Module,core,app=0,launched=false,first=false,stopping=false,language=query.get('language')==='lang_zh-hans'?'chs':'jp',options={},music=true,musicMode='none';
const inputActive=()=>!!core&&(launched||roomPreview)&&!document.hidden&&!stopping;
const keyboard=createBrowserKeyboard({
 hostContext:{protocol,game,epoch:Number(query.get('runtimeEpoch')),origin:location.origin,ready:inputActive,reset:clearKeyboard},
 accept:code=>!!scanCodes[code],
 send:(code,down)=>{
  const captured=practiceKey({code},down);
  if(!down||!captured)core?.th11_key(scanCodes[code],+down);
 },
});
function clearKeyboard(){keyboard.clear();core?.th14_practice_state?.(9,0);core?.th11_keys_clear();}
let frames=0,lastHealth=0,lastFrame=0,maxGap=0,lastPresented=0,saveTimer=null,storageSync=Promise.resolve();
const cancelTouches=bindOutsideTouches(document,canvas,()=>core,()=>launched&&options.touchEnabled);
const error=reason=>{
 const message=reason?.stack||String(reason);
 console.error(reason);
 crashReporter?.fatal(reason,{source:'th14-runtime'});
 const node=$('#error');if(node){
  if(crashReporter?.report())node.setAttribute('data-crash-reported','');
  node.textContent=crashReporter?.report()?'游戏已中断，请查看错误报告。':message;
 }
 emit('error',{message,error:message});
};
const u32=(ptr,count)=>new Int32Array(Module.HEAPU8.buffer,ptr,count);
const sync=(populate=false)=>{const current=storageSync.then(()=>new Promise((r,j)=>Module.FS.syncfs(populate,e=>e?j(e):r())));storageSync=current.catch(()=>{});return current;};
const coreError=()=>{const p=core.th11_error(),end=Module.HEAPU8.indexOf(0,p);return new TextDecoder().decode(Module.HEAPU8.slice(p,end<0?p+256:end));};
async function callCore(name,...args){
 const call=()=>Module.ccall?Module.ccall(name,'number',args.map(()=> 'number'),args,{async:true}):core[name](...args);
 const runtime=Module.th14AsyncRuntime??Module.th14WebGpuRuntime;return runtime?runtime.run(call):call();
}
const save=async()=>{if((launched||roomPreview)&&!await callCore('th11_save_scores'))throw Error(coreError());return sync(false);};
function buildWords(hash){
 if(!/^[a-f\d]{64}$/i.test(hash??''))throw Error('TH14 多人运行包身份无效');
 return Array.from({length:4},(_,i)=>parseInt(hash.slice(i*8,i*8+8),16));
}
async function openRoom(){
 const o=options,count=o.netplayPlayerCount,watch=o.netplaySpectator===true,seat=watch?0:o.netplayPlayer;
 const mode=o.netplayAdonisMode??0,delay=o.netplayInputDelay??0,prediction=o.netplayPredictionLimit??(mode===0?12:0);
 const relay=new URL(o.netplayUrl),room=relay.searchParams.get('room'),run=relay.searchParams.get('run');
 if(!['ws:','wss:'].includes(relay.protocol)||relay.username||relay.password||relay.hash||
    !room?.startsWith('th14mp-')||!/^[1-9]\d*$/.test(run??'')||![2,3].includes(count)||
    !Number.isInteger(seat)||seat<0||seat>=count||![0,1].includes(mode)||prediction!==(mode===0?12:0)||o.netplayInputDelayAuto||
    (o.netplayChallengeMode!==undefined&&typeof o.netplayChallengeMode!=='boolean')||
    (o.netplayChallengeKeepBullets!==undefined&&typeof o.netplayChallengeKeepBullets!=='boolean')||
    (o.netplayChallengeKeepBullets===true&&o.netplayChallengeMode!==true)||
    !Number.isInteger(o.netplaySeed)||o.netplaySeed<0||o.netplaySeed>65535||
    !Number.isInteger(o.netplayDifficulty)||o.netplayDifficulty<0||o.netplayDifficulty>4||
    !Number.isInteger(delay)||delay<0||delay>(mode===0?8:9)||
    !Array.isArray(o.netplayLoadouts)||o.netplayLoadouts.length!==count||
    o.netplayLoadouts.some(p=>!p||![0,1,2].includes(p.character)||![0,1].includes(p.shot))||
    watch&&(typeof o.netplaySpectatorId!=='string'||!o.netplaySpectatorId))
  throw Error('TH14 房间配置无效');
 const id=new DataView(await crypto.subtle.digest('SHA-256',new TextEncoder().encode('th14mp:'+room+':'+run)));
 const words=[2,count,seat,o.netplayDifficulty,o.netplaySeed,id.getUint32(0,true),id.getUint32(4,true)||1,delay];
 for(let i=0;i<3;++i)words.push(o.netplayLoadouts[i]?.character??0,o.netplayLoadouts[i]?.shot??0);
 words.push(...roomBuild,prediction,+(o.netplayChallengeMode===true)+2*(o.netplayChallengeKeepBullets===true));
 Module.eaglerOptions=o;
 const url=new TextEncoder().encode(relay.href+'\0'),viewer=new TextEncoder().encode((watch?o.netplaySpectatorId:'')+'\0');
 const pointers=[core.malloc(words.length*4),core.malloc(url.length),core.malloc(viewer.length)];
 try{
  if(pointers.some(p=>!p))throw Error('TH14 房间内存不足');
  Module.HEAPU32.set(words,pointers[0]>>>2);Module.HEAPU8.set(url,pointers[1]);Module.HEAPU8.set(viewer,pointers[2]);
  if(!await callCore('th14_room_open',pointers[0],words.length,pointers[1],pointers[2]))throw Error('TH14 房间连接失败');
 }finally{pointers.forEach(p=>core.free(p));}
 globalThis.__eaglerNetplayLanActive=true;
 globalThis.__eaglerNetplaySpectator=watch;
 globalThis.__eaglerNetplayInputDelayFrames=delay;
}
async function roomReplayNative(name,bytes){
 const p=core.malloc(bytes.length),b=core.malloc(16);
 try{
  if(!p||!b)throw Error('录像内存不足');
  Module.HEAPU8.set(bytes,p);Module.HEAPU32.set(roomBuild,b>>>2);
  return await callCore(name,p,bytes.length,b);
 }finally{core.free(p);core.free(b);}
}
let spectatorReport=null;
async function pollSpectator(){
 if(!roomRuntime||!launched||!options.netplaySpectator)return;
 const p=await callCore('th14_room_spectator_state');
 if(!p)return;
 const end=Module.HEAPU8.indexOf(0,p);
 if(end<=p||end-p>131072)throw Error('观战时间轴无效');
 spectatorReport=JSON.parse(new TextDecoder().decode(Module.HEAPU8.slice(p,end)));
 emit('runtime-info',{spectatorPlayback:spectatorReport});
}
async function spectatorCommand(m){
 if(!spectatorReport||m.run!==spectatorReport.run||!['pause','play','seek','live'].includes(m.action)||
    (m.action==='seek'&&(!Number.isSafeInteger(m.frame)||m.frame<spectatorReport.min||m.frame>spectatorReport.edge)))
  throw Error('观战操作无效');
 const bytes=new TextEncoder().encode(m.run+'\0'+m.action+'\0');
 const p=core.malloc(bytes.length);
 if(!p)throw Error('观战内存不足');
 try{
  Module.HEAPU8.set(bytes,p);
  if(!await callCore('th14_room_spectator_control',p,p+m.run.length+1,m.frame??0))throw Error('观战操作失败');
 }finally{core.free(p);}
 await pollSpectator();return {};
}
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
function apply(){applyTouchOptions(core,options);core.th14_always_hitbox?.(+(options.alwaysHitbox===true));core.th11_music_enabled(+music);}
async function resumeForegroundAudio(forcePause=false){
 if(replayViewer?.state&&(replayViewer.state.paused||replayViewer.state.seeking||replayViewer.state.ended)){await replayViewer.foreground();return false;}
 if(!Module||!core||!(launched||roomPreview)||document.hidden)return false;
 if(forcePause)core.sdl_loop_pause(1);
 return resumeRuntimeAudio(Module,core,()=>!!core&&(launched||roomPreview)&&!document.hidden);
}
async function closeAudio(){
 // The launcher owns the shared context. SDL owns and must disconnect its
 // stream, but closing that stream must not close the launcher's context.
 const close=()=>{const s=Module.SDL3,context=s?.audioContext,borrowed=parent!==window&&context===parent.__touhouAudioContext;
  if(borrowed)s.audioContext=undefined;
  try{core.th11_audio_close();}finally{if(borrowed)s.audioContext=context;}};
 const runtime=Module.th14AsyncRuntime??Module.th14WebGpuRuntime;if(runtime)await runtime.run(close);else close();
}
async function stop(){
 if(stopping)return;
 stopping=true;core?.th14_practice_state?.(10,0);clearKeyboard();
 try{
  if(roomOwned&&!options.replayViewer){await returnToRoom();emit('room-menu');return;}
  core.th11_loop_stop();if(replayViewer)await replayViewer.close();await save();
  if(roomRuntime)await callCore('th14_room_close');
  await closeAudio();launched=false;emit('exit',{code:0,status:'success'});
 }finally{stopping=false;}
}
function path(value){const name=String(value).replaceAll('\\','/').toLowerCase().replace(/^\/savesth11\//,'').replace(/^\//,'');if(roomRuntime){const path=name.replace(/^savesth11mp\//,'');if(!/^replay\/th11mp_(?:\d{2}|ud[0-9a-f]{4})\.rpyx$/.test(path))throw Error('存档路径无效');return path;}if(!/^(?:scoreth11\.dat|th11\.cfg|replay\/th11_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?)$/.test(name))throw Error('存档路径无效');return name;}
async function initializeGame(){
 if(!await callCore('th11_initialize'))throw Error(coreError());
 const runtime=Module.th14AsyncRuntime??Module.th14WebGpuRuntime;
 if(runtime){core=runtime.bindCore(core);window.core=core;}
 if(core.th11_phase()===4&&!await callCore('th11_return_title'))throw Error(coreError());
}
async function presentRoom(command){
 if(roomPreview)return {presented:true};
 const presented=new Promise((resolve,reject)=>{roomPresented={resolve,reject};});
 void presented.catch(()=>{});
 try{
  if(!await callCore(command))throw Error(coreError()||'无法打开房间标题');
  roomPreview=true;
  core.th11_loop_start();
  void resumeForegroundAudio();
  await presented;
  return {presented:true};
 }catch(e){roomPreview=false;throw e;}
 finally{roomPresented=null;}
}
async function returnToRoom(){
 await save();
 clearKeyboard();cancelTouches();core.th11_loop_stop();
 launched=false;roomPreview=false;
 globalThis.__eaglerNetplayLanActive=false;globalThis.__eaglerNetplaySpectator=false;
 spectatorReport=null;
 const result=await presentRoom('th14_room_return');
 await save();
 return result;
}
async function launch(){
 if(launched)return;clearKeyboard();
 const previewCheck=roomPreview&&options.netplayMode!=='lan'&&!options.replayViewer;
 if(roomPreview){
  if(!previewCheck)core.th11_loop_stop();
  await save();
 }else await initializeGame();
 if(options.netplayMode==='lan'){
  if(!roomRuntime)throw Error('请使用 TH14 多人运行包');
  await openRoom();
 }else if(roomRuntime&&!previewCheck&&!options.replayViewer&&!options.multiplayerPreflight)throw Error('请先加入 TH14 房间');
 roomPreview=false;launched=true;apply();first=false;lastPresented=0;lastHealth=performance.now();lastFrame=0;frames=0;maxGap=0;
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
function roomPlayback(){
 if(replayViewer)return replayViewer;
 let state=null,owner=null,timer=null;
 const report=()=>{if(state)emit('runtime-info',{playback:{...state}});return state;};
 const read=async value=>{
  const target=path(value),bytes=Module.FS.readFile(saveRoot()+'/'+target);
  if(!await roomReplayNative('th14_room_replay_validate',bytes))throw Error('TH14 多人录像与运行包不兼容');
  const data=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength),total=data.getUint32(32,true);
  const stage=bytes[43]===4?7:1;
  return {path:target,bytes,total,stage,description:{character:bytes[48],shot:bytes[49],difficulty:bytes[43],stages:[{id:stage,frames:total}]}};
 };
 const poll=()=>{
  if(!state)return;
  state.frame=core.th14_room_frame();state.ended=!!core.th14_room_finished();
  if(state.ended)state.paused=true;
  report();
 };
 const open=async file=>{
  await callCore('th14_room_close');
  if(!await roomReplayNative('th14_room_replay_open',file.bytes))throw Error('TH14 多人录像启动失败');
  owner=file;state={kind:'replay',path:file.path,description:file.description,stage:file.stage,
   left:file.description.character,right:file.description.shot,difficulty:file.description.difficulty,
   frame:0,startFrame:0,total:file.total,seeking:false,target:null,rate:1,ended:false,paused:false};
  if(timer===null)timer=setInterval(poll,250);
 };
 replayViewer={
  get state(){return state;},
  async foreground(){},
  async close(){if(timer!==null)clearInterval(timer);timer=null;state=null;owner=null;},
  async command(m){
   if(m.action==='inspect')return {description:(await read(m.path)).description};
   if(m.action==='open')await open(await read(m.path));
   else if(!state)throw Error('没有正在播放的录像');
   else if(m.action==='pause'||m.action==='play'){
    if(state.ended&&m.action==='play')await open(owner);
    state.paused=m.action==='pause';await callCore('th14_room_replay_pause',+state.paused);
   }else throw Error('不支持的录像操作');
   return {playback:report()};
  }
 };
 return replayViewer;
}
async function command(m){switch(m.command){
 case 'configure':{
  if(launched)throw Error('不能配置正在运行的游戏');
  const nextLanguage=m.language==='lang_zh-hans'?'chs':'jp';
  if(roomPreview&&(m.runtimePack||nextLanguage!==language))throw Error('请重新加载游戏以切换语言');
  language=nextLanguage;options=normalizeOptions(m.options);music=m.music!=='none';musicMode=m.music;
  await installResources(m.sharedResources);await installResources(m.runtimeResources);await installResources(m.resources);
  if(m.runtimePack)await installRuntimePack(m.runtimePack);
  configurePractice();apply();return {};
 }
 case 'prepare-room':
  if(!roomRuntime||launched||options.netplayMode==='lan')throw Error('请先配置多人游戏');
  roomOwned=true;
  if(!roomPreview)await initializeGame();
  return presentRoom('th14_room_preview');
 case 'return-room':
  if(!roomRuntime)throw Error('请使用多人运行包');
  return returnToRoom();
 case 'resources':await installResources(m.resources);return {};
 case 'keyboard':if(inputActive())keyboard.event(m,!!m.down,'hosted');return {};
 case 'keyboard-clear':clearKeyboard();return {};
 case 'touch-cancel':cancelTouches();return {};
 case 'direct-touch':directTouch(core,canvas,m,{width:innerWidth,height:innerHeight});return {};
 case 'touch-controls':practiceTouchControls(m);touchControls(core,options,m);return {};
 case 'launch':launchSerial=Number(m.launchSerial)||0;await launch();return {};
 case 'playback':if(game!=='th14')throw Error('不支持的录像操作');return (roomRuntime?roomPlayback():replayPlayback()).command(m);
 case 'spectator-playback':return spectatorCommand(m);
 case 'sync':await save();return {};
 case 'list':{const files=[];for(const dir of ['','/replay'])for(const name of Module.FS.readdir(saveRoot()+dir)){const n=(dir+'/'+name).replace(/^\//,'');try{path(n);}catch{continue;}const s=Module.FS.stat(saveRoot()+'/'+n);if(Module.FS.isFile(s.mode)){files.push({path:n,size:s.size});}}return {files};}
 case 'read':return {bytes:Array.from(Module.FS.readFile(saveRoot()+'/'+path(m.path)))};
 case 'write':{
  if(launched&&!options.replayViewer)throw Error('请先退出游戏');const target=path(m.path),bytes=new Uint8Array(m.bytes||[]);
  if(!bytes.length||bytes.length>64*1024*1024)throw Error('文件大小无效');
  if(roomRuntime){
   if(!await roomReplayNative('th14_room_replay_validate',bytes))throw Error('TH14 多人录像与运行包不兼容');
   Module.FS.writeFile(saveRoot()+'/'+target,bytes);await sync(false);return {};
  }
  const p=core.malloc(bytes.length);if(!p)throw Error('文件导入内存不足');let valid=false;try{Module.HEAPU8.set(bytes,p);valid=!!await callCore('th11_validate_file',target==='scoreth11.dat'?0:target==='th11.cfg'?2:1,p,bytes.length);}finally{core.free(p);}
  if(!valid)throw Error('文件不是有效的地灵殿存档或录像');
  if(target.startsWith('replay/')&&!validateMotionReplay(bytes))throw Error('录像数据无效');
  Module.FS.writeFile('/savesth11/'+target,bytes);await sync(false);return {};
 }
 case 'remove':{if(launched&&!options.replayViewer)throw Error('请先退出游戏');Module.FS.unlink(saveRoot()+'/'+path(m.path));await sync(false);return {};}
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
 const down=name==='keydown';
 if(!inputActive()||(down&&(event.altKey||event.metaKey)))return;
 if(keyboard.mapped()){
  if(keyboard.mapNative(event,down)){event.preventDefault();event.stopImmediatePropagation();}
 }else if(keyboard.event(event,down)){event.preventDefault();event.stopPropagation();}
},{capture:true});
// A mobile browser may terminate a hidden page before pagehide's IDB callback.
// Persist during play as well, with writes serialized by the sync chain.
setInterval(()=>{if((launched||roomPreview)&&!document.hidden&&core)try{queue=queue.then(()=>save()).catch(error);}catch(e){error(e);}},30000);
setInterval(()=>{void pollSpectator().catch(error);},300);
const startupAbort=new AbortController();
window.addEventListener('pagehide',event=>{if(!event.persisted)startupAbort.abort();},{once:true});
const initialized=(async()=>{
 let audioContext;try{audioContext=parent.__touhouAudioContext;}catch{}
 Module=await createModule({canvas,noInitialRun:true,resetBrowserKeyboard:()=>keyboard.clear(),...(audioContext?{SDL3:{audioContext}}:{}),print:console.log,printErr:console.error,
  onAbort:reason=>crashReporter?.fatal(new Error(String(reason)),{source:'wasm-abort'}),
  instantiateWasm(imports,ready){return WebAssembly.instantiateStreaming(fetch('./th11-sdl.wasm'),imports).then(({instance,module})=>{core=instance.exports;ready(instance,module);return core;});}});
 if(core.th14_audio_levels)Module.eaglerSetAudioLevels=(music,effects)=>core.th14_audio_levels(music,effects);
 window.Module=Module;window.FS=Module.FS;window.core=core;
 crashReporter?.bindCore(Module,{build:runtimeBuild,autoSample:false,hookAbort:false});
 Module.onWebGpuError=error;
 Module.onAsyncError=error;
 if(typeof Module._th14_async_frame==='function'){
  const {Th14AsyncRuntime}=await import('./Th14AsyncRuntime.mjs');
  Module.th14AsyncRuntime=new Th14AsyncRuntime(Module);
 }
 observeMusicWrites(Module,core,game);
 roomRuntime=typeof core.th14_room_open==='function';
 if(roomRuntime)roomBuild=buildWords(runtimeBuild.sha256);
 Module.FS.mkdirTree(saveRoot());Module.FS.mount(Module.IDBFS,{},saveRoot());await sync(true);
 Module.FS.mkdirTree(saveRoot()+'/replay');Module.FS.symlink(saveRoot(),'/save');
 await mountData();
 let last=performance.now(),ticks=0;
 // TH14 normal menu retirement stops tick_once with phase=finished.
 // Complete host persistence/cleanup before classifying other false frames.
 Module.onGameFrame=(ok,ms,count)=>{
  if(roomPreview){
   if(!ok){const failure=Error(coreError());if(roomPresented)roomPresented.reject(failure);else error(failure);}
   else if(roomPresented&&core.th14_room_preview_ready()){$('#loading').textContent='';roomPresented.resolve();}
   return;
  }
  if(!ok){if(game==='th14'&&core.th11_phase()===4&&options.netplayMode!=='lan'){void stop().catch(error);return;}error(Error(coreError()));return;}ticks+=count;
  if(roomRuntime&&options.netplayMode==='lan'){
   const ptr=core.th14_room_rollback();
   if(ptr){
    const [frame,confirmed,corrections,replayedFrames,,prediction]=u32(ptr,6);
    globalThis.__eaglerNetplayLanFrame=options.netplaySpectator?core.th14_room_frame():frame;
    globalThis.__eaglerNetplayLanConfirmed=confirmed;
    globalThis.__eaglerNetplayLanRollback=corrections;
    globalThis.__eaglerNetplayLanResimulated=replayedFrames;
    globalThis.__eaglerNetplayRollback={enabled:prediction!==0,corrections,replayedFrames};
    globalThis.__th14PeerTransport=globalThis.__eaglerPeerTransport;
   }
  }
  if(!first&&(options.netplayMode!=='lan'||core.th14_room_frame?.()>0)){first=true;last=performance.now();ticks=0;emit('first-frame');}
  const now=performance.now();if(now-last>=1000){crashReporter?.sample();emit('frame-health',{fps:ticks*1000/(now-last),maxGapMs:ms});const a=u32(core.th11_audio_statistics(),9);emit('audio-health',{queuedMs:a[4]*1000/44100,minQueuedMs:0,backend:'script',underruns:a[3]});last=now;ticks=0;}
  if((options.netplayMode==='lan'?core.th14_room_finished?.():core.th11_phase()===4)&&!options.replayViewer&&!options.netplaySpectator)void stop().catch(error);
 };
 emit('ready',roomRuntime?{saveRoot:saveRoot(),roomPreview:true,roomReturn:true}:{});
})().catch(e=>{if((game==='th14'&&th14PageLeaving)||isSupersededRuntimeError(e)){console.debug('Runtime navigation superseded');return false;}error(e);throw e;});
async function mountData(){
 // eagler-touhou managed package: the retail archive arrives through the parent
 // (written to /th11.dat, which the C++ archive layer opens) and the baked font
 // tables through the sibling resources.json.
 await mountManagedData(Module,{game,parentWindow:parent,query,emit,signal:startupAbort.signal});
}
