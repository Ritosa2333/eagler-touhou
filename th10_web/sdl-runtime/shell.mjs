// Platform shell for the upstream eagler-touhou/1 Launcher contract.
// Game construction, input, timing, rendering, text and sound belong to C++.
import createModule from './th10-sdl.mjs';
import {createOptionalPractice} from './practice-loader.mjs';
import {bindOutsideTouches} from './eagler-host.mjs';
import {createReplayFilePolicy} from './replay-file-policy.mjs';
import {normalizeOptions,applyTouchOptions,touchControls,suspendRuntimeAudio,resumeRuntimeAudio,directTouch,ensureSharedFontAlias,installResources as installHostResources,observeMusicWrites,mountManagedData,isSupersededRuntimeError} from './eagler-host.mjs';
import {initializeSaveStorage,migrateLegacySaves} from './save-storage.mjs';
const protocol='eagler-touhou/1',game='th10',query=new URLSearchParams(location.search),canvas=document.querySelector('canvas');
const runtimeVariant=query.get('runtimeVariant')??'normal',multiplayerRuntime=runtimeVariant==='multiplayer';
const epoch=Number(query.get('runtimeEpoch'));
const validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...fields},location.origin);
let Module,core,app=0,launched=false,first=false,closing=false,language=query.get('language')==='lang_zh-hans'?'chs':'jp',options={},music=true;
let practice;
let frames=0,lastHealth=0,lastFrame=0,maxGap=0,lastPresented=0,saveTimer=null;
const cancelTouches=bindOutsideTouches(document,canvas,()=>core,()=>launched&&options.touchEnabled);
const error=reason=>{const message=reason?.stack||String(reason);if(multiplayerRuntime){window.__eaglerNetplayFailed=true;window.__eaglerNetplayError=message;}document.querySelector('#error').textContent=message;emit('error',{message,error:message});console.error(reason);};
const u32=(ptr,count)=>new Uint32Array(core.memory.buffer,ptr,count);
const cstring=(text,fn)=>{const bytes=new TextEncoder().encode(text+'\0'),p=core.graphics_allocate(bytes.length);try{new Uint8Array(core.memory.buffer,p,bytes.length).set(bytes);return fn(p);}finally{core.graphics_free(p);}};
const readCString=ptr=>{if(!ptr)return '';const heap=new Uint8Array(core.memory.buffer);let end=ptr;while(end<heap.length&&heap[end])++end;return new TextDecoder().decode(heap.subarray(ptr,end));};
const netplayHashes=Object.create(null);
function updateNetplayDiagnostics(){
 if(!multiplayerRuntime||!app||!core.multiplayer_netplay_status)return;
 const state=Array.from(new Int32Array(core.memory.buffer,core.multiplayer_netplay_status(app),11));
 const transport=Array.from(new Uint32Array(core.memory.buffer,core.multiplayer_transport_status(app),15));
 const frame=state[3];
 window.__eaglerNetplayLanActive=state[9]===1;
 window.__eaglerNetplayLanFrame=frame;
 window.__eaglerNetplayLanConfirmed=state[4]>=0?state[4]:undefined;
 window.__eaglerNetplayTransport=transport[13]===1?'rtc':transport[13]===2?'relay':transport[13]===3?'spectator':'';
 const lifecycle=new Int32Array(core.memory.buffer,core.multiplayer_lifecycle_status(app),12);
 window.__eaglerNetplayLanRollback=lifecycle[8];
 window.__eaglerNetplayLanResimulated=lifecycle[9];
 if(frame>=0&&frame<=1000000&&state[9]===1){
  const hash=new Uint32Array(core.memory.buffer,core.multiplayer_portable_hashes(app),12);
  if(hash[0]===1){netplayHashes[String(frame)]=String(hash[1]>>>0);delete netplayHashes[String(frame-512)];}
 }
 window.__eaglerNetplayLanHashes=netplayHashes;
}
async function configureNetplay(){
 if(!multiplayerRuntime||options.netplayMode!=='lan')return;
 const url=new URL(options.netplayUrl),room=url.searchParams.get('room'),run=url.searchParams.get('run');
 const count=options.netplayPlayerCount,seat=options.netplaySpectator?0:options.netplayPlayer;
 const loadouts=options.netplayLoadouts;
 if(!['ws:','wss:'].includes(url.protocol)||!room||!run||![2,3].includes(count)||
    !Number.isInteger(seat)||seat<0||seat>=count||
    !Number.isInteger(options.netplaySeed)||options.netplaySeed<0||options.netplaySeed>65535||
    !Number.isInteger(options.netplayDifficulty)||options.netplayDifficulty<0||options.netplayDifficulty>4||
    !Array.isArray(loadouts)||loadouts.length!==count)throw Error('Invalid TH10 multiplayer options');
 const identity=new TextEncoder().encode(`th10mp:${url.origin}${url.pathname}:${room}:${run}`);
 const digest=new DataView(await crypto.subtle.digest('SHA-256',identity));
 const low=digest.getUint32(0,true),high=digest.getUint32(4,true)||1;
 const words=[2,count,seat,options.netplayDifficulty,options.netplaySeed,low,high];
 for(let i=0;i<3;i++){
  const value=loadouts[i]||{character:0,shot:0};
  if(!Number.isInteger(value.character)||value.character<0||value.character>1||
     !Number.isInteger(value.shot)||value.shot<0||value.shot>2)throw Error('Invalid TH10 multiplayer loadout');
  words.push(value.character,value.shot);
 }
 const pointer=core.files_allocate(words.length*4);
 try{
  new Uint32Array(core.memory.buffer,pointer,words.length).set(words);
  if(!core.multiplayer_configure(app,pointer,words.length))throw Error('TH10 multiplayer session rejected');
 }finally{core.files_free(pointer);}
 const connected=options.netplaySpectator
  ?cstring(options.netplayUrl,relay=>cstring(options.netplaySpectatorId,id=>core.multiplayer_spectator_connect(app,relay,id)))
  :cstring(options.netplayUrl,relay=>core.multiplayer_connect(app,relay));
 if(!connected)throw Error('TH10 multiplayer transport rejected');
 window.__eaglerNetplayFailed=false;window.__eaglerNetplayError='';
 updateNetplayDiagnostics();
}
const replayFiles=createReplayFilePolicy({game:10,multiplayer:multiplayerRuntime,validateMultiplayer(bytes){
 if(typeof core.multiplayer_replay_validate!=='function')throw Error('Multiplayer Replay capability is missing');
 const p=core.files_allocate(bytes.length);if(!p)throw Error('Replay allocation failed');
 try{new Uint8Array(core.memory.buffer,p,bytes.length).set(bytes);return core.multiplayer_replay_validate(p,bytes.length)===1;}
 finally{core.files_free(p);}
}});
let storage;
const root=()=>storage.root(language);
let storageSync=Promise.resolve();
const sync=populate=>{const current=storageSync.then(()=>new Promise((resolve,reject)=>Module.FS.syncfs(populate,e=>e?reject(e):resolve())));storageSync=current.catch(()=>{});return current;};
async function migrateSaves(){
 await migrateLegacySaves(storage,{indexedDB,filesystem:Module.FS,sync,importReplayName:(path,bytes)=>replayFiles.imported(path,bytes)});
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
    !Array.isArray(manifest.files)||manifest.files.length>256)throw Error('Invalid TH10 language pack manifest');
 for(const file of manifest.files)
  if(typeof file?.path!=='string'||!file.path.startsWith('/thcrap/th10/')||file.path.includes('\\')||file.path.includes('..')||
     !Number.isInteger(file.bytes)||file.bytes<0)throw Error('Invalid TH10 language pack file');
}
async function installRuntimePack(pack){
 if(launched)throw Error('Runtime resources cannot be changed after launch');
 if(typeof pack?.url!=='string'||typeof pack.language!=='string'||
    !Number.isInteger(pack.bytes)||pack.bytes<=0||
    !pack.manifest||!Array.isArray(pack.files))throw Error('Invalid TH10 language pack');
 const url=new URL(pack.url,location.href);
 if(url.origin!==location.origin)throw Error('Cross-origin TH10 language pack');
 assertRuntimePackManifest(pack.manifest,pack);
 const expected=new Map(pack.manifest.files.map(file=>[file.path,file]));
 if(pack.files.length!==expected.size)throw Error('TH10 language pack file count mismatch');
 const verified=[];
 for(const file of pack.files){
  if(typeof file?.path!=='string'||!file.path.startsWith('/thcrap/th10/')||file.path.includes('\\')||file.path.includes('..')||
     !(file.bytes instanceof Uint8Array))throw Error('Invalid TH10 language pack path');
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
function applyOptions(){applyTouchOptions(core,options);if(app)core.application_touch_display?.(app,options.alwaysHitbox?1:0);practice?.configure(options);}
function status(){return Array.from(new Int32Array(core.memory.buffer,core.sdl_game_status(),10));}
// The native Replay owner decides whether it is still reconstructing the
// selected stage. This overlay reports that state; it never skips input or
// substitutes a Launcher-owned stage/Replay cursor.
let replaySeekOverlay=null;
function updateReplaySeek(){
 if(!multiplayerRuntime||!core?.multiplayer_replay_status)return;
 const s=app?u32(core.multiplayer_replay_status(app),13):null;
 const seeking=s?.[0]===1&&s[8]===1&&s[11]===1;
 if(!replaySeekOverlay&&!seeking)return;
 if(!replaySeekOverlay){
  replaySeekOverlay=document.createElement('output');replaySeekOverlay.id='replay-seek';
  replaySeekOverlay.setAttribute('role','progressbar');replaySeekOverlay.setAttribute('aria-valuemin','0');replaySeekOverlay.setAttribute('aria-valuemax','100');
  replaySeekOverlay.style.cssText='position:fixed;inset:0;z-index:20;place-content:center;text-align:center;background:rgba(0,0,0,.82);color:white;font:16px sans-serif;pointer-events:none;white-space:pre-line';
  document.body.appendChild(replaySeekOverlay);
 }
 replaySeekOverlay.hidden=!seeking;replaySeekOverlay.style.display=seeking?'grid':'none';
 if(seeking){const percent=Math.min(100,Math.floor(s[4]*100/(s[7]+1)));
  replaySeekOverlay.setAttribute('aria-valuenow',String(percent));
  replaySeekOverlay.textContent=language==='chs'?`正在定位至第 ${s[12]} 关 · ${percent}%\nEsc 退出录像`:`Seeking to Stage ${s[12]} · ${percent}%\nEsc to exit Replay`;
 }
}
function save(){if(app)core.application_save(app);return sync(false);}
async function resumeForegroundAudio(forcePause=false){
 if(!core||!launched||document.hidden)return false;
 if(forcePause)core.sdl_loop_pause(1);
 return resumeRuntimeAudio(Module,core,()=>!!core&&launched&&!document.hidden);
}
async function stop(){if(closing)return;closing=true;try{practice?.close();core.sdl_loop_stop();await save();core.sdl_game_close();await sync(false);app=0;launched=false;updateReplaySeek();emit('exit',{code:0,status:'success'});}finally{closing=false;}}
async function launch(){
 if(launched)return;
 ensureSharedFontAlias(Module,language);
 const mode=Module.touhouMusicMode||'none';music=mode!=='none'&&mode!=='midi';core.sdl_ogg_decode_mode?.(options.oggDecodeMode==='full');
 core.sdl_music_enabled?.(music);app=core.sdl_game_open(false,options.netplayMode==='lan'?options.netplaySeed:Date.now()&65535);if(!app)throw Error('C++ game initialization failed');
 try{await configureNetplay();}catch(reason){core.sdl_game_close();app=0;throw reason;}
 applyOptions();launched=true;first=false;lastPresented=0;lastHealth=performance.now();lastFrame=0;frames=0;maxGap=0;
 canvas.focus({preventScroll:true});core.sdl_loop_pause(1);if(!document.hidden)void resumeForegroundAudio();core.sdl_loop_start(app);
 emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++',architecture:'eagler-touhou/1',version:'3.5.1-sdl3'});
}
async function command(message){
 switch(message.command){
 case 'configure':if(launched)throw Error('Cannot configure a running game');language=message.language==='lang_zh-hans'?'chs':'jp';options=normalizeOptions(message.options);if(!['ogg','midi','none'].includes(message.music))throw Error('Invalid music mode');Module.touhouMusicMode=message.music;Module.eaglerOptions=options;music=message.music!=='none';await installResources(message.sharedResources);await installResources(message.runtimeResources);await installResources(message.resources);if(message.runtimePack)await installRuntimePack(message.runtimePack);applyOptions();return {};
 case 'resources':await installResources(message.resources);return {};
 case 'keyboard':{const code=runtimeKeyboardCode(message);if(!code)return {};if(!practice?.key(code,!!message.down))cstring(code,p=>core.sdl_key(p,!!message.down));return {};}
 case 'thprac-mouse':practice?.mouse(message);return {};
 case 'keyboard-clear':practice?.clear();core.sdl_keys_clear();return {};
 case 'touch-cancel':cancelTouches();return {};
 case 'direct-touch':directTouch(core,canvas,message,{width:innerWidth,height:innerHeight});return {};
 case 'touch-controls':touchControls(core,options,message);return {};
 case 'launch':await launch();return {};
 case 'sync':await save();return {};
 case 'list':{const files=[];for(const dir of ['', '/replay'])for(const name of Module.FS.readdir(root()+dir)){const path=(dir+'/'+name).replace(/^\//,'');try{storage.relativeSave(path);}catch{continue;}const full=root()+'/'+path,s=Module.FS.stat(full);if(Module.FS.isFile(s.mode)){const bytes=Module.FS.readFile(full);files.push({path:replayFiles.exported(path,bytes),size:s.size});}}return {files};}
 case 'read':{const path=replayFiles.physical(storage.relativeSave(message.path));return {bytes:Array.from(Module.FS.readFile(root()+'/'+path))};}
 case 'write':{if(!Array.isArray(message.bytes)||message.bytes.length>16*1024*1024||message.bytes.some(b=>!Number.isInteger(b)||b<0||b>255))throw Error('Invalid save bytes');const bytes=new Uint8Array(message.bytes),path=replayFiles.imported(storage.relativeSave(message.path),bytes);
  if(multiplayerRuntime&&/\.rpy$/.test(path)){const target=root()+'/'+path,temporary=target+'.pending';
   try{Module.FS.writeFile(temporary,bytes);Module.FS.rename(temporary,target);}catch(error){try{Module.FS.unlink(temporary);}catch{}throw error;}
  }else Module.FS.writeFile(root()+'/'+path,bytes);
  await sync(false);return {};}
 case 'remove':{const path=replayFiles.physical(storage.relativeSave(message.path));Module.FS.unlink(root()+'/'+path);await sync(false);return {};}
 default:throw Error('Unsupported runtime command: '+message.command);
 }
}
function runtimeKeyboardCode(message){
 const code=String(message.code||'');if(code&&code!=='Unidentified')return code;
 const key=String(message.key||'').toLowerCase(),location=Number(message.location)||0;
 const byKey={z:'KeyZ',x:'KeyX',shift:location===2?'ShiftRight':'ShiftLeft',escape:'Escape',esc:'Escape',arrowup:'ArrowUp',arrowdown:'ArrowDown',arrowleft:'ArrowLeft',arrowright:'ArrowRight',control:location===2?'ControlRight':'ControlLeft',q:'KeyQ',s:'KeyS',home:'Home',enter:location===3?'NumpadEnter':'Enter',d:'KeyD',r:'KeyR',tab:'Tab',backspace:'Backspace'};
 if(byKey[key])return byKey[key];if(/^f(?:[1-7]|12)$/.test(key))return key.toUpperCase();
 const keyCode=Number(message.keyCode)||0,byCode={8:'Backspace',9:'Tab',13:location===3?'NumpadEnter':'Enter',16:location===2?'ShiftRight':'ShiftLeft',17:location===2?'ControlRight':'ControlLeft',27:'Escape',36:'Home',37:'ArrowLeft',38:'ArrowUp',39:'ArrowRight',40:'ArrowDown',68:'KeyD',81:'KeyQ',82:'KeyR',83:'KeyS',88:'KeyX',90:'KeyZ',112:'F1',113:'F2',114:'F3',115:'F4',116:'F5',117:'F6',118:'F7',123:'F12'};
 return byCode[keyCode]||'';
}
let queue=Promise.resolve();
window.addEventListener('message',event=>{const m=event.data;if(!validEpoch||event.source!==parent||event.origin!==location.origin||m?.protocol!==protocol||m.game!==game||m.epoch!==epoch||typeof m.command!=='string')return;
 queue=queue.then(async()=>{if(await initialized===false)return;try{const result=await command(m);if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:true,...result},location.origin);}catch(e){if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:false,error:String(e),errno:e.errno},location.origin);else error(e);}}).catch(error);
});
document.addEventListener('visibilitychange',()=>{if(!core||!launched)return;core.sdl_keys_clear();cancelTouches();if(document.hidden){suspendRuntimeAudio(Module,core);queue=queue.then(save).catch(error);}else void resumeForegroundAudio(true);});
window.addEventListener('blur',()=>{if(core){practice?.clear();core.sdl_keys_clear();cancelTouches();}});
window.addEventListener('pagehide',()=>{cancelTouches();if(core&&launched){suspendRuntimeAudio(Module,core);void save().catch(console.error);}});
window.addEventListener('pageshow',()=>{if(core&&launched&&!document.hidden)void resumeForegroundAudio(true);});
canvas.addEventListener('webglcontextlost',event=>{event.preventDefault();core?.sdl_loop_pause(1);error('图形环境已失效，请退出后重新开始。');});
for(const name of ['pointerdown','keydown'])window.addEventListener(name,()=>{if(Module?.SDL3?.audioContext?.state!=='running')void resumeForegroundAudio(true);},{capture:true});
for(const name of ['keydown','keyup'])window.addEventListener(name,event=>{
 if(options.thpracEnabled&&practice?.key(event.code,name==='keydown'))event.preventDefault();
},{capture:true});
const initialized=(async()=>{
 let audioContext;try{audioContext=parent.__touhouAudioContext||parent.__th10AudioContext;}catch{}
 Module=await createModule({canvas,noInitialRun:true,...(audioContext?{SDL3:{audioContext}}:{}),print:console.log,printErr:console.error,
  instantiateWasm(imports,ready){return WebAssembly.instantiateStreaming(fetch('./th10-sdl.wasm'),imports).then(({instance,module})=>{core=instance.exports;ready(instance,module);return core;});}
 });
 storage=await initializeSaveStorage({game,runtimeVariant,setCompiledVariant:value=>core.sdl_files_variant(value),filesystem:Module.FS,idbfs:Module.IDBFS,sync,
  beforeMount(){window.Module=Module;window.FS=Module.FS;observeMusicWrites(Module,core,game);}});
 practice=await createOptionalPractice({core,getApp:()=>app,canvas,clearKeys:()=>core.sdl_keys_clear(),setMusic:value=>core.sdl_music_enabled(value),setPaused:value=>core.sdl_loop_pause(value||document.hidden?1:0)});
 for(const lang of ['jp','chs'])Module.FS.mkdirTree(storage.namespace+'/'+lang+'/replay');await migrateSaves();await mountData();cstring('#screen',core.sdl_canvas);
 Module.runtimePrepare=()=>!document.hidden;
 Module.runtimeFinish=(result,duration)=>{
  updateReplaySeek();
  practice?.tick();
  updateNetplayDiagnostics();
  const now=performance.now(),p=u32(core.sdl_stats(),6)[5];if(p!==lastPresented){frames++;if(lastFrame)maxGap=Math.max(maxGap,now-lastFrame);lastFrame=now;lastPresented=p;if(!first){first=true;emit('first-frame');}}
  if(result||core.application_error(app)){const code=core.application_error(app);if(code){let message='Game error '+code;if(multiplayerRuntime&&(code===-4||code===-5)){const describe=core.multiplayer_error_detail||(code===-5?core.multiplayer_network_error:null);const detail=describe?readCString(describe(app)):'';if(detail)message+=': '+detail;}error(message);core.sdl_loop_pause(1);}else queueMicrotask(()=>void stop().catch(error));}
  if(now-lastHealth>=1000){emit('frame-health',{fps:frames*1000/(now-lastHealth),maxGapMs:maxGap,frameMs:duration});const a=u32(core.sdl_audio_stats(),12);emit('audio-health',{queuedMs:a[5]*1000/44100,minQueuedMs:a[7]*1000/44100,backend:'script',underruns:0,robust:true});frames=0;maxGap=0;lastHealth=now;}
 };
 Module.runtimeFileChanged=()=>{if(saveTimer!==null)return;saveTimer=setTimeout(()=>{saveTimer=null;queue=queue.then(()=>sync(false)).catch(error);},0);};
 Module.runtimeStopped=()=>{};Module.callMain=launch;
 window.__th10Runtime={core,Module,get app(){return app;},status,launch,stop,command};
 emit('ready');
})().catch(e=>{if(isSupersededRuntimeError(e)){console.debug('Runtime navigation superseded');return false;}error(e);throw e;});
