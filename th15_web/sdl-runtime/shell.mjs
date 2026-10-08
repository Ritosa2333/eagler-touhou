import {installStartupBranding,finishStartupAnimation} from './startup-branding.mjs';
import createModule from './th15.mjs';
import {createBrowserKeyboard} from './directory-keyboard.mjs';
import {scanCodes} from './keyboard.mjs';
const runtimeBuild=/*TH15_BUILD_INFO*/{version:'development-incomplete',completeGame:false};
const game='th15',protocol='eagler-touhou/1',query=new URLSearchParams(location.search),canvas=document.querySelector('canvas'),$=s=>document.querySelector(s);
const epoch=Number(query.get('runtimeEpoch'));
const validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
let core,launched=false,first=false,stopping=false,options={},music=true,chain=Promise.resolve(),queue=Promise.resolve();
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...fields},location.origin);
const keyboard=createBrowserKeyboard({accept:code=>!!scanCodes[code],send:(code,down)=>core?._th15_key(scanCodes[code],+down)});
function clearKeyboard(){keyboard.clear();core?._th15_keys_clear();}
const err=()=>{const p=core._th15_error(),end=core.HEAPU8.indexOf(0,p);return new TextDecoder().decode(core.HEAPU8.subarray(p,end<0?p+256:end));};
const fatal=e=>{const message=e?.message||String(e);$('#error').textContent=message;core?._th15_loop_pause(1);emit('error',{error:message});};
const sync=(populate=false)=>{const current=chain.then(()=>new Promise((r,j)=>core.FS.syncfs(populate,e=>e?j(e):r())));chain=current.catch(()=>{});return current;};
const save=()=>{if(launched&&!core._th15_save_scores())throw Error(err());return sync();};
function closeAudio(){
 // The launcher owns the shared context. SDL owns and must disconnect its
 // stream, but closing that stream must not close the launcher's context.
 const s=core.SDL3,context=s?.audioContext,borrowed=parent!==window&&context===parent.__touhouAudioContext;
 if(borrowed)s.audioContext=undefined;
 try{core._th15_audio_close();}finally{if(borrowed)s.audioContext=context;}
}
async function stop(){if(stopping)return;stopping=true;try{core._th15_loop_stop();await save();closeAudio();launched=false;emit('exit',{code:0,status:'success'});}finally{stopping=false;}}
function path(value){const name=String(value).replaceAll('\\','/').toLowerCase().replace(/^\/savesth15\//,'').replace(/^\//,'');if(!/^(?:scoreth15\.dat|th15\.cfg|replay\/th15_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?|autosave\/save[0-3]_[0-4]\.dat)$/.test(name))throw Error('存档路径无效');return name;}
function apply(){core._th15_limit_presentation(+!!options.limitPresentationTo60);core._th15_touch_options(+!!options.touchEnabled,Math.max(0,['touch','touch-unlimited','joystick','joystick-free'].indexOf(options.touchMovementMode)),Math.max(100,Math.min(300,Number(options.touchSensitivity)||100))/100,+(options.touchFocusMode==='two-finger'),+!!options.doubleTapBombEnabled);core._th15_music_enabled(+music);}
async function resource(r){
 if(!/^\/music\/(?:th15_\d{2}|th128_08)\.ogg$/.test(r.path)&&!/^\/fonts\/(?:font[0-7]|cp932|blend4444)\.bin$/.test(r.path))throw Error('资源路径无效');
 const u=new URL(r.url,location.href);if(u.origin!==location.origin||!['http:','https:','blob:'].includes(u.protocol))throw Error('资源来源无效');
 const response=await fetch(u);if(!response.ok)throw Error('资源读取失败');const bytes=new Uint8Array(await response.arrayBuffer());
 const expected=r.bytes??r.size;if(bytes.length>64*1024*1024||(expected!=null&&bytes.length!==expected))throw Error('资源大小错误');
 core.FS.mkdirTree(r.path.slice(0,r.path.lastIndexOf('/')));core.FS.writeFile(r.path,bytes,{canOwn:true});
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
async function command(m){switch(m.command){
case 'configure':if(launched)throw Error('不能配置正在运行的游戏');if(!['ogg','none'].includes(m.music))throw Error('不支持的音乐模式');options=m.options||{};music=m.music==='ogg';for(const r of [...(m.runtimeResources||[]),...(m.resources||[])])await resource(r);if(m.runtimePack)await installRuntimePack(m.runtimePack);apply();return {};
case 'resources':for(const r of m.resources||[])await resource(r);return {};
case 'keyboard':if(launched&&!document.hidden&&!stopping)keyboard.event(m,!!m.down,'hosted');return {};
case 'keyboard-clear':clearKeyboard();return {};
case 'touch-cancel':core._th15_touch_cancel();return {};
case 'direct-touch':{if(m.type==='cancel'){core._th15_touch_cancel();return {};}if(!['down','move','up'].includes(m.type)||![m.x,m.y,m.id].every(Number.isFinite))return {};const b=canvas.getBoundingClientRect();if(b.width&&b.height)core._th15_touch(({down:0,move:1,up:2,cancel:2})[m.type]??2,Number(m.id)||0,(Number(m.x)*innerWidth-b.left)/b.width,(Number(m.y)*innerHeight-b.top)/b.height);return {};}
case 'touch-controls':{const t=m.controls||m;if(Number.isFinite(t.touchSensitivity)&&t.touchSensitivity>=100&&t.touchSensitivity<=300&&t.touchSensitivity!==options.touchSensitivity){options.touchSensitivity=t.touchSensitivity;apply();}core._th15_touch_controls(+!!options.touchEnabled,+!!t.fireEnabled,+!!t.focusEnabled,t.bombSerial>>>0,t.escapeSerial>>>0);core._th15_touch_stick(Number(t.joystickX)||0,Number(t.joystickY)||0);return {};}
case 'launch':if(!launched){await installStartupBranding(core,{game,builtAt:(await(await fetch('./manifest.json')).json()).builtAt});let scaleOption=query.get('renderScale');if(!scaleOption)try{scaleOption=new URLSearchParams(parent.location.search).get('renderScale');}catch{}const requested=Number(scaleOption);core._th15_render_scale(options.renderScale===1?1:options.renderScale===2?2:requested===1?1:2);if(!core._th15_prepare_loading())throw Error(err());$('#loading').textContent='';const startupImageAt=performance.now();performance.mark('eagler-startup-image');await new Promise(requestAnimationFrame);await new Promise(requestAnimationFrame);if(!core._th15_initialize())throw Error(err());await finishStartupAnimation(startupImageAt,frames=>core._th15_draw_startup(frames));performance.mark('eagler-startup-menu-ready');launched=true;apply();first=false;$('#loading').textContent='';core._th15_loop_start();emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++',architecture:protocol,...runtimeBuild});}return {};
case 'sync':await save();return {};
case 'list':{const files=[];for(const dir of ['','/replay','/autosave'])for(const name of core.FS.readdir('/savesth15'+dir)){const n=(dir+'/'+name).replace(/^\//,'');try{path(n);}catch{continue;}const s=core.FS.stat('/savesth15/'+n);if(core.FS.isFile(s.mode))files.push({path:n,size:s.size});}return {files};}
case 'read':return {bytes:Array.from(core.FS.readFile('/savesth15/'+path(m.path)))};
case 'write':{
 if(launched)throw Error('请先退出游戏');const target=path(m.path),bytes=new Uint8Array(m.bytes||[]);
 if(!bytes.length||bytes.length>(target.startsWith('autosave/')?100*1024*1024+96:64*1024*1024))throw Error('文件大小无效');
 const p=core._malloc(bytes.length);if(!p)throw Error('文件导入内存不足');let valid=false;try{core.HEAPU8.set(bytes,p);valid=!!core._th15_validate_file(target==='scoreth15.dat'?0:target==='th15.cfg'?2:target.startsWith('autosave/')?3:1,p,bytes.length);}finally{core._free(p);}
 if(!valid)throw Error('文件不是有效的绀珠传存档或录像');if(target.startsWith('autosave/')){const parts=/save([0-3])_([0-4])/.exec(target),header=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength);if(header.getInt32(16,true)!==Number(parts[1])||header.getInt32(20,true)!==Number(parts[2])||header.getInt32(24,true)<1||header.getInt32(24,true)>7)throw Error('章节存档与角色、难度不一致');}core.FS.writeFile('/savesth15/'+target,bytes);await sync();return {};
}
case 'remove':if(launched)throw Error('请先退出游戏');core.FS.unlink('/savesth15/'+path(m.path));await sync();return {};
default:throw Error('不支持的操作');}}
window.addEventListener('message',e=>{const m=e.data;if(e.source!==parent||e.origin!==location.origin||m?.protocol!==protocol||m.game!==game||!validEpoch||m.epoch!==epoch||typeof m.command!=='string')return;queue=queue.then(async()=>{if(await initialized===false)return;try{const result=await command(m);if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:true,...result},location.origin);}catch(e){if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:false,error:String(e),errno:e?.errno},location.origin);else fatal(e);}}).catch(fatal);});
document.addEventListener('visibilitychange',()=>{if(launched){clearKeyboard();core._th15_touch_cancel();core._th15_loop_pause(+document.hidden);if(document.hidden)try{void save().catch(fatal);}catch(e){fatal(e);}}});
window.addEventListener('blur',()=>{clearKeyboard();core?._th15_touch_cancel();});
// Keyboard events in the child realm do not bubble to the launcher. SDL's
// canvas listener alone misses keys when the child BODY owns focus.
for(const event of ['keydown','keyup'])window.addEventListener(event,e=>{if(!launched||e.altKey||e.metaKey)return;if(keyboard.event(e,event==='keydown')){e.preventDefault();e.stopPropagation();}},{capture:true});
for(const event of ['pointerdown','keydown'])window.addEventListener(event,()=>core?.SDL3?.audioContext?.resume().catch(()=>{}),{capture:true});
for(const [name,type] of [['pointerdown',0],['pointermove',1],['pointerup',2],['pointercancel',2]])document.body.addEventListener(name,e=>{
 if(!launched||!options.touchEnabled||e.pointerType==='mouse')return;if(e.type==='pointercancel'){core._th15_touch_cancel();return;}e.preventDefault();if(type===0)document.body.setPointerCapture(e.pointerId);const b=canvas.getBoundingClientRect();if(b.width&&b.height)core._th15_touch(type,e.pointerId,(e.clientX-b.left)/b.width,(e.clientY-b.top)/b.height);
});
canvas.addEventListener('webglcontextlost',e=>{e.preventDefault();fatal(Error('图形环境失效，请退出后重新开始。'));});
window.addEventListener('pagehide',()=>{clearKeyboard();core?._th15_touch_cancel();if(launched){core._th15_loop_stop();try{void save().catch(fatal);}catch(e){fatal(e);}closeAudio();}});
// A mobile browser may terminate a hidden page before pagehide's IDB callback.
// Persist during play as well, with writes serialized by the sync chain.
setInterval(()=>{if(launched&&!document.hidden)try{void save().catch(fatal);}catch(e){fatal(e);}},30000);
const initialized=(async()=>{
 let last=performance.now(),ticks=0,presents=0,maxFrameMs=0;
 core=await createModule({canvas,printErr:console.error,onGameFrame(ok,ms,count){if(!ok){fatal(Error(err()));return;}ticks+=count;presents++;maxFrameMs=Math.max(maxFrameMs,ms);if(!first){first=true;last=performance.now();ticks=0;presents=0;maxFrameMs=0;emit('first-frame');}const now=performance.now();if(now-last>=1000){const p=core._th15_audio_statistics();if(p){const a=Array.from(core.HEAPU32.subarray(p>>>2,(p>>>2)+11));emit('audio-health',{ready:!!a[0],callbacks:a[1],renderedFrames:a[2],errorCode:a[3],queuedFrames:a[4],queuedMs:a[4]*1000/44100,backend:'SDL3',track:a[5],cursor:a[9],musicEnabled:music});}emit('frame-health',{fps:presents*1000/(now-last),simulationFps:ticks*1000/(now-last),maxGapMs:maxFrameMs});last=now;ticks=0;presents=0;maxFrameMs=0;}if(core._th15_phase()===4)void stop().catch(fatal);}});
 core.SDL3=core.SDL3||{};if(parent!==window&&parent.__touhouAudioContext)core.SDL3.audioContext=parent.__touhouAudioContext;
 window.Module=core;window.FS=core.FS;core.resetBrowserKeyboard=()=>keyboard.clear();
 core.FS.mkdirTree('/savesth15');core.FS.mount(core.IDBFS,{},'/savesth15');await sync(true);core.FS.mkdirTree('/savesth15/replay');core.FS.mkdirTree('/savesth15/autosave');core.FS.symlink('/savesth15','/save');
 if(!validEpoch||query.get('managedData')!=='1'||parent===window||typeof parent.__eaglerPrepareManagedRuntimeDataV1!=='function')throw Error('请从 eagler-touhou 启动此运行时');
 const {buffer}=await parent.__eaglerPrepareManagedRuntimeDataV1({game,generation:query.get('gameGeneration'),epoch});
 if(buffer.byteLength<16||buffer.byteLength>128*1024*1024)throw Error('游戏资源大小错误');
 core.FS.writeFile('/th15.dat',new Uint8Array(buffer),{canOwn:true});
 const response=await fetch('./resources.json');if(!response.ok)throw Error('资源清单加载失败');const manifest=await response.json();
 if(manifest.schema!=='eagler-sdl-resources/1'||manifest.game!==game||!Array.isArray(manifest.resources)||manifest.resources.length!==10)throw Error('资源清单无效');
 for(const r of manifest.resources)await resource(r);
 emit('ready');
})().catch(e=>{if(e?.name==='AbortError'&&e?.message==='EAGLER_RUNTIME_SESSION_SUPERSEDED')return false;fatal(e);throw e;});
