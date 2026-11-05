// Platform shell for the upstream eagler-touhou/1 Launcher contract.
// Game construction, input, timing, rendering, text and sound belong to C++.
// Mirrors th10/th20 shell.mjs and imports the shared eagler-host transport.
import {installStartupBranding,finishStartupAnimation} from './startup-branding.mjs';
import createModule from './th11-sdl.mjs';
import {scanCodes} from './keyboard.mjs';
import {createBrowserKeyboard} from './directory-keyboard.mjs';
import {validateMotionReplay} from './motion-replay.mjs';
import {bindOutsideTouches,normalizeOptions,applyTouchOptions,touchControls,suspendRuntimeAudio,resumeRuntimeAudio,directTouch,installResources as installHostResources,observeMusicWrites,mountManagedData,isSupersededRuntimeError} from './eagler-host.mjs';
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
const runtimeBuild=/*TH11_BUILD_INFO*/{version:'development-incomplete',completeGame:false};
const protocol='eagler-touhou/1',game='th11',query=new URLSearchParams(location.search),canvas=document.querySelector('canvas');
const epoch=Number(query.get('runtimeEpoch'));
const validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...fields},location.origin);
const $=s=>document.querySelector(s);
let Module,core,app=0,launched=false,first=false,stopping=false,language=query.get('language')==='lang_zh-hans'?'chs':'jp',options={},music=true,musicMode='none';
let frames=0,lastHealth=0,lastFrame=0,maxGap=0,lastPresented=0,saveTimer=null,storageSync=Promise.resolve();
const cancelTouches=bindOutsideTouches(document,canvas,()=>core,()=>launched&&options.touchEnabled);
const error=reason=>{const message=reason?.stack||String(reason);const node=$('#error');if(node)node.textContent=message;emit('error',{message,error:message});console.error(reason);};
const u32=(ptr,count)=>new Int32Array(Module.HEAPU8.buffer,ptr,count);
const sync=(populate=false)=>{const current=storageSync.then(()=>new Promise((r,j)=>Module.FS.syncfs(populate,e=>e?j(e):r())));storageSync=current.catch(()=>{});return current;};
const coreError=()=>{const p=core.th11_error(),end=Module.HEAPU8.indexOf(0,p);return new TextDecoder().decode(Module.HEAPU8.subarray(p,end<0?p+256:end));};
const save=()=>{if(launched&&!core.th11_save_scores())throw Error(coreError());return sync(false);};
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
function apply(){Module.eaglerOptions=options;core.th11_always_hitbox(+(options.alwaysHitbox===true));applyTouchOptions(core,options);core.th11_music_enabled(+music);}
let thpracKeyboardBits=0;
function thpracKey(code,down){const bit=code==='Backspace'?1:code==='Tab'?1<<8:code==='F12'?1<<9:code==='KeyU'?1<<10:/^F[1-7]$/.test(code)?1<<Number(code.slice(1)):0;if(!bit||!options.thpracEnabled)return false;if(down)thpracKeyboardBits|=bit;else thpracKeyboardBits&=~bit;(Module.eaglerControls??={}).thpracKeyboardBits=thpracKeyboardBits;return true;}
function clearPracticeKeys(){thpracKeyboardBits=0;if(Module)(Module.eaglerControls??={}).thpracKeyboardBits=0;}
const keyboard=createBrowserKeyboard({hostContext:{protocol,game,epoch,origin:location.origin,ready:()=>launched&&!document.hidden&&!options.netplaySpectator&&!stopping,reset:clearKeyboard,},
 accept:code=>!!scanCodes[code],
 send(code,down){if(!core)return;if(thpracKey(code,down))return;core.th11_key(scanCodes[code],+down);},
 onClear:clearPracticeKeys
});
function clearKeyboard(){keyboard.clear();core?.th11_keys_clear();}
async function resumeForegroundAudio(forcePause=false){
 if(localReplay.active&&localReplay.audioPaused)return false;
 if(!Module||!core||!launched||document.hidden)return false;
 if(forcePause)core.sdl_loop_pause(1);
 return resumeRuntimeAudio(Module,core,()=>(!localReplay.active||!localReplay.audioPaused)&&!!core&&launched&&!document.hidden);
}
function closeAudio(){
 // The launcher owns the shared context. SDL owns and must disconnect its
 // stream, but closing that stream must not close the launcher's context.
 const s=Module.SDL3,context=s?.audioContext,borrowed=parent!==window&&context===parent.__touhouAudioContext;
 if(borrowed)s.audioContext=undefined;
 try{core.th11_audio_close();}finally{if(borrowed)s.audioContext=context;}
}
async function stop(){if(stopping)return;stopping=true;localReplay.close();try{clearKeyboard();core.th11_loop_stop();await save();closeAudio();launched=false;emit('exit',{code:0,status:'success'});}finally{stopping=false;}}
function path(value){const name=String(value).replaceAll('\\','/').toLowerCase().replace(/^\/savesth11\//,'').replace(/^\//,'');if(!/^(?:scoreth11\.dat|th11\.cfg|replay\/(?:[^./\\\x00-\x1f][^/\\\x00-\x1f]{0,127}\/)?th11_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?)$/.test(name))throw Error('存档路径无效');return name;}
const localReplay=createNativeLocalReplay({core:()=>core,module:()=>Module,physical:value=>'/savesth11/'+path(value),stopLoop:()=>{clearKeyboard();core.th11_loop_stop();},pauseAudio:paused=>core.sdl_loop_pause(+paused),resumeAudio:()=>resumeForegroundAudio(),onState:playback=>emit('runtime-info',{playback}),onError:error});
async function launch(){
 if(launched)return;clearKeyboard();
 await installStartupBranding(Module,{game,builtAt:(await(await fetch('./manifest.json')).json()).builtAt});
 if(!core.th11_prepare_loading())throw Error(coreError());
 const startupImageAt=performance.now();performance.mark('eagler-startup-image');await new Promise(requestAnimationFrame);await new Promise(requestAnimationFrame);
 if(!core.th11_initialize())throw Error(coreError());await finishStartupAnimation(startupImageAt,frames=>core.th11_draw_loading(frames));performance.mark('eagler-startup-menu-ready');
 if(core.th11_phase()===4&&!core.th11_return_title())throw Error(coreError());
 launched=true;apply();first=false;lastPresented=0;lastHealth=performance.now();lastFrame=0;frames=0;maxGap=0;
 const loading=$('#loading');if(loading)loading.textContent='';
 canvas.focus({preventScroll:true});core.sdl_loop_pause(1);if(!document.hidden)void resumeForegroundAudio();core.th11_loop_start();
 emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++',architecture:protocol,version:runtimeBuild.version});
}
async function command(m){switch(m.command){
 case 'configure':if(launched)throw Error('不能配置正在运行的游戏');language=m.language==='lang_zh-hans'?'chs':'jp';options=normalizeOptions(m.options);music=m.music!=='none';musicMode=m.music;await installResources(m.sharedResources);await installResources(m.runtimeResources);await installResources(m.resources);if(m.runtimePack)await installRuntimePack(m.runtimePack);apply();return {};
 case 'resources':await installResources(m.resources);return {};
 case 'keyboard':if(launched&&!document.hidden&&!stopping)keyboard.event(m,!!m.down,'hosted');return {};
 case 'keyboard-clear':clearKeyboard();return {};
 case 'thprac-mouse':{if(!options.thpracEnabled||!core.sdl_thprac_mouse)return {};const r=canvas.getBoundingClientRect(),scale=Math.min(r.width/640,r.height/480);core.sdl_thprac_mouse(m.type==='down'?1:m.type==='up'?2:0,(Number(m.x)-r.left-(r.width-640*scale)/2)/scale,(Number(m.y)-r.top-(r.height-480*scale)/2)/scale);return {};}
 case 'touch-cancel':cancelTouches();return {};
 case 'direct-touch':directTouch(core,canvas,m,{width:innerWidth,height:innerHeight});return {};
 case 'touch-controls':touchControls(core,options,m,Module);return {};
 case 'playback':if(!launched||options.replayViewer!==true)throw Error('请先打开录像管理器');return localReplay.command(m);
 case 'launch':await launch();return {};
 case 'sync':await save();return {};
 case 'list':return {files:listLocalReplayFiles(Module.FS,'/savesth11',path)};
 case 'read':return {bytes:Array.from(Module.FS.readFile('/savesth11/'+path(m.path)))};
 case 'write':{
  if(launched&&!options.replayViewer)throw Error('请先退出游戏');const target=path(m.path),bytes=new Uint8Array(m.bytes||[]);
  if(!bytes.length||bytes.length>64*1024*1024)throw Error('文件大小无效');
  const p=core.malloc(bytes.length);if(!p)throw Error('文件导入内存不足');let valid=false;try{Module.HEAPU8.set(bytes,p);valid=!!core.th11_validate_file(target==='scoreth11.dat'?0:target==='th11.cfg'?2:1,p,bytes.length);}finally{core.free(p);}
  if(!valid)throw Error('文件不是有效的地灵殿存档或录像');
  if(target.startsWith('replay/')&&!validateMotionReplay(bytes))throw Error('录像数据无效');
  Module.FS.mkdirTree(('/savesth11'+'/'+target).slice(0,('/savesth11'+'/'+target).lastIndexOf('/')));Module.FS.writeFile('/savesth11/'+target,bytes);await sync(false);return {};
 }
 case 'remove':{if(launched&&!options.replayViewer)throw Error('请先退出游戏');Module.FS.unlink('/savesth11/'+path(m.path));await sync(false);return {};}
 default:throw Error('不支持的操作');}}
let queue=Promise.resolve();
async function dispatchCommand(m){
 try{const result=await command(m);if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:true,...result},location.origin);}
 catch(e){if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:false,error:String(e),errno:e?.errno},location.origin);else error(e);}
}
window.addEventListener('message',event=>{
 const m=event.data;if(!validEpoch||event.source!==parent||event.origin!==location.origin||m?.protocol!==protocol||m.game!==game||m.epoch!==epoch||typeof m.command!=='string')return;
 if(m.command==='keyboard'||m.command==='keyboard-clear'){void dispatchCommand(m);return;}
 queue=queue.then(async()=>{if(await initialized===false)return;await dispatchCommand(m);}).catch(error);
});
document.addEventListener('visibilitychange',()=>{if(!core||!launched)return;clearKeyboard();cancelTouches();if(document.hidden){suspendRuntimeAudio(Module,core);queue=queue.then(save).catch(error);}else void resumeForegroundAudio(true);});
window.addEventListener('blur',()=>{clearKeyboard();if(core)cancelTouches();});
window.addEventListener('eagler-thprac-menu',event=>emit('thprac-menu',{open:!!event.detail?.open}));
window.addEventListener('pagehide',()=>{clearKeyboard();cancelTouches();if(core&&launched){suspendRuntimeAudio(Module,core);void save().catch(console.error);}});
window.addEventListener('pageshow',()=>{if(core&&launched&&!document.hidden)void resumeForegroundAudio(true);});
canvas.addEventListener('webglcontextlost',event=>{event.preventDefault();clearKeyboard();core?.sdl_loop_pause(1);error('图形环境已失效，请退出后重新开始。');});
for(const name of ['pointerdown','keydown'])window.addEventListener(name,()=>{if(Module?.SDL3?.audioContext?.state!=='running')void resumeForegroundAudio(true);},{capture:true});
for(const name of ['keydown','keyup'])window.addEventListener(name,event=>{
 if(!core||!launched||stopping||document.hidden)return;
 if(keyboard.mapNative(event,name==='keydown')){event.preventDefault();event.stopImmediatePropagation();return;}
 if(keyboard.event(event,name==='keydown'))event.preventDefault();
},{capture:true});
// A mobile browser may terminate a hidden page before pagehide's IDB callback.
// Persist during play as well, with writes serialized by the sync chain.
setInterval(()=>{if(launched&&!document.hidden&&core)try{queue=queue.then(()=>save()).catch(error);}catch(e){error(e);}},30000);
const initialized=(async()=>{
 let audioContext;try{audioContext=parent.__touhouAudioContext;}catch{}
 Module=await createModule({canvas,noInitialRun:true,resetBrowserKeyboard:()=>keyboard.clear(),...(audioContext?{SDL3:{audioContext}}:{}),print:console.log,printErr:console.error,
  instantiateWasm(imports,ready){return WebAssembly.instantiateStreaming(fetch('./th11-sdl.wasm'),imports).then(({instance,module})=>{core=instance.exports;ready(instance,module);return core;});}});
 window.Module=Module;window.FS=Module.FS;window.core=core;
 observeMusicWrites(Module,core,game);
 Module.FS.mkdirTree('/savesth11');Module.FS.mount(Module.IDBFS,{},'/savesth11');await sync(true);
 Module.FS.mkdirTree('/savesth11/replay');Module.FS.symlink('/savesth11','/save');
 await mountData();
 let last=performance.now(),ticks=0;
 Module.onGameFrame=(ok,ms,count)=>{if(!ok){error(Error(coreError()));return;}ticks+=count;if(!first){first=true;last=performance.now();ticks=0;emit('first-frame');}const now=performance.now();if(now-last>=1000){emit('frame-health',{fps:ticks*1000/(now-last),maxGapMs:ms});const a=u32(core.th11_audio_statistics(),9);emit('audio-health',{queuedMs:a[4]*1000/44100,minQueuedMs:0,backend:'script',underruns:a[3]});last=now;ticks=0;}if(core.th11_phase()===4)void stop().catch(error);};
 emit('ready');
})().catch(e=>{if(isSupersededRuntimeError(e)){console.debug('Runtime navigation superseded');return false;}error(e);throw e;});
async function mountData(){
 // eagler-touhou managed package: the retail archive arrives through the parent
 // (written to /th11.dat, which the C++ archive layer opens) and the baked font
 // tables through the sibling resources.json.
 await mountManagedData(Module,{game,parentWindow:parent,query,emit});
}
