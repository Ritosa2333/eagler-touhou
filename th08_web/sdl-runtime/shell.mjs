// Platform shell for the upstream eagler-touhou/1 Launcher contract.
// Game construction, input, timing, rendering, text and sound belong to C++.
import {createBrowserKeyboard} from './directory-keyboard.mjs';
import createModule from './th08-sdl.mjs';
import {createPractice} from './practice.mjs';
import {bindOutsideTouches} from './eagler-host.mjs';
import {exportReplayName,importReplayName} from './motion-replay.mjs';
import {normalizeOptions,applyTouchOptions,touchControls,suspendRuntimeAudio,resumeRuntimeAudio,directTouch,ensureSharedFontAlias,installResources as installHostResources,observeMusicWrites,mountManagedData,isSupersededRuntimeError} from './eagler-host.mjs';
import {initializeSaveStorage,migrateLegacySaves} from './save-storage.mjs';
import {validateMultiplayerOptions,configureMultiplayer,updateNetworkDiagnostics,networkError} from './multiplayer-host.mjs';
const protocol='eagler-touhou/1',game='th08',query=new URLSearchParams(location.search),canvas=document.querySelector('canvas');
const runtimeVariant=query.get('runtimeVariant')??'normal',multiplayerRuntime=runtimeVariant==='multiplayer';
const epoch=Number(query.get('runtimeEpoch'));
const validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...fields},location.origin);
let Module,core,app=0,launched=false,first=false,closing=false,language=query.get('language')==='lang_zh-hans'?'chs':'jp',options={},music=true;
let runtimeBuildWords=null;
let runtimeBuildIdentity='',runtimeWasmIdentity='';
let traceRequested=false,traceStarted=false,traceHostButton=null;
const traceGlobalFields=['time','totalTime','timeRequirement','pointValue','clock','score','points','graze','rngSeed','rngBackup','rngCalls','stage','gameFlags','spellFlags','pendingTime',
 'pauseState','scenePaused','retrying','enemyFrames','bulletActive','bulletTimer','bulletCancelFrames','bulletCounter',
 'effectCursor','effectActive','effectFrames','supervisorActive','supervisorTarget','playFrames','gameSupervisor'];
const tracePilotFields=['power','lives','bombs','gauge','deaths','life','predead','lifeTimer','clearFrames','contextPower','contextTime','contextBombs','contextGauge','gameFlags','focus','bombActive','bombTimer','shootTimer','xBits','yBits','buttons','spirit','cancelItem','gaugeLock'];
const traceItemFields=['slot','active','type','state','age','owner','recipient','xBits','yBits','vxBits','vyBits','targetXBits','targetYBits','next','previous','maxValue'];
function traceRead(frame=0xffffffff,detail=false){
 if(!app||!core?.multiplayer_resource_trace_read)return null;
 const pointer=core.multiplayer_resource_trace_read(app,frame,detail?1:0),heap=new Uint8Array(core.memory.buffer);let end=pointer;
 while(end<heap.length&&heap[end])++end;
 return JSON.parse(new TextDecoder().decode(heap.subarray(pointer,end)));
}
function traceStart(){
 if(!multiplayerRuntime||!app||!core?.multiplayer_resource_trace_enable)return false;
 const frame=u32(core.multiplayer_netplay_status(app),12)[3];
 if(!core.multiplayer_resource_trace_enable(app,frame))return false;
 traceStarted=true;
 if(traceHostButton){traceHostButton.disabled=false;traceHostButton.textContent='导出同步记录';}
 return true;
}
function exportResourceTrace(download=true){
 if(!traceStarted||!core.multiplayer_resource_trace_freeze(app))throw Error('同步记录尚未开始，请在进入游戏后导出');
 const meta=traceRead(),records=[];
 if(meta.oldestFrame!==0xffffffff&&meta.latestFrame!==0xffffffff){
  if(meta.latestFrame-meta.oldestFrame>4096)throw Error('Invalid diagnostic frame window');
  for(let frame=meta.oldestFrame;frame<=meta.latestFrame;frame++){
   const sample=traceRead(frame,true);if(sample?.record)records.push(sample.record);
  }
 }
 const {record:_record,...metadata}=traceRead(0xffffffff,true);
 const report={schema:'th08mp/resource-trace/1',runtimeGeneration:runtimeBuildIdentity,wasmSha256:runtimeWasmIdentity,
  capturedAt:new Date().toISOString(),userAgent:navigator.userAgent,metadata,
  valueFields:[...traceGlobalFields,...Array.from({length:meta.playerCount},(_,seat)=>tracePilotFields.map(field=>`P${seat+1}.${field}`)).flat(),'itemCursor','itemCount','itemHead','itemTail'],
  itemFields:traceItemFields,inputFields:['buttons','analogMode','xBits','yBits','unlimited','touchUsed','touchBomb'],
  options:{touchMovementMode:options.touchMovementMode,alwaysHitbox:!!options.alwaysHitbox,multiplayerLocalPlayerVisibility:!!options.multiplayerLocalPlayerVisibility},records};
 // Export reads a frozen observer, not a paused or modified simulation. No
 // Relay URL, TURN credential, save file or private configuration is included.
 if(download){const url=URL.createObjectURL(new Blob([JSON.stringify(report)],{type:'application/json'}));const owner=traceHostButton?.ownerDocument||document;const a=owner.createElement('a');
  a.href=url;a.download=`th08mp-P${meta.localPlayer+1}-${meta.sessionId}-${meta.latestFrame}.json`;owner.body.append(a);a.click();a.remove();setTimeout(()=>URL.revokeObjectURL(url),30000);}
 if(traceHostButton){traceHostButton.textContent='同步记录已导出';traceHostButton.disabled=true;}
 return report;
}
function removeResourceTraceHostButton(){
 if(traceHostButton){traceHostButton.remove();traceHostButton=null;}
}
function installResourceTrace(){
 if(!multiplayerRuntime||!core.multiplayer_resource_trace_enable)return;
 traceRequested=query.get('th08Trace')==='1';
 try{traceRequested||=new URL(parent.location.href).searchParams.get('th08Trace')==='1';}catch{}
 if(!traceRequested)return;
 try{
  const host=parent.document.querySelector('#player');if(!host)return;
  parent.document.getElementById('th08ResourceTraceExport')?.remove();
  const button=parent.document.createElement('button');button.id='th08ResourceTraceExport';button.type='button';button.textContent='同步记录：待开局';button.disabled=true;
  button.setAttribute('aria-label','导出 TH08 联机同步记录');
  button.style.cssText='position:absolute;z-index:70;top:calc(max(10px,env(safe-area-inset-top)) + 50px);right:max(10px,env(safe-area-inset-right));min-width:104px;height:42px;padding:0 10px;border:0;border-radius:10px;background:rgba(91,27,37,.96);color:#ffe9eb;box-shadow:0 4px 14px rgba(0,0,0,.5),inset 0 0 0 1px rgba(255,126,137,.14);font:700 10px/1 sans-serif;letter-spacing:.03em;pointer-events:auto;touch-action:manipulation;-webkit-user-select:none;user-select:none;-webkit-touch-callout:none;-webkit-tap-highlight-color:transparent';
  for(const name of ['pointerdown','pointermove','pointerup','pointercancel','touchstart','touchmove','touchend','touchcancel','mousedown','mouseup'])
   button.addEventListener(name,event=>event.stopPropagation(),{passive:true});
  button.addEventListener('click',event=>{event.stopPropagation();try{exportResourceTrace(true);}catch(reason){button.disabled=false;button.textContent=String(reason?.message||reason);}});
  host.append(button);traceHostButton=button;
 }catch{}
}
let practice;
const keyboard=createBrowserKeyboard({
 send(code,down){if(!core)return;if(options.thpracEnabled&&practice?.key(code,down))return;cstring(code,p=>core.sdl_key(p,+down));},
 onClear(){practice?.clear();}
});
function clearKeyboard(){keyboard.clear();core?.sdl_keys_clear();}
let frames=0,lastHealth=0,lastFrame=0,maxGap=0,lastPresented=0,saveTimer=null;
const cancelTouches=bindOutsideTouches(document,canvas,()=>core,()=>launched&&options.touchEnabled);
const error=reason=>{const message=reason?.stack||String(reason);if(multiplayerRuntime){window.__eaglerNetplayFailed=true;window.__eaglerNetplayError=message;}document.querySelector('#error').textContent=message;emit('error',{message,error:message});console.error(reason);};
const u32=(ptr,count)=>new Uint32Array(core.memory.buffer,ptr,count);
const cstring=(text,fn)=>{const bytes=new TextEncoder().encode(text+'\0'),p=core.allocate(bytes.length);try{new Uint8Array(core.memory.buffer,p,bytes.length).set(bytes);return fn(p);}finally{core.deallocate(p);}};
let storage;
const root=()=>storage.root(language);
let storageSync=Promise.resolve();
const sync=populate=>{const current=storageSync.then(()=>new Promise((resolve,reject)=>Module.FS.syncfs(populate,e=>e?reject(e):resolve())));storageSync=current.catch(()=>{});return current;};
async function migrateSaves(){
 await migrateLegacySaves(storage,{indexedDB,filesystem:Module.FS,sync,importReplayName:(path,bytes)=>importReplayName(path,bytes,8)});
}
async function mountData(){await mountManagedData(Module,{game,parentWindow:parent,query,emit});}
async function installResources(resources=[]){return installHostResources(Module,resources,{game,emit});}
// thcrap-style offline language pack (eagler-touhou/1 configure.runtimePack):
// bytes arrive inline, already hash-verified by the Launcher. The shell
// re-validates schema/game/language/paths/sizes before touching MEMFS.
let runtimePackFiles=[];
function assertRuntimePackManifest(manifest,pack){
 if(manifest?.schema!=='eagler-touhou/thcrap-static-pack/1'||manifest.game!==game||
    manifest.language!==pack.language||typeof manifest.runtimeVersion!=='string'||
    !Array.isArray(manifest.files)||manifest.files.length>256)throw Error('Invalid TH08 language pack manifest');
 for(const file of manifest.files)
  if(typeof file?.path!=='string'||!file.path.startsWith('/thcrap/th08/')||file.path.includes('\\')||file.path.includes('..')||
     !Number.isInteger(file.bytes)||file.bytes<0)throw Error('Invalid TH08 language pack file');
}
async function installRuntimePack(pack){
 if(launched)throw Error('Runtime resources cannot be changed after launch');
 if(typeof pack?.url!=='string'||typeof pack.language!=='string'||
    !Number.isInteger(pack.bytes)||pack.bytes<=0||
    !pack.manifest||!Array.isArray(pack.files))throw Error('Invalid TH08 language pack');
 const url=new URL(pack.url,location.href);
 if(url.origin!==location.origin)throw Error('Cross-origin TH08 language pack');
 assertRuntimePackManifest(pack.manifest,pack);
 const expected=new Map(pack.manifest.files.map(file=>[file.path,file]));
 if(pack.files.length!==expected.size)throw Error('TH08 language pack file count mismatch');
 const verified=[];
 for(const file of pack.files){
  if(typeof file?.path!=='string'||!file.path.startsWith('/thcrap/th08/')||file.path.includes('\\')||file.path.includes('..')||
     !(file.bytes instanceof Uint8Array))throw Error('Invalid TH08 language pack path');
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
function applyOptions(){applyTouchOptions(core,options.netplaySpectator?{...options,touchEnabled:false}:options);core.sdl_touch_display?.(options.alwaysHitbox?1:0);if(app&&multiplayerRuntime)core.multiplayer_local_player_visibility?.(app,options.multiplayerLocalPlayerVisibility&&!options.netplaySpectator&&!options.replayViewer?1:0);practice?.configure({...options,thpracEnabled:options.thpracEnabled&&options.netplayMode!=='lan'});}
function status(){return Array.from(new Int32Array(core.memory.buffer,core.sdl_game_status(),10));}
function save(){if(app)core.save(app);return sync(false);}
// Backgrounding only persists the runtime filesystem. core.save() runs the
// result-screen score save, which attaches the result scene and flushes the
// shared renderer; that must not run against a title/practice frame whose
// pending batch has no valid texture.
function persist(){return sync(false);}
async function resumeForegroundAudio(forcePause=false){
 if(!core||!launched||document.hidden)return false;
 if(forcePause)core.sdl_loop_pause(1);
 return resumeRuntimeAudio(Module,core,()=>!!core&&launched&&!document.hidden);
}
async function stop(){if(closing)return;closing=true;clearKeyboard();try{practice?.close();core.sdl_loop_stop();await save();core.sdl_game_close();window.dispatchEvent(new CustomEvent('touhou-midi-close'));await sync(false);app=0;launched=false;emit('exit',{code:0,status:'success'});}finally{removeResourceTraceHostButton();closing=false;}}
async function launch(){
 if(launched)return;clearKeyboard();
 ensureSharedFontAlias(Module);
 // Localized text falls back to Unifont for glyphs MS Gothic does not carry
 // (sdl/FontHost.cpp). Alias it into the canonical /fonts layout when a
 // language pack or a Chinese UI language is in play.
 if(language==='chs'||runtimePackFiles.length){
  try{Module.FS.stat('/unifont.otf');try{Module.FS.unlink('/fonts/unifont.otf');}catch{}Module.FS.symlink('/unifont.otf','/fonts/unifont.otf');}
  catch{console.warn('th08: /unifont.otf unavailable; CJK glyph fallback disabled');}
 }
 traceStarted=false;
 const mode=Module.touhouMusicMode||'none';music=mode!=='none';core.sdl_ogg_decode_mode?.(options.oggDecodeMode==='full');core.sdl_music_source?.(mode==='midi'?2:1);
 const multiplayer=multiplayerRuntime?validateMultiplayerOptions(options):null;
 const replayViewer=multiplayerRuntime&&options.replayViewer===true;
 const multiplayerPreflight=multiplayerRuntime&&options.multiplayerPreflight===true;
 if((replayViewer&&multiplayer)||(multiplayerPreflight&&(multiplayer||replayViewer)))
  throw Error('Conflicting TH08 multiplayer Runtime role');
 if(multiplayerRuntime&&!multiplayer&&!replayViewer&&!multiplayerPreflight)
  throw Error('TH08 multiplayer Runtime requires a room start, preflight or Replay viewer');
 if(!multiplayerRuntime&&options.netplayMode==='lan')throw Error('Network session requires the multiplayer Runtime');
 core.sdl_music_enabled?.(music);app=core.sdl_game_open(multiplayer?multiplayer.seed:Date.now()>>>0);if(!app)throw Error('C++ game initialization failed');
 try{
 const total=core.sdl_prepare_total();for(let i=0;i<total;i++){if(core.sdl_prepare_next()<0)throw Error('资源预载失败 '+i);if(i%12===11){document.querySelector('#loading').textContent='正在准备游戏资源 '+(i+1)+' / '+total;await new Promise(resolve=>setTimeout(resolve,0));}}
 if(replayViewer&&!core.multiplayer_replay_viewer(app))throw Error('TH08 native Replay viewer rejected');
 // Spectator protection precedes native initialize(), which itself writes the
 // config/score namespace. Waiting until the first gameplay frame is too late.
 if(multiplayer?.spectator)await configureMultiplayer(core,app,options,{runtimeBuildWords});
 if(!core.sdl_game_initialize())throw Error('永夜抄初始化失败');document.querySelector('#loading').textContent='';
 if(multiplayer&&!multiplayer.spectator)await configureMultiplayer(core,app,options,{runtimeBuildWords});
 }catch(reason){core.sdl_game_close();app=0;throw reason;}
 if(multiplayer){window.__eaglerNetplayFailed=false;window.__eaglerNetplayError='';updateNetworkDiagnostics(core,app);}
 applyOptions();launched=true;first=false;lastPresented=0;lastHealth=performance.now();lastFrame=0;frames=0;maxGap=0;
 canvas.focus({preventScroll:true});core.sdl_loop_pause(1);if(!document.hidden)await resumeForegroundAudio();if(query.get('manual')!=='1')core.sdl_loop_start();
 emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++',architecture:'eagler-touhou/1',version:'3.4.1-sdl3'});
}
let replayReopening=false;
function replayWords(){const p=core.multiplayer_replay_status(app);return Array.from(new Uint32Array(core.memory.buffer,p,12));}
function replayByteCall(bytes,callback){const p=core.allocate(bytes.length);if(!p)throw Error('Replay allocation failed');
 try{new Uint8Array(core.memory.buffer,p,bytes.length).set(bytes);return callback(p,bytes.length);}finally{core.deallocate(p);}}
async function reopenReplay(path,stage){
 // This is an internal Runtime lifecycle, not a Launcher room command or
 // synthetic playback engine. Reconstruct a fresh native world and leave the
 // parent epoch, filesystem mount and room ownership unchanged.
 const bytes=path?Module.FS.readFile(root()+'/'+storage.relativeSave(path)):null;
 const seed=bytes?replayByteCall(bytes,(p,n)=>core.multiplayer_replay_seed(p,n)):0;
 if(seed===0xffffffff)throw Error('Invalid multiplayer Replay');
 core.sdl_loop_stop();
 window.dispatchEvent(new CustomEvent('touhou-midi-close'));
 app=core.multiplayer_replay_reopen(seed);if(!app)throw Error('Replay native world allocation failed');
 if(bytes){if(!replayByteCall(bytes,(p,n)=>core.multiplayer_replay_load(app,p,n,stage)))throw Error('Replay native bootstrap rejected');}
 else if(!core.multiplayer_replay_viewer(app))throw Error('Replay viewer bootstrap rejected');
 const total=core.sdl_prepare_total();for(let i=0;i<total;++i){
  if(core.sdl_prepare_next()<0)throw Error('Replay resource preparation failed');
  if(i%12===11)await new Promise(resolve=>setTimeout(resolve,0));
 }
 if(!core.sdl_game_initialize())throw Error('Replay native initialization failed');
 core.sdl_keys_clear();applyOptions();replayReopening=false;
 core.sdl_loop_pause(1);if(!document.hidden)await resumeForegroundAudio();
 if(query.get('manual')!=='1')core.sdl_loop_start();
}
async function command(message){
 if(multiplayerRuntime&&launched&&options.netplaySpectator&&
    ['keyboard','thprac-mouse','direct-touch','touch-controls'].includes(message.command))return {};
 switch(message.command){
 case 'configure':if(launched)throw Error('Cannot configure a running game');language=message.language==='lang_zh-hans'?'chs':'jp';options=normalizeOptions(message.options);if(multiplayerRuntime)options.thpracEnabled=false;if(!['ogg','midi','none'].includes(message.music))throw Error('Invalid music mode');Module.touhouMusicMode=message.music;Module.eaglerOptions=options;music=message.music!=='none';await installResources(message.sharedResources);await installResources(message.runtimeResources);await installResources(message.resources);if(message.runtimePack)await installRuntimePack(message.runtimePack);applyOptions();return {};
 case 'resources':if(multiplayerRuntime&&launched)throw Error('Cannot replace resources during a multiplayer run');await installResources(message.resources);return {};
 case 'keyboard':if(launched&&!document.hidden&&!closing)keyboard.event(message,!!message.down,'hosted');return {};
 case 'thprac-mouse':practice?.mouse(message);return {};
 case 'keyboard-clear':clearKeyboard();return {};
 case 'touch-cancel':cancelTouches();return {};
 case 'direct-touch':directTouch(core,canvas,message,{width:innerWidth,height:innerHeight});return {};
 case 'touch-controls':touchControls(core,options,message);return {};
 case 'launch':await launch();return {};
 case 'sync':await save();return {};
 case 'list':{const files=[];for(const dir of ['', '/replay'])for(const name of Module.FS.readdir(root()+dir)){const path=(dir+'/'+name).replace(/^\//,'');try{storage.relativeSave(path);}catch{continue;}const full=root()+'/'+path,s=Module.FS.stat(full);if(Module.FS.isFile(s.mode)){const bytes=Module.FS.readFile(full);files.push({path:multiplayerRuntime?path:exportReplayName(path,bytes,8),size:s.size});}}return {files};}
 case 'read':{let path=storage.relativeSave(message.path);if(!multiplayerRuntime&&path.endsWith('.rpyx'))path=path.slice(0,-1);return {bytes:Array.from(Module.FS.readFile(root()+'/'+path))};}
 case 'write':{if(multiplayerRuntime&&launched)throw Error('Cannot import saves during a multiplayer run');if(!Array.isArray(message.bytes)||message.bytes.length>16*1024*1024||message.bytes.some(b=>!Number.isInteger(b)||b<0||b>255))throw Error('Invalid save bytes');const bytes=new Uint8Array(message.bytes),relative=storage.relativeSave(message.path);
  if(multiplayerRuntime&&relative.startsWith('replay/')&&(!relative.endsWith('.rpyx')||!replayByteCall(bytes,(p,n)=>core.multiplayer_replay_validate(p,n))))throw Error('Invalid TH08 multiplayer Replay');
  const path=multiplayerRuntime?relative:importReplayName(relative,bytes,8);Module.FS.writeFile(root()+'/'+path,bytes);await sync(false);return {};}
 case 'remove':{if(multiplayerRuntime&&launched)throw Error('Cannot remove saves during a multiplayer run');let path=storage.relativeSave(message.path);if(!multiplayerRuntime&&path.endsWith('.rpyx'))path=path.slice(0,-1);Module.FS.unlink(root()+'/'+path);await sync(false);return {};}
 default:throw Error('Unsupported runtime command: '+message.command);
 }
}
let queue=Promise.resolve();
async function dispatchCommand(m){
 try{const result=await command(m);if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:true,...result},location.origin);}
 catch(e){if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:false,error:String(e),errno:e?.errno},location.origin);else error(e);}
}
window.addEventListener('message',event=>{
 const m=event.data;if(!validEpoch||event.source!==parent||event.origin!==location.origin||m?.protocol!==protocol||m.game!==game||m.epoch!==epoch||typeof m.command!=='string')return;
 // Device events must not wait behind filesystem/resource operations. Their
 // synchronous command bodies observe the current owner and lifecycle now.
 if(m.command==='keyboard'||m.command==='keyboard-clear'){void dispatchCommand(m);return;}
 queue=queue.then(async()=>{if(await initialized===false)return;await dispatchCommand(m);}).catch(error);
});
document.addEventListener('visibilitychange',()=>{if(!core||!launched)return;clearKeyboard();cancelTouches();if(document.hidden){suspendRuntimeAudio(Module,core);queue=queue.then(persist).catch(error);}else void resumeForegroundAudio(true);});
window.addEventListener('blur',()=>{if(core){practice?.clear();clearKeyboard();cancelTouches();}});
window.addEventListener('pagehide',()=>{clearKeyboard();cancelTouches();removeResourceTraceHostButton();if(core&&launched){suspendRuntimeAudio(Module,core);void persist().catch(console.error);}});
window.addEventListener('pageshow',()=>{if(core&&launched&&!document.hidden)void resumeForegroundAudio(true);});
canvas.addEventListener('webglcontextlost',event=>{event.preventDefault();clearKeyboard();core?.sdl_loop_pause(1);error('图形环境已失效，请退出后重新开始。');});
for(const name of ['pointerdown','keydown'])window.addEventListener(name,()=>{if(Module?.SDL3?.audioContext?.state!=='running')void resumeForegroundAudio(true);},{capture:true});
for(const name of ['keydown','keyup'])window.addEventListener(name,event=>{
 if(!core||!launched||closing||document.hidden)return;
 const handled=keyboard.event(event,name==='keydown');
 if(handled&&options.thpracEnabled&&/^(Backspace|Tab|F[1-7]|F12)$/.test(event.code))event.preventDefault();
},{capture:true});
const initialized=(async()=>{
 if(!Number.isSafeInteger(epoch)||epoch<=0)throw Error('Invalid runtimeEpoch navigation binding');
 if(multiplayerRuntime){
  const manifest=await fetch('./manifest.json').then(response=>{if(!response.ok)throw Error('Missing TH08 Runtime manifest');return response.json();});
  const directory=new URL('.',location.href).pathname.split('/').filter(Boolean).at(-1)||'';
  const sha=/^[a-f0-9]{64}$/i.test(directory)?directory:manifest?.execution?.sha256;
  runtimeBuildIdentity=sha;runtimeWasmIdentity=manifest?.execution?.sha256||'';
  if(typeof sha!=='string'||!/^[a-f0-9]{64}$/i.test(sha))throw Error('Invalid TH08 Runtime build identity');
  runtimeBuildWords=Array.from({length:4},(_,i)=>Number.parseInt(sha.slice(i*8,i*8+8),16)>>>0);
  if(!runtimeBuildWords.some(Boolean))throw Error('Empty TH08 Runtime build identity');
 }
 let audioContext;try{audioContext=parent.__touhouAudioContext||parent.__th10AudioContext;}catch{}
 Module=await createModule({canvas,noInitialRun:true,resetBrowserKeyboard:()=>keyboard.clear(),...(audioContext?{SDL3:{audioContext}}:{}),print:console.log,printErr:console.error,
  instantiateWasm(imports,ready){return WebAssembly.instantiateStreaming(fetch('./th08-sdl.wasm'),imports).then(({instance,module})=>{core=instance.exports;ready(instance,module);return core;});}
 });
 storage=await initializeSaveStorage({game,runtimeVariant,setCompiledVariant:value=>core.sdl_files_variant(value),filesystem:Module.FS,idbfs:Module.IDBFS,sync,
  beforeMount(){window.Module=Module;window.FS=Module.FS;observeMusicWrites(Module,core,game);}});
 practice=createPractice({core,getApp:()=>app,canvas,clearKeys:clearKeyboard,setMusic:value=>core.sdl_music_enabled(value),setPaused:value=>core.sdl_loop_pause(value||document.hidden?1:0)});
 Module.FS.mkdirTree(storage.namespace+'/replay');await migrateSaves();await mountData();cstring('#screen',core.sdl_canvas);
 Module.runtimePrepare=()=>!document.hidden;
 Module.runtimeFinish=(result,duration)=>{
  if(replayReopening)return;
  if(traceRequested&&!traceStarted&&app)traceStart();
  if(multiplayerRuntime&&app&&options.replayViewer&&core.multiplayer_replay_status){
   const state=replayWords(),pointer=core.multiplayer_replay_request(app);
   const memory=new Uint8Array(core.memory.buffer);let end=pointer;while(pointer&&end<memory.length&&memory[end]&&end-pointer<512)++end;
   const path=pointer?new TextDecoder().decode(memory.subarray(pointer,end)):'';
   if(path||state[9]){replayReopening=true;core.sdl_loop_pause(1);
    queue=queue.then(()=>reopenReplay(path,state[10])).catch(error);return;}
  }
  practice.tick();
  if(multiplayerRuntime)updateNetworkDiagnostics(core,app);
  if(multiplayerRuntime&&window.__eaglerNetplaySpectatorFinished&&!closing){
   // Admission belongs to the observed run. Close this Runtime, not the
   // parent-owned room; a new generation requires a new lobby admission.
   core.sdl_loop_pause(1);queueMicrotask(()=>void stop().catch(error));return;
  }
  const now=performance.now(),p=u32(core.sdl_stats(),6)[5];if(p!==lastPresented){frames++;if(lastFrame)maxGap=Math.max(maxGap,now-lastFrame);lastFrame=now;lastPresented=p;if(!first){first=true;emit('first-frame');}}
  if(result||status()[2]){if(status()[2]){error((multiplayerRuntime&&networkError(core,app))||'Game error '+status()[2]);core.sdl_loop_pause(1);}else queueMicrotask(()=>void stop().catch(error));}
  if(now-lastHealth>=1000){emit('frame-health',{fps:frames*1000/(now-lastHealth),maxGapMs:maxGap,frameMs:duration});const a=u32(core.sdl_audio_stats(),12);emit('audio-health',{queuedMs:a[5]*1000/44100,minQueuedMs:a[7]*1000/44100,backend:'script',underruns:0,robust:true});frames=0;maxGap=0;lastHealth=now;}
 };
 Module.runtimeFileChanged=()=>{if(saveTimer!==null)return;saveTimer=setTimeout(()=>{saveTimer=null;queue=queue.then(()=>sync(false)).catch(error);},0);};
 Module.runtimeStopped=()=>{};Module.runtimeNotice=message=>emit('notice',{message});Module.runtimeMidi=(op,data)=>{if(op===62&&music&&Module.touhouMusicMode==='midi')window.dispatchEvent(new CustomEvent('touhou-midi',{detail:{bytes:Array.from(data||[])}}));if(op===61)window.dispatchEvent(new CustomEvent('touhou-midi-close'));};Module.callMain=launch;
 window.__th08Runtime={core,Module,get app(){return app;},status,launch,stop,command,resourceTrace:{start:traceStart,read:traceRead,export:exportResourceTrace}};
 installResourceTrace();
 emit('ready');
})().catch(e=>{if(isSupersededRuntimeError(e)){console.debug('Runtime navigation superseded');return false;}error(e);throw e;});
