// Platform shell for the upstream eagler-touhou/1 Launcher contract.
// Game construction, input, timing, rendering, text and sound belong to C++.
// Mirrors th10/th20 shell.mjs and imports the shared eagler-host transport.
import {installStartupBranding,finishStartupAnimation} from './startup-branding.mjs';
import createModule from './th11-sdl.mjs';
import {scanCodes} from './keyboard.mjs';
import {createBrowserKeyboard} from './directory-keyboard.mjs';
import {createDirectInputBridge} from './direct-input-bridge.mjs';
import {validateMotionReplay} from './motion-replay.mjs';
import {bindOutsideTouches,normalizeOptions,applyTouchOptions,touchControls,suspendRuntimeAudio,resumeRuntimeAudio,directTouch,installResources as installHostResources,observeMusicWrites,mountManagedData,isSupersededRuntimeError} from './eagler-host.mjs';
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
function createLocalReplay({inspect,open,step,state,present=()=>{},deferPresentation=()=>false,stopLoop,prepare=async()=>{},audioState=()=>true,onState=()=>{},onError=()=>{},raf=fn=>requestAnimationFrame(fn),cancel=id=>cancelAnimationFrame(id),defer,cancelDeferred,now=()=>performance.now(),hidden=()=>document.hidden}) {
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
  function syncAudio(){return audioState({paused:closed||!selected||paused,hidden:hidden(),seeking:target!==null,ended:!!selected&&!!state().ended})!==false;}
  function schedule(){if(animation||continuation||closed)return;if(target!==null&&!hidden())continuation=soon(draw);else animation=raf(draw);}
  function draw(time){animation=continuation=0;if(closed||!selected)return;
    try {
      if(!syncAudio()||hidden()){last=null;schedule();return;}
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
      syncAudio();
      if(time-lastReport>=100){lastReport=time;report();}schedule();
    }catch(error){paused=true;target=null;syncAudio();report();onError(error);}
  }
  const reset=()=>{last=null;credit=0;target=null;};
  async function restart(nextPath,stage){
    // Keep the title's startup loop alive until native menu initialization is
    // complete. The startup picture's first-frame notification is too early.
    await prepare();if(closed)throw Error('录像播放器已关闭');
    cancelScheduled();stopLoop();
    try{await open(nextPath,stage);}catch(error){selected=null;reset();syncAudio();onState(null);throw error;}
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
      syncAudio();report();return {};
    },
    close(){closed=true;cancelScheduled();channel?.port1.close();channel?.port2.close();selected=null;syncAudio();},
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
function createNativeLocalReplay({core: getCore,module: getModule,physical,openReplay,stepReplay,replayState,stopLoop,pauseAudio,resumeAudio,onState,onError}) {
  const api=key=>{const c=getCore();return c[key]??c['_'+key];};
  let audioPaused=true,outputPaused=null,wasSeeking=false,resuming=null;
  function discardTransientSounds(){const pool=getModule()?.eaglerSfxOutput;if(pool)for(const voice of pool.voices.values())if(!voice.loop)pool.stop(voice);}
  function audioState({paused,hidden,seeking,ended}){
    audioPaused=paused||hidden||seeking||ended;
    if(seeking!==wasSeeking){discardTransientSounds();wasSeeking=seeking;}
    const context=getModule()?.SDL3?.audioContext;
    const gated=!!context&&context.state!=='running';
    const stop=audioPaused||gated;
    const changed=outputPaused!==stop;
    if(changed){pauseAudio(stop);outputPaused=stop;}
    // A policy-blocked resume may remain pending. It never owns the command
    // queue, and logical playback waits for output instead of queuing SFX.
    if(!audioPaused&&(changed||gated)&&resuming?.context!==context){
      const pending={context};resuming=pending;
      try{Promise.resolve(resumeAudio?resumeAudio():context?.resume()).catch(()=>{}).finally(()=>{if(resuming===pending)resuming=null;});}
      catch{resuming=null;}
    }
    return !gated||audioPaused;
  }
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
    open(path,stage){outputPaused=null;if(openReplay)return openReplay(path,stage);const bytes=cbytes(physical(path));
      if(bytesCall(bytes,p=>api('eagler_replay_open')(p,stage))!==1)throw Error(api('eagler_replay_error')?text(api('eagler_replay_error')()):'录像原生启动失败');},
    step(render){const ok=stepReplay?stepReplay(render):api('eagler_replay_step')?.(+render)===1;
      if(!ok&&api('eagler_replay_error'))throw Error(text(api('eagler_replay_error')()));
      (api('eagler_replay_audio_pump')??api('sdl_audio_pump'))?.();return ok;},
    present(){api('eagler_replay_present')?.();},
    deferPresentation(){return typeof api('eagler_replay_present')==='function';},
    state(){const preparing=api('eagler_replay_preparing'),extra=preparing?{preparing:!!preparing()}:{};
      if(replayState)return {...replayState(),...extra};const p=api('eagler_replay_state')(),v=Array.from(new Uint32Array(heap().buffer,p,5));return {frame:v[0],total:v[1],ended:!!v[2],stage:v[3],startStage:v[4],...extra};},
    stopLoop,...(pauseAudio?{audioState}:{}),onState,onError,
  });
  return {...bridge,get active(){return bridge.active;},get audioPaused(){return audioPaused;}};
}
// END GENERATED LOCAL REPLAY
const runtimeBuild=/*TH11_BUILD_INFO*/{version:'development-incomplete',completeGame:false,multiplayer:false};
const multiplayerRuntime=runtimeBuild.multiplayer===true,saveRoot=multiplayerRuntime?'/savesth11mp':'/savesth11';
const createMultiplayerRuntime=/*TH11_MULTIPLAYER_FACTORY*/multiplayerRuntime?(await import('./multiplayer.mjs')).createMultiplayerRuntime:null;
const protocol='eagler-touhou/1',game='th11',query=new URLSearchParams(location.search),canvas=document.querySelector('canvas');
const epoch=Number(query.get('runtimeEpoch'));
const validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
const runtimeDocument=document,hosted=parent!==window,ownerFrame=hosted?window.frameElement:null;
let launchSerial=0,roomOwned=false;
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...((event==='first-frame'||event==='room-menu')?{launchSerial}:{}),...fields},location.origin);
const spectatorPlayback=createSpectatorPlaybackBridge({
 active:()=>multiplayerRuntime&&launched&&options.netplaySpectator===true&&!stopping,memory:()=>new Uint8Array(core.memory.buffer),
 state:()=>core?.th11_mp_spectator_state?.()||0,control:(run,action,frame)=>core?.th11_mp_spectator_control?.(run,action,frame)||0,
 allocate:size=>core.malloc(size),deallocate:pointer=>core.free(pointer),emit,
});
const $=s=>document.querySelector(s);
let Module,core,app=0,multiplayer=null,runtimeManifest=null,launched=false,first=false,stopping=false,language=query.get('language')==='lang_zh-hans'?'chs':'jp',options={},music=true,musicMode='none';
let frames=0,lastHealth=0,lastFrame=0,maxGap=0,lastPresented=0,saveTimer=null,storageSync=Promise.resolve();
let initializedReady=false,launchSettled=false,touchLifetime=0,touchRetired=false,touchPageSuspended=false,titleInitialized=false,roomPreview=false;
// Native TH11 reset/cancel preserves its action serials. Keep both mappings
// for this document/core lifetime, including a stop and subsequent launch.
// Rejected snapshots consume only the source baseline, never a future pulse.
let sourceBomb=0,sourceEscape=0,mappedBomb=0,mappedEscape=0;
class SupersededTouchCommand extends Error {}
function ownsTouchDocument(){
 try{return document===runtimeDocument&&(!hosted||!!ownerFrame&&ownerFrame.isConnected&&ownerFrame.contentWindow===window&&ownerFrame.contentDocument===runtimeDocument);}
 catch{return false;}
}
function touchReady(){
 return initializedReady&&!!core&&launched&&launchSettled&&!stopping&&!touchRetired&&!touchPageSuspended&&!document.hidden&&
  !!options.touchEnabled&&!options.netplaySpectator&&(!multiplayerRuntime||!!multiplayer?.acceptInput())&&ownsTouchDocument();
}
function cancelTouchInput(){
 const delivered=touchReady();cancelTouches();
 // Native cancel_transient clears focus, axes and pending pulses, but keeps
 // the accepted fire toggle and serials. Neutralization creates no action.
 if(!delivered&&initializedReady&&core)core.sdl_touch_cancel();
}
function invalidateTouches(terminal=false){
 ++touchLifetime;launchSettled=false;if(terminal)touchRetired=true;cancelTouchInput();
}
function acceptsTouch(){
 if(!ownsTouchDocument()){if(!touchRetired)invalidateTouches(true);return false;}
 return touchReady();
}
function assertTouchLifetime(value){
 if(!ownsTouchDocument()){if(!touchRetired)invalidateTouches(true);}
 if(value!==touchLifetime||touchRetired||stopping)throw new SupersededTouchCommand('Runtime launch was superseded');
}
const cancelTouches=bindOutsideTouches(document,canvas,()=>touchReady()?core:null,acceptsTouch);
const error=reason=>{invalidateTouches(true);const message=reason?.stack||String(reason);if(multiplayerRuntime){window.__eaglerNetplayFailed=true;window.__eaglerNetplayError=message;}else{const node=$('#error');if(node)node.textContent=message;}emit('error',{message,error:message});console.error(reason);};
const u32=(ptr,count)=>new Int32Array(Module.HEAPU8.buffer,ptr,count);
const sync=(populate=false)=>{const current=storageSync.then(()=>new Promise((r,j)=>Module.FS.syncfs(populate,e=>e?j(e):r())));storageSync=current.catch(()=>{});return current;};
const coreError=()=>{const p=core.th11_error(),end=Module.HEAPU8.indexOf(0,p);return new TextDecoder().decode(Module.HEAPU8.subarray(p,end<0?p+256:end));};
const save=()=>{if(launched){if(multiplayerRuntime)multiplayer.flush();else if(!core.th11_save_scores())throw Error(coreError());}return sync(false);};
async function installResources(resources=[]){return installHostResources(Module,resources,{game,emit});}
// The Launcher has already verified the ZIP identity. Match TH10's Runtime
// manifest/path/size checks before exposing any file to the native game.
let runtimePackFiles=[];
function assertRuntimePackManifest(manifest,pack){
 if(manifest?.schema!=='eagler-touhou/thcrap-static-pack/1'||manifest.game!==game||
    manifest.language!==pack.language||typeof manifest.runtimeVersion!=='string'||
    !Array.isArray(manifest.files)||manifest.files.length>256)throw Error('Invalid TH11 language pack manifest');
 for(const file of manifest.files)
  if(typeof file?.path!=='string'||!file.path.startsWith('/thcrap/th11/')||file.path.includes('\\')||file.path.includes('..')||
     !Number.isInteger(file.bytes)||file.bytes<0)throw Error('Invalid TH11 language pack file');
}
async function installRuntimePack(pack){
 if(launched)throw Error('Runtime resources cannot be changed after launch');
 if(typeof pack?.url!=='string'||typeof pack.language!=='string'||
    !Number.isInteger(pack.bytes)||pack.bytes<=0||!pack.manifest||!Array.isArray(pack.files))throw Error('Invalid TH11 language pack');
 if(new URL(pack.url,location.href).origin!==location.origin)throw Error('Cross-origin TH11 language pack');
 assertRuntimePackManifest(pack.manifest,pack);
 const expected=new Map(pack.manifest.files.map(file=>[file.path,file]));
 if(pack.files.length!==expected.size)throw Error('TH11 language pack file count mismatch');
 const verified=[];
 for(const file of pack.files){
  if(typeof file?.path!=='string'||!file.path.startsWith('/thcrap/th11/')||file.path.includes('\\')||file.path.includes('..')||
     !(file.bytes instanceof Uint8Array))throw Error('Invalid TH11 language pack path');
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
function apply(){Module.eaglerOptions=options;core.th11_always_hitbox(+(options.alwaysHitbox===true));applyTouchOptions(core,options);core.th11_music_enabled(+music);multiplayer?.applyOptions();}
let thpracKeyboardBits=0;
function thpracKey(code,down){const bit=code==='Backspace'?1:code==='Tab'?1<<8:code==='F12'?1<<9:/^F[1-7]$/.test(code)?1<<Number(code.slice(1)):0;if(!bit||!options.thpracEnabled)return false;if(down)thpracKeyboardBits|=bit;else thpracKeyboardBits&=~bit;(Module.eaglerControls??={}).thpracKeyboardBits=thpracKeyboardBits;return true;}
function clearPracticeKeys(){thpracKeyboardBits=0;if(Module)(Module.eaglerControls??={}).thpracKeyboardBits=0;}
const keyboard=createBrowserKeyboard({hostContext:{protocol,game,epoch,origin:location.origin,ready:()=>launched&&!document.hidden&&!options.netplaySpectator&&!stopping,reset:clearKeyboard,},
 accept:code=>!!scanCodes[code],
 send(code,down){if(!core||(multiplayerRuntime&&!multiplayer?.acceptInput()))return;if(thpracKey(code,down))return;core.th11_key(scanCodes[code],+down);},
 onClear:clearPracticeKeys
});
function clearKeyboard(){keyboard.clear();core?.th11_keys_clear();}
async function resumeForegroundAudio(forcePause=false){
 if(localReplay.active&&localReplay.audioPaused)return false;
 if(!Module||!core||!launched||document.hidden)return false;
 if(forcePause&&!multiplayerRuntime)core.sdl_loop_pause(1);
 const resumed=await resumeRuntimeAudio(Module,core,()=>(!localReplay.active||!localReplay.audioPaused)&&!!core&&launched&&!document.hidden);multiplayer?.afterAudioResume();return resumed;
}
function closeAudio(){
 // The launcher owns the shared context. SDL owns and must disconnect its
 // stream, but closing that stream must not close the launcher's context.
 const s=Module.SDL3,context=s?.audioContext,borrowed=parent!==window&&context===parent.__touhouAudioContext;
 if(borrowed)s.audioContext=undefined;
 try{core.th11_audio_close();}finally{if(borrowed)s.audioContext=context;}
}
async function stop({returnToMenu=false}={}){if(stopping)return;if(roomOwned&&!options.replayViewer){stopping=true;try{await save();await command({command:'return-room'});emit('room-menu');}finally{stopping=false;}return;}localReplay.close();spectatorPlayback.stop();invalidateTouches();stopping=true;try{clearKeyboard();core.th11_loop_stop();await save();multiplayer?.stop();closeAudio();launched=false;emit('exit',{code:0,status:'success',...(multiplayerRuntime&&returnToMenu?{returnToMenu:true}:{})});}finally{stopping=false;}}
function path(value){const name=String(value).replaceAll('\\','/').toLowerCase().replace(multiplayerRuntime?/^\/savesth11(?:mp)?\//:/^\/savesth11\//,'').replace(/^\//,'');const valid=multiplayerRuntime?/^replay\/(?:[^./\\\x00-\x1f][^/\\\x00-\x1f]{0,127}\/)?th11_(?:\d{2}|ud[a-z0-9]{4})\.rpy$/:/^(?:scoreth11\.dat|th11\.cfg|replay\/(?:[^./\\\x00-\x1f][^/\\\x00-\x1f]{0,127}\/)?th11_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?)$/;if(!valid.test(name))throw Error('存档路径无效');return name;}
const localReplay=createNativeLocalReplay({core:()=>core,module:()=>Module,physical:value=>saveRoot+'/'+path(value),replayState:()=>{const p=core.eagler_replay_state(),v=Array.from(new Uint32Array(core.memory.buffer,p,5));const u=multiplayerRuntime?new Uint32Array(core.memory.buffer,core.th11_mp_replay_ui_status(),12):null;return {frame:v[0],total:v[1],ended:!!v[2],stage:v[3],startStage:v[4],bootstrapTarget:u?.[4]??0};},stopLoop:()=>{clearKeyboard();core.th11_loop_stop();},pauseAudio:paused=>core.sdl_loop_pause(+paused),resumeAudio:()=>resumeForegroundAudio(),onState:playback=>emit('runtime-info',{playback}),onError:error});
async function initializeTitle(receivedTouchLifetime=touchLifetime){
 if(titleInitialized)return;
 assertTouchLifetime(receivedTouchLifetime);
 clearKeyboard();
 runtimeManifest??=await(await fetch('./manifest.json')).json();assertTouchLifetime(receivedTouchLifetime);
 await installStartupBranding(Module,{game,builtAt:runtimeManifest.builtAt});assertTouchLifetime(receivedTouchLifetime);
 if(!core.th11_prepare_loading())throw Error(coreError());
 const startupImageAt=performance.now();performance.mark('eagler-startup-image');await new Promise(requestAnimationFrame);assertTouchLifetime(receivedTouchLifetime);await new Promise(requestAnimationFrame);assertTouchLifetime(receivedTouchLifetime);
 if(!core.th11_initialize())throw Error(coreError());await finishStartupAnimation(startupImageAt,frames=>core.th11_draw_loading(frames));assertTouchLifetime(receivedTouchLifetime);performance.mark('eagler-startup-menu-ready');
 if(!multiplayerRuntime&&core.th11_phase()===4&&!core.th11_return_title())throw Error(coreError());
 titleInitialized=true;
}
async function launch(receivedTouchLifetime=touchLifetime){
 assertTouchLifetime(receivedTouchLifetime);
 if(launched){if(!launchSettled)throw Error('Runtime launch did not complete');return;}
 launchSettled=false;
 if(roomPreview)core.th11_loop_stop();
 await initializeTitle(receivedTouchLifetime);roomPreview=false;
 launched=true;apply();first=false;lastPresented=0;lastHealth=performance.now();lastFrame=0;frames=0;maxGap=0;
 const loading=$('#loading');if(loading)loading.textContent='';
 const mode=multiplayerRuntime?await multiplayer.launch():{runLoop:true};assertTouchLifetime(receivedTouchLifetime);
 if(mode.runLoop)canvas.focus({preventScroll:true});if(!multiplayerRuntime)core.sdl_loop_pause(1);if(!document.hidden)void resumeForegroundAudio();if(mode.runLoop)core.th11_loop_start();
 if(multiplayerRuntime&&!mode.runLoop){await new Promise(requestAnimationFrame);assertTouchLifetime(receivedTouchLifetime);first=true;emit('first-frame');}
 launchSettled=true;
 if(options.netplaySpectator)spectatorPlayback.start();
 emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++',architecture:protocol,version:runtimeBuild.version});
}
// Both carriers use this synchronous input-only dispatch. It never steps the
// world, polls the network, pumps audio, or starts asynchronous lifecycle work.
function dispatchInput(m){switch(m.command){
 case 'keyboard':if(launched&&!document.hidden&&!stopping){if(!multiplayer?.keyboard(m,!!m.down)&&(!multiplayerRuntime||multiplayer.acceptInput()))keyboard.event(m,!!m.down,'hosted');}return true;
 case 'keyboard-clear':clearKeyboard();return true;
 case 'touch-cancel':if(!ownsTouchDocument()){if(!touchRetired)invalidateTouches(true);}else cancelTouchInput();return true;
 case 'direct-touch':if(acceptsTouch())directTouch(core,canvas,m,{width:innerWidth,height:innerHeight});return true;
 case 'touch-controls':{
  const controls=m.controls??m,bomb=controls.bombSerial>>>0,escape=controls.escapeSerial>>>0;
  const changedBomb=bomb!==sourceBomb,changedEscape=escape!==sourceEscape;
  sourceBomb=bomb;sourceEscape=escape;if(!acceptsTouch())return true;
  const nextBomb=(mappedBomb+Number(changedBomb))>>>0,nextEscape=(mappedEscape+Number(changedEscape))>>>0;
  touchControls(core,options,{controls:{...controls,bombSerial:nextBomb,escapeSerial:nextEscape}});
  mappedBomb=nextBomb;mappedEscape=nextEscape;return true;
 }
 default:return false;
}}
async function command(m,receivedTouchLifetime=touchLifetime){
 if(dispatchInput(m))return {};
 switch(m.command){
 case 'configure':if(launched)throw Error('不能配置正在运行的游戏');language=m.language==='lang_zh-hans'?'chs':'jp';options=normalizeOptions(m.options);if(multiplayerRuntime){if(options.netplayMode==='lan'||options.replayViewer)multiplayer.validateOptions(options);}else if(options.netplayMode==='lan'||options.netplaySpectator)throw Error('此 Runtime 不支持多人游戏');music=m.music!=='none';musicMode=m.music;await installResources(m.sharedResources);await installResources(m.runtimeResources);await installResources(m.resources);if(m.runtimePack)await installRuntimePack(m.runtimePack);apply();return {};
 case 'resources':await installResources(m.resources);return {};
 case 'thprac-mouse':{if(!options.thpracEnabled||!core.sdl_thprac_mouse)return {};const r=canvas.getBoundingClientRect(),scale=Math.min(r.width/640,r.height/480);core.sdl_thprac_mouse(m.type==='down'?1:m.type==='up'?2:0,(Number(m.x)-r.left-(r.width-640*scale)/2)/scale,(Number(m.y)-r.top-(r.height-480*scale)/2)/scale);return {};}
 case 'spectator-playback':return spectatorPlayback.command(m);
 case 'playback':if(!launched||options.replayViewer!==true)throw Error('请先打开录像管理器');return localReplay.command(m);
 case 'prepare-room':roomOwned=true;
  if(!multiplayerRuntime||launched)throw Error('Invalid room preview owner');
  await initializeTitle(receivedTouchLifetime);
  if(!core.th11_room_preview())throw Error(coreError()||'Room preview rejected');
  roomPreview=true;core.th11_loop_start();$('#loading').textContent='';await new Promise(requestAnimationFrame);return {presented:true};
 case 'return-room':roomOwned=true;
  if(!multiplayerRuntime)throw Error('Room menu return is unavailable');
  spectatorPlayback.stop();clearKeyboard();core.th11_loop_stop();multiplayer.stop();
  if(!core.th11_return_title()||!core.th11_room_preview())throw Error(coreError()||'Native room return rejected');
  roomPreview=true;launched=false;launchSettled=false;core.th11_loop_start();await new Promise(requestAnimationFrame);return {presented:true};
 case 'launch':launchSerial=Number(m.launchSerial)||0;await launch(receivedTouchLifetime);return {};
 case 'sync':await save();return {};
 case 'list':return {files:listLocalReplayFiles(Module.FS,saveRoot,path)};
 case 'read':return {bytes:Array.from(Module.FS.readFile(saveRoot+'/'+path(m.path)))};
 case 'write':{
  if(launched&&!options.replayViewer)throw Error('请先退出游戏');if(multiplayerRuntime&&(!Array.isArray(m.bytes)||m.bytes.some(value=>!Number.isInteger(value)||value<0||value>255)))throw Error('录像数据无效');const target=path(m.path),bytes=new Uint8Array(m.bytes||[]);
  if(!bytes.length||bytes.length>64*1024*1024)throw Error('文件大小无效');
  if(multiplayerRuntime)multiplayer.inspect(bytes);
  else{const p=core.malloc(bytes.length);if(!p)throw Error('文件导入内存不足');let valid=false;try{Module.HEAPU8.set(bytes,p);valid=!!core.th11_validate_file(target==='scoreth11.dat'?0:target==='th11.cfg'?2:1,p,bytes.length);}finally{core.free(p);}if(!valid)throw Error('文件不是有效的地灵殿存档或录像');if(target.startsWith('replay/')&&!validateMotionReplay(bytes))throw Error('录像数据无效');}
  Module.FS.mkdirTree((saveRoot+'/'+target).slice(0,(saveRoot+'/'+target).lastIndexOf('/')));
  if(multiplayerRuntime){const file=saveRoot+'/'+target,temporary=file+'.pending';try{Module.FS.writeFile(temporary,bytes);Module.FS.rename(temporary,file);}catch(error){try{Module.FS.unlink(temporary);}catch{}throw error;}}
  else Module.FS.writeFile(saveRoot+'/'+target,bytes);await sync(false);return {};
 }
 case 'remove':{if(launched&&!options.replayViewer)throw Error('请先退出游戏');Module.FS.unlink(saveRoot+'/'+path(m.path));await sync(false);return {};}
 default:throw Error('不支持的操作');}}
let queue=Promise.resolve();
if(validEpoch)Object.defineProperty(window,'__eaglerDirectInputBridge',{configurable:true,
 value:createDirectInputBridge({protocol,game,epoch,origin:location.origin,ready:()=>!!core,
  dispatch:dispatchInput})});
async function dispatchCommand(m,receivedTouchLifetime=touchLifetime){
 try{const result=await command(m,receivedTouchLifetime);if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:true,...result},location.origin);}
 catch(e){if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:false,error:String(e),errno:e?.errno},location.origin);else if(!(e instanceof SupersededTouchCommand))error(e);}
}
window.addEventListener('message',event=>{
 const m=event.data;if(!validEpoch||event.source!==parent||event.origin!==location.origin||m?.protocol!==protocol||m.game!==game||m.epoch!==epoch||typeof m.command!=='string')return;
 // The validated input task must not wait for resource or IDB operations.
 // Its synchronous command body checks current document/lifecycle eligibility;
 // it never advances the simulation or services the network here.
 if(m.command==='keyboard'||m.command==='keyboard-clear'||m.command==='direct-touch'||m.command==='touch-controls'||m.command==='touch-cancel'){void dispatchCommand(m);return;}
 const receivedTouchLifetime=touchLifetime;
 queue=queue.then(async()=>{if(await initialized===false)return;await dispatchCommand(m,receivedTouchLifetime);}).catch(error);
});
document.addEventListener('visibilitychange',()=>{cancelTouchInput();if(!core||!launched)return;clearKeyboard();if(document.hidden){suspendRuntimeAudio(Module,core);queue=queue.then(save).catch(error);}else void resumeForegroundAudio(true);});
window.addEventListener('blur',()=>{clearKeyboard();cancelTouchInput();});
window.addEventListener('eagler-thprac-menu',event=>emit('thprac-menu',{open:!!event.detail?.open}));
window.addEventListener('pagehide',event=>{
 // BFCache keeps this document/core. Suspend transient gestures without
 // retiring its launch or resetting the accepted/source action serials.
 if(event.persisted){touchPageSuspended=true;cancelTouchInput();}else invalidateTouches(true);
 clearKeyboard();if(core&&launched){suspendRuntimeAudio(Module,core);void save().catch(console.error);}
});
window.addEventListener('pageshow',event=>{
 if(!ownsTouchDocument()){if(!touchRetired)invalidateTouches(true);return;}
 if(touchRetired)return;
 if(event.persisted)touchPageSuspended=false;
 if(!touchPageSuspended&&core&&launched&&!document.hidden)void resumeForegroundAudio(true);
});
canvas.addEventListener('webglcontextlost',event=>{event.preventDefault();clearKeyboard();core?.sdl_loop_pause(1);error('图形环境已失效，请退出后重新开始。');});
for(const name of ['pointerdown','keydown'])window.addEventListener(name,()=>{if(Module?.SDL3?.audioContext?.state!=='running')void resumeForegroundAudio(true);},{capture:true});
for(const name of ['keydown','keyup'])window.addEventListener(name,event=>{
 if(!core||!launched||stopping||document.hidden)return;
 if(multiplayerRuntime){if(multiplayer.keyboard(event,name==='keydown')){event.preventDefault();return;}if(!multiplayer.acceptInput())return;}
 if(keyboard.mapNative(event,name==='keydown')){event.preventDefault();event.stopImmediatePropagation();return;}
 if(keyboard.event(event,name==='keydown'))event.preventDefault();
},{capture:true});
// A mobile browser may terminate a hidden page before pagehide's IDB callback.
// Persist during play as well, with writes serialized by the sync chain.
setInterval(()=>{if(launched&&!document.hidden&&core)try{queue=queue.then(()=>save()).catch(error);}catch(e){error(e);}},30000);
const initialized=(async()=>{
 if(multiplayerRuntime)runtimeManifest=await fetch('./manifest.json').then(response=>{if(!response.ok)throw Error('TH11 多人 Runtime manifest 缺失');return response.json();});
 let audioContext;try{audioContext=parent.__touhouAudioContext;}catch{}
 // Emscripten only observes the hook's success callback, not its returned
 // Promise. Route asynchronous failure to this initialization's error owner.
 let rejectWasm;const wasmFailure=new Promise((_,reject)=>{rejectWasm=reject;});
 Module=await Promise.race([createModule({canvas,noInitialRun:true,resetBrowserKeyboard:()=>keyboard.clear(),...(audioContext?{SDL3:{audioContext}}:{}),print:console.log,printErr:console.error,
  instantiateWasm(imports,ready){void WebAssembly.instantiateStreaming(fetch('./th11-sdl.wasm'),imports).then(({instance,module})=>{core=instance.exports;ready(instance,module);}).catch(rejectWasm);return {};}}),wasmFailure]);
 window.Module=Module;window.FS=Module.FS;window.core=core;
 observeMusicWrites(Module,core,game);
 Module.FS.mkdirTree(saveRoot);Module.FS.mount(Module.IDBFS,{},saveRoot);await sync(true);
 Module.FS.mkdirTree(saveRoot+'/replay');Module.FS.symlink(saveRoot,'/save');
 await mountData();
 if(multiplayerRuntime)multiplayer=createMultiplayerRuntime({Module,core,getOptions:()=>options,getLanguage:()=>language,manifest:runtimeManifest,emit,onError:error,onExit:stop,sync,canvas});
 let last=performance.now(),ticks=0;
 Module.onGameFrame=(ok,ms,count)=>{if(!ok){if(multiplayerRuntime)multiplayer.fail(Error(coreError()));else error(Error(coreError()));return;}try{multiplayer?.frame();}catch(reason){multiplayer.fail(reason);return;}ticks+=count;if(!first&&(!multiplayerRuntime||count>0)){first=true;last=performance.now();ticks=0;emit('first-frame');}const now=performance.now();if(now-last>=1000){emit('frame-health',{fps:ticks*1000/(now-last),maxGapMs:ms});const a=u32(core.th11_audio_statistics(),9);emit('audio-health',{queuedMs:a[4]*1000/44100,minQueuedMs:0,backend:'script',underruns:a[3]});last=now;ticks=0;}if(!multiplayerRuntime&&core.th11_phase()===4)void stop().catch(error);};
 window.__th11Runtime={core,Module,get app(){return launched?1:0;},get multiplayer(){return multiplayer;},get saveRoot(){return saveRoot;},launch,stop,command};
 if(!ownsTouchDocument()||touchRetired)return false;
 initializedReady=true;emit('ready',multiplayerRuntime?{roomPreview:true,roomReturn:true}:{});
})().catch(e=>{if(isSupersededRuntimeError(e)){console.debug('Runtime navigation superseded');return false;}error(e);throw e;});
async function mountData(){
 // eagler-touhou managed package: the retail archive arrives through the parent
 // (written to /th11.dat, which the C++ archive layer opens) and the baked font
 // tables through the sibling resources.json.
 await mountManagedData(Module,{game,parentWindow:parent,query,emit});
}
