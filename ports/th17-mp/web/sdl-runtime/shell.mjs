import {createBrowserKeyboard} from './directory-keyboard.mjs';
import {createDirectInputBridge} from './direct-input-bridge.mjs';
import createModule from './th17.mjs';
import {scanCodes} from './keyboard.mjs';
import {gpuTiming} from './gpu-timing.mjs';
import {createNativeLocalReplay,listLocalReplayFiles} from './local-replay-native.mjs';
import {createSpectatorPlaybackBridge} from './spectator-playback.mjs';
const runtimeBuild=/*TH17_BUILD_INFO*/{version:'development-incomplete',completeGame:false};
const game='th17',protocol='eagler-touhou/1',query=new URLSearchParams(location.search),canvas=document.querySelector('canvas'),$=s=>document.querySelector(s);
const epoch=Number(query.get('runtimeEpoch')),validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
let configured=false;
let roomRuntime=false;
let roomPreview=false,roomOwned=false,runtimeLanguage='ja',saveRoot='/savesth17',launchSerial=0;
let core,gpuMonitor,launched=false,first=false,stopping=false,options={},music=true,chain=Promise.resolve(),queue=Promise.resolve();
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...((event==='first-frame'||event==='room-menu')?{launchSerial}:{}),...fields},location.origin);
const inputActive=()=>(launched||roomPreview&&options.netplayMode!=='lan')&&!document.hidden&&!stopping;
const keyboard=createBrowserKeyboard({hostContext:{protocol,game,epoch,origin:location.origin,ready:inputActive,reset:clearKeyboard},accept:code=>!!scanCodes[code],send:(code,down)=>core?._th17_key(scanCodes[code],+down)});
function clearKeyboard(){invalidateTouchGeometry();keyboard.clear();core?._th17_keys_clear();}
const err=()=>{const p=core._th17_error(),end=core.HEAPU8.indexOf(0,p);return new TextDecoder().decode(core.HEAPU8.subarray(p,end<0?p+256:end));};
const fatal=e=>{const message=e?.message||String(e);$('#error').textContent=message;core?._th17_loop_pause(1);emit('error',{error:message});};
const sync=(populate=false)=>{const current=chain.then(()=>new Promise((r,j)=>core.FS.syncfs(populate,e=>e?j(e):r())));chain=current.catch(()=>{});return current;};
const save=()=>{if(roomRuntime&&launched&&!options.replayViewer&&!options.netplaySpectator&&!core._th17_mp_replay_save())throw Error(err());if(launched&&!core._th17_save_scores())throw Error(err());return sync();};
function closeAudio(){
 // The launcher owns the shared context. SDL owns and must disconnect its
 // stream, but closing that stream must not close the launcher's context.
 const s=core.SDL3,context=s?.audioContext,borrowed=parent!==window&&context===parent.__touhouAudioContext;
 if(borrowed)s.audioContext=undefined;
 try{core._th17_audio_close();}finally{if(borrowed)s.audioContext=context;}
}
async function stop(){
 if(stopping)return;
 stopping=true;
 try{
  await save();
  if(roomOwned&&!options.replayViewer){await command({command:'return-room'});emit('room-menu');return;}
  localReplay.close();spectatorPlayback.stop();core._th17_loop_stop();core._th17_room_close?.();
  gpuMonitor?.close();gpuMonitor=null;closeAudio();launched=false;emit('exit',{code:0,status:'success'});
 }finally{stopping=false;}
}
function path(value){
 const name=String(value).replaceAll('\\','/').toLowerCase().replace(/^\/savesth17(?:mp)?\//,'').replace(/^\//,'');
 if(!/^(?:scoreth17\.dat|th17\.cfg|replay\/(?:[^./\\\x00-\x1f][^/\\\x00-\x1f]{0,127}\/)?th17(?:mp)?_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?)$/.test(name))throw Error('存档路径无效');
 if(roomRuntime&&name.startsWith('replay/')&&!/^replay\/(?:[^/]+\/)?th17mp_/.test(name))throw Error('请导入鬼形兽多人录像');
 return name;
}
function apply(){core.eaglerOptions=options;core._th17_always_hitbox(+(options.alwaysHitbox===true));core._th17_touch_options(+!!options.touchEnabled,Math.max(0,['touch','touch-unlimited','joystick','joystick-free'].indexOf(options.touchMovementMode)),Math.max(100,Math.min(300,Number(options.touchSensitivity)||100))/100,+(options.touchFocusMode==='two-finger'),+!!options.doubleTapBombEnabled);core._th17_music_enabled(+music);}
const sharedFonts=['/msgothic.ttc','/unifont.otf'];
const fontNames=['cp932.bin'];
async function resource(r){
 if(!r||(!/^\/music\/(?:th17_\d{2}|th128_08)\.ogg$/.test(r.path)&&!fontNames.some(n=>r.path==='/fonts/'+n)&&!sharedFonts.includes(r.path)))throw Error('资源路径无效');
 const u=new URL(r.url,location.href);if(u.origin!==location.origin||!['http:','https:','blob:'].includes(u.protocol))throw Error('资源来源无效');
 const response=await fetch(u);if(!response.ok)throw Error('资源读取失败');const bytes=new Uint8Array(await response.arrayBuffer());
 const expected=r.bytes??r.size;if(bytes.length>64*1024*1024||(expected!=null&&bytes.length!==expected))throw Error('资源大小错误');
 core.FS.mkdirTree(r.path.slice(0,r.path.lastIndexOf('/'))||'/');core.FS.writeFile(r.path,bytes,{canOwn:true});
 emit('transfer',{mode:r.path.startsWith('/music/')?'ogg':'runtime',loaded:bytes.length,total:bytes.length,path:r.path});
}
const runtimeLocalePath='/thcrap/th17/runtime-language.txt';
let runtimePackFiles=[];
function mountLanguagePack(pack,language){
 if(language==='ja'&&!pack){
  for(const path of runtimePackFiles)core.FS.unlink(path);
  runtimePackFiles=[];
  return;
 }
 const manifest=pack?.manifest;
 if(typeof pack?.url!=='string'||!/^lang_[a-z0-9]+(?:-[a-z0-9]+)*$/i.test(language)||pack.language!==language||
    new URL(pack.url,location.href).origin!==location.origin||!Number.isSafeInteger(pack.bytes)||pack.bytes<=0||
    manifest?.schema!=='eagler-touhou/thcrap-static-pack/1'||manifest.game!==game||manifest.language!==language||
    typeof manifest.runtimeVersion!=='string'||!Array.isArray(manifest.files)||manifest.files.length>256||
    !Array.isArray(pack.files)||pack.files.length!==manifest.files.length)throw Error('TH17 语言包清单无效');
 const safe=path=>typeof path==='string'&&path.startsWith('/thcrap/th17/')&&path.length<=240&&
  !path.includes('\\')&&!path.includes('\0')&&path.split('/').slice(1).every(part=>part&&part!=='.'&&part!=='..');
 const expected=new Map();
 for(const file of manifest.files){
  if(!safe(file?.path)||file.path===runtimeLocalePath||expected.has(file.path)||!Number.isSafeInteger(file.bytes)||file.bytes<0||file.bytes>64*1024*1024)throw Error('TH17 语言包路径或大小无效');
  expected.set(file.path,file.bytes);
 }
 for(const file of pack.files){
  if(!(file?.bytes instanceof Uint8Array)||expected.get(file.path)!==file.bytes.length)throw Error('TH17 语言包文件与清单不符');
  expected.delete(file.path);
 }
 for(const path of runtimePackFiles)core.FS.unlink(path);
 runtimePackFiles=[];
 for(const file of pack.files){
  core.FS.mkdirTree(file.path.slice(0,file.path.lastIndexOf('/')));
  core.FS.writeFile(file.path,file.bytes,{canOwn:true});
  runtimePackFiles.push(file.path);
 }
 core.FS.mkdirTree('/thcrap/th17');
 core.FS.writeFile(runtimeLocalePath,language);
 runtimePackFiles.push(runtimeLocalePath);
}
function openReplay(value){const name=path(value).replace(/^replay\//,'');if(!/\.rpyx?$/.test(name))throw Error('录像路径无效');const bytes=new TextEncoder().encode(name+String.fromCharCode(0)),ptr=core._malloc(bytes.length);if(!ptr)throw Error('录像内存不足');try{core.HEAPU8.set(bytes,ptr);if(!core._th17_open_replay(ptr,0))throw Error('录像启动失败');}finally{core._free(ptr);}}
function initializeGame(){
 let scaleOption=query.get('renderScale');
 if(!scaleOption)try{scaleOption=new URLSearchParams(parent.location.search).get('renderScale');}catch{}
 const requested=Number(scaleOption);
 const scale=[options.renderScale,requested].find(value=>value===1||value===2)??2;
 core._th17_render_scale(scale);
 if(!core._th17_initialize())throw Error(err());
}
async function openRoom(){
 const o=options,loadouts=o.netplayLoadouts,count=o.netplayPlayerCount,spectator=o.netplaySpectator===true,seat=spectator?0:o.netplayPlayer;
 const relay=new URL(o.netplayUrl),room=relay.searchParams.get('room'),run=relay.searchParams.get('run');
 const delay=o.netplayInputDelay??0,mode=o.netplayAdonisMode??0;
 if(o.netplayMode!=='lan'||![0,1].includes(mode)||o.netplayInputDelayAuto||
    (o.netplayChallengeMode!==undefined&&typeof o.netplayChallengeMode!=='boolean')||
    o.replayPath||o.thpracEnabled||!['ws:','wss:'].includes(relay.protocol)||relay.username||relay.password||
    relay.hash||!room?.startsWith('th17mp-')||!/^[1-9]\d*$/.test(run??'')||
    ![2,3].includes(count)||!Number.isInteger(seat)||seat<0||seat>=count||
    !Number.isInteger(o.netplaySeed)||o.netplaySeed<0||o.netplaySeed>65535||
    !Number.isInteger(o.netplayDifficulty)||o.netplayDifficulty<0||o.netplayDifficulty>4||
    !Number.isInteger(delay)||delay<0||delay>9||(mode===0&&delay!==0)||
    (o.netplayPredictionLimit!==undefined&&o.netplayPredictionLimit!==(mode===0?12:0))||
    !Array.isArray(loadouts)||loadouts.length!==count||
    loadouts.some(p=>!p||![0,1,2].includes(p.character)||![0,1,2].includes(p.shot))||
    spectator&&(typeof o.netplaySpectatorId!=='string'||!o.netplaySpectatorId))
  throw Error('TH17 房间配置无效');
 const manifest=await(await fetch('./manifest.json')).json();
 const build=manifest.execution?.sha256;
 if(manifest.product!=='th17mp'||manifest.features?.multiplayer!==true||!/^[a-f\d]{64}$/i.test(build??''))
  throw Error('TH17 多人运行包身份无效');
 const digest=new DataView(await crypto.subtle.digest('SHA-256',new TextEncoder().encode('th17mp:'+room+':'+run)));
 const words=[3,count,seat,o.netplayDifficulty,o.netplaySeed,digest.getUint32(0,true),digest.getUint32(4,true)||1,delay];
 for(let i=0;i<3;i++)words.push(loadouts[i]?.character??0,loadouts[i]?.shot??0);
 for(let i=0;i<4;i++)words.push(parseInt(build.slice(i*8,i*8+8),16));
 words.push(+(mode===0),+(o.netplayChallengeMode===true));
 const url=new TextEncoder().encode(relay.href+'\0'),identity=new TextEncoder().encode((spectator?o.netplaySpectatorId:'')+'\0');
 const params=core._malloc(words.length*4),address=core._malloc(url.length),viewer=spectator?core._malloc(identity.length):0;
 try{
  if(!params||!address||spectator&&!viewer)throw Error('TH17 房间内存不足');
  core.HEAPU32.set(words,params>>>2);core.HEAPU8.set(url,address);
  if(viewer)core.HEAPU8.set(identity,viewer);
  if(!core._th17_room_open(params,words.length,address,viewer))throw Error('TH17 房间连接失败');
 }finally{core._free(viewer);core._free(address);core._free(params);}
 globalThis.__eaglerNetplayLanActive=true;globalThis.__eaglerNetplaySpectator=spectator;
 globalThis.__eaglerNetplayInputDelayFrames=delay;globalThis.__eaglerNetplayAdonisMode=mode;
}
function reportRoom(){
 if(!roomRuntime||options.netplayMode!=='lan'||options.netplaySpectator)return;
 const pointer=core._th17_room_rollback()>>>2,values=Array.from(core.HEAPU32.subarray(pointer,pointer+6));
 globalThis.__eaglerNetplayLanFrame=values[0];globalThis.__eaglerNetplayLanConfirmed=values[1];
 globalThis.__eaglerNetplayLanRollback=values[2];globalThis.__eaglerNetplayLanResimulated=values[3];
 globalThis.__eaglerNetplayRollback={enabled:options.netplayAdonisMode===0,corrections:values[2],replayedFrames:values[3]};
}
const spectatorPlayback=createSpectatorPlaybackBridge({
 active:()=>launched&&options.netplaySpectator===true,
 memory:()=>core.HEAPU8,state:()=>core._th17_mp_spectator_state(),
 control:(run,action,frame)=>core._th17_mp_spectator_control(run,action,frame),
 allocate:n=>core._malloc(n),deallocate:p=>core._free(p),emit,
 complete:(pointer,size)=>core._th17_mp_spectator_result(pointer,size),
});
const localReplay=createNativeLocalReplay({
 core:()=>core,module:()=>core,physical:value=>saveRoot+'/'+path(value),
 stopLoop:()=>{clearKeyboard();core._th17_loop_stop();},
 pauseAudio:paused=>core._th17_loop_pause(+paused),
 waitForAudio:false,
 deferPresentation:false,
 async openReplay(value,stage){
  if(!roomRuntime)throw Error('请使用多人录像运行包');
  core._th17_loop_stop();core._th17_room_close();initializeGame();
  const manifest=await(await fetch('./manifest.json')).json(),build=manifest.execution?.sha256;
  if(!/^[a-f\d]{64}$/i.test(build??''))throw Error('多人录像运行包身份无效');
  const bytes=core.FS.readFile(saveRoot+'/'+path(value));
  const pointer=core._malloc(bytes.length),identity=core._malloc(16);
  try{
   if(!pointer||!identity)throw Error('录像内存不足');
   core.HEAPU8.set(bytes,pointer);
   core.HEAPU32.set(Array.from({length:4},(_,i)=>parseInt(build.slice(i*8,i*8+8),16)),identity>>>2);
   if(!core._th17_mp_replay_load(pointer,bytes.length,identity,stage))throw Error(err()||'多人录像与运行包不兼容');
  }finally{core._free(identity);core._free(pointer);}
 },
 replayState:()=>{
  const pointer=core._eagler_replay_state()>>>2,values=Array.from(core.HEAPU32.subarray(pointer,pointer+6));
  return {frame:values[0],total:values[1],ended:!!values[2],stage:values[3],startStage:values[4],bootstrapTarget:values[5]};
 },
 onState:playback=>emit('runtime-info',{playback}),onError:fatal,
});
const liveInputCommands=new Set(['keyboard','keyboard-clear','touch-cancel','direct-touch','touch-controls']);
let cachedTouchGeometry=null;
function invalidateTouchGeometry(){cachedTouchGeometry=null;}
function touchGeometry(){
 if(!cachedTouchGeometry){
  const bounds=canvas.getBoundingClientRect();
  cachedTouchGeometry={left:bounds.left,top:bounds.top,width:bounds.width,height:bounds.height,
   viewportWidth:innerWidth,viewportHeight:innerHeight};
 }
 return cachedTouchGeometry;
}
for(const event of ['resize','scroll','fullscreenchange'])
 window.addEventListener(event,invalidateTouchGeometry,{capture:true,passive:true});
// The managed canvas is centered in a fixed viewport. A gesture keeps its
// geometry until the viewport or canvas changes; motion never forces layout.
new ResizeObserver(invalidateTouchGeometry).observe(canvas);
for(const event of ['resize','scroll'])
 window.visualViewport?.addEventListener(event,invalidateTouchGeometry,{passive:true});
function dispatchInput(m){
 if(!core)return false;
 switch(m.command){
case 'keyboard':if(typeof m.code!=='string'||typeof m.down!=='boolean')throw Error('键盘参数无效');if(inputActive())keyboard.event(m,m.down,'hosted');return true;
case 'keyboard-clear':clearKeyboard();return true;
case 'touch-cancel':invalidateTouchGeometry();core._th17_touch_cancel();return true;
case 'direct-touch':{if(m.type==='cancel'){invalidateTouchGeometry();core._th17_touch_cancel();return true;}if(!['down','move','up'].includes(m.type)||!Number.isSafeInteger(m.id)||!Number.isFinite(m.x)||!Number.isFinite(m.y))throw Error('触点参数无效');if(!inputActive())return true;if(m.type==='down')invalidateTouchGeometry();const b=touchGeometry();if(b.width&&b.height)core._th17_touch(({down:0,move:1,up:2})[m.type],m.id,(m.x*b.viewportWidth-b.left)/b.width,(m.y*b.viewportHeight-b.top)/b.height);return true;}
case 'touch-controls':{const t=m.controls||m;core.eaglerControls=t;if(t.touchSensitivity!=null){if(!(Number.isFinite(t.touchSensitivity)&&t.touchSensitivity>=100&&t.touchSensitivity<=300))throw Error('触控灵敏度无效');if(t.touchSensitivity!==options.touchSensitivity){options.touchSensitivity=t.touchSensitivity;apply();}}core._th17_touch_controls(+!!options.touchEnabled,+!!t.fireEnabled,+!!t.focusEnabled,t.bombSerial>>>0,t.escapeSerial>>>0);core._th17_touch_stick(Number(t.joystickX)||0,Number(t.joystickY)||0);return true;}
default:return false;
 }
}
if(validEpoch)Object.defineProperty(window,'__eaglerDirectInputBridge',{configurable:true,
 value:createDirectInputBridge({protocol,game,epoch,origin:location.origin,ready:()=>!!core,dispatch:dispatchInput})});
async function command(m){
 if(dispatchInput(m))return {};
 switch(m.command){
case 'configure':{
 if(launched)throw Error('游戏已启动');
 if(!['ogg','none'].includes(m.music))throw Error('音乐模式无效');
 const nextOptions=m.options||{},language=m.language||m.runtimePack?.language||'ja';
 if(nextOptions.touchSensitivity!=null&&!(Number.isFinite(nextOptions.touchSensitivity)&&nextOptions.touchSensitivity>=100&&nextOptions.touchSensitivity<=300))throw Error('触控灵敏度无效');
 if(roomPreview&&(language!==runtimeLanguage||m.runtimePack))throw Error('请重新加载游戏以切换语言');
 configured=false;
 for(const r of [...(m.sharedResources||[]),...(m.runtimeResources||[]),...(m.resources||[])])await resource(r);
 if(!roomPreview){mountLanguagePack(m.runtimePack,language);runtimeLanguage=language;}
 options=nextOptions;music=m.music!=='none';apply();configured=true;return {};
}
case 'prepare-room':
 roomOwned=true;
 if(!roomRuntime||!configured||launched||options.netplayMode==='lan')throw Error('请先配置多人游戏');
 if(!roomPreview){
  initializeGame();
  if(!core._th17_room_preview())throw Error(err()||'TH17 房间预览加载失败');
  roomPreview=true;$('#loading').textContent='';core._th17_loop_start();
 }
 return {presented:true};
case 'resources':for(const r of m.resources||[])await resource(r);return {};
case 'thprac-mouse':{
 if(!options.thpracEnabled||!['down','move','up'].includes(m.type)||![m.x,m.y].every(Number.isFinite))return {};
 const r=canvas.getBoundingClientRect(),scale=Math.min(r.width/640,r.height/480);
 if(scale>0)core._th17_thprac_mouse(m.type==='down'?1:m.type==='up'?2:0,(m.x-r.left-(r.width-640*scale)/2)/scale,(m.y-r.top-(r.height-480*scale)/2)/scale);
 return {};
}
case 'spectator-playback':return spectatorPlayback.command(m);
case 'playback':
 if(!launched||!roomRuntime||options.replayViewer!==true)throw Error('请先打开多人录像管理器');
 return localReplay.command(m);
case 'return-room':
 if(!roomRuntime)throw Error('请使用多人运行包');
 roomOwned=true;spectatorPlayback.stop();clearKeyboard();core._th17_loop_stop();
 if(!core._th17_room_return()||!core._th17_room_preview())throw Error(err()||'房间标题恢复失败');
 globalThis.__eaglerNetplayLanActive=false;
 roomPreview=true;launched=false;first=false;core._th17_loop_start();return {presented:true};
case 'launch':
 launchSerial=Number(m.launchSerial)||0;
 if(!configured)throw Error('请先配置游戏');
 if(!launched){
  const network=options.netplayMode==='lan';
  if(network&&!roomRuntime)throw Error('请使用 TH17 多人运行包');
  if(roomPreview)core._th17_loop_stop();else initializeGame();
  if(network)await openRoom();else if(!roomRuntime&&options.replayPath)openReplay(options.replayPath);
  launched=true;roomPreview=false;apply();first=false;$('#loading').textContent='';
  if(roomRuntime&&options.replayViewer){first=true;emit('first-frame');}else core._th17_loop_start();
  if(options.netplaySpectator)spectatorPlayback.start();
  emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++ · WEB '+runtimeBuild.version.split('-sdl3-')[0],architecture:protocol,...runtimeBuild});
 }
 return {};
case 'sync':await save();return {};
case 'list':return {files:listLocalReplayFiles(core.FS,saveRoot,path)};
case 'read':return {bytes:Array.from(core.FS.readFile(saveRoot+'/'+path(m.path)))};
case 'write':{
 if(launched&&!options.replayViewer)throw Error('请先退出游戏');const target=path(m.path),bytes=new Uint8Array(m.bytes||[]);
 if(!bytes.length||bytes.length>64*1024*1024)throw Error('文件大小无效');
 const p=core._malloc(bytes.length);if(!p)throw Error('文件导入内存不足');let valid=false;
 try{core.HEAPU8.set(bytes,p);valid=roomRuntime&&target.startsWith('replay/')?!!core._th17_mp_replay_validate(p,bytes.length):!!core._th17_validate_file(target==='scoreth17.dat'?0:target==='th17.cfg'?2:1,p,bytes.length);}finally{core._free(p);}
 if(!valid)throw Error('文件不是有效的鬼形兽存档或录像');
 const file=saveRoot+'/'+target;core.FS.mkdirTree(file.slice(0,file.lastIndexOf('/')));
 core.FS.writeFile(file,bytes);await sync();return {};
}
case 'remove':if(launched&&!options.replayViewer)throw Error('请先退出游戏');core.FS.unlink(saveRoot+'/'+path(m.path));await sync();return {};
default:throw Error('不支持的操作');}}
window.addEventListener('message',e=>{const m=e.data;if(e.source!==parent||e.origin!==location.origin||m?.protocol!==protocol||m.game!==game||!validEpoch||m.epoch!==epoch||typeof m.command!=='string')return;
 if(core&&liveInputCommands.has(m.command)){
  try{
   dispatchInput(m);
   if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:true},location.origin);
  }catch(error){
   if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:false,error:String(error)},location.origin);
   else fatal(error);
  }
  return;
 }
 queue=queue.then(async()=>{if(await initialized===false)return;try{const result=await command(m);if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:true,...result},location.origin);}catch(e){if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:false,error:String(e),errno:e?.errno},location.origin);else fatal(e);}}).catch(fatal);});
window.addEventListener('eagler-thprac-menu',e=>emit('thprac-menu',{open:!!e.detail?.open}));
document.addEventListener('visibilitychange',()=>{if(launched||roomPreview){clearKeyboard();cancelCanvasPointers();core._th17_loop_pause(+document.hidden);if(document.hidden&&launched)try{void save().catch(fatal);}catch(e){fatal(e);}}});
window.addEventListener('blur',()=>{clearKeyboard();cancelCanvasPointers();});
// Keyboard events in the child realm do not bubble to the launcher. SDL's
// canvas listener alone misses keys when the child BODY owns focus.
for(const event of ['keydown','keyup'])window.addEventListener(event,e=>{if(!inputActive()||e.altKey||e.metaKey)return;if(keyboard.mapNative(e,event==='keydown')){e.preventDefault();e.stopImmediatePropagation();return;}if(keyboard.event(e,event==='keydown')){e.preventDefault();e.stopPropagation();}},{capture:true});
for(const event of ['pointerdown','keydown'])window.addEventListener(event,()=>core?.SDL3?.audioContext?.resume().catch(()=>{}),{capture:true});
const canvasPointers=new Set();
function cancelCanvasPointers(){
 invalidateTouchGeometry();
 canvasPointers.clear();
 core?._th17_touch_cancel();
}
canvas.addEventListener('pointerdown',e=>{
 if(!inputActive()||!options.touchEnabled||e.pointerType==='mouse')return;
 e.preventDefault();
 canvasPointers.add(e.pointerId);
 invalidateTouchGeometry();
 try{canvas.setPointerCapture(e.pointerId);}catch(error){if(error.name!=='NotFoundError')throw error;}
 sendCanvasTouch(0,e);
});
function sendCanvasTouch(type,e){
 const bounds=touchGeometry();
 if(bounds.width&&bounds.height)core._th17_touch(type,e.pointerId,(e.clientX-bounds.left)/bounds.width,(e.clientY-bounds.top)/bounds.height);
}
for(const [name,type] of [['pointermove',1],['pointerup',2]])window.addEventListener(name,e=>{
 if(!canvasPointers.has(e.pointerId))return;
 e.preventDefault();
 if(type===2)canvasPointers.delete(e.pointerId);
 if(inputActive()&&options.touchEnabled)sendCanvasTouch(type,e);
}, {capture:true});
for(const event of ['pointercancel','lostpointercapture'])window.addEventListener(event,e=>{
 if(canvasPointers.has(e.pointerId))cancelCanvasPointers();
}, {capture:true});
canvas.addEventListener('webglcontextlost',e=>{e.preventDefault();fatal(Error('图形环境失效，请退出后重新开始。'));});
window.addEventListener('pagehide',()=>{if(launched||roomPreview){core._th17_loop_stop();if(launched)try{void save().catch(fatal);}catch(e){fatal(e);}core._th17_room_close?.();closeAudio();}});
// A mobile browser may terminate a hidden page before pagehide's IDB callback.
// Persist during play as well, with writes serialized by the sync chain.
setInterval(()=>{if(launched&&!document.hidden)try{void save().catch(fatal);}catch(e){fatal(e);}},30000);
const initialized=(async()=>{
 let last=performance.now(),lastPresent=null,ticks=0,presents=0,maxGapMs=0;
 core=await createModule({
  canvas,printErr:console.error,
  onGameFrameStart(){gpuMonitor??=gpuTiming(canvas.getContext('webgl2'));gpuMonitor.begin();},
  onGameFrame(ok,ms,count,presented){
   gpuMonitor?.end();
   if(!ok){fatal(Error(err()));return;}
   if(!launched)return;
   reportRoom();
   const now=performance.now();
   ticks+=count;
   if(presented){
    ++presents;
    if(!first){first=true;last=now;ticks=0;presents=0;maxGapMs=0;emit('first-frame');}
    else if(lastPresent!==null)maxGapMs=Math.max(maxGapMs,now-lastPresent);
    lastPresent=now;
   }
   if(first&&now-last>=1000){
    const p=core._th17_audio_statistics();
    if(p){
     const a=core.HEAPU32.subarray(p>>>2,(p>>>2)+11);
     emit('audio-health',{ready:!!a[0],callbacks:a[1],renderedFrames:a[2],errorCode:a[3],queuedFrames:a[4],queuedMs:a[4]*1000/44100,backend:'SDL3',track:a[5],cursor:a[9],musicEnabled:music});
    }
    const openGap=lastPresent===null?0:now-lastPresent;
    emit('frame-health',{fps:presents*1000/(now-last),simulationFps:ticks*1000/(now-last),maxGapMs:Math.max(maxGapMs,openGap),gpuMs:gpuMonitor?.value()??null});
    last=now;ticks=0;presents=0;maxGapMs=0;
   }
   if(core._th17_phase()===4)void stop().catch(fatal);
  }
 });
 core.SDL3=core.SDL3||{};if(parent!==window&&parent.__touhouAudioContext)core.SDL3.audioContext=parent.__touhouAudioContext;
 window.Module=core;window.FS=core.FS;core.resetBrowserKeyboard=()=>keyboard.clear();
 roomRuntime=typeof core._th17_room_open==='function';
 saveRoot=roomRuntime?'/savesth17mp':'/savesth17';
 core.FS.mkdirTree(saveRoot);core.FS.mount(core.IDBFS,{},saveRoot);await sync(true);
 core.FS.mkdirTree(saveRoot+'/replay');core.FS.symlink(saveRoot,'/save');
 if(!validEpoch||query.get('managedData')!=='1'||parent===window||typeof parent.__eaglerPrepareManagedRuntimeDataV1!=='function')throw Error('请从 eagler-touhou 启动此运行时');
 const {buffer}=await parent.__eaglerPrepareManagedRuntimeDataV1({game,generation:query.get('gameGeneration'),epoch});
 if(buffer.byteLength<16||buffer.byteLength>128*1024*1024)throw Error('游戏资源大小错误');
 core.FS.writeFile('/th17.dat',new Uint8Array(buffer),{canOwn:true});
 const response=await fetch('./resources.json');if(!response.ok)throw Error('资源清单加载失败');const manifest=await response.json();
 if(manifest.schema!=='eagler-sdl-resources/1'||manifest.game!==game||!Array.isArray(manifest.resources)||manifest.resources.length!==fontNames.length||new Set(manifest.resources.map(r=>r.path)).size!==fontNames.length||manifest.resources.some(r=>!fontNames.some(n=>r.path==='/fonts/'+n)))throw Error('资源清单无效');
 for(const r of manifest.resources)await resource(r);
 emit('ready',{saveRoot,roomPreview:roomRuntime,roomReturn:roomRuntime});
})().catch(e=>{if(e?.name==='AbortError'&&e?.message==='EAGLER_RUNTIME_SESSION_SUPERSEDED')return false;fatal(e);throw e;});
