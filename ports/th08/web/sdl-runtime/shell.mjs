// Platform shell for the upstream eagler-touhou/1 Launcher contract.
// Game construction, input, timing, rendering, text and sound belong to C++.
import {createBrowserKeyboard} from './directory-keyboard.mjs';
import createModule from './th08-sdl.mjs';
import {createPractice} from './practice.mjs';
import {bindOutsideTouches} from './eagler-host.mjs';
import {exportReplayName,importReplayName} from './motion-replay.mjs';
import {normalizeOptions,applyTouchOptions,touchControls,suspendRuntimeAudio,resumeRuntimeAudio,directTouch,ensureSharedFontAlias,installResources as installHostResources,observeMusicWrites,mountManagedData,isSupersededRuntimeError} from './eagler-host.mjs';
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
const epoch=Number(query.get('runtimeEpoch'));
const validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...fields},location.origin);
let Module,core,app=0,launched=false,first=false,closing=false,language=query.get('language')==='lang_zh-hans'?'chs':'jp',options={},music=true;
let practice;
const keyboard=createBrowserKeyboard({
 hostContext:{protocol,game,epoch,origin:location.origin,ready:()=>launched&&!document.hidden&&!options.netplaySpectator&&!closing,reset:clearKeyboard,},
 send(code,down){if(!core)return;if(options.thpracEnabled&&practice?.key(code,down))return;cstring(code,p=>core.sdl_key(p,+down));},
 onClear(){practice?.clear();}
});
function clearKeyboard(){keyboard.clear();core?.sdl_keys_clear();}
let frames=0,lastHealth=0,lastFrame=0,maxGap=0,lastPresented=0,saveTimer=null;
const cancelTouches=bindOutsideTouches(document,canvas,()=>core,()=>launched&&options.touchEnabled);
const error=reason=>{const message=reason?.stack||String(reason);document.querySelector('#error').textContent=message;emit('error',{message,error:message});console.error(reason);};
const u32=(ptr,count)=>new Uint32Array(core.memory.buffer,ptr,count);
const cstring=(text,fn)=>{const bytes=new TextEncoder().encode(text+'\0'),p=core.allocate(bytes.length);try{new Uint8Array(core.memory.buffer,p,bytes.length).set(bytes);return fn(p);}finally{core.deallocate(p);}};
const root=()=>'/savesth08';
let storageSync=Promise.resolve();
const sync=populate=>{const current=storageSync.then(()=>new Promise((resolve,reject)=>Module.FS.syncfs(populate,e=>e?reject(e):resolve())));storageSync=current.catch(()=>{});return current;};
function relativeSave(path){
 if(typeof path!=='string'||path.length>200)throw Error('Invalid save path');
 path=path.replaceAll('\\','/').toLowerCase();
 if(path.startsWith('/savesth08/'))path=path.slice('/savesth08/'.length);
 else path=path.replace(/^\//,'');
 if(!/^(?:score\.dat|th08\.cfg|replay\/(?:[^./\\\x00-\x1f][^/\\\x00-\x1f]{0,127}\/)?th8_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?)$/.test(path))throw Error('Invalid save path: '+path);
 return path;
}
function fileExists(path){return Module.FS.analyzePath(path).exists;}
async function migrateSaves(){
 if(fileExists('/savesth08/.migration-v3'))return;
 const databases=typeof indexedDB.databases==='function'?await indexedDB.databases():null;
 for(const lang of ['jp']){
  const name='th08-native-1.00d';if(databases&&!databases.some(db=>db.name===name))continue;
  const db=await new Promise((resolve,reject)=>{const request=indexedDB.open(name);request.onerror=()=>reject(request.error);request.onsuccess=()=>resolve(request.result);});
  try{if(!db.objectStoreNames.contains('files'))continue;
   const entries=await new Promise((resolve,reject)=>{const tx=db.transaction('files','readonly'),rows=[];tx.objectStore('files').openCursor().onsuccess=e=>{const c=e.target.result;if(c){rows.push([c.key,c.value]);c.continue();}};tx.oncomplete=()=>resolve(rows);tx.onerror=()=>reject(tx.error);});
   for(const [name,value] of entries){let path;try{path=relativeSave(name);}catch{continue;}const bytes=value instanceof Blob?new Uint8Array(await value.arrayBuffer()):new Uint8Array(value);path=importReplayName(path,bytes,8);const target='/savesth08/'+path;if(!fileExists(target)){Module.FS.mkdirTree(target.slice(0,target.lastIndexOf('/')));Module.FS.writeFile(target,bytes);}}
  }finally{db.close();}
 }
 Module.FS.writeFile('/savesth08/.migration-v3',new Uint8Array([1]));await sync(false);
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
function applyOptions(){applyTouchOptions(core,options);core.sdl_touch_display?.(options.alwaysHitbox?1:0);practice?.configure(options);}
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
async function stop(){if(closing)return;closing=true;localReplay.close();clearKeyboard();try{practice?.close();core.sdl_loop_stop();await save();core.sdl_game_close();window.dispatchEvent(new CustomEvent('touhou-midi-close'));await sync(false);app=0;launched=false;emit('exit',{code:0,status:'success'});}finally{closing=false;}}
const localReplay=createNativeLocalReplay({core:()=>core,module:()=>Module,physical:value=>root()+'/'+relativeSave(value).replace(/\.rpyx$/i,'.rpy'),stopLoop:()=>{clearKeyboard();core.sdl_loop_stop();},onState:playback=>emit('runtime-info',{playback}),onError:error});
async function launch(){
 if(launched)return;clearKeyboard();
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
 const mode=Module.touhouMusicMode||'none';music=mode!=='none';core.sdl_ogg_decode_mode?.(options.oggDecodeMode==='full');core.sdl_music_source?.(mode==='midi'?2:1);
 core.sdl_music_enabled?.(music);app=core.sdl_game_open(Date.now()>>>0);if(!app)throw Error('C++ game initialization failed');
 if(!core.sdl_prepare_loading()||!core.sdl_draw_loading(0))throw Error('开头加载画面初始化失败');
 // Two RAF boundaries let the original frame reach the compositor before
 // resource work resumes. The startup picture shares its two-second deadline with resource work.
 const startupImageAt=performance.now();performance.mark('eagler-startup-image');await new Promise(requestAnimationFrame);await new Promise(requestAnimationFrame);let loadingPaint=performance.now();
 const total=core.sdl_prepare_total();for(let i=0;i<total;i++){if(core.sdl_prepare_next()<0)throw Error('资源预载失败 '+i);if(i%12===11){const now=performance.now();if(now-loadingPaint>=1000/60){if(!core.sdl_draw_loading(1))throw Error('开头加载画面绘制失败');loadingPaint=now;await new Promise(requestAnimationFrame);}else await new Promise(resolve=>setTimeout(resolve,0));}}
 if(!core.sdl_game_initialize())throw Error('永夜抄初始化失败');await new Promise(resolve=>setTimeout(resolve,Math.max(0,2000-(performance.now()-startupImageAt))));performance.mark('eagler-startup-menu-ready');document.querySelector('#loading').textContent='';
 applyOptions();launched=true;first=false;lastPresented=0;lastHealth=performance.now();lastFrame=0;frames=0;maxGap=0;
 canvas.focus({preventScroll:true});core.sdl_loop_pause(1);if(!document.hidden)await resumeForegroundAudio();if(query.get('manual')!=='1')core.sdl_loop_start();
 emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++',architecture:'eagler-touhou/1',version:'3.4.1-sdl3'});
}
async function command(message){
 switch(message.command){
 case 'configure':if(launched)throw Error('Cannot configure a running game');language=message.language==='lang_zh-hans'?'chs':'jp';options=normalizeOptions(message.options);if(!['ogg','midi','none'].includes(message.music))throw Error('Invalid music mode');Module.touhouMusicMode=message.music;Module.eaglerOptions=options;music=message.music!=='none';await installResources(message.sharedResources);await installResources(message.runtimeResources);await installResources(message.resources);if(message.runtimePack)await installRuntimePack(message.runtimePack);applyOptions();return {};
 case 'resources':await installResources(message.resources);return {};
 case 'keyboard':if(launched&&!document.hidden&&!closing)keyboard.event(message,!!message.down,'hosted');return {};
 case 'thprac-mouse':practice?.mouse(message);return {};
 case 'keyboard-clear':clearKeyboard();return {};
 case 'touch-cancel':cancelTouches();return {};
 case 'direct-touch':directTouch(core,canvas,message,{width:innerWidth,height:innerHeight});return {};
 case 'touch-controls':touchControls(core,options,message,Module);return {};
 case 'playback':if(!launched||options.replayViewer!==true)throw Error('请先打开录像管理器');return localReplay.command(message);
 case 'launch':await launch();return {};
 case 'sync':await save();return {};
 case 'list':return {files:listLocalReplayFiles(Module.FS,root(),relativeSave,path=>({path:exportReplayName(path,Module.FS.readFile(root()+'/'+path),8)}))};
 case 'read':{let path=relativeSave(message.path);if(path.endsWith('.rpyx'))path=path.slice(0,-1);return {bytes:Array.from(Module.FS.readFile(root()+'/'+path))};}
 case 'write':{if(!Array.isArray(message.bytes)||message.bytes.length>16*1024*1024||message.bytes.some(b=>!Number.isInteger(b)||b<0||b>255))throw Error('Invalid save bytes');const bytes=new Uint8Array(message.bytes),path=importReplayName(relativeSave(message.path),bytes,8);const target=root()+'/'+path;Module.FS.mkdirTree(target.slice(0,target.lastIndexOf('/')));Module.FS.writeFile(target,bytes);await sync(false);return {};}
 case 'remove':{let path=relativeSave(message.path);if(path.endsWith('.rpyx'))path=path.slice(0,-1);Module.FS.unlink(root()+'/'+path);await sync(false);return {};}
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
window.addEventListener('blur',()=>{if(core){clearKeyboard();cancelTouches();}});
window.addEventListener('pagehide',()=>{clearKeyboard();cancelTouches();if(core&&launched){suspendRuntimeAudio(Module,core);void persist().catch(console.error);}});
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
 let audioContext;try{audioContext=parent.__touhouAudioContext||parent.__th10AudioContext;}catch{}
 Module=await createModule({canvas,noInitialRun:true,resetBrowserKeyboard:()=>keyboard.clear(),...(audioContext?{SDL3:{audioContext}}:{}),print:console.log,printErr:console.error,
  instantiateWasm(imports,ready){return WebAssembly.instantiateStreaming(fetch('./th08-sdl.wasm'),imports).then(({instance,module})=>{core=instance.exports;ready(instance,module);return core;});}
 });
 window.Module=Module;window.FS=Module.FS;observeMusicWrites(Module,core,game);Module.FS.mkdirTree('/savesth08');Module.FS.mount(Module.IDBFS,{},'/savesth08');await sync(true);
 practice=createPractice({core,getApp:()=>app,canvas,clearKeys:clearKeyboard,setMusic:value=>core.sdl_music_enabled(value),setPaused:value=>core.sdl_loop_pause(value||document.hidden?1:0)});
 Module.FS.mkdirTree('/savesth08/replay');await migrateSaves();await mountData();cstring('#screen',core.sdl_canvas);
 Module.runtimePrepare=()=>!document.hidden;
 Module.runtimeFinish=(result,duration)=>{
  practice.tick();
  const now=performance.now(),p=u32(core.sdl_stats(),6)[5];if(p!==lastPresented){frames++;if(lastFrame)maxGap=Math.max(maxGap,now-lastFrame);lastFrame=now;lastPresented=p;if(!first){first=true;emit('first-frame');}}
  if(result||status()[2]){if(status()[2]){error('Game error '+status()[2]);core.sdl_loop_pause(1);}else queueMicrotask(()=>void stop().catch(error));}
  if(now-lastHealth>=1000){emit('frame-health',{fps:frames*1000/(now-lastHealth),maxGapMs:maxGap,frameMs:duration});const a=u32(core.sdl_audio_stats(),12);emit('audio-health',{queuedMs:a[5]*1000/44100,minQueuedMs:a[7]*1000/44100,backend:'script',underruns:0,robust:true});frames=0;maxGap=0;lastHealth=now;}
 };
 Module.runtimeFileChanged=()=>{if(saveTimer!==null)return;saveTimer=setTimeout(()=>{saveTimer=null;queue=queue.then(()=>sync(false)).catch(error);},0);};
 Module.runtimeStopped=()=>{};Module.runtimeNotice=message=>emit('notice',{message});Module.runtimeMidi=(op,data)=>{if(op===62&&music&&Module.touhouMusicMode==='midi')window.dispatchEvent(new CustomEvent('touhou-midi',{detail:{bytes:Array.from(data||[])}}));if(op===61)window.dispatchEvent(new CustomEvent('touhou-midi-close'));};Module.callMain=launch;
 window.__th08Runtime={core,Module,get app(){return app;},status,launch,stop,command};
 emit('ready');
})().catch(e=>{if(isSupersededRuntimeError(e)){console.debug('Runtime navigation superseded');return false;}error(e);throw e;});
