// Shared eagler-touhou/1 transport. Game clocks, input sampling and audio stay in C++.

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
 return typeof path==='string'&&(new RegExp('^/bgm-ogg/'+game+'_(?:[0-9]{2}|13b)\\.ogg$').test(path)||/^\/fonts\/[a-z0-9_.-]+$/.test(path)||path==='/msgothic.ttc'||path==='/unifont.otf'||(new RegExp('^/thcrap/'+game+'/[a-z0-9_/.-]+$').test(path)&&!path.includes('..')&&!path.includes('//')));
}
export function ensureSharedFontAlias(Module,language){
 const source=language==='chs'?'/unifont.otf':'/msgothic.ttc';
 const target=language==='chs'?'/fonts/simhei.ttf':'/fonts/msgothic.ttc';
 if(!Module?.FS?.mkdirTree||!Module.FS.stat||!Module.FS.symlink)throw Error('Runtime filesystem unavailable');
 Module.FS.mkdirTree('/fonts');
 try{Module.FS.stat(source);}catch{throw Error('Shared Runtime font unavailable: '+source);}
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
  const bytes=new Uint8Array(await response.arrayBuffer());
  const expected=resource.bytes??resource.size;
  if(bytes.length>64*1024*1024||(expected!=null&&bytes.length!==Number(expected)))throw Error('Resource size mismatch: '+resource.path);
  Module.FS.mkdirTree(resource.path.slice(0,resource.path.lastIndexOf('/')));
  Module.FS.writeFile(resource.path,bytes,{canOwn:true});
  emit('transfer',{mode:resource.path.startsWith('/bgm-ogg/')?'ogg':'runtime',loaded:bytes.length,total:bytes.length,path:resource.path});
 }
}
export function observeMusicWrites(Module,core,game) {
 const write=Module.FS.writeFile;
 Module.FS.writeFile=function(path,...args){
  const result=write.call(this,path,...args);
  if(typeof path==='string'&&path.startsWith('/bgm-ogg/')&&resourcePath(path,game))core.sdl_music_resource_changed?.();
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
 if(bytes.byteLength<16||bytes.byteLength>128*1024*1024)throw Error('Invalid game DATA size');
 Module.FS.mkdirTree('/game');Module.FS.writeFile('/game/'+game+'.dat',bytes,{canOwn:true});
 const response=await fetcher(new URL('./resources.json',base));if(!response.ok)throw Error('Runtime resource manifest unavailable');
 const manifest=await response.json();if(manifest.game!==game||manifest.schema!=='eagler-sdl-resources/1')throw Error('Invalid Runtime resource manifest');
 await installResources(Module,manifest.resources,{game,emit,base,fetcher});
}
