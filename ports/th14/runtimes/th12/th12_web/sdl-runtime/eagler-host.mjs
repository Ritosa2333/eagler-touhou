// Shared eagler-touhou/1 transport for the TH11 portable runtime. Game clocks,
// input sampling and audio stay in C++; this module only relays Launcher
// commands and owns the managed DATA / resource install contract.
// Mirrors th10/th20 th*_web/sdl-runtime/eagler-host.mjs with TH11 paths.

 // Canvas-origin gestures remain owned by SDL. Only letterbox-origin gestures
 // enter this bridge; capture keeps crossing into canvas from double delivery.
export function bindOutsideTouches(doc,canvas,getCore,isEnabled) {
 doc.documentElement.style.touchAction='none';
 const active=new Map();let nextId=-1073741824;
 const coordinates=event=>{
  const rect=canvas.getBoundingClientRect();
  if(rect.width<=0||rect.height<=0||!Number.isFinite(event.clientX)||!Number.isFinite(event.clientY))return null;
  return [(event.clientX-rect.left)/rect.width,(event.clientY-rect.top)/rect.height];
 };
 const consume=event=>{event.preventDefault();event.stopPropagation();};
 const down=event=>{
  if(event.pointerType!=='touch'||event.target===canvas||!isEnabled()||!getCore())return;
  const point=coordinates(event);if(!point)return;
  const touch={id:nextId--,point};active.set(event.pointerId,touch);
  getCore().sdl_touch(0,touch.id,...point);
  try{event.target.setPointerCapture?.(event.pointerId);}catch{}
  consume(event);
 };
 const move=event=>{
  const touch=active.get(event.pointerId);if(!touch)return;
  const point=coordinates(event);if(point){touch.point=point;getCore()?.sdl_touch(1,touch.id,...point);}
  consume(event);
 };
 const up=event=>{
  const touch=active.get(event.pointerId);if(!touch)return;
  active.delete(event.pointerId);
  // Cancellation must not turn an interrupted tap into a menu/bomb gesture.
  if(event.type==='pointercancel'||event.type==='lostpointercapture')cancel();
  else getCore()?.sdl_touch(2,touch.id,...(coordinates(event)||touch.point));
  consume(event);
 };
 function cancel(){active.clear();getCore()?.sdl_touch_cancel();}
 for(const [name,handler] of [['pointerdown',down],['pointermove',move],['pointerup',up],['pointercancel',up],['lostpointercapture',up]])
  doc.addEventListener(name,handler,{capture:true,passive:false});
 return cancel;
}
export function normalizeOptions(value={}) {
 const modes=['touch','touch-unlimited','joystick','joystick-free'];
 const sensitivity=Number(value.touchSensitivity??100);
 return {...value,touchEnabled:!!value.touchEnabled,
  touchMovementMode:modes.includes(value.touchMovementMode)?value.touchMovementMode:'touch',
  touchSensitivity:Number.isFinite(sensitivity)?Math.max(100,Math.min(300,sensitivity)):100};
}
export function applyTouchOptions(core,options) {
 core.sdl_touch_options(!!options.touchEnabled,options.touchMovementMode==='touch-unlimited',options.touchSensitivity/100);
 core.sdl_touch_gestures?.(options.touchFocusMode==='two-finger',!!options.doubleTapBombEnabled);
 core.sdl_touch_mode?.(['touch','touch-unlimited','joystick','joystick-free'].indexOf(options.touchMovementMode));
}
export function touchControls(core,options,message) {
 const c=message.controls??message,sensitivity=Number(c.touchSensitivity);
 // Frequent snapshots must not reconfigure the gesture/mode owner.
 if(Number.isFinite(sensitivity)&&sensitivity>=100&&sensitivity<=300&&sensitivity!==options.touchSensitivity){
  options.touchSensitivity=sensitivity;
  core.sdl_touch_options(!!options.touchEnabled,options.touchMovementMode==='touch-unlimited',sensitivity/100);
 }
 // Launcher axes are signed 16-bit magnitudes; normalization belongs to C++.
 const axis=v=>Number.isFinite(Number(v))?Math.max(-32767,Math.min(32767,Number(v))):0;
 core.sdl_touch_controls(!!c.fireEnabled,!!c.focusEnabled,c.bombSerial>>>0,c.escapeSerial>>>0,axis(c.joystickX),axis(c.joystickY));
}
const detachedRuntimeAudioNodes=new WeakSet();
export function suspendRuntimeAudio(Module,core) {
 if(!core)return false;
 const node=Module?.SDL3?.audio_playback?.scriptProcessorNode;
 if(node&&!detachedRuntimeAudioNodes.has(node)){
  try{node.disconnect();detachedRuntimeAudioNodes.add(node);}catch{}
 }
 core.sdl_loop_pause?.(1);
 return true;
}
export async function resumeRuntimeAudio(Module,core,isForeground=()=>true) {
 if(!core||!isForeground())return false;
 const context=Module?.SDL3?.audioContext;
 if(context&&context.state!=='running'){
  try{await context.resume();}catch{}
 }
 if(!isForeground()||(context&&context.state!=='running'))return false;
 const node=Module?.SDL3?.audio_playback?.scriptProcessorNode;
 if(node&&detachedRuntimeAudioNodes.has(node)){
  if(!context)return false;
  try{node.connect(context.destination);detachedRuntimeAudioNodes.delete(node);}catch{return false;}
 }
 core.sdl_loop_pause?.(0);
 return true;
}
export function directTouch(core,canvas,message,viewport) {
 const type={down:0,move:1,up:2,cancel:2}[message.type??message.action];
 const x=Number(message.x),y=Number(message.y),id=Number(message.id);
 if(type===undefined||!Number.isFinite(x)||!Number.isFinite(y)||!Number.isSafeInteger(id))return;
 const rect=canvas.getBoundingClientRect();
 if(rect.width<=0||rect.height<=0)return;
 core.sdl_touch(type,id,(x*viewport.width-rect.left)/rect.width,(y*viewport.height-rect.top)/rect.height);
}
export function resourcePath(path,game) {
 if(game==='th14'&&path==='/msgothic.ttc')return true;
 if(game==='th14'&&typeof path==='string'&&/^\/music\/(?:th14_[0-9]{2}|th128_08)\.(?:ogg|flac)$/.test(path))return true;
 return typeof path==='string'&&((new RegExp('^/music/'+game+'_(?:[0-9]{2})\\.ogg$').test(path)||/^\/music\/th10_17\.ogg$/.test(path))||/^\/fonts\/[a-z0-9_.-]+$/.test(path)||path==='/unifont.otf'||(new RegExp('^/thcrap/'+game+'/[a-z0-9_/.-]+$').test(path)&&!path.includes('..')&&!path.includes('//')));
}
export function ensureSharedFontAlias(Module,language){
 const source='/unifont.otf';
 const target='/fonts/unifont.otf';
 if(!Module?.FS?.mkdirTree||!Module.FS.stat||!Module.FS.symlink)throw Error('Runtime filesystem unavailable');
 try{Module.FS.stat(source);}catch{return null;}
 Module.FS.mkdirTree('/fonts');
 try{Module.FS.unlink?.(target);}catch{}
 Module.FS.symlink(source,target);
 return {source,target};
}
export async function installResources(Module,resources=[],{game,emit=()=>{},base=globalThis.location?.href,fetcher=globalThis.fetch}={}) {
 if(!Array.isArray(resources)||resources.length>128)throw Error('Invalid resource list');
 for(const resource of resources){
  if(!resourcePath(resource.path,game))throw Error('Invalid runtime resource path');
  const url=new URL(resource.url,base);if(url.origin!==new URL(base).origin||!['http:','https:','blob:'].includes(url.protocol))throw Error('Resource must be same-origin');
  const response=await fetcher(url);if(!response.ok)throw Error('Resource download failed: '+resource.path);
  const expected=resource.bytes??resource.size;
  const compressed=resource.encoding==='gzip';
  if(compressed&&(game!=='th14'||!/^\/fonts\/font[0-7]\.bin$/.test(resource.path)||
      !url.pathname.endsWith(resource.path+'.gz')||!Number.isSafeInteger(expected)||expected<1||expected>64*1024*1024||
      !/^[a-f0-9]{64}$/.test(resource.sha256)))throw Error('Invalid TH14 compressed font descriptor');
  const bytes=new Uint8Array(await (compressed
    ? new Response(response.body.pipeThrough(new DecompressionStream('gzip'))).arrayBuffer()
    : response.arrayBuffer()));
  if(bytes.length>64*1024*1024||(expected!=null&&bytes.length!==Number(expected)))throw Error('Resource size mismatch: '+resource.path);
  if(compressed){
   const hash=new Uint8Array(await crypto.subtle.digest('SHA-256',bytes));
   if(Array.from(hash,n=>n.toString(16).padStart(2,'0')).join('')!==resource.sha256)throw Error('Resource hash mismatch: '+resource.path);
  }
  Module.FS.mkdirTree(resource.path.slice(0,resource.path.lastIndexOf('/')));
  Module.FS.writeFile(resource.path,bytes,{canOwn:true});
  // Native FLAC assets are part of Runtime preparation. Only the optional
  // OGG delivery flow has a separate music-complete event from the Host.
  const ogg=/^\/(?:music|bgm-ogg)\//.test(resource.path)&&!/\.flac$/i.test(resource.path);
  emit('transfer',{mode:ogg?'ogg':'runtime',kind:ogg?'music':'game',...(/\.flac$/i.test(resource.path)?{title:'正在准备原版音乐（无损）…',label:resource.path.split('/').pop()}:{}),loaded:bytes.length,total:bytes.length,path:resource.path});
 }
}
export function observeMusicWrites(Module,core,game) {
 const write=Module.FS.writeFile;
 Module.FS.writeFile=function(path,...args){
  const result=write.call(this,path,...args);
  if(typeof path==='string'&&/^\/music\//.test(path)&&resourcePath(path,game))core.sdl_music_resource_changed?.();
  return result;
 };
}
export function isSupersededRuntimeError(error){
 return error?.name==='AbortError'&&error?.message==='EAGLER_RUNTIME_SESSION_SUPERSEDED';
}
export async function mountManagedData(Module,{game,parentWindow,query,fetcher=globalThis.fetch,base=globalThis.location?.href,emit}){
 if(query.get('managedData')!=='1'||typeof parentWindow?.__eaglerPrepareManagedRuntimeDataV1!=='function')throw Error('请从 eagler-touhou 启动此运行时');
 const epoch=Number(query.get('runtimeEpoch'));if(!Number.isSafeInteger(epoch)||epoch<=0)throw Error('Runtime navigation epoch unavailable');
 const result=await parentWindow.__eaglerPrepareManagedRuntimeDataV1({game,generation:query.get('gameGeneration'),epoch});
 // Cross-frame ArrayBuffers need not pass this realm's instanceof check.
 const bytes=new Uint8Array(result.buffer);
 if(bytes.byteLength<16||bytes.byteLength>512*1024*1024)throw Error('Invalid game DATA size');
 // The TH11 C++ archive layer opens /th11.dat directly.
 Module.FS.writeFile('/'+game+'.dat',bytes,{canOwn:true});
 const response=await fetcher(new URL('./resources.json',base));if(!response.ok)throw Error('Runtime resource manifest unavailable');
 const manifest=await response.json();if(manifest.game!==game||manifest.schema!=='eagler-sdl-resources/1')throw Error('Invalid Runtime resource manifest');
 await installResources(Module,manifest.resources,{game,emit,base,fetcher});
}

// Host ownership and transport only. C++ validates files, creates the real
// Application replay, consumes recorded inputs and advances every game tick.
export function createReplayViewerHostTh14({read,describe,open,status,step,rate,pause,close,emit,
 schedule=(f,ms)=>setTimeout(f,ms),cancel=id=>clearTimeout(id),visible=()=>true,onError=()=>{}}) {
 let active=null,revision=0,pollTimer=null,seekTimer=null;
 const rates=[0,.5,1,2,4,8];
 const publish=()=>{if(active)emit({...active.state});};
 const stopTimer=name=>{if(name==='poll'&&pollTimer!==null){cancel(pollTimer);pollTimer=null;}
  if(name==='seek'&&seekTimer!==null){cancel(seekTimer);seekTimer=null;}};
 const stopSeek=()=>{++revision;stopTimer('seek');if(active){active.state.seeking=false;active.state.target=null;}};
 const readValidated=async path=>{
  const file=await read(path);if(!(file.bytes instanceof Uint8Array)||!file.bytes.length||file.bytes.length>64*1024*1024)throw Error('录像大小无效');
  const bytes=new Uint8Array(file.bytes),description=await describe(bytes);
  if(!description?.stages?.length)throw Error('录像信息无效');return {path:file.path,bytes,description};
 };
 const timeline=description=>{
  let total=0;return description.stages.map(stage=>{const item={...stage,start:total};total+=stage.frames;return item;});
 };
 async function update(owner=active){
  if(!owner||owner!==active)return;
  const native=await status();if(owner!==active)return;
  const entry=owner.timeline.find(s=>s.id===native.stage);
  if(!entry||!native.active)return;
  owner.state.stage=native.stage;owner.state.frame=entry.start+Math.min(native.frame,entry.frames);owner.state.ended=!!native.ended;
  if(native.ended){owner.state.frame=entry.start+entry.frames;owner.state.paused=true;await pause(true);}
  if(owner===active)publish();
 }
 function poll(){
  pollTimer=schedule(async()=>{pollTimer=null;const owner=active;
   try{if(owner&&!owner.state.seeking)await update(owner);}catch(e){onError(e);}
   if(active)poll();
  },250);
 }
 async function begin(file,stage,paused=false,playRate=1){
  const entries=timeline(file.description),selected=entries.find(s=>s.id===stage);
  if(!selected)throw Error('录像中没有该关卡');
  stopSeek();stopTimer('poll');const ticket=revision;
  await pause(true);await open(file.bytes,stage);await rate(playRate);
  if(ticket!==revision)return null;
  const owner={...file,timeline:entries,selectedStage:stage,state:{kind:'replay',path:file.path,description:file.description,
   left:file.description.character,right:file.description.shot??0,difficulty:file.description.difficulty,stage,
   frame:selected.start,startFrame:selected.start,total:entries.reduce((n,s)=>n+s.frames,0),
   paused,rate:playRate,seeking:false,target:null,ended:false}};
  active=owner;await pause(paused||!visible());publish();poll();return owner;
 }
 async function seek(value){
  const owner=active;if(!owner)throw Error('没有正在播放的录像');
  if(!Number.isSafeInteger(value)||value<owner.state.startFrame||value>owner.state.total)throw Error('录像位置无效');
  const entry=[...owner.timeline].reverse().find(s=>s.start<=value);
  stopSeek();const ticket=revision;await pause(true);await open(owner.bytes,entry.id);await rate(owner.state.rate);
  if(owner!==active||ticket!==revision)return;
  owner.state.seeking=true;owner.state.target=value;owner.state.ended=false;publish();
  const pump=async()=>{
   seekTimer=null;if(owner!==active||ticket!==revision)return;
   if(!visible()){seekTimer=schedule(pump,250);return;}
   try{
    const result=await step(value-entry.start,16);
    if(owner!==active||ticket!==revision)return;
    if(result<0)throw Error('录像跳转失败');
    await update(owner);if(owner!==active||ticket!==revision)return;
    if(result){owner.state.seeking=false;owner.state.target=null;await pause(owner.state.paused||owner.state.ended||!visible());publish();}
    else seekTimer=schedule(pump,0);
   }catch(e){if(owner===active&&ticket===revision){owner.state.seeking=false;owner.state.target=null;owner.state.paused=true;publish();onError(e);}}
  };
  seekTimer=schedule(pump,0);
 }
 return {
  get state(){return active?{...active.state}:null;},
  async foreground(){if(active)await pause(active.state.paused||active.state.seeking||active.state.ended||!visible());},
  async close(){stopSeek();stopTimer('poll');active=null;await close();},
  async command(message){
   const action=message.action;
   if(action==='inspect'){const file=await readValidated(message.path);return {description:file.description};}
   if(action==='open'){
    const file=await readValidated(message.path),stage=message.stage??file.description.stages[0].id;
    if(!Number.isSafeInteger(stage))throw Error('录像关卡无效');await begin(file,stage);return {playback:this.state};
   }
   const owner=active;if(!owner)throw Error('没有正在播放的录像');
   if(action==='pause'){stopSeek();owner.state.paused=true;await pause(true);await update(owner);}
   else if(action==='play'){
    if(owner.state.ended)await begin(owner,owner.selectedStage,false,owner.state.rate);
    else{owner.state.paused=false;await pause(owner.state.seeking||!visible());publish();}
   }else if(action==='rate'){
    if(!rates.includes(message.value))throw Error('录像倍率无效');await rate(message.value);owner.state.rate=message.value;publish();
   }else if(action==='seek')await seek(message.value);
   else if(action==='relocate'){
    if(message.path!==owner.path)throw Error('录像路径已变化');const next=await readValidated(message.target);
    if(next.bytes.length!==owner.bytes.length||!next.bytes.every((b,i)=>b===owner.bytes[i]))throw Error('目标不是同一份录像');
    owner.path=next.path;owner.state.path=next.path;publish();
   }else throw Error('不支持的录像操作');
   return {playback:this.state};
  }
 };
}
