import {createBrowserKeyboard} from './directory-keyboard.mjs';
import createModule from './th17.mjs';
import {scanCodes} from './keyboard.mjs';
import {gpuTiming} from './gpu-timing.mjs';
const runtimeBuild=/*TH17_BUILD_INFO*/{version:'development-incomplete',completeGame:false};
const game='th17',protocol='eagler-touhou/1',query=new URLSearchParams(location.search),canvas=document.querySelector('canvas'),$=s=>document.querySelector(s);
const epoch=Number(query.get('runtimeEpoch')),validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
let configured=false;
let core,gpuMonitor,launched=false,first=false,stopping=false,options={},music=true,chain=Promise.resolve(),queue=Promise.resolve();
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...fields},location.origin);
const keyboard=createBrowserKeyboard({hostContext:{protocol,game,epoch,origin:location.origin,ready:()=>launched&&!document.hidden&&!stopping,reset:clearKeyboard},accept:code=>!!scanCodes[code],send:(code,down)=>core?._th17_key(scanCodes[code],+down)});
function clearKeyboard(){keyboard.clear();core?._th17_keys_clear();}
const err=()=>{const p=core._th17_error(),end=core.HEAPU8.indexOf(0,p);return new TextDecoder().decode(core.HEAPU8.subarray(p,end<0?p+256:end));};
const fatal=e=>{const message=e?.message||String(e);$('#error').textContent=message;core?._th17_loop_pause(1);emit('error',{error:message});};
const sync=(populate=false)=>{const current=chain.then(()=>new Promise((r,j)=>core.FS.syncfs(populate,e=>e?j(e):r())));chain=current.catch(()=>{});return current;};
const save=()=>{if(launched&&!core._th17_save_scores())throw Error(err());return sync();};
function closeAudio(){
 // The launcher owns the shared context. SDL owns and must disconnect its
 // stream, but closing that stream must not close the launcher's context.
 const s=core.SDL3,context=s?.audioContext,borrowed=parent!==window&&context===parent.__touhouAudioContext;
 if(borrowed)s.audioContext=undefined;
 try{core._th17_audio_close();}finally{if(borrowed)s.audioContext=context;}
}
async function stop(){if(stopping)return;stopping=true;try{core._th17_loop_stop();gpuMonitor?.close();gpuMonitor=null;await save();closeAudio();launched=false;emit('exit',{code:0,status:'success'});}finally{stopping=false;}}
function path(value){const name=String(value).replaceAll('\\','/').toLowerCase().replace(/^\/savesth17\//,'').replace(/^\//,'');if(!/^(?:scoreth17\.dat|th17\.cfg|replay\/th17_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?)$/.test(name))throw Error('存档路径无效');return name;}
function apply(){core._th17_always_hitbox(+(options.alwaysHitbox===true));core._th17_touch_options(+!!options.touchEnabled,Math.max(0,['touch','touch-unlimited','joystick','joystick-free'].indexOf(options.touchMovementMode)),Math.max(100,Math.min(300,Number(options.touchSensitivity)||100))/100,+(options.touchFocusMode==='two-finger'),+!!options.doubleTapBombEnabled);core._th17_music_enabled(+music);}
const fontNames=[...Array.from({length:8},(_,i)=>'font'+i+'.bin'),'cp932.bin','blend4444.bin'];
async function resource(r){
 if(!r||(!/^\/music\/(?:th17_\d{2}|th128_08)\.ogg$/.test(r.path)&&!fontNames.some(n=>r.path==='/fonts/'+n)))throw Error('资源路径无效');
 const u=new URL(r.url,location.href);if(u.origin!==location.origin||!['http:','https:','blob:'].includes(u.protocol))throw Error('资源来源无效');
 const response=await fetch(u);if(!response.ok)throw Error('资源读取失败');const bytes=new Uint8Array(await response.arrayBuffer());
 const expected=r.bytes??r.size;if(bytes.length>64*1024*1024||(expected!=null&&bytes.length!==expected))throw Error('资源大小错误');
 core.FS.mkdirTree(r.path.slice(0,r.path.lastIndexOf('/')));core.FS.writeFile(r.path,bytes,{canOwn:true});
 emit('transfer',{mode:r.path.startsWith('/music/')?'ogg':'runtime',loaded:bytes.length,total:bytes.length,path:r.path});
}
function openReplay(value){const name=path(value).replace(/^replay\//,'');if(!/\.rpyx?$/.test(name))throw Error('录像路径无效');const bytes=new TextEncoder().encode(name+String.fromCharCode(0)),ptr=core._malloc(bytes.length);if(!ptr)throw Error('录像内存不足');try{core.HEAPU8.set(bytes,ptr);if(!core._th17_open_replay(ptr,0))throw Error('录像启动失败');}finally{core._free(ptr);}}
async function command(m){switch(m.command){
case 'configure':if(launched)throw Error('游戏已启动');if(!['ogg','none'].includes(m.music))throw Error('音乐模式无效');if(m.language&&m.language!=='ja')throw Error('此构建使用日文原版');options=m.options||{};if(options.touchSensitivity!=null&&!(Number.isFinite(options.touchSensitivity)&&options.touchSensitivity>=100&&options.touchSensitivity<=300))throw Error('触控灵敏度无效');music=m.music!=='none';for(const r of [...(m.runtimeResources||[]),...(m.resources||[])])await resource(r);apply();configured=true;return {};
case 'resources':for(const r of m.resources||[])await resource(r);return {};
case 'keyboard':if(typeof m.code!=='string'||typeof m.down!=='boolean')throw Error('键盘参数无效');if(launched)keyboard.event(m,m.down,'hosted');return {};
case 'keyboard-clear':clearKeyboard();return {};
case 'touch-cancel':core._th17_touch_cancel();return {};
case 'direct-touch':{if(!['down','move','up'].includes(m.type)||!Number.isSafeInteger(m.id)||!Number.isFinite(m.x)||!Number.isFinite(m.y))throw Error('触点参数无效');const b=canvas.getBoundingClientRect();if(b.width&&b.height)core._th17_touch(({down:0,move:1,up:2})[m.type],m.id,(m.x*innerWidth-b.left)/b.width,(m.y*innerHeight-b.top)/b.height);return {};}
case 'touch-controls':{const t=m.controls||m;if(t.touchSensitivity!=null){if(!(Number.isFinite(t.touchSensitivity)&&t.touchSensitivity>=100&&t.touchSensitivity<=300))throw Error('触控灵敏度无效');if(t.touchSensitivity!==options.touchSensitivity){options.touchSensitivity=t.touchSensitivity;apply();}}core._th17_touch_controls(+!!options.touchEnabled,+!!t.fireEnabled,+!!t.focusEnabled,t.bombSerial>>>0,t.escapeSerial>>>0);core._th17_touch_stick(Number(t.joystickX)||0,Number(t.joystickY)||0);return {};}
case 'launch':if(!configured)throw Error('请先配置游戏');if(!launched){let scaleOption=query.get('renderScale');if(!scaleOption)try{scaleOption=new URLSearchParams(parent.location.search).get('renderScale');}catch{}const requested=Number(scaleOption);core._th17_render_scale(options.renderScale===1?1:options.renderScale===2?2:requested===1?1:2);if(!core._th17_initialize())throw Error(err());if(options.replayPath)openReplay(options.replayPath);launched=true;apply();first=false;$('#loading').textContent='';core._th17_loop_start();emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++ · WEB '+runtimeBuild.version.split('-sdl3-')[0],architecture:protocol,...runtimeBuild});}return {};
case 'sync':await save();return {};
case 'list':{const files=[];for(const dir of ['','/replay'])for(const name of core.FS.readdir('/savesth17'+dir)){const n=(dir+'/'+name).replace(/^\//,'');try{path(n);}catch{continue;}const s=core.FS.stat('/savesth17/'+n);if(core.FS.isFile(s.mode))files.push({path:n,size:s.size});}return {files};}
case 'read':return {bytes:Array.from(core.FS.readFile('/savesth17/'+path(m.path)))};
case 'write':{
 if(launched)throw Error('请先退出游戏');const target=path(m.path),bytes=new Uint8Array(m.bytes||[]);
 if(!bytes.length||bytes.length>64*1024*1024)throw Error('文件大小无效');
 const p=core._malloc(bytes.length);if(!p)throw Error('文件导入内存不足');let valid=false;try{core.HEAPU8.set(bytes,p);valid=!!core._th17_validate_file(target==='scoreth17.dat'?0:target==='th17.cfg'?2:1,p,bytes.length);}finally{core._free(p);}
 if(!valid)throw Error('文件不是有效的鬼形兽存档或录像');core.FS.writeFile('/savesth17/'+target,bytes);await sync();return {};
}
case 'remove':if(launched)throw Error('请先退出游戏');core.FS.unlink('/savesth17/'+path(m.path));await sync();return {};
default:throw Error('不支持的操作');}}
window.addEventListener('message',e=>{const m=e.data;if(e.source!==parent||e.origin!==location.origin||m?.protocol!==protocol||m.game!==game||!validEpoch||m.epoch!==epoch||typeof m.command!=='string')return;queue=queue.then(async()=>{if(await initialized===false)return;try{const result=await command(m);if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:true,...result},location.origin);}catch(e){if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:false,error:String(e),errno:e?.errno},location.origin);else fatal(e);}}).catch(fatal);});
document.addEventListener('visibilitychange',()=>{if(launched){clearKeyboard();core._th17_touch_cancel();core._th17_loop_pause(+document.hidden);if(document.hidden)try{void save().catch(fatal);}catch(e){fatal(e);}}});
window.addEventListener('blur',()=>{clearKeyboard();core?._th17_touch_cancel();});
// Keyboard events in the child realm do not bubble to the launcher. SDL's
// canvas listener alone misses keys when the child BODY owns focus.
for(const event of ['keydown','keyup'])window.addEventListener(event,e=>{if(!launched||e.altKey||e.metaKey)return;if(keyboard.mapNative(e,event==='keydown')){e.preventDefault();e.stopImmediatePropagation();return;}if(keyboard.event(e,event==='keydown')){e.preventDefault();e.stopPropagation();}},{capture:true});
for(const event of ['pointerdown','keydown'])window.addEventListener(event,()=>core?.SDL3?.audioContext?.resume().catch(()=>{}),{capture:true});
for(const [name,type] of [['pointerdown',0],['pointermove',1],['pointerup',2],['pointercancel',2]])document.body.addEventListener(name,e=>{
 if(!launched||!options.touchEnabled||e.pointerType==='mouse')return;e.preventDefault();if(e.type==='pointercancel'){core._th17_touch_cancel();return;}if(type===0)document.body.setPointerCapture(e.pointerId);const b=canvas.getBoundingClientRect();if(b.width&&b.height)core._th17_touch(type,e.pointerId,(e.clientX-b.left)/b.width,(e.clientY-b.top)/b.height);
});
canvas.addEventListener('webglcontextlost',e=>{e.preventDefault();fatal(Error('图形环境失效，请退出后重新开始。'));});
window.addEventListener('pagehide',()=>{if(launched){core._th17_loop_stop();try{void save().catch(fatal);}catch(e){fatal(e);}closeAudio();}});
// A mobile browser may terminate a hidden page before pagehide's IDB callback.
// Persist during play as well, with writes serialized by the sync chain.
setInterval(()=>{if(launched&&!document.hidden)try{void save().catch(fatal);}catch(e){fatal(e);}},30000);
const initialized=(async()=>{
 let last=performance.now(),ticks=0,presents=0,maxFrameMs=0;
 core=await createModule({canvas,printErr:console.error,onGameFrameStart(){gpuMonitor??=gpuTiming(canvas.getContext('webgl2'));gpuMonitor.begin();},onGameFrame(ok,ms,count){gpuMonitor?.end();if(!ok){fatal(Error(err()));return;}ticks+=count;presents++;maxFrameMs=Math.max(maxFrameMs,ms);if(!first){first=true;last=performance.now();ticks=0;presents=0;maxFrameMs=0;emit('first-frame');}const now=performance.now();if(now-last>=1000){const p=core._th17_audio_statistics();if(p){const a=Array.from(core.HEAPU32.subarray(p>>>2,(p>>>2)+11));emit('audio-health',{ready:!!a[0],callbacks:a[1],renderedFrames:a[2],errorCode:a[3],queuedFrames:a[4],queuedMs:a[4]*1000/44100,backend:'SDL3',track:a[5],cursor:a[9],musicEnabled:music});}emit('frame-health',{fps:presents*1000/(now-last),simulationFps:ticks*1000/(now-last),maxGapMs:maxFrameMs,gpuMs:gpuMonitor?.value()??null});last=now;ticks=0;presents=0;maxFrameMs=0;}if(core._th17_phase()===4)void stop().catch(fatal);}});
 core.SDL3=core.SDL3||{};if(parent!==window&&parent.__touhouAudioContext)core.SDL3.audioContext=parent.__touhouAudioContext;
 window.Module=core;window.FS=core.FS;core.resetBrowserKeyboard=()=>keyboard.clear();
 core.FS.mkdirTree('/savesth17');core.FS.mount(core.IDBFS,{},'/savesth17');await sync(true);core.FS.mkdirTree('/savesth17/replay');core.FS.symlink('/savesth17','/save');
 if(!validEpoch||query.get('managedData')!=='1'||parent===window||typeof parent.__eaglerPrepareManagedRuntimeDataV1!=='function')throw Error('请从 eagler-touhou 启动此运行时');
 const {buffer}=await parent.__eaglerPrepareManagedRuntimeDataV1({game,generation:query.get('gameGeneration'),epoch});
 if(buffer.byteLength<16||buffer.byteLength>128*1024*1024)throw Error('游戏资源大小错误');
 core.FS.writeFile('/th17.dat',new Uint8Array(buffer),{canOwn:true});
 const response=await fetch('./resources.json');if(!response.ok)throw Error('资源清单加载失败');const manifest=await response.json();
 if(manifest.schema!=='eagler-sdl-resources/1'||manifest.game!==game||!Array.isArray(manifest.resources)||manifest.resources.length!==fontNames.length||new Set(manifest.resources.map(r=>r.path)).size!==fontNames.length||manifest.resources.some(r=>!fontNames.some(n=>r.path==='/fonts/'+n)))throw Error('资源清单无效');
 for(const r of manifest.resources)await resource(r);
 emit('ready',{saveRoot:'/savesth17'});
})().catch(e=>{if(e?.name==='AbortError'&&e?.message==='EAGLER_RUNTIME_SESSION_SUPERSEDED')return false;fatal(e);throw e;});
