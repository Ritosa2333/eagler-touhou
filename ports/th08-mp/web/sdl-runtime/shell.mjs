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
// BEGIN GENERATED SPECTATOR PLAYBACK
// Generated from eagler-common/browser/spectator-playback.mjs.
// Native playback owns every cursor and restore. The shell only transports
// authenticated Launcher commands and publishes native reports.
function createSpectatorPlaybackBridge({active,memory,state,control,allocate,deallocate,emit,
  schedule=globalThis.setInterval,cancel=globalThis.clearInterval}) {
  let timer=null,last=null;
  const clear=()=>{if(last){last=null;emit('runtime-info',{spectatorPlayback:null});}};
  function poll() {
    if(!active()){clear();return null;}
    try {
      const pointer=state();
      if(!pointer){clear();return null;}
      const heap=memory();
      if(!(heap instanceof Uint8Array)||!Number.isInteger(pointer)||pointer<1||pointer>=heap.length)
        throw Error('Invalid native spectator report address');
      const limit=Math.min(heap.length,pointer+131072);let end=pointer;
      while(end<limit&&heap[end])++end;
      if(end===limit)throw Error('Native spectator report exceeds its bound');
      const report=JSON.parse(new TextDecoder('utf-8',{fatal:true}).decode(heap.subarray(pointer,end)));
      if(report?.version!==1||typeof report.run!=='string'||!/^[A-Za-z0-9_-]{1,64}$/.test(report.run)||
         !Number.isInteger(report.revision)||report.revision<1||report.revision>0xfffffffe||
         !Array.isArray(report.nodes)||report.nodes.length>512)throw Error('Invalid native spectator report');
      if(last?.run===report.run&&(report.revision<=last.revision||report.received<last.received))return last;
      last=report;emit('runtime-info',{spectatorPlayback:report});return report;
    } catch(error) {
      clear();return null;
    }
  }
  function command(message) {
    if(!active())throw Error('Spectator playback is not active');
    if(typeof message?.run!=='string'||!/^[A-Za-z0-9_-]{1,64}$/.test(message.run)||
       !['pause','play','seek','live'].includes(message.action)||
       (message.action==='seek'&&(!Number.isInteger(message.frame)||message.frame<0||message.frame>0xfffffffe)))
      throw Error('Invalid spectator playback command');
    const report=poll();
    if(!report||message.run!==report.run)throw Error('Spectator playback run has changed');
    if(message.action==='seek'&&(message.frame<report.min||message.frame>report.edge))
      throw Error('Spectator seek is outside received history');
    // Keep terminators as zero bytes. The classic inline HTML optimizer can
    // turn a JS NUL escape into a literal NUL, which HTML replaces by U+FFFD.
    const run=new TextEncoder().encode(message.run),action=new TextEncoder().encode(message.action);
    const actionOffset=run.length+1,bytes=new Uint8Array(actionOffset+action.length+1);
    bytes.set(run);bytes.set(action,actionOffset);
    const pointer=allocate(bytes.length);
    if(!Number.isInteger(pointer)||pointer<1)throw Error('Spectator command allocation failed');
    try {
      const heap=memory();
      if(pointer>heap.length-bytes.length)throw Error('Invalid spectator command allocation');
      heap.set(bytes,pointer);
      if(!control(pointer,pointer+actionOffset,message.action==='seek'?message.frame:0))
        throw Error('Spectator playback command could not be applied');
    } finally {deallocate(pointer);}
    poll();return {};
  }
  return Object.freeze({poll,command,
    start(){if(timer===null)timer=schedule(poll,250);poll();},
    stop(){if(timer!==null)cancel(timer);timer=null;clear();},
  });
}
// END GENERATED SPECTATOR PLAYBACK
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
const protocol='eagler-touhou/1',game='th08',query=new URLSearchParams(location.search),canvas=document.querySelector('canvas');
const runtimeVariant=query.get('runtimeVariant')??'normal',multiplayerRuntime=runtimeVariant==='multiplayer';
const createAdonisCalibration=multiplayerRuntime?(await import('./adonis-calibration.mjs')).createAdonisCalibration:null;
const epoch=Number(query.get('runtimeEpoch'));
const validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
const runtimeDocument=document,hosted=parent!==window,ownerFrame=hosted?window.frameElement:null;
let launchSerial=0,roomOwned=false;
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...((event==='first-frame'||event==='room-menu')?{launchSerial}:{}),...fields},location.origin);
const spectatorPlayback=createSpectatorPlaybackBridge({
 active:()=>multiplayerRuntime&&launched&&options.netplaySpectator===true&&!closing,memory:()=>new Uint8Array(core.memory.buffer),
 state:()=>core?.multiplayer_spectator_playback_state?.(app)||0,control:(run,action,frame)=>core?.multiplayer_spectator_playback_control?.(app,run,action,frame)||0,
 allocate:size=>core.allocate(size),deallocate:pointer=>core.deallocate(pointer),emit,
});
let Module,core,app=0,launched=false,first=false,closing=false,language=query.get('language')==='lang_zh-hans'?'chs':'jp',options={},music=true;
let initializedReady=false,launchSettled=false,replayReopening=false,touchLifetime=0,touchRetired=false,roomPreview=false;
// Source serials consume every received snapshot, including ineligible input.
// Only accepted changes advance native serials; game open/reopen resets those
// mapped counters, while the source baseline remains owned by this document.
let sourceBomb=0,sourceEscape=0,mappedBomb=0,mappedEscape=0;
let runtimeBuildWords=null,calibration=null;
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
const keyboard=createBrowserKeyboard({hostContext:{protocol,game,epoch,origin:location.origin,ready:()=>launched&&!document.hidden&&!options.netplaySpectator&&!closing,reset:clearKeyboard,},
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
 return initializedReady&&!!core&&!!app&&launched&&launchSettled&&!closing&&!replayReopening&&
  !touchRetired&&!document.hidden&&!!options.touchEnabled&&!options.netplaySpectator&&ownsTouchDocument();
}
function cancelTouchInput(){
 const delivered=touchReady();cancelTouches();
 // Native cancel_transient already clears focus/stick/short pulses while
 // preserving the Launcher's fire toggle and accepted serial baseline.
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
 if(!core||!app||!launched||closing||touchRetired||!ownsTouchDocument()||document.hidden)return false;
 if(forcePause)core.sdl_loop_pause(1);
 return resumeRuntimeAudio(Module,core,()=>!!core&&!!app&&launched&&!closing&&!touchRetired&&ownsTouchDocument()&&!document.hidden);
}
async function stop(){if(closing)return;if(roomOwned&&!options.replayViewer){closing=true;try{await save();await command({command:'return-room'});emit('room-menu');}finally{closing=false;}return;}closing=true;localReplay.close();spectatorPlayback.stop();invalidateTouches();calibration?.stop();clearKeyboard();try{practice?.close();core.sdl_loop_stop();await save();core.sdl_game_close();window.dispatchEvent(new CustomEvent('touhou-midi-close'));await sync(false);app=0;launched=false;emit('exit',{code:0,status:'success'});}finally{removeResourceTraceHostButton();closing=false;}}
let localReplayStart=0;
const localReplay=createNativeLocalReplay({core:()=>core,module:()=>Module,
 physical:value=>root()+'/'+storage.relativeSave(value),stopLoop:()=>{clearKeyboard();core.sdl_loop_stop();},
 openReplay:async(path,stage)=>{if(!multiplayerRuntime){return cstring(root()+'/'+storage.relativeSave(path).replace(/\.rpyx$/i,'.rpy'),p=>{if(!core.eagler_replay_open(p,stage))throw Error('录像原生启动失败');});}localReplayStart=stage;replayReopening=true;try{await reopenReplay(path,stage);core.sdl_loop_stop();}finally{replayReopening=false;}},
 stepReplay:()=>core.sdl_loop_tick(app,1/60,17)===0,
 replayState:()=>{if(!multiplayerRuntime){const v=Array.from(new Uint32Array(core.memory.buffer,core.eagler_replay_state(),5));return {frame:v[0],total:v[1],ended:!!v[2],stage:v[3],startStage:v[4]};}const s=replayWords();return {frame:s[4],total:s[5],ended:!!(s[9]||s[11]),stage:status()[1],startStage:localReplayStart,bootstrapTarget:s[8]};},
 onState:playback=>emit('runtime-info',{playback}),onError:error});
async function initializeTitle({roomTitle=false}={}){
 if(app){
  const multiplayer=multiplayerRuntime?validateMultiplayerOptions(options):null;
  if(multiplayer)await configureMultiplayer(core,app,options,{runtimeBuildWords});
  return multiplayer;
 }
 clearKeyboard();
 const {installStartupBranding}=await import('./startup-branding.mjs');
 const build=await(await fetch('./manifest.json')).json();
 await installStartupBranding(Module,{game:'th08',builtAt:build.builtAt});
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
 if((replayViewer&&multiplayer)||(multiplayerPreflight&&(multiplayer||replayViewer))||(roomTitle&&(multiplayer||replayViewer)))
  throw Error('Conflicting TH08 multiplayer Runtime role');
 if(multiplayerRuntime&&!multiplayer&&!replayViewer&&!multiplayerPreflight&&!roomTitle)
  throw Error('TH08 multiplayer Runtime requires a room start, preflight or Replay viewer');
 if(!multiplayerRuntime&&options.netplayMode==='lan')throw Error('Network session requires the multiplayer Runtime');
 core.sdl_music_enabled?.(music);app=core.sdl_game_open(multiplayer?multiplayer.seed:Date.now()>>>0);if(!app)throw Error('C++ game initialization failed');
 mappedBomb=mappedEscape=0;
 try{
 if(!core.sdl_prepare_loading()||!core.sdl_draw_loading(0))throw Error('开头加载画面初始化失败');
 const startupImageAt=performance.now();
 // Two RAF boundaries let the original frame reach the compositor before
 // resource work resumes. There is no minimum splash duration.
 await new Promise(requestAnimationFrame);await new Promise(requestAnimationFrame);let loadingPaint=performance.now();
 const total=core.sdl_prepare_total();for(let i=0;i<total;i++){if(core.sdl_prepare_next()<0)throw Error('资源预载失败 '+i);if(i%12===11){const now=performance.now();if(now-loadingPaint>=1000/60){if(!core.sdl_draw_loading(1))throw Error('开头加载画面绘制失败');loadingPaint=now;await new Promise(requestAnimationFrame);}else await new Promise(resolve=>setTimeout(resolve,0));}}
 if(replayViewer&&!core.multiplayer_replay_viewer(app))throw Error('TH08 native Replay viewer rejected');
 // Spectator protection precedes native initialize(), which itself writes the
 // config/score namespace. Waiting until the first gameplay frame is too late.
 if(multiplayer?.spectator)await configureMultiplayer(core,app,options,{runtimeBuildWords});
 if(!core.sdl_game_initialize())throw Error('永夜抄初始化失败');await new Promise(resolve=>setTimeout(resolve,Math.max(0,2000-(performance.now()-startupImageAt))));performance.mark('eagler-startup-menu-ready');document.querySelector('#loading').textContent='';
 if(multiplayer&&!multiplayer.spectator)await configureMultiplayer(core,app,options,{runtimeBuildWords});
 }catch(reason){calibration?.stop();core.sdl_game_close();app=0;throw reason;}
 return multiplayer;
}
async function launch(){
 if(launched)return;const touchTicket=touchLifetime;launchSettled=false;clearKeyboard();
 if(roomPreview)core.sdl_loop_stop();
 const multiplayer=await initializeTitle();roomPreview=false;
 if(multiplayer){window.__eaglerNetplayFailed=false;window.__eaglerNetplayError='';updateNetworkDiagnostics(core,app);if(multiplayer.adonisMode&&!multiplayer.spectator)calibration?.start();else calibration?.stop();}
 applyOptions();launched=true;first=false;lastPresented=0;lastHealth=performance.now();lastFrame=0;frames=0;maxGap=0;
 canvas.focus({preventScroll:true});core.sdl_loop_pause(1);if(!document.hidden)await resumeForegroundAudio();if(query.get('manual')!=='1')core.sdl_loop_start();
 launchSettled=touchTicket===touchLifetime&&!touchRetired&&!closing&&!!app&&ownsTouchDocument();
 if(options.netplaySpectator)spectatorPlayback.start();
 emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++',architecture:'eagler-touhou/1',version:'3.4.1-sdl3'});
}
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
 mappedBomb=mappedEscape=0;
 if(bytes){if(!replayByteCall(bytes,(p,n)=>core.multiplayer_replay_load(app,p,n,stage)))throw Error('Replay native bootstrap rejected');}
 else if(!core.multiplayer_replay_viewer(app))throw Error('Replay viewer bootstrap rejected');
 const total=core.sdl_prepare_total();for(let i=0;i<total;++i){
  if(core.sdl_prepare_next()<0)throw Error('Replay resource preparation failed');
  if(i%12===11)await new Promise(resolve=>setTimeout(resolve,0));
 }
 if(!core.sdl_game_initialize())throw Error('Replay native initialization failed');
 core.sdl_keys_clear();applyOptions();
 core.sdl_loop_pause(1);if(!document.hidden)await resumeForegroundAudio();
 if(query.get('manual')!=='1')core.sdl_loop_start();
 replayReopening=false;
}
async function command(message){
 if(multiplayerRuntime&&launched&&options.netplaySpectator&&
    ['keyboard','thprac-mouse'].includes(message.command))return {};
 switch(message.command){
 case 'configure':if(launched)throw Error('Cannot configure a running game');language=message.language==='lang_zh-hans'?'chs':'jp';options=normalizeOptions(message.options);if(multiplayerRuntime)options.thpracEnabled=false;if(!['ogg','midi','none'].includes(message.music))throw Error('Invalid music mode');Module.touhouMusicMode=message.music;Module.eaglerOptions=options;music=message.music!=='none';await installResources(message.sharedResources);await installResources(message.runtimeResources);await installResources(message.resources);if(message.runtimePack)await installRuntimePack(message.runtimePack);applyOptions();return {};
 case 'resources':if(multiplayerRuntime&&launched&&!options.replayViewer)throw Error('Cannot replace resources during a multiplayer run');await installResources(message.resources);return {};
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
 case 'spectator-playback':return spectatorPlayback.command(message);
 case 'playback':if(!launched||options.replayViewer!==true)throw Error('请先打开录像管理器');return localReplay.command(message);
 case 'prepare-room':roomOwned=true;{
  if(!multiplayerRuntime||launched)throw Error('Invalid room preview owner');
  await initializeTitle({roomTitle:true});
  if(!core.sdl_room_preview())throw Error('Native room preview rejected');
  roomPreview=true;core.sdl_loop_start();
  const deadline=performance.now()+60000;
  while(!core.sdl_room_preview_ready()){
   if(closing||touchRetired||performance.now()>deadline)throw Error('Native room title did not become ready');
   await new Promise(requestAnimationFrame);
  }
  return {presented:true};
 }
 case 'return-room':roomOwned=true;{
  if(!multiplayerRuntime||!core.sdl_room_return)throw Error('Room menu return is unavailable');
  spectatorPlayback.stop();calibration?.stop();clearKeyboard();core.sdl_loop_stop();
  if(!core.sdl_room_return())throw Error('Native room return rejected');
  roomPreview=true;core.sdl_loop_pause(document.hidden?1:0);core.sdl_loop_start();const deadline=performance.now()+60000;
  while(!core.sdl_room_preview_ready()){
   if(closing||touchRetired||performance.now()>deadline)throw Error('Native room title did not become ready');
   await new Promise(requestAnimationFrame);
  }
  launched=false;launchSettled=false;return {presented:true};
 }
 case 'launch':launchSerial=Number(message.launchSerial)||0;await launch();return {};
 case 'sync':await save();return {};
 case 'list':return {files:listLocalReplayFiles(Module.FS,root(),storage.relativeSave,path=>({path:multiplayerRuntime?path:exportReplayName(path,Module.FS.readFile(root()+'/'+path),8)}))};
 case 'read':{let path=storage.relativeSave(message.path);if(!multiplayerRuntime&&path.endsWith('.rpyx'))path=path.slice(0,-1);return {bytes:Array.from(Module.FS.readFile(root()+'/'+path))};}
 case 'write':{if(multiplayerRuntime&&launched&&!options.replayViewer)throw Error('Cannot import saves during a multiplayer run');if(!Array.isArray(message.bytes)||message.bytes.length>16*1024*1024||message.bytes.some(b=>!Number.isInteger(b)||b<0||b>255))throw Error('Invalid save bytes');const bytes=new Uint8Array(message.bytes),relative=storage.relativeSave(message.path);
  if(multiplayerRuntime&&relative.startsWith('replay/')&&(!relative.endsWith('.rpyx')||!replayByteCall(bytes,(p,n)=>core.multiplayer_replay_validate(p,n))))throw Error('Invalid TH08 multiplayer Replay');
  const path=multiplayerRuntime?relative:importReplayName(relative,bytes,8);Module.FS.writeFile(root()+'/'+path,bytes);await sync(false);return {};}
 case 'remove':{if(multiplayerRuntime&&launched&&!options.replayViewer)throw Error('Cannot remove saves during a multiplayer run');let path=storage.relativeSave(message.path);if(!multiplayerRuntime&&path.endsWith('.rpyx'))path=path.slice(0,-1);Module.FS.unlink(root()+'/'+path);await sync(false);return {};}
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
 if(m.command==='keyboard'||m.command==='keyboard-clear'||m.command==='direct-touch'||m.command==='touch-controls'||m.command==='touch-cancel'){void dispatchCommand(m);return;}
 queue=queue.then(async()=>{if(await initialized===false)return;await dispatchCommand(m);}).catch(error);
});
document.addEventListener('visibilitychange',()=>{cancelTouchInput();if(!core||!launched)return;clearKeyboard();if(document.hidden){suspendRuntimeAudio(Module,core);queue=queue.then(persist).catch(error);}else void resumeForegroundAudio(true);});
window.addEventListener('blur',()=>{cancelTouchInput();if(core){practice?.clear();clearKeyboard();}});
window.addEventListener('pagehide',()=>{invalidateTouches(true);clearKeyboard();removeResourceTraceHostButton();if(core&&launched){suspendRuntimeAudio(Module,core);void persist().catch(console.error);}});
window.addEventListener('pageshow',()=>{if(core&&launched&&!document.hidden)void resumeForegroundAudio(true);});
canvas.addEventListener('webglcontextlost',event=>{event.preventDefault();clearKeyboard();core?.sdl_loop_pause(1);error('图形环境已失效，请退出后重新开始。');});
for(const name of ['pointerdown','keydown'])window.addEventListener(name,()=>{if(Module?.SDL3?.audioContext?.state!=='running')void resumeForegroundAudio(true);},{capture:true});
for(const name of ['keydown','keyup'])window.addEventListener(name,event=>{
 if(!core||!launched||closing||document.hidden)return;
 if(keyboard.mapNative(event,name==='keydown')){event.preventDefault();event.stopImmediatePropagation();return;}
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
 calibration=createAdonisCalibration?.({core,getApp:()=>app,getOptions:()=>options,emit,game,
  build:runtimeBuildIdentity,onError:error,pause:()=>core.sdl_loop_pause(1)});
 Module.runtimeFinish=(result,duration)=>{
  if(replayReopening)return;
  if(traceRequested&&!traceStarted&&app)traceStart();
  if(multiplayerRuntime&&app&&options.replayViewer&&core.multiplayer_replay_status){
   const state=replayWords(),pointer=core.multiplayer_replay_request(app);
   const memory=new Uint8Array(core.memory.buffer);let end=pointer;while(pointer&&end<memory.length&&memory[end]&&end-pointer<512)++end;
   const path=pointer?new TextDecoder().decode(memory.subarray(pointer,end)):'';
   if(path||state[9]){replayReopening=true;cancelTouchInput();core.sdl_loop_pause(1);
    queue=queue.then(()=>reopenReplay(path,state[10])).catch(error);return;}
  }
  practice.tick();
  if(multiplayerRuntime){updateNetworkDiagnostics(core,app);calibration?.pump();}
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
 initializedReady=true;
 emit('ready',multiplayerRuntime?{roomPreview:true,roomReturn:typeof core.sdl_room_return==='function'}:{});
})().catch(e=>{if(isSupersededRuntimeError(e)){console.debug('Runtime navigation superseded');return false;}error(e);throw e;});
