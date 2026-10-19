import {installStartupBranding,finishStartupAnimation} from './startup-branding.mjs';
import createModule from './th15.mjs';
import {createBrowserKeyboard} from './directory-keyboard.mjs';
import {scanCodes} from './keyboard.mjs';
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
const runtimeBuild=/*TH15_BUILD_INFO*/{version:'development-incomplete',completeGame:false};
const game='th15',protocol='eagler-touhou/1',query=new URLSearchParams(location.search),canvas=document.querySelector('canvas'),$=s=>document.querySelector(s);
const epoch=Number(query.get('runtimeEpoch'));
const validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
const saveRoot=runtimeBuild.multiplayer?'/savesth15mp':'/savesth15';
let multiplayerBridge=null;
let core,launched=false,first=false,stopping=false,options={},music=true,chain=Promise.resolve(),queue=Promise.resolve();
let roomPreview=false;
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...fields},location.origin);
const spectatorPlayback=createSpectatorPlaybackBridge({
 active:()=>runtimeBuild.multiplayer&&launched&&options.netplaySpectator===true&&!stopping,memory:()=>core.HEAPU8,
 state:()=>core?._th15_mp_spectator_state?.()||0,control:(run,action,frame)=>core?._th15_mp_spectator_control?.(run,action,frame)||0,
 allocate:size=>core._malloc(size),deallocate:pointer=>core._free(pointer),emit,
});
const keyboard=createBrowserKeyboard({hostContext:{protocol,game,epoch,origin:location.origin,ready:()=>launched&&!document.hidden&&!options.netplaySpectator&&!stopping,reset:clearKeyboard,},accept:code=>!!scanCodes[code],send:(code,down)=>core?._th15_key(scanCodes[code],+down)});
function clearKeyboard(){keyboard.clear();core?._th15_keys_clear();}
const err=()=>{let p=runtimeBuild.multiplayer?core._th15_mp_error():core._th15_error();if(runtimeBuild.multiplayer&&!core.HEAPU8[p])p=core._th15_error();const end=core.HEAPU8.indexOf(0,p);return new TextDecoder().decode(core.HEAPU8.subarray(p,end<0?p+256:end));};
const fatal=e=>{const message=e?.message||String(e);$('#error').textContent=message;core?._th15_loop_pause(1);emit('error',{error:message});};
const sync=(populate=false)=>{const current=chain.then(()=>new Promise((r,j)=>core.FS.syncfs(populate,e=>e?j(e):r())));chain=current.catch(()=>{});return current;};
const save=()=>{multiplayerBridge?.save();if(launched&&!core._th15_save_scores())throw Error(err());return sync();};
function closeAudio(){
 // The launcher owns the shared context. SDL owns and must disconnect its
 // stream, but closing that stream must not close the launcher's context.
 const s=core.SDL3,context=s?.audioContext,borrowed=parent!==window&&context===parent.__touhouAudioContext;
 if(borrowed)s.audioContext=undefined;
 try{core._th15_audio_close();}finally{if(borrowed)s.audioContext=context;}
}
async function stop(){if(stopping)return;stopping=true;localReplay.close();spectatorPlayback.stop();try{core._th15_loop_stop();await save();multiplayerBridge?.close();closeAudio();launched=false;emit('exit',{code:0,status:'success'});}finally{stopping=false;}}
function path(value){const name=String(value).replaceAll('\\','/').toLowerCase().replace(/^\/savesth15(?:mp)?\//,'').replace(/^\//,'');if(!/^(?:scoreth15\.dat|th15\.cfg|replay\/(?:[^./\\\x00-\x1f][^/\\\x00-\x1f]{0,127}\/)?th15(?:mp)?_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?|autosave\/save[0-3]_[0-4]\.dat)$/.test(name))throw Error('存档路径无效');return name;}
function apply(){core.eaglerOptions=options;core._th15_always_hitbox(+(options.alwaysHitbox===true));core._th15_limit_presentation(+!!options.limitPresentationTo60);core._th15_touch_options(+!!options.touchEnabled,Math.max(0,['touch','touch-unlimited','joystick','joystick-free'].indexOf(runtimeBuild.multiplayer&&options.touchMovementMode==='joystick-free'?'joystick':options.touchMovementMode)),Math.max(100,Math.min(300,Number(options.touchSensitivity)||100))/100,+(options.touchFocusMode==='two-finger'),+!!options.doubleTapBombEnabled);core._th15_music_enabled(+music);}
async function resource(r){
 if(!['/msgothic.ttc','/unifont.otf','/fonts/cp932.bin'].includes(r.path)&&!/^\/music\/(?:th15_\d{2}|th128_08)\.ogg$/.test(r.path))throw Error('资源路径无效');
 const u=new URL(r.url,location.href);if(u.origin!==location.origin||!['http:','https:','blob:'].includes(u.protocol))throw Error('资源来源无效');
 const response=await fetch(u);if(!response.ok)throw Error('资源读取失败');const bytes=new Uint8Array(await response.arrayBuffer());
 const expected=r.bytes??r.size;if(bytes.length>64*1024*1024||(expected!=null&&bytes.length!==expected))throw Error('资源大小错误');
 core.FS.mkdirTree(r.path.slice(0,r.path.lastIndexOf('/'))||'/');core.FS.writeFile(r.path,bytes,{canOwn:true});
 emit('transfer',{mode:r.path.startsWith('/music/')?'ogg':'runtime',loaded:bytes.length,total:bytes.length,path:r.path});
}
let runtimePackFiles=[];
async function installRuntimePack(pack){
 if(launched)throw Error('Runtime resources cannot be changed after launch');
 if(typeof pack?.url!=='string'||typeof pack.language!=='string'||!Number.isInteger(pack.bytes)||pack.bytes<=0||!pack.manifest||!Array.isArray(pack.files)||new URL(pack.url,location.href).origin!==location.origin)throw Error('Invalid TH15 language pack');
 const manifest=pack.manifest;
 if(manifest.schema!=='eagler-touhou/thcrap-static-pack/1'||manifest.game!==game||manifest.language!==pack.language||typeof manifest.runtimeVersion!=='string'||!Array.isArray(manifest.files)||manifest.files.length>256)throw Error('Invalid TH15 language manifest');
 const safe=path=>typeof path==='string'&&path.startsWith('/thcrap/th15/')&&path.length<=240&&!path.includes('\\')&&path.split('/').slice(1).every(p=>p&&p!=='.'&&p!=='..');
 const expected=new Map();for(const file of manifest.files){if(!safe(file.path)||!Number.isInteger(file.bytes)||file.bytes<0||file.bytes>64*1024*1024||expected.has(file.path))throw Error('Invalid TH15 language file');expected.set(file.path,file);}
 if(pack.files.length!==expected.size)throw Error('TH15 language file count mismatch');
 const seen=new Set();for(const file of pack.files){if(!safe(file?.path)||seen.has(file.path)||!(file.bytes instanceof Uint8Array)||file.bytes.length!==expected.get(file.path)?.bytes)throw Error('TH15 language file mismatch');seen.add(file.path);}
 for(const path of runtimePackFiles){try{core.FS.unlink(path);}catch{}}runtimePackFiles=[];
 for(const file of pack.files){core.FS.mkdirTree(file.path.slice(0,file.path.lastIndexOf('/')));core.FS.writeFile(file.path,file.bytes,{canOwn:true});runtimePackFiles.push(file.path);}
 // Runtime-owned locale marker comes from the validated manifest, not text guessing.
 const localePath='/thcrap/th15/runtime-language.txt';core.FS.writeFile(localePath,pack.language);runtimePackFiles.push(localePath);
}
const localReplay=createNativeLocalReplay({core:()=>core,module:()=>core,physical:value=>saveRoot+'/'+path(value),
 stopLoop:()=>{clearKeyboard();core._th15_loop_stop();},
 openReplay:async(value,stage)=>{if(runtimeBuild.multiplayer)return multiplayerBridge.openReplay(path(value),stage);const bytes=new TextEncoder().encode(saveRoot+'/'+path(value)+'\0'),p=core._malloc(bytes.length);try{core.HEAPU8.set(bytes,p);if(!core._eagler_replay_open(p,stage))throw Error(err());}finally{core._free(p);}},
 replayState:()=>{const p=core._eagler_replay_state(),v=Array.from(core.HEAPU32.subarray(p/4,p/4+6));return {frame:v[0],total:v[1],ended:!!v[2],stage:v[3],startStage:v[4],bootstrapTarget:v[5]};},
 onState:playback=>emit('runtime-info',{playback}),onError:fatal});
async function initializeTitle(){
 await installStartupBranding(core,{game,builtAt:(await(await fetch('./manifest.json')).json()).builtAt});
 let scaleOption=query.get('renderScale');if(!scaleOption)try{scaleOption=new URLSearchParams(parent.location.search).get('renderScale');}catch{}
 const requested=Number(scaleOption);core._th15_render_scale(options.renderScale===1?1:options.renderScale===2?2:requested===1?1:2);
 if(!core._th15_prepare_loading())throw Error(err());$('#loading').textContent='';
 const startupImageAt=performance.now();performance.mark('eagler-startup-image');
 await new Promise(requestAnimationFrame);await new Promise(requestAnimationFrame);
 if(!core._th15_initialize())throw Error(err());
 await finishStartupAnimation(startupImageAt,frames=>core._th15_draw_startup(frames));
 performance.mark('eagler-startup-menu-ready');
}
async function command(m){switch(m.command){
case 'prepare-room':
 if(!runtimeBuild.multiplayer||launched||options.netplayMode==='lan'||options.replayViewer)throw Error('Room preview requires an unlaunched multiplayer title');
 if(!roomPreview){await initializeTitle();if(!core._th15_room_preview())throw Error(err()||'Room title did not present');roomPreview=true;core._th15_loop_start();$('#loading').textContent='';await new Promise(requestAnimationFrame);}
 return {presented:true};
case 'configure':if(launched)throw Error('不能配置正在运行的游戏');if(!['ogg','none'].includes(m.music))throw Error('不支持的音乐模式');options=m.options||{};music=m.music==='ogg';for(const r of [...(m.sharedResources||[]),...(m.runtimeResources||[]),...(m.resources||[])])await resource(r);if(m.runtimePack)await installRuntimePack(m.runtimePack);apply();return {};
case 'resources':for(const r of m.resources||[])await resource(r);return {};
case 'keyboard':if(launched&&!document.hidden&&!stopping)keyboard.event(m,!!m.down,'hosted');return {};
case 'keyboard-clear':clearKeyboard();return {};
case 'touch-cancel':core._th15_touch_cancel();return {};
case 'direct-touch':{if(m.type==='cancel'){core._th15_touch_cancel();return {};}if(!['down','move','up'].includes(m.type)||![m.x,m.y,m.id].every(Number.isFinite))return {};const b=canvas.getBoundingClientRect();if(b.width&&b.height)core._th15_touch(({down:0,move:1,up:2,cancel:2})[m.type]??2,Number(m.id)||0,(Number(m.x)*innerWidth-b.left)/b.width,(Number(m.y)*innerHeight-b.top)/b.height);return {};}
case 'touch-controls':{const t=m.controls||m;core.eaglerControls=t;if(Number.isFinite(t.touchSensitivity)&&t.touchSensitivity>=100&&t.touchSensitivity<=300&&t.touchSensitivity!==options.touchSensitivity){options.touchSensitivity=t.touchSensitivity;apply();}core._th15_touch_controls(+!!options.touchEnabled,+!!t.fireEnabled,+!!t.focusEnabled,t.bombSerial>>>0,t.escapeSerial>>>0);core._th15_touch_stick(Number(t.joystickX)||0,Number(t.joystickY)||0);return {};}
case 'thprac-mouse':{if(!options.thpracEnabled||!['down','move','up'].includes(m.type)||![m.x,m.y].every(Number.isFinite))return {};const r=canvas.getBoundingClientRect(),scale=Math.min(r.width/640,r.height/480);if(scale>0)core._th15_thprac_mouse(m.type==='down'?1:m.type==='up'?2:0,(m.x-r.left-(r.width-640*scale)/2)/scale,(m.y-r.top-(r.height-480*scale)/2)/scale);return {};}
case 'spectator-playback':return spectatorPlayback.command(m);
case 'playback':if(!launched||options.replayViewer!==true)throw Error('请先打开录像管理器');return localReplay.command(m);
case 'launch':if(!launched){if(roomPreview)core._th15_loop_stop();else await initializeTitle();launched=true;roomPreview=false;apply();first=false;$('#loading').textContent='';if(!runtimeBuild.multiplayer||await multiplayerBridge.launch())core._th15_loop_start();if(runtimeBuild.multiplayer&&options.replayViewer){first=true;emit('first-frame');}if(options.netplaySpectator)spectatorPlayback.start();emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++',architecture:protocol,...runtimeBuild});}return {};
case 'sync':await save();return {};
case 'list':return {files:listLocalReplayFiles(core.FS,saveRoot,path)};
case 'read':return {bytes:Array.from(core.FS.readFile(saveRoot+'/'+path(m.path)))};
case 'write':{
 if(launched&&!options.replayViewer)throw Error('请先退出游戏');const target=path(m.path),bytes=new Uint8Array(m.bytes||[]);
 if(!bytes.length||bytes.length>(target.startsWith('autosave/')?100*1024*1024+96:64*1024*1024))throw Error('文件大小无效');
 const p=core._malloc(bytes.length);if(!p)throw Error('文件导入内存不足');let valid=false;try{core.HEAPU8.set(bytes,p);valid=runtimeBuild.multiplayer&&target.startsWith('replay/')?!!core._th15_mp_replay_validate(p,bytes.length):!!core._th15_validate_file(target==='scoreth15.dat'?0:target==='th15.cfg'?2:target.startsWith('autosave/')?3:1,p,bytes.length);}finally{core._free(p);}
 if(!valid)throw Error('文件不是有效的绀珠传存档或录像');if(target.startsWith('autosave/')){const parts=/save([0-3])_([0-4])/.exec(target),header=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);if(header.getInt32(16,true)!==Number(parts[1])||header.getInt32(20,true)!==Number(parts[2])||header.getInt32(24,true)<1||header.getInt32(24,true)>7)throw Error('章节存档与角色、难度不一致');}core.FS.mkdirTree((saveRoot+'/'+target).slice(0,(saveRoot+'/'+target).lastIndexOf('/')));core.FS.writeFile(saveRoot+'/'+target,bytes);await sync();return {};
}
case 'remove':if(launched&&!options.replayViewer)throw Error('请先退出游戏');core.FS.unlink(saveRoot+'/'+path(m.path));await sync();return {};
default:throw Error('不支持的操作');}}
window.addEventListener('message',e=>{const m=e.data;if(e.source!==parent||e.origin!==location.origin||m?.protocol!==protocol||m.game!==game||!validEpoch||m.epoch!==epoch||typeof m.command!=='string')return;queue=queue.then(async()=>{if(await initialized===false)return;try{const result=await command(m);if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:true,...result},location.origin);}catch(e){if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:false,error:String(e),errno:e?.errno},location.origin);else fatal(e);}}).catch(fatal);});
document.addEventListener('visibilitychange',()=>{if(launched){clearKeyboard();core._th15_touch_cancel();core._th15_loop_pause(+document.hidden);if(document.hidden)try{void save().catch(fatal);}catch(e){fatal(e);}}});
window.addEventListener('blur',()=>{clearKeyboard();core?._th15_touch_cancel();});
window.addEventListener('eagler-thprac-menu',e=>emit('thprac-menu',{open:!!e.detail?.open}));
// Keyboard events in the child realm do not bubble to the launcher. SDL's
// canvas listener alone misses keys when the child BODY owns focus.
for(const event of ['keydown','keyup'])window.addEventListener(event,e=>{if(!launched)return;if(keyboard.mapNative(e,event==='keydown')){e.preventDefault();e.stopImmediatePropagation();return;}if(e.altKey||e.metaKey)return;if(keyboard.event(e,event==='keydown')){e.preventDefault();e.stopPropagation();}},{capture:true});
for(const event of ['pointerdown','keydown'])window.addEventListener(event,()=>core?.SDL3?.audioContext?.resume().catch(()=>{}),{capture:true});
for(const [name,type] of [['pointerdown',0],['pointermove',1],['pointerup',2],['pointercancel',2]])document.body.addEventListener(name,e=>{
 if(!launched||!options.touchEnabled||e.pointerType==='mouse')return;if(e.type==='pointercancel'){core._th15_touch_cancel();return;}e.preventDefault();if(type===0)document.body.setPointerCapture(e.pointerId);const b=canvas.getBoundingClientRect();if(b.width&&b.height)core._th15_touch(type,e.pointerId,(e.clientX-b.left)/b.width,(e.clientY-b.top)/b.height);
});
canvas.addEventListener('webglcontextlost',e=>{e.preventDefault();fatal(Error('图形环境失效，请退出后重新开始。'));});
window.addEventListener('pagehide',()=>{clearKeyboard();core?._th15_touch_cancel();if(roomPreview){core._th15_loop_stop();closeAudio();}else if(launched){core._th15_loop_stop();try{void save().catch(fatal);}catch(e){fatal(e);}closeAudio();}});
// A mobile browser may terminate a hidden page before pagehide's IDB callback.
// Persist during play as well, with writes serialized by the sync chain.
setInterval(()=>{if(launched&&!document.hidden)try{void save().catch(fatal);}catch(e){fatal(e);}},30000);
const initialized=(async()=>{
 let last=performance.now(),ticks=0,presents=0,maxFrameMs=0;
 core=await createModule({canvas,printErr:console.error,onGameFrame(ok,ms,count){if(!ok){fatal(Error(err()));return;}ticks+=count;presents++;maxFrameMs=Math.max(maxFrameMs,ms);multiplayerBridge?.frame();if(!first){first=true;last=performance.now();ticks=0;presents=0;maxFrameMs=0;emit('first-frame');}const now=performance.now();if(now-last>=1000){const p=core._th15_audio_statistics();if(p){const a=Array.from(core.HEAPU32.subarray(p>>>2,(p>>>2)+11));emit('audio-health',{ready:!!a[0],callbacks:a[1],renderedFrames:a[2],errorCode:a[3],queuedFrames:a[4],queuedMs:a[4]*1000/44100,backend:'SDL3',track:a[5],cursor:a[9],musicEnabled:music});}emit('frame-health',{fps:presents*1000/(now-last),simulationFps:ticks*1000/(now-last),maxGapMs:maxFrameMs});last=now;ticks=0;presents=0;maxFrameMs=0;}if(core._th15_phase()===4)void stop().catch(fatal);}});
 core.SDL3=core.SDL3||{};if(parent!==window&&parent.__touhouAudioContext)core.SDL3.audioContext=parent.__touhouAudioContext;
 window.Module=core;window.FS=core.FS;core.resetBrowserKeyboard=()=>keyboard.clear();
 core.FS.mkdirTree(saveRoot);core.FS.mount(core.IDBFS,{},saveRoot);await sync(true);core.FS.mkdirTree(saveRoot+'/replay');core.FS.mkdirTree(saveRoot+'/autosave');core.FS.symlink(saveRoot,'/save');
 if(!validEpoch||query.get('managedData')!=='1'||parent===window||typeof parent.__eaglerPrepareManagedRuntimeDataV1!=='function')throw Error('请从 eagler-touhou 启动此运行时');
 const {buffer}=await parent.__eaglerPrepareManagedRuntimeDataV1({game,generation:query.get('gameGeneration'),epoch});
 if(buffer.byteLength<16||buffer.byteLength>128*1024*1024)throw Error('游戏资源大小错误');
 core.FS.writeFile('/th15.dat',new Uint8Array(buffer),{canOwn:true});
 const response=await fetch('./resources.json');if(!response.ok)throw Error('资源清单加载失败');const manifest=await response.json();
 if(manifest.schema!=='eagler-sdl-resources/1'||manifest.game!==game||!Array.isArray(manifest.resources)||manifest.resources.length!==1||manifest.resources[0]?.path!=='/fonts/cp932.bin')throw Error('资源清单无效');
 for(const r of manifest.resources)await resource(r);
 if(runtimeBuild.multiplayer){const {createMultiplayerBridge}=await import('./multiplayer.mjs');const manifest=await(await fetch('./manifest.json')).json();multiplayerBridge=createMultiplayerBridge({core,getOptions:()=>options,manifest,emit,onError:fatal,onExit:stop});}
 emit('ready',{roomPreview:runtimeBuild.multiplayer===true});
})().catch(e=>{if(e?.name==='AbortError'&&e?.message==='EAGLER_RUNTIME_SESSION_SUPERSEDED')return false;fatal(e);throw e;});
