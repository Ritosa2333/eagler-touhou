import {createBrowserKeyboard} from './directory-keyboard.mjs';
import createModule from './th09.mjs';
import {installStartupBranding} from './startup-branding.mjs';
import {scanCodes} from './keyboard.mjs';
import {SharedNetplay} from './shared-netplay.mjs';
import {exportReplayName,importReplayName} from './motion-replay.mjs';
import {createAutoReplay} from './auto-replay.mjs';
import {createReplayPlayer} from './replay-player.mjs';
const game='th09',protocol='eagler-touhou/1',query=new URLSearchParams(location.search),canvas=document.querySelector('canvas'),$=s=>document.querySelector(s);
// Variant identity is stamped by the builder, including offline Replay viewers.
const runtimeVariant=document.querySelector('meta[name="eagler-runtime-variant"]')?.content;
if(!['normal','multiplayer'].includes(runtimeVariant))throw Error('Invalid TH09 Runtime variant');
const saveRoot=runtimeVariant==='multiplayer'?'/savesth09mp':'/savesth09';
const epoch=Number(query.get('runtimeEpoch')),validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
const runtimeDocument=document,hosted=parent!==window,ownerFrame=hosted?window.frameElement:null;
let core,launched=false,first=false,options={},music=true,chain=Promise.resolve(),queue=Promise.resolve(),revision=0,netplay,stopping=false,networkOverlayPending=false;
let initializedReady=false,launchSettled=false,lifetime=0,stopPending=false,retired=false,configApplying=null,appliedTouchPolicy=0,autoReplay,replayPlayer;
// Native serials survive game open/reset/pause. These four markers belong to
// this core, not to a game frame or a gesture. Ineligible input is never replayed.
let sourceBomb=0,sourceEscape=0,mappedBomb=0,mappedEscape=0,lastTouchFire=false;
class SupersededRuntimeCommand extends Error {}
function ownsDocument(){
 try{return document===runtimeDocument&&(!hosted||!!ownerFrame&&ownerFrame.isConnected&&ownerFrame.contentWindow===window&&ownerFrame.contentDocument===runtimeDocument);}
 catch{return false;}
}
function neutralTouch(enabled=!!options.touchEnabled&&!configApplying&&!stopPending&&!retired&&!options.netplaySpectator&&!netplay?.spectator){
 if(!initializedReady||!core||!launched)return;
 core._th09_touch_cancel();
 // controls already clears both stick axes; unchanged mapped serials retain
 // accepted pulses and cannot create a bomb/escape while neutralizing focus.
 core._th09_touch_controls(+enabled,+lastTouchFire,0,mappedBomb,mappedEscape);
}
function revokeLifetime(terminal=false){
 if(!stopPending){stopPending=true;++lifetime;launchSettled=false;neutralTouch(false);}
 launchSettled=false;if(terminal)retired=true;
}
function acceptsTouch(){
 if(!ownsDocument()){if(!retired)revokeLifetime(true);return false;}
 return initializedReady&&!!core&&launched&&launchSettled&&!stopPending&&!stopping&&!retired&&!configApplying&&!document.hidden&&!networkOverlayPending&&!!options.touchEnabled&&!options.netplaySpectator&&!netplay?.spectator&&!options.replayViewer;
}
function receiveLifetime(){return stopPending||stopping||retired||!ownsDocument()?-1:lifetime;}
function assertLifetime(value,owner=core){
 if(!ownsDocument()){if(!retired)revokeLifetime(true);throw new SupersededRuntimeCommand('Runtime document was replaced');}
 if(value<0||value!==lifetime||owner!==core||stopPending||stopping||retired)throw new SupersededRuntimeCommand('Runtime operation was superseded');
}
function touchPolicy(value){
 const mode=Math.max(0,['touch','touch-unlimited','joystick','joystick-free'].indexOf(value.touchMovementMode));
 return (value.touchEnabled?1:0)|(mode<<1)|(value.touchFocusMode==='two-finger'?8:0)|(value.doubleTapBombEnabled?16:0)|(value.netplaySpectator?32:0);
}
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...fields},location.origin);
const values=(fn,n)=>Array.from(core.HEAP32.subarray(fn()/4,fn()/4+n));
const status=()=>({title:values(core._th09_title_status,8),session:values(core._th09_session_status,8),touch:values(core._th09_touch_state,4)});
const err=()=>{const a=core.HEAPU8.subarray(core._th09_error());return new TextDecoder().decode(a.subarray(0,a.indexOf(0)));};
const keyboard=createBrowserKeyboard({accept:code=>!!scanCodes[code],send:(code,down)=>core?._th09_key(scanCodes[code],+down)});
function clearKeyboard(){keyboard.clear();core?._th09_keys_clear();}
function acceptsKeyboard(){return launched&&!stopping&&!stopPending&&!retired&&!document.hidden&&!networkOverlayPending&&!options.netplaySpectator&&!netplay?.spectator&&!options.replayViewer;}
const fatal=e=>{revokeLifetime(true);const message=e?.message||String(e);$('#error').textContent=message;clearKeyboard();core?._th09_loop_pause(1);emit('error',{error:message});};
const sync=(populate=false)=>{const current=chain.then(()=>new Promise((r,j)=>core.FS.syncfs(populate,e=>e?j(e):r())));chain=current.catch(()=>{});return current;};
function path(value){
 let name=String(value).replace(new RegExp('^'+saveRoot+'/'),'').replace(/^\//,'');
 const parts=name.split('/');parts[parts.length-1]=parts[parts.length-1].toLowerCase();name=parts.join('/');
 const replay=parts[0]==='replay'&&(parts.length===2||(parts.length===3&&parts[1].length<=128&&!/[<>:"\\|?*\u0000-\u001f\u007f]/.test(parts[1])&&!/[. ]$/.test(parts[1])))&&/^th9_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?$/.test(parts.at(-1));
 if(name.length>240||(!['score.dat','th09.cfg'].includes(name)&&!replay))throw Error('存档路径无效');return name;
}
function apply(){const sensitivity=Number(options.touchSensitivity??100);options.touchSensitivity=Number.isFinite(sensitivity)?Math.max(100,Math.min(300,sensitivity)):100;core.eaglerOptions=options;core._th09_touch_options(+!!options.touchEnabled,Math.max(0,['touch','touch-unlimited','joystick','joystick-free'].indexOf(options.touchMovementMode)),options.touchSensitivity/100,+(options.touchFocusMode==='two-finger'),+!!options.doubleTapBombEnabled);appliedTouchPolicy=touchPolicy(options);if(launched)core._th09_music_enabled(+music);}
async function resource(r,receivedLifetime,owner){
 assertLifetime(receivedLifetime,owner);
 if(!(/^\/(?:music|fonts)\/[a-z0-9_.-]+$/.test(r.path)||r.path==='/msgothic.ttc'||r.path==='/unifont.otf'))throw Error('资源路径无效');
 const u=new URL(r.url,location.href);if(u.origin!==location.origin)throw Error('资源来源无效');
 const response=await fetch(u);assertLifetime(receivedLifetime,owner);if(!response.ok)throw Error('资源读取失败');
 const bytes=new Uint8Array(await response.arrayBuffer());assertLifetime(receivedLifetime,owner);
 owner.FS.mkdirTree(r.path.slice(0,r.path.lastIndexOf('/'))||'/');owner.FS.writeFile(r.path,bytes);
 emit('transfer',{mode:r.path.startsWith('/music/')?'ogg':'runtime',loaded:bytes.length,total:bytes.length,path:r.path});
}
// Same runtimePack contract as the TH10 shell: Launcher verifies SHA-256,
// Runtime rechecks manifest identity, path scope and each mounted file size.
let runtimePackFiles=[];
function assertRuntimePackManifest(manifest,pack){
 if(manifest?.schema!=='eagler-touhou/thcrap-static-pack/1'||manifest.game!==game||
    manifest.language!==pack.language||typeof manifest.runtimeVersion!=='string'||
    !Array.isArray(manifest.files)||manifest.files.length>256)throw Error('Invalid TH09 language pack manifest');
 for(const file of manifest.files)
  if(typeof file?.path!=='string'||!file.path.startsWith('/thcrap/th09/')||file.path.includes('\\')||file.path.includes('..')||
     !Number.isInteger(file.bytes)||file.bytes<0)throw Error('Invalid TH09 language pack file');
}
async function installRuntimePack(pack){
 if(launched)throw Error('Runtime resources cannot be changed after launch');
 if(typeof pack?.url!=='string'||typeof pack.language!=='string'||
    !Number.isInteger(pack.bytes)||pack.bytes<=0||
    !pack.manifest||!Array.isArray(pack.files))throw Error('Invalid TH09 language pack');
 const url=new URL(pack.url,location.href);
 if(url.origin!==location.origin)throw Error('Cross-origin TH09 language pack');
 assertRuntimePackManifest(pack.manifest,pack);
 const expected=new Map(pack.manifest.files.map(file=>[file.path,file]));
 if(pack.files.length!==expected.size)throw Error('TH09 language pack file count mismatch');
 const verified=[];
 for(const file of pack.files){
  if(typeof file?.path!=='string'||!file.path.startsWith('/thcrap/th09/')||file.path.includes('\\')||file.path.includes('..')||
     !(file.bytes instanceof Uint8Array))throw Error('Invalid TH09 language pack path');
  const declaration=expected.get(file.path);
  if(!declaration||file.bytes.length!==declaration.bytes)throw Error(file.path+': size mismatch');
  verified.push({path:file.path,bytes:file.bytes});
 }
 for(const path of runtimePackFiles){try{core.FS.unlink(path);}catch{}}
 runtimePackFiles=[];
 for(const file of verified){
  core.FS.mkdirTree(file.path.slice(0,file.path.lastIndexOf('/')));
  core.FS.writeFile(file.path,file.bytes,{canOwn:true});runtimePackFiles.push(file.path);
 }
}
async function save(){if(launched&&!netplay?.spectator&&!core._th09_save_snapshot())throw Error('保存失败');await sync();}
async function stop(){if(stopping)return;revokeLifetime();stopping=true;clearKeyboard();try{networkOverlayPending=false;replayPlayer?.close();replayPlayer=null;core._th09_loop_stop();await netplay?.finishSpectatorOutput();netplay?.close();await save();core._th09_game_close();launched=false;delete window.__th09Runtime;if(!retired)stopPending=false;emit('exit',{code:0,status:'success'});}finally{stopping=false;}}
function openNetwork(){if(!launched||options.netplayMode==='lan'||!status().title[1]||networkOverlayPending)return;networkOverlayPending=true;neutralTouch();core._th09_loop_pause(1);emit('network-request');}
async function command(m,receivedLifetime=receiveLifetime()){switch(m.command){
case 'configure':{
 const owner=core;assertLifetime(receivedLifetime,owner);if(!initializedReady)throw Error('Runtime initialization is incomplete');
 const nextOptions=m.options||{},ticket={lifetime:receivedLifetime,owner};
 // A policy change cancels the old gesture before options can be replaced
 // across resource awaits. Ordinary controls/sensitivity do not take this path.
 if(configApplying||touchPolicy(nextOptions)!==appliedTouchPolicy){configApplying=ticket;neutralTouch(false);}
 options=nextOptions;music=m.music==='ogg';
 for(const r of [...(m.sharedResources||[]),...(m.runtimeResources||[]),...(m.resources||[])]){await resource(r,receivedLifetime,owner);assertLifetime(receivedLifetime,owner);}
 if(m.runtimePack){await installRuntimePack(m.runtimePack);assertLifetime(receivedLifetime,owner);}
 assertLifetime(receivedLifetime,owner);
 if(configApplying&&configApplying!==ticket)throw new SupersededRuntimeCommand('Runtime configuration was superseded');
 if(core.FS.analyzePath('/msgothic.ttc').exists){try{core.FS.unlink('/fonts/msgothic.ttc');}catch{}core.FS.symlink('/msgothic.ttc','/fonts/msgothic.ttc');}
 apply();assertLifetime(receivedLifetime,owner);if(configApplying===ticket)configApplying=null;return {};
}
case 'resources':{
 const owner=core;assertLifetime(receivedLifetime,owner);
 for(const r of m.resources||[]){await resource(r,receivedLifetime,owner);assertLifetime(receivedLifetime,owner);}
 return {};
}
case 'keyboard':if(acceptsKeyboard())keyboard.event(m,!!m.down,'hosted');return {};
case 'keyboard-clear':clearKeyboard();return {};
case 'touch-cancel':if(!ownsDocument()){if(!retired)revokeLifetime(true);}else neutralTouch();return {};
case 'direct-touch':{if(!acceptsTouch())return {};const b=canvas.getBoundingClientRect();core._th09_touch(({down:0,move:1,up:2,cancel:2})[m.type]??2,Number(m.id)||0,(Number(m.x)*innerWidth-b.left)/b.width,(Number(m.y)*innerHeight-b.top)/b.height);return {};}
case 'touch-controls':{
 const t=m.controls||m,bomb=t.bombSerial>>>0,escape=t.escapeSerial>>>0,changedBomb=bomb!==sourceBomb,changedEscape=escape!==sourceEscape;
 sourceBomb=bomb;sourceEscape=escape;if(!acceptsTouch())return {};
 const sensitivity=Number(t.touchSensitivity);
 if(sensitivity>=100&&sensitivity<=300&&sensitivity!==options.touchSensitivity){options.touchSensitivity=sensitivity;apply();}
 const nextBomb=(mappedBomb+Number(changedBomb))>>>0,nextEscape=(mappedEscape+Number(changedEscape))>>>0;
 core._th09_touch_controls(+!!options.touchEnabled,+!!t.fireEnabled,+!!t.focusEnabled,nextBomb,nextEscape);
 mappedBomb=nextBomb;mappedEscape=nextEscape;lastTouchFire=!!t.fireEnabled;
 core._th09_touch_stick(Number(t.joystickX)||0,Number(t.joystickY)||0);return {};
}
case 'launch':{
 const owner=core;assertLifetime(receivedLifetime,owner);
 if(!initializedReady||configApplying)throw Error('Runtime initialization or configuration is incomplete');
 if(launched){if(!launchSettled)throw Error('Runtime launch did not complete');return {};}
 launchSettled=false;
 clearKeyboard();if(!core.FS.analyzePath('/fonts/msgothic.ttc').exists&&core.FS.analyzePath('/msgothic.ttc').exists)core.FS.symlink('/msgothic.ttc','/fonts/msgothic.ttc');
 const build=await(await fetch('./version.json')).json();assertLifetime(receivedLifetime,owner);
 await installStartupBranding(core,{game,builtAt:build.builtAt});assertLifetime(receivedLifetime,owner);
 if(!core._th09_prepare_loading())throw Error(err());
 const startupImageAt=performance.now();performance.mark('eagler-startup-image');
 await new Promise(requestAnimationFrame);assertLifetime(receivedLifetime,owner);
 await new Promise(requestAnimationFrame);assertLifetime(receivedLifetime,owner);
 if(!core._th09_game_open(Date.now()&65535))throw Error(err());
 if(options.replayPath!==undefined){
  if(options.netplayMode==='lan')throw Error('不能在联机对战中打开录像');
  const selected=path(options.replayPath);if(!selected.startsWith('replay/'))throw Error('录像路径无效');
  const bytes=core.FS.readFile(saveRoot+'/'+selected.replace(/\.rpyx$/,'.rpy'));
  importReplayName(selected,bytes,9);
  const pointer=core._th09_file_buffer(bytes.length);if(!pointer)throw Error('录像为空或过大');
  core.HEAPU8.set(bytes,pointer);if(!core._th09_play_replay(bytes.length))throw Error(err());
 }
 await new Promise(resolve=>setTimeout(resolve,Math.max(0,2000-(performance.now()-startupImageAt))));assertLifetime(receivedLifetime,owner);
 performance.mark('eagler-startup-menu-ready');launched=true;apply();first=false;
 netplay=options.netplayMode==='lan'?new SharedNetplay(core,{
  onStatus:t=>emit('notice',{message:t}),onTiming:timing=>emit('runtime-info',{netplayTiming:timing}),
  onMatch:th09Match=>emit('runtime-info',{th09Match}),
  onClose:reason=>{if(receivedLifetime!==lifetime||owner!==core||retired)return;revokeLifetime();if(reason)emit('notice',{message:reason});queueMicrotask(()=>void stop().catch(fatal));},
  onResult:()=>emit('notice',{message:'对局结束，可在游戏内保存 Replay'})}):null;
 $('#loading').textContent='';core._th09_loop_start();if(netplay){core._th09_loop_pause(1);await netplay.connect(options);}
 assertLifetime(receivedLifetime,owner);launchSettled=true;
 window.__th09Runtime={core,netplay,status,save,command};emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++',architecture:protocol,version:'2026.09.20-fix'});
 return {};
}
case 'playback':{
 assertLifetime(receivedLifetime);
 if(!launched||!launchSettled||!options.replayViewer||options.netplayMode==='lan')throw Error('当前游戏不支持录像控制');
 if(m.action==='open'){
  const selected=path(m.path);if(!selected.startsWith('replay/'))throw Error('录像路径无效');
  const bytes=core.FS.readFile(saveRoot+'/'+selected.replace(/\.rpyx$/,'.rpy'));importReplayName(selected,bytes,9);
  replayPlayer??=createReplayPlayer({core,onState:playback=>{if(ownsDocument()&&!retired&&!stopPending)emit('runtime-info',{playback});},onError:fatal});
  clearKeyboard();return {playback:replayPlayer.play(selected,bytes)};
 }
 if(!replayPlayer)throw Error('请先选择录像');return {playback:replayPlayer.control(m.action,m.value)};
}
case 'sync':await save();return {};
case 'list':{const files=[];const scan=dir=>{for(const name of core.FS.readdir(saveRoot+dir)){if(name==='.'||name==='..')continue;const n=(dir+'/'+name).replace(/^\//,''),full=saveRoot+'/'+n,s=core.FS.stat(full);if(dir==='/replay'&&core.FS.isDir(s.mode)){scan('/'+n);continue;}try{path(n);}catch{continue;}if(core.FS.isFile(s.mode)){const replayMatch=autoReplay.readMatch(full);files.push({path:exportReplayName(n,core.FS.readFile(full),9),size:s.size,modifiedAt:Number(s.mtime),...(replayMatch?{replayMatch}:{})});}}};scan('');scan('/replay');return {files};}
case 'read':return {bytes:Array.from(core.FS.readFile(saveRoot+'/'+path(m.path).replace(/\.rpyx$/,'.rpy')))};
case 'write':{if(!Array.isArray(m.bytes)||m.bytes.length>16*1024*1024||m.bytes.some(b=>!Number.isInteger(b)||b<0||b>255))throw Error('存档数据无效');const bytes=Uint8Array.from(m.bytes),name=importReplayName(path(m.path),bytes,9);if(launched&&(!options.replayViewer||options.netplayMode==='lan'||!name.startsWith('replay/')))throw Error('请先退出游戏再导入');if(name.endsWith('.rpy')&&core.FS.analyzePath(saveRoot+'/'+name).exists)throw Error('录像文件已存在，请使用其他文件名');const kind=name.endsWith('.rpy')?1:name==='score.dat'?0:2,p=core._th09_file_buffer(bytes.length);if(!p)throw Error('文件为空或过大');core.HEAPU8.set(bytes,p);if(!core._th09_file_valid(kind,bytes.length))throw Error('文件损坏或不是花映塚 1.50a 的存档/录像');core.FS.mkdirTree((saveRoot+'/'+name).slice(0,(saveRoot+'/'+name).lastIndexOf('/')));core.FS.writeFile(saveRoot+'/'+name,bytes);autoReplay.removeMatch(saveRoot+'/'+name);if(runtimeVariant==='multiplayer')autoReplay.writeMatch(saveRoot+'/'+name,m.replayMatch);await sync();return {};}
case 'remove':if(launched&&(!options.replayViewer||options.netplayMode==='lan'||!path(m.path).startsWith('replay/')))throw Error('请先退出游戏');{const full=saveRoot+'/'+path(m.path).replace(/\.rpyx$/,'.rpy');core.FS.unlink(full);autoReplay.removeMatch(full);const parent=full.slice(0,full.lastIndexOf('/'));if(parent!==saveRoot+'/replay'&&parent.startsWith(saveRoot+'/replay/')){try{core.FS.rmdir(parent);}catch{}}}await sync();return {};
case 'spectator-room-state':{
 assertLifetime(receivedLifetime);
 if(!Number.isInteger(m.runId)||m.runId<0||m.runId>0xffffffff||typeof m.waiting!=='boolean')throw Error('TH09 观战房间状态无效');
 netplay?.roomState(m.runId,m.waiting);return {};
}
case 'network-open':openNetwork();return {};
case 'network-cancel':networkOverlayPending=false;clearKeyboard();neutralTouch();core._th09_loop_pause(+document.hidden);canvas.focus();return {};
default:throw Error('不支持的操作：'+m.command);}}
async function dispatchCommand(m,receivedLifetime=receiveLifetime(),waitForInitialized=false){
 try{if(waitForInitialized)await initialized;const result=await command(m,receivedLifetime);if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:true,...result},location.origin);}
 catch(e){if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:false,error:String(e),errno:e?.errno},location.origin);else if(!(e instanceof SupersededRuntimeCommand))fatal(e);}
}
window.addEventListener('message',event=>{
 const m=event.data;if(!validEpoch||event.source!==parent||event.origin!==location.origin||m?.protocol!==protocol||m.game!==game||m.epoch!==epoch||typeof m.command!=='string')return;
 // Device events must not wait behind filesystem/resource operations. Their
 // synchronous command bodies observe the current owner and lifecycle now.
 if(m.command==='keyboard'||m.command==='keyboard-clear'||m.command==='direct-touch'||m.command==='touch-controls'||m.command==='touch-cancel'){void dispatchCommand(m,lifetime);return;}
 // This receiver-local lifetime fences queued launch/configure continuations.
 // It cannot identify a parent postMessage that has not reached this listener.
 const receivedLifetime=receiveLifetime();
 queue=queue.then(()=>dispatchCommand(m,receivedLifetime,true)).catch(e=>{if(!(e instanceof SupersededRuntimeCommand))fatal(e);});
});
// Room editing belongs to the Launcher overlay in the parent realm, so the
// Launcher only forwards keys while its own chrome holds focus. The Launcher
// focuses this Runtime document when the player opens, which means gameplay
// keys arrive here instead; handle them in this realm too.
for(const event of ['pointerdown','keydown'])window.addEventListener(event,()=>core?.SDL3?.audioContext?.resume().catch(()=>{}),{capture:true});
for(const [event,down] of [['keydown',1],['keyup',0]])window.addEventListener(event,e=>{
 if(!core||!acceptsKeyboard())return;
 keyboard.event(e,down);
},{capture:true});
document.addEventListener('visibilitychange',()=>{if(launched){clearKeyboard();neutralTouch();core._th09_loop_pause(+(document.hidden||networkOverlayPending||(options.netplayMode==='lan'&&(netplay?.recovering||netplay?.disconnected||(!netplay?.active&&!netplay?.finished)))));if(document.hidden)void save().catch(fatal);}});
window.addEventListener('blur',()=>{clearKeyboard();neutralTouch();});
// Ordinary Android/iOS Launcher touch uses the parent owner. Retain this
// document's pointer path for its existing direct-input carrier as well.
for(const [name,type] of [['pointerdown',0],['pointermove',1],['pointerup',2],['pointercancel',2]])document.body.addEventListener(name,e=>{
 if(e.pointerType==='mouse'||e.target.closest('button,input,dialog')||!acceptsTouch())return;
 e.preventDefault();if(type===0)document.body.setPointerCapture(e.pointerId);const b=canvas.getBoundingClientRect();core._th09_touch(type,e.pointerId,(e.clientX-b.left)/b.width,(e.clientY-b.top)/b.height);
});
canvas.addEventListener('webglcontextlost',e=>{e.preventDefault();clearKeyboard();fatal(Error('图形环境失效，请退出后重新开始。'));});window.addEventListener('pagehide',()=>{revokeLifetime(true);replayPlayer?.close();clearKeyboard();if(launched){netplay?.close();core._th09_loop_pause(1);void save();}});
const initialized=(async()=>{
 const initializingLifetime=lifetime;let last=performance.now(),lastFrames=0;
  core=await createModule({canvas,resetBrowserKeyboard:()=>keyboard.clear(),printErr:console.error,onNetworkRequest:openNetwork,onNetworkReplayBoundary:(frame,winner)=>winner<0?autoReplay?.begin(frame):autoReplay?.end(frame,winner),onNetworkResult:()=>netplay?.result(),onNetworkInput:(...args)=>netplay?.input(...args),onNetworkSpectatorBegin:(...args)=>netplay?.beginMatch(...args),onNetworkSpectatorEnd:(...args)=>netplay?.endMatch(...args),onNetworkSpectatorRound:(...args)=>netplay?.roundStart(...args),onNetworkSpectatorFrame:(...args)=>netplay?.publish(...args),onGameFrame(ok,ms){if(!ok){fatal(Error(err()));return;}netplay?.frame();if(!first){first=true;emit('first-frame');}const current=core._th09_storage_revision();if(current!==revision){revision=current;void sync().catch(fatal);}const now=performance.now(),frames=status().title[0];if(now-last>=1000){emit('frame-health',{fps:(frames-lastFrames)*1000/(now-last),maxGapMs:ms});emit('audio-health',{backend:'script',robust:core.SDL3?.audioContext?.state==='running'});last=now;lastFrames=frames;}const title=status().title;if(title[7]===0||(netplay?.finished&&title[1]&&title[2]===1)||(netplay?.spectator&&!netplay.timelineExperiment&&title[1]&&(title[2]===14||title[2]===1)))void stop().catch(fatal);}});
 assertLifetime(initializingLifetime,core);
 window.Module=core;window.FS=core.FS;core.SDL3=core.SDL3||{};if(parent!==window&&parent.__touhouAudioContext)core.SDL3.audioContext=parent.__touhouAudioContext;
 core.FS.mkdirTree(saveRoot);core.FS.mount(core.IDBFS,{},saveRoot);await sync(true);assertLifetime(initializingLifetime,core);core.FS.mkdirTree(saveRoot+'/replay');core.FS.symlink(saveRoot,'/save');
 autoReplay=createAutoReplay({core,saveRoot,options:()=>options,enabled:()=>runtimeVariant==='multiplayer'&&options.netplayMode==='lan',sync:()=>{revision=core._th09_storage_revision();return sync();},notice:message=>emit('notice',{message})});
 if(query.get('managedData')!=='1'||parent===window||typeof parent.__eaglerPrepareManagedRuntimeDataV1!=='function')throw Error('请从 eagler-touhou 启动花映塚');
 const buffer=(await parent.__eaglerPrepareManagedRuntimeDataV1({game,generation:query.get('gameGeneration'),epoch})).buffer;assertLifetime(initializingLifetime,core);
 if(Object.prototype.toString.call(buffer)!=='[object ArrayBuffer]'||buffer.byteLength<1000000)throw Error('花映塚资源无效');core.FS.writeFile('/th09.dat',new Uint8Array(buffer));
 core.FS.mkdirTree('/fonts');for(const name of ['cp932.bin','blend.bin']){const response=await fetch('./fonts/'+name);assertLifetime(initializingLifetime,core);if(!response.ok)throw Error('字体表缺失：'+name);const bytes=new Uint8Array(await response.arrayBuffer());assertLifetime(initializingLifetime,core);core.FS.writeFile('/fonts/'+name,bytes);}
 assertLifetime(initializingLifetime,core);initializedReady=true;emit('ready');if(query.get('standalone')==='1')await command({command:'launch'});
})().catch(e=>{if(!(e instanceof SupersededRuntimeCommand))fatal(e);throw e;});
void initialized.catch(()=>{});
