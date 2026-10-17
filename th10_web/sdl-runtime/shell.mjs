// Platform shell for the upstream eagler-touhou/1 Launcher contract.
// Game construction, input, timing, rendering, text and sound belong to C++.
import {createBrowserKeyboard} from './directory-keyboard.mjs';
import {installStartupBranding} from './startup-branding.mjs';
import createModule from './th10-sdl.mjs';
import {createOptionalPractice} from './practice-loader.mjs';
import {bindOutsideTouches} from './eagler-host.mjs';
import {createReplayFilePolicy} from './replay-file-policy.mjs';
import {normalizeOptions,applyTouchOptions,touchControls,suspendRuntimeAudio,resumeRuntimeAudio,directTouch,ensureSharedFontAlias,installResources as installHostResources,observeMusicWrites,mountManagedData,isSupersededRuntimeError} from './eagler-host.mjs';
import {initializeSaveStorage,migrateLegacySaves} from './save-storage.mjs';
import {createSaveSync} from './save-sync.mjs';
// BEGIN GENERATED LOCAL REPLAY
// Generated from eagler-common/browser/local-replay*.mjs.
// The native adapter owns parsing, stage bootstrap, fixed ticks and EOF.
// This host owns only asynchronous command ordering and presentation pacing.
function createLocalReplay({inspect,open,step,state,present=()=>{},deferPresentation=()=>false,stopLoop,prepare=async()=>{},onState=()=>{},onError=()=>{},raf=fn=>requestAnimationFrame(fn),cancel=id=>cancelAnimationFrame(id),defer,cancelDeferred,now=()=>performance.now(),hidden=()=>document.hidden}) {
  let selected=null,path='',paused=false,rate=1,target=null,startFrame=0,last=null,credit=0,animation=0,continuation=0,closed=false,lastReport=-Infinity;
  let channel=null,pendingTask=null,taskSerial=0;
  function soon(fn){
    if(defer)return defer(fn);
    // Seeking is CPU work, independent of the display refresh rate. Message
    // tasks yield to input/painting between bounded slices without the nested
    // setTimeout clamp, or waiting a whole RAF interval after every slice.
    if(!channel){channel=new MessageChannel();channel.port1.onmessage=event=>{const task=pendingTask;if(task?.id!==event.data)return;pendingTask=null;task.fn(now());};channel.port1.unref?.();channel.port2.unref?.();}
    const id=++taskSerial;pendingTask={id,fn};channel.port2.postMessage(id);return id;
  }
  function cancelScheduled(){if(animation)cancel(animation);if(continuation){if(cancelDeferred)cancelDeferred(continuation);else pendingTask=null;}animation=continuation=0;}
  function report(){if(!selected)return;const native=state();onState({kind:'replay',path,difficulty:selected.difficulty,...native,startFrame,paused,rate,seeking:target!==null,target,description:selected});}
  function schedule(){if(animation||continuation||closed)return;if(target!==null&&!hidden())continuation=soon(draw);else animation=raf(draw);}
  function draw(time){animation=continuation=0;if(closed||!selected)return;
    try {
      if(hidden()){last=null;schedule();return;}
      const delta=last===null?0:Math.min(.25,Math.max(0,(time-last)/1000));last=time;
      const unlimited=target!==null||rate===0;
      credit=Math.min(240,credit+delta*60*rate);
      const deferred=unlimited&&deferPresentation();
      const allowed=paused&&target===null?0:unlimited?4096:Math.floor(credit+1e-9),start=now();let count=0,shown=true,current=state();
      while(count<allowed&&!current.ended&&(count===0||now()-start<8)) {
        if(target!==null&&current.frame>=target&&!current.preparing)break;
        shown=!deferred&&(count===0||count+1===allowed||target!==null&&!current.preparing&&current.frame+1>=target);
        if(step(shown)===false)throw Error('录像原生执行失败');
        count++;current=state();
      }
      // Adapters may defer only the window swap, while keeping every native
      // Calc/Draw and capture operation. Publish the latest complete frame.
      if(count&&!shown)present();
      if(!unlimited)credit=Math.max(0,credit-count);
      if(target!==null&&!current.preparing&&(current.frame>=target||current.ended)){target=null;last=null;credit=0;report();}
      if(time-lastReport>=100){lastReport=time;report();}schedule();
    }catch(error){paused=true;target=null;report();onError(error);}
  }
  const reset=()=>{last=null;credit=0;target=null;};
  async function restart(nextPath,stage){
    // Keep the title's startup loop alive until native menu initialization is
    // complete. The startup picture's first-frame notification is too early.
    await prepare();if(closed)throw Error('录像播放器已关闭');
    cancelScheduled();stopLoop();
    try{await open(nextPath,stage);}catch(error){selected=null;reset();onState(null);throw error;}
    if(closed)throw Error('录像播放器已关闭');
    reset();startFrame=state().bootstrapTarget??0;target=startFrame||state().preparing?startFrame:null;
  }
  return {
    async command(message){
      if(closed)throw Error('录像播放器已关闭');
      const {action,value}=message;
      if(action==='inspect')return {description:await inspect(message.path)};
      if(action==='open'){
        const next=await inspect(message.path),stage=message.stage??next.stages[0]?.id;
        if(!next.stages.some(item=>item.id===stage))throw Error('录像没有此关卡');
        await restart(message.path,stage);path=message.path;selected=next;paused=false;schedule();
      }else {
        if(!selected)throw Error('请先选择录像关卡');
        if(action==='pause')paused=true;
        else if(action==='play'){if(state().ended)await restart(path,state().startStage);paused=false;}
        else if(action==='rate'){if(![0,.5,1,2,4,8].includes(value))throw Error('播放速度无效');rate=value;}
        else if(action==='seek'){
          const current=state();if(!Number.isSafeInteger(value)||value<startFrame||value>current.total)throw Error('录像位置无效');
          if(value<current.frame||current.ended)await restart(path,current.startStage);
          reset();target=value;
        }else throw Error('不支持的录像操作');
        last=null;credit=0;cancelScheduled();schedule();
      }
      report();return {};
    },
    close(){closed=true;cancelScheduled();channel?.port1.close();channel?.port2.close();selected=null;},
    get active(){return !!selected&&!closed;},
  };
}


// Replay workspaces expose one optional folder level. Keep native IDBFS as the
// writer and do not walk arbitrary save/resource subdirectories.
function listLocalReplayFiles(fs,root,validate,describe=(path)=>({path})) {
  const directories=['','/replay','/hint','/autosave'],files=[];
  if(fs.analyzePath(root+'/replay').exists)for(const name of fs.readdir(root+'/replay')){
    if(name==='.'||name==='..'||/[\\/\x00-\x1f]/.test(name))continue;
    if(fs.isDir(fs.stat(root+'/replay/'+name).mode))directories.push('/replay/'+name);
  }
  for(const directory of directories){const full=root+directory;if(!fs.analyzePath(full).exists)continue;
    for(const name of fs.readdir(full)){const path=(directory+'/'+name).replace(/^\//,'');try{validate(path);}catch{continue;}
      const stat=fs.stat(root+'/'+path);if(fs.isFile(stat.mode))files.push({...describe(path),size:stat.size,modifiedAt:stat.mtime?.getTime()});
    }
  }
  return files;
}
// Adapt the small native ABI without teaching the host any Replay file format.
function createNativeLocalReplay({core: getCore,module: getModule,physical,openReplay,stepReplay,replayState,stopLoop,onState,onError}) {
  const api=key=>{const c=getCore();return c[key]??c['_'+key];};
  const heap=()=>getModule().HEAPU8??new Uint8Array(getCore().memory.buffer);
  const text=p=>{if(!p)throw Error('录像无法解析');const b=heap(),end=b.indexOf(0,p);if(end<0||end-p>65536)throw Error('录像信息无效');return new TextDecoder().decode(b.subarray(p,end));};
  // Do not put a NUL escape in an inline shell string. HTML optimizers may
  // materialize it and the HTML parser replaces that byte with U+FFFD.
  const cbytes=value=>{const text=new TextEncoder().encode(value),bytes=new Uint8Array(text.length+1);bytes.set(text);return bytes;};
  function bytesCall(bytes,fn){
    const c=getCore(),alloc=api('eagler_replay_allocate')??api('malloc')??c.allocate??c.graphics_allocate,free=api('eagler_replay_free')??api('free')??c.deallocate??c.release??c.graphics_free;
    const p=alloc(bytes.length);if(!p)throw Error('录像内存不足');
    try{heap().set(bytes,p);return fn(p,bytes.length);}finally{free(p);}
  }
  const bridge=createLocalReplay({
    async prepare(){const ready=api('eagler_replay_ready');if(!ready)return;const deadline=performance.now()+30000;
      while(!ready()){if(performance.now()>deadline)throw Error('录像播放器初始化超时');await new Promise(requestAnimationFrame);}},
    inspect(path){const p=physical(path),fn=api('eagler_replay_inspect');if(!fn)throw Error('此 Runtime 需要更新后才能解析录像');
      const bytes=getModule().FS.readFile(p);if(!bytes.length||bytes.length>64*1024*1024)throw Error('录像大小无效');
      return bytesCall(bytes,(ptr,size)=>bytesCall(cbytes(p),name=>{const d=JSON.parse(text(fn(ptr,size,name)));
        if(Array.isArray(d.nameBytes))d.name=new TextDecoder('shift_jis').decode(new Uint8Array(d.nameBytes)).replace(/\0.*$/,'').trim();return d;}));},
    open(path,stage){if(openReplay)return openReplay(path,stage);const bytes=cbytes(physical(path));
      if(bytesCall(bytes,p=>api('eagler_replay_open')(p,stage))!==1)throw Error(api('eagler_replay_error')?text(api('eagler_replay_error')()):'录像原生启动失败');},
    step(render){const ok=stepReplay?stepReplay(render):api('eagler_replay_step')?.(+render)===1;
      if(!ok&&api('eagler_replay_error'))throw Error(text(api('eagler_replay_error')()));return ok;},
    present(){api('eagler_replay_present')?.();},
    deferPresentation(){return typeof api('eagler_replay_present')==='function';},
    state(){const preparing=api('eagler_replay_preparing'),extra=preparing?{preparing:!!preparing()}:{};
      if(replayState)return {...replayState(),...extra};const p=api('eagler_replay_state')(),v=Array.from(new Uint32Array(heap().buffer,p,5));return {frame:v[0],total:v[1],ended:!!v[2],stage:v[3],startStage:v[4],...extra};},
    stopLoop,onState,onError,
  });
  return bridge;
}
// END GENERATED LOCAL REPLAY
const protocol='eagler-touhou/1',game='th10',query=new URLSearchParams(location.search),canvas=document.querySelector('canvas');
const runtimeVariant=query.get('runtimeVariant')??'normal',multiplayerRuntime=runtimeVariant==='multiplayer';
const createAdonisCalibration=multiplayerRuntime?(await import('./adonis-calibration.mjs')).createAdonisCalibration:null;
const epoch=Number(query.get('runtimeEpoch'));
const validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
const runtimeDocument=document,hosted=parent!==window,ownerFrame=hosted?window.frameElement:null;
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...fields},location.origin);
let Module,core,app=0,launched=false,first=false,closing=false,language=query.get('language')==='lang_zh-hans'?'chs':'jp',options={},music=true;
let initializedReady=false,launchSettled=false,touchLifetime=0,touchRetired=false;
// Source serials consume all authenticated snapshots, including dropped input.
// Only accepted changes advance native serials. A real sdl_game_open resets
// mapped serials; TH10 Replay stays in the same native Application/session.
let sourceBomb=0,sourceEscape=0,mappedBomb=0,mappedEscape=0;
let practice;
let calibration=null,runtimeBuild='',runtimeBuildWords=[];
const keyboard=createBrowserKeyboard({
 send(code,down){if(!core)return;if(options.thpracEnabled&&practice?.key(code,down))return;cstring(code,p=>core.sdl_key(p,+down));},
 onClear(){practice?.clear();}
});
function clearKeyboard(){keyboard.clear();core?.sdl_keys_clear();}
let frames=0,lastHealth=0,lastFrame=0,maxGap=0,lastPresented=0,saveTimer=null;
function ownsTouchDocument(){
 try{return document===runtimeDocument&&(!hosted||!!ownerFrame&&ownerFrame.isConnected&&ownerFrame.contentWindow===window&&ownerFrame.contentDocument===runtimeDocument);}
 catch{return false;}
}
function touchReady(){
 // Replay menus and Esc during playback/seek remain native viewer controls.
 // Playback and spectator ownership of game input is enforced by C++.
 return initializedReady&&!!core&&!!app&&launched&&launchSettled&&!closing&&
  !touchRetired&&!document.hidden&&!!options.touchEnabled&&!options.netplaySpectator&&ownsTouchDocument();
}
function cancelTouchInput(){
 const delivered=touchReady();cancelTouches();
 // Native cancel_transient clears focus/stick/pulses, preserving fire and
 // the accepted action serials. Also cancel when the lifecycle is ineligible.
 if(!delivered&&initializedReady&&core)core.sdl_touch_cancel();
}
function invalidateTouches(terminal=false){
 ++touchLifetime;launchSettled=false;if(terminal)touchRetired=true;cancelTouchInput();
}
function acceptsTouch(){
 if(!ownsTouchDocument()){if(!touchRetired)invalidateTouches(true);return false;}
 return touchReady();
}
const cancelTouches=bindOutsideTouches(document,canvas,()=>touchReady()?core:null,acceptsTouch);
const error=reason=>{invalidateTouches(true);const message=reason?.stack||String(reason);if(multiplayerRuntime){window.__eaglerNetplayFailed=true;window.__eaglerNetplayError=message;}document.querySelector('#error').textContent=message;emit('error',{message,error:message});console.error(reason);};
const u32=(ptr,count)=>new Uint32Array(core.memory.buffer,ptr,count);
const cstring=(text,fn)=>{const bytes=new TextEncoder().encode(text+'\0'),p=core.graphics_allocate(bytes.length);try{new Uint8Array(core.memory.buffer,p,bytes.length).set(bytes);return fn(p);}finally{core.graphics_free(p);}};
const readCString=ptr=>{if(!ptr)return '';const heap=new Uint8Array(core.memory.buffer);let end=ptr;while(end<heap.length&&heap[end])++end;return new TextDecoder().decode(heap.subarray(ptr,end));};
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
}
async function configureNetplay(){
 if(!multiplayerRuntime||options.netplayMode!=='lan')return;
 const url=new URL(options.netplayUrl),room=url.searchParams.get('room'),run=url.searchParams.get('run');
 const count=options.netplayPlayerCount,seat=options.netplaySpectator?0:options.netplayPlayer;
 const loadouts=options.netplayLoadouts;
 let inputDelay=options.netplayInputDelay===undefined?0:Number(options.netplayInputDelay);
 let mode=options.netplayAdonisMode??0,automatic=options.netplayInputDelayAuto??false;
 const reserve=options.netplayPredictionReserve??2;
 if(!Number.isInteger(mode)||mode<0||mode>2||typeof automatic!=='boolean'||
    !Number.isInteger(reserve)||reserve<1||reserve>2)throw Error('Invalid TH10 measured timing policy');
 // Both rollback choices use the verified zero-delay path. Pure delay keeps
 // its existing measured/fixed policy and ABI.
 if(mode===0||mode===2){mode=0;inputDelay=0;automatic=false;}
 if(automatic&&inputDelay)throw Error('Invalid TH10 measured timing policy');
 if(!['ws:','wss:'].includes(url.protocol)||!room||!run||![2,3].includes(count)||
    !Number.isInteger(seat)||seat<0||seat>=count||
    !Number.isInteger(options.netplaySeed)||options.netplaySeed<0||options.netplaySeed>65535||
    !Number.isInteger(options.netplayDifficulty)||options.netplayDifficulty<0||options.netplayDifficulty>4||
    !Number.isInteger(inputDelay)||inputDelay<0||inputDelay>(mode?9:8)||
    !Array.isArray(loadouts)||loadouts.length!==count)throw Error('Invalid TH10 multiplayer options');
 const identity=new TextEncoder().encode(`th10mp:${url.origin}${url.pathname}:${room}:${run}`);
 const digest=new DataView(await crypto.subtle.digest('SHA-256',identity));
 const low=digest.getUint32(0,true),high=digest.getUint32(4,true)||1;
 const challenge=options.netplayChallengeMode===true;
 const words=[challenge?5:mode?4:3,count,seat,options.netplayDifficulty,options.netplaySeed,low,high,inputDelay];
 for(let i=0;i<3;i++){
  const value=loadouts[i]||{character:0,shot:0};
  if(!Number.isInteger(value.character)||value.character<0||value.character>1||
     !Number.isInteger(value.shot)||value.shot<0||value.shot>2)throw Error('Invalid TH10 multiplayer loadout');
  words.push(value.character,value.shot);
 }
 if(mode||challenge){if(runtimeBuildWords.length!==4)throw Error('Missing TH10 immutable Runtime identity');words.push(mode,+automatic,reserve,...runtimeBuildWords);}
 if(challenge)words.push(1);
 const pointer=core.files_allocate(words.length*4);
 try{
  new Uint32Array(core.memory.buffer,pointer,words.length).set(words);
  if(!core.multiplayer_configure(app,pointer,words.length))throw Error('TH10 multiplayer session rejected');
 }finally{core.files_free(pointer);}
 const connected=options.netplaySpectator
  ?cstring(options.netplayUrl,relay=>cstring(options.netplaySpectatorId,id=>core.multiplayer_spectator_connect(app,relay,id)))
  :cstring(options.netplayUrl,relay=>core.multiplayer_connect(app,relay));
 if(!connected)throw Error('TH10 multiplayer transport rejected');
 window.__eaglerNetplayInputDelayFrames=options.netplaySpectator?0:inputDelay;
 window.__eaglerNetplayFailed=false;window.__eaglerNetplayError='';
 updateNetplayDiagnostics();
 if(mode&&!options.netplaySpectator)calibration?.start();
}
const replayFiles=createReplayFilePolicy({game:10,multiplayer:multiplayerRuntime,validateMultiplayer(bytes){
 if(typeof core.multiplayer_replay_validate!=='function')throw Error('Multiplayer Replay capability is missing');
 const p=core.files_allocate(bytes.length);if(!p)throw Error('Replay allocation failed');
 try{new Uint8Array(core.memory.buffer,p,bytes.length).set(bytes);return core.multiplayer_replay_validate(p,bytes.length)===1;}
 finally{core.files_free(p);}
}});
let storage;
const root=()=>storage.root(language);
const sync=createSaveSync(()=>Module);
const fileExists=path=>Module.FS.analyzePath(path).exists;
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
function applyOptions(){
 applyTouchOptions(core,options.netplaySpectator?{...options,touchEnabled:false}:options);
 if(app){
  core.application_touch_display?.(app,options.alwaysHitbox?1:0);
  const hints=['hint_user.txt','hint_auto.txt'].some(name=>fileExists(root()+'/hint/'+name));
  core.application_original_options?.(app,options.faithBarEnabled?1:0,hints?1:0);
  if(multiplayerRuntime)core.multiplayer_local_player_visibility?.(app,options.multiplayerLocalPlayerVisibility&&!options.netplaySpectator&&!options.replayViewer?1:0);
 }
 practice?.configure(options);
}
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
 if(!core||!app||!launched||closing||touchRetired||document.hidden||!ownsTouchDocument())return false;
 if(forcePause)core.sdl_loop_pause(1);
 return resumeRuntimeAudio(Module,core,()=>!!core&&!!app&&launched&&!closing&&!touchRetired&&!document.hidden&&ownsTouchDocument());
}
async function stop(){if(closing)return;closing=true;localReplay.close();invalidateTouches();calibration?.stop();clearKeyboard();try{practice?.close();core.sdl_loop_stop();await save();core.sdl_game_close();await sync(false);app=0;launched=false;updateReplaySeek();emit('exit',{code:0,status:'success'});}finally{closing=false;}}
const localReplay=createNativeLocalReplay({core:()=>core,module:()=>Module,physical:value=>root()+'/'+replayFiles.physical(storage.relativeSave(value)),replayState:()=>{const v=Array.from(new Uint32Array(core.memory.buffer,core.eagler_replay_state(),multiplayerRuntime?6:5));return {frame:v[0],total:v[1],ended:!!v[2],stage:v[3],startStage:v[4],bootstrapTarget:v[5]??0};},stopLoop:()=>{clearKeyboard();core.sdl_loop_stop();},onState:playback=>emit('runtime-info',{playback}),onError:error});
async function launch(){
 if(launched)return;
 const touchTicket=touchLifetime;launchSettled=false;clearKeyboard();
 await installStartupBranding(Module,{game,builtAt:(await(await fetch('./manifest.json')).json()).builtAt});
 // The localized Runtime still constructs the Japanese core. Keep its font
 // alias alongside the selected localization font and user-file root.
 ensureSharedFontAlias(Module,'jp');
 ensureSharedFontAlias(Module,language);
 Module.eaglerSaveLanguage=language;
 const mode=Module.touhouMusicMode||'none';music=mode!=='none'&&mode!=='midi';core.sdl_ogg_decode_mode?.(options.oggDecodeMode==='full');
 core.sdl_music_enabled?.(music);app=core.sdl_game_open(false,options.netplayMode==='lan'?options.netplaySeed:Date.now()&65535);if(!app)throw Error('C++ game initialization failed');
 mappedBomb=mappedEscape=0;
 try{await configureNetplay();}catch(reason){calibration?.stop();core.sdl_game_close();app=0;throw reason;}
 applyOptions();launched=true;first=false;lastPresented=0;lastHealth=performance.now();lastFrame=0;frames=0;maxGap=0;
 canvas.focus({preventScroll:true});core.sdl_loop_pause(1);if(!document.hidden)await resumeForegroundAudio();core.sdl_loop_start(app);
 launchSettled=touchTicket===touchLifetime&&!touchRetired&&!closing&&!!app&&ownsTouchDocument();
 emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++',architecture:'eagler-touhou/1',version:'3.5.1-sdl3'});
}
async function command(message){
 switch(message.command){
 case 'configure':if(launched)throw Error('Cannot configure a running game');language=message.language==='lang_zh-hans'?'chs':'jp';options=normalizeOptions(message.options);if(!['ogg','midi','none'].includes(message.music))throw Error('Invalid music mode');Module.touhouMusicMode=message.music;Module.eaglerOptions=options;music=message.music!=='none';await installResources(message.sharedResources);await installResources(message.runtimeResources);await installResources(message.resources);if(message.runtimePack)await installRuntimePack(message.runtimePack);applyOptions();return {};
 case 'resources':await installResources(message.resources);return {};
 case 'keyboard':if(launched&&!document.hidden&&!closing)keyboard.event(message,!!message.down,'hosted');return {};
 case 'thprac-mouse':practice?.mouse(message);return {};
 case 'keyboard-clear':clearKeyboard();return {};
 case 'touch-cancel':cancelTouchInput();return {};
 case 'direct-touch':if(acceptsTouch())directTouch(core,canvas,message,{width:innerWidth,height:innerHeight});return {};
 case 'touch-controls':{
  const controls=message.controls??message,bomb=controls.bombSerial>>>0,escape=controls.escapeSerial>>>0;
  const changedBomb=bomb!==sourceBomb,changedEscape=escape!==sourceEscape;
  sourceBomb=bomb;sourceEscape=escape;if(!acceptsTouch())return {};
  const nextBomb=(mappedBomb+Number(changedBomb))>>>0,nextEscape=(mappedEscape+Number(changedEscape))>>>0;
  touchControls(core,options,{controls:{...controls,bombSerial:nextBomb,escapeSerial:nextEscape}});
  mappedBomb=nextBomb;mappedEscape=nextEscape;return {};
 }
 case 'playback':if(!launched||options.replayViewer!==true)throw Error('请先打开录像管理器');return localReplay.command(message);
 case 'launch':await launch();return {};
 case 'sync':await save();return {};
 case 'list':return {files:listLocalReplayFiles(Module.FS,root(),storage.relativeSave,path=>({path:replayFiles.exported(path,Module.FS.readFile(root()+'/'+path))}))};
 case 'read':{const path=replayFiles.physical(storage.relativeSave(message.path));return {bytes:Array.from(Module.FS.readFile(root()+'/'+path))};}
 case 'write':{if(!Array.isArray(message.bytes)||message.bytes.length>16*1024*1024||message.bytes.some(b=>!Number.isInteger(b)||b<0||b>255))throw Error('Invalid save bytes');const bytes=new Uint8Array(message.bytes),path=replayFiles.imported(storage.relativeSave(message.path),bytes);
  const target=root()+'/'+path;Module.FS.mkdirTree(target.slice(0,target.lastIndexOf('/')));
  if(multiplayerRuntime&&/\.rpy$/.test(path)){const target=root()+'/'+path,temporary=target+'.pending';
   try{Module.FS.writeFile(temporary,bytes);Module.FS.rename(temporary,target);}catch(error){try{Module.FS.unlink(temporary);}catch{}throw error;}
  }else Module.FS.writeFile(root()+'/'+path,bytes);
  await sync(false);return {};}
 case 'remove':{const path=replayFiles.physical(storage.relativeSave(message.path));Module.FS.unlink(root()+'/'+path);await sync(false);return {};}
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
 if(['keyboard','keyboard-clear','direct-touch','touch-controls','touch-cancel'].includes(m.command)){void dispatchCommand(m);return;}
 queue=queue.then(async()=>{if(await initialized===false)return;await dispatchCommand(m);}).catch(error);
});
document.addEventListener('visibilitychange',()=>{if(!core||!launched)return;clearKeyboard();cancelTouchInput();if(document.hidden){suspendRuntimeAudio(Module,core);queue=queue.then(save).catch(error);}else void resumeForegroundAudio(true);});
window.addEventListener('blur',()=>{if(core){clearKeyboard();cancelTouchInput();}});
window.addEventListener('pagehide',()=>{invalidateTouches(true);clearKeyboard();if(core&&launched){suspendRuntimeAudio(Module,core);void save().catch(console.error);}});
window.addEventListener('pageshow',()=>{if(core&&launched&&!document.hidden)void resumeForegroundAudio(true);});
canvas.addEventListener('webglcontextlost',event=>{event.preventDefault();clearKeyboard();core?.sdl_loop_pause(1);error('图形环境已失效，请退出后重新开始。');});
for(const name of ['pointerdown','keydown'])window.addEventListener(name,()=>{if(Module?.SDL3?.audioContext?.state!=='running')void resumeForegroundAudio(true);},{capture:true});
for(const name of ['keydown','keyup'])window.addEventListener(name,event=>{
 if(!core||!launched||closing||document.hidden)return;
 const handled=keyboard.event(event,name==='keydown');
 if(handled&&options.thpracEnabled&&/^(Backspace|Tab|F[1-7]|F12)$/.test(event.code))event.preventDefault();
},{capture:true});
const initialized=(async()=>{
 if(multiplayerRuntime){
  const manifest=await fetch('./manifest.json').then(r=>{if(!r.ok)throw Error('Missing TH10 Runtime manifest');return r.json();});
  runtimeBuild=manifest.buildIdentity||manifest.execution?.sha256||'';
  if(!/^[a-f0-9]{64}$/i.test(runtimeBuild))throw Error('Invalid TH10 Runtime build identity');
  runtimeBuildWords=Array.from({length:4},(_,i)=>parseInt(runtimeBuild.slice(i*8,i*8+8),16)>>>0);
 }
 let audioContext;try{audioContext=parent.__touhouAudioContext||parent.__th10AudioContext;}catch{}
 Module=await createModule({canvas,noInitialRun:true,resetBrowserKeyboard:()=>keyboard.clear(),...(audioContext?{SDL3:{audioContext}}:{}),print:console.log,printErr:console.error,
  instantiateWasm(imports,ready){return WebAssembly.instantiateStreaming(fetch('./th10-sdl.wasm'),imports).then(({instance,module})=>{core=instance.exports;ready(instance,module);return core;});}
 });
 storage=await initializeSaveStorage({game,runtimeVariant,setCompiledVariant:value=>core.sdl_files_variant(value),filesystem:Module.FS,idbfs:Module.IDBFS,sync,
  beforeMount(){window.Module=Module;window.FS=Module.FS;observeMusicWrites(Module,core,game);}});
 practice=await createOptionalPractice({core,getApp:()=>app,canvas,clearKeys:clearKeyboard,setMusic:value=>core.sdl_music_enabled(value),setPaused:value=>core.sdl_loop_pause(value||document.hidden?1:0)});
 for(const lang of ['jp','chs'])Module.FS.mkdirTree(storage.namespace+'/'+lang+'/replay');await migrateSaves();await mountData();cstring('#screen',core.sdl_canvas);
 Module.runtimePrepare=()=>!document.hidden;
 calibration=createAdonisCalibration?.({core,getApp:()=>app,getOptions:()=>options,emit,game,
  build:runtimeBuild,onError:error,pause:()=>core.sdl_loop_pause(1)});
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
 initializedReady=true;
 emit('ready');
})().catch(e=>{if(isSupersededRuntimeError(e)){console.debug('Runtime navigation superseded');return false;}error(e);throw e;});
