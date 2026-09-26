import createModule from './th09.mjs';
import {scanCodes} from './keyboard.mjs';
import {SharedNetplay} from './shared-netplay.mjs';
import {exportReplayName,importReplayName} from './motion-replay.mjs';
const game='th09',protocol='eagler-touhou/1',query=new URLSearchParams(location.search),canvas=document.querySelector('canvas'),$=s=>document.querySelector(s);
// Variant identity is stamped by the builder, including offline Replay viewers.
const runtimeVariant=document.querySelector('meta[name="eagler-runtime-variant"]')?.content;
if(!['normal','multiplayer'].includes(runtimeVariant))throw Error('Invalid TH09 Runtime variant');
const saveRoot=runtimeVariant==='multiplayer'?'/savesth09mp':'/savesth09';
const epoch=Number(query.get('runtimeEpoch')),validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
let core,launched=false,first=false,options={},music=true,chain=Promise.resolve(),queue=Promise.resolve(),revision=0,netplay,stopping=false,networkOverlayPending=false;
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...fields},location.origin);
const values=(fn,n)=>Array.from(core.HEAP32.subarray(fn()/4,fn()/4+n));
const status=()=>({title:values(core._th09_title_status,8),session:values(core._th09_session_status,8),touch:values(core._th09_touch_state,4)});
const err=()=>{const a=core.HEAPU8.subarray(core._th09_error());return new TextDecoder().decode(a.subarray(0,a.indexOf(0)));};
function runtimeKeyboardCode(message){
 const code=String(message.code||'');if(code&&code!=='Unidentified')return code;
 const key=String(message.key||'').toLowerCase(),location=Number(message.location)||0;
 const byKey={z:'KeyZ',x:'KeyX',shift:location===2?'ShiftRight':'ShiftLeft',escape:'Escape',esc:'Escape',arrowup:'ArrowUp',arrowdown:'ArrowDown',arrowleft:'ArrowLeft',arrowright:'ArrowRight',control:location===2?'ControlRight':'ControlLeft',enter:location===3?'NumpadEnter':'Enter',tab:'Tab',backspace:'Backspace'};
 if(byKey[key])return byKey[key];const keyCode=Number(message.keyCode)||0,byCode={8:'Backspace',9:'Tab',13:location===3?'NumpadEnter':'Enter',16:location===2?'ShiftRight':'ShiftLeft',17:location===2?'ControlRight':'ControlLeft',27:'Escape',37:'ArrowLeft',38:'ArrowUp',39:'ArrowRight',40:'ArrowDown',88:'KeyX',90:'KeyZ'};
 return byCode[keyCode]||'';
}
const fatal=e=>{const message=e?.message||String(e);$('#error').textContent=message;core?._th09_loop_pause(1);emit('error',{error:message});};
const sync=(populate=false)=>{const current=chain.then(()=>new Promise((r,j)=>core.FS.syncfs(populate,e=>e?j(e):r())));chain=current.catch(()=>{});return current;};
function path(value){let name=String(value).replaceAll('\\','/').toLowerCase().replace(new RegExp('^'+saveRoot+'/'),'').replace(/^\//,'');if(!/^(?:score\.dat|th09\.cfg|replay\/th9_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?)$/.test(name))throw Error('存档路径无效');return name;}
function apply(){const sensitivity=Number(options.touchSensitivity??100);options.touchSensitivity=Number.isFinite(sensitivity)?Math.max(100,Math.min(300,sensitivity)):100;core.eaglerOptions=options;core._th09_touch_options(+!!options.touchEnabled,Math.max(0,['touch','touch-unlimited','joystick','joystick-free'].indexOf(options.touchMovementMode)),options.touchSensitivity/100,+(options.touchFocusMode==='two-finger'),+!!options.doubleTapBombEnabled);if(launched)core._th09_music_enabled(+music);}
async function resource(r){if(!(/^\/(?:music|fonts)\/[a-z0-9_.-]+$/.test(r.path)||r.path==='/msgothic.ttc'||r.path==='/unifont.otf'))throw Error('资源路径无效');const u=new URL(r.url,location.href);if(u.origin!==location.origin)throw Error('资源来源无效');const response=await fetch(u);if(!response.ok)throw Error('资源读取失败');const bytes=new Uint8Array(await response.arrayBuffer());core.FS.mkdirTree(r.path.slice(0,r.path.lastIndexOf('/'))||'/');core.FS.writeFile(r.path,bytes);emit('transfer',{mode:r.path.startsWith('/music/')?'ogg':'runtime',loaded:bytes.length,total:bytes.length,path:r.path});}
// Same runtimePack contract as the TH10 shell: Launcher verifies SHA-256,
// Runtime rechecks manifest identity, path scope and each mounted file size.
let runtimePackFiles=[];
function assertRuntimePackManifest(manifest,pack){
 if(manifest?.schema!=='eagler-touhou/thcrap-static-pack/1'||manifest.game!==game||
    manifest.language!==pack.language||typeof manifest.runtimeVersion!=='string'||
    !Array.isArray(manifest.files)||manifest.files.length>256)throw Error('Invalid TH09 language pack manifest');
 for(const file of manifest.files)
  if(typeof file?.path!=='string'||!file.path.startsWith('/thcrap/th09/')||file.path.includes('\\')||file.path.includes('..')||
     !Number.isInteger(file.bytes)||file.bytes<0)throw Error('Invalid TH09 language pack file');
}
async function installRuntimePack(pack){
 if(launched)throw Error('Runtime resources cannot be changed after launch');
 if(typeof pack?.url!=='string'||typeof pack.language!=='string'||
    !Number.isInteger(pack.bytes)||pack.bytes<=0||
    !pack.manifest||!Array.isArray(pack.files))throw Error('Invalid TH09 language pack');
 const url=new URL(pack.url,location.href);
 if(url.origin!==location.origin)throw Error('Cross-origin TH09 language pack');
 assertRuntimePackManifest(pack.manifest,pack);
 const expected=new Map(pack.manifest.files.map(file=>[file.path,file]));
 if(pack.files.length!==expected.size)throw Error('TH09 language pack file count mismatch');
 const verified=[];
 for(const file of pack.files){
  if(typeof file?.path!=='string'||!file.path.startsWith('/thcrap/th09/')||file.path.includes('\\')||file.path.includes('..')||
     !(file.bytes instanceof Uint8Array))throw Error('Invalid TH09 language pack path');
  const declaration=expected.get(file.path);
  if(!declaration||file.bytes.length!==declaration.bytes)throw Error(file.path+': size mismatch');
  verified.push({path:file.path,bytes:file.bytes});
 }
 for(const path of runtimePackFiles){try{core.FS.unlink(path);}catch{}}
 runtimePackFiles=[];
 for(const file of verified){
  core.FS.mkdirTree(file.path.slice(0,file.path.lastIndexOf('/')));
  core.FS.writeFile(file.path,file.bytes,{canOwn:true});runtimePackFiles.push(file.path);
 }
}
async function save(){if(launched&&!netplay?.spectator&&!core._th09_save_snapshot())throw Error('保存失败');await sync();}
async function stop(){if(stopping)return;stopping=true;try{networkOverlayPending=false;netplay?.close();core._th09_loop_stop();await save();core._th09_game_close();launched=false;delete window.__th09Runtime;emit('exit',{code:0,status:'success'});}finally{stopping=false;}}
function openNetwork(){if(!launched||options.netplayMode==='lan'||!status().title[1]||networkOverlayPending)return;networkOverlayPending=true;core._th09_loop_pause(1);emit('network-request');}
async function command(m){switch(m.command){
case 'configure':options=m.options||{};music=m.music==='ogg';for(const r of [...(m.sharedResources||[]),...(m.runtimeResources||[]),...(m.resources||[])])await resource(r);if(m.runtimePack)await installRuntimePack(m.runtimePack);if(core.FS.analyzePath('/msgothic.ttc').exists){try{core.FS.unlink('/fonts/msgothic.ttc');}catch{}core.FS.symlink('/msgothic.ttc','/fonts/msgothic.ttc');}apply();return {};
case 'resources':for(const r of m.resources||[])await resource(r);return {};
case 'keyboard':{const code=runtimeKeyboardCode(m);if(scanCodes[code])core._th09_key(scanCodes[code],+!!m.down);return {};}
case 'keyboard-clear':core._th09_keys_clear();return {};
case 'touch-cancel':core._th09_touch_cancel();return {};
case 'direct-touch':{const b=canvas.getBoundingClientRect();core._th09_touch(({down:0,move:1,up:2,cancel:2})[m.type]??2,Number(m.id)||0,(Number(m.x)*innerWidth-b.left)/b.width,(Number(m.y)*innerHeight-b.top)/b.height);return {};}
case 'touch-controls':{const t=m.controls||m,sensitivity=Number(t.touchSensitivity);if(sensitivity>=100&&sensitivity<=300&&sensitivity!==options.touchSensitivity){options.touchSensitivity=sensitivity;apply();}core._th09_touch_controls(+!!options.touchEnabled,+!!t.fireEnabled,+!!t.focusEnabled,t.bombSerial>>>0,t.escapeSerial>>>0);core._th09_touch_stick(Number(t.joystickX)||0,Number(t.joystickY)||0);return {};}
case 'launch':if(!launched){if(!core.FS.analyzePath('/fonts/msgothic.ttc').exists&&core.FS.analyzePath('/msgothic.ttc').exists)core.FS.symlink('/msgothic.ttc','/fonts/msgothic.ttc');if(!core._th09_game_open(Date.now()&65535))throw Error(err());launched=true;apply();first=false;netplay=options.netplayMode==='lan'?new SharedNetplay(core,{onStatus:t=>emit('notice',{message:t}),onClose:reason=>{if(reason)emit('notice',{message:reason});queueMicrotask(()=>void stop().catch(fatal));},onResult:()=>emit('notice',{message:'对局结束，可在游戏内保存 Replay'})}):null;$('#loading').textContent='';core._th09_loop_start();if(netplay){core._th09_loop_pause(1);await netplay.connect(options);}window.__th09Runtime={core,netplay,status,save,command};emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++',architecture:protocol,version:'2026.09.20-fix'});}return {};
case 'sync':await save();return {};
case 'list':{const files=[];for(const dir of ['', '/replay'])for(const name of core.FS.readdir(saveRoot+dir)){const n=(dir+'/'+name).replace(/^\//,'');try{path(n);}catch{continue;}const full=saveRoot+'/'+n,s=core.FS.stat(full);if(core.FS.isFile(s.mode))files.push({path:exportReplayName(n,core.FS.readFile(full),9),size:s.size});}return {files};}
case 'read':return {bytes:Array.from(core.FS.readFile(saveRoot+'/'+path(m.path).replace(/\.rpyx$/,'.rpy')))};
case 'write':{if(!Array.isArray(m.bytes)||m.bytes.length>16*1024*1024||m.bytes.some(b=>!Number.isInteger(b)||b<0||b>255))throw Error('存档数据无效');const bytes=Uint8Array.from(m.bytes),name=importReplayName(path(m.path),bytes,9);if(launched)throw Error('请先退出游戏再导入');const kind=name.endsWith('.rpy')?1:name==='score.dat'?0:2,p=core._th09_file_buffer(bytes.length);if(!p)throw Error('文件为空或过大');core.HEAPU8.set(bytes,p);if(!core._th09_file_valid(kind,bytes.length))throw Error('文件损坏或不是花映塚 1.50a 的存档/录像');core.FS.mkdirTree(saveRoot+'/replay');core.FS.writeFile(saveRoot+'/'+name,bytes);await sync();return {};}
case 'remove':if(launched)throw Error('请先退出游戏');core.FS.unlink(saveRoot+'/'+path(m.path).replace(/\.rpyx$/,'.rpy'));await sync();return {};
case 'network-open':openNetwork();return {};
case 'network-cancel':networkOverlayPending=false;core._th09_keys_clear();core._th09_loop_pause(+document.hidden);canvas.focus();return {};
default:throw Error('不支持的操作');}}
window.addEventListener('message',e=>{const m=e.data;if(!validEpoch||e.source!==parent||e.origin!==location.origin||m?.protocol!==protocol||m.game!==game||m.epoch!==epoch||typeof m.command!=='string')return;queue=queue.then(async()=>{await initialized;try{const result=await command(m);if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:true,...result},location.origin);}catch(e){if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:false,error:String(e),errno:e?.errno},location.origin);else fatal(e);}}).catch(fatal);});
// Room editing belongs to the Launcher overlay in the parent realm, so the
// Launcher only forwards keys while its own chrome holds focus. The Launcher
// focuses this Runtime document when the player opens, which means gameplay
// keys arrive here instead; handle them in this realm too.
for(const event of ['pointerdown','keydown'])window.addEventListener(event,()=>core?.SDL3?.audioContext?.resume().catch(()=>{}),{capture:true});
for(const [event,down] of [['keydown',1],['keyup',0]])window.addEventListener(event,e=>{
 if(!launched||!core)return;
 const code=runtimeKeyboardCode({code:e.code,key:e.key,keyCode:e.keyCode,location:e.location});
 if(scanCodes[code])core._th09_key(scanCodes[code],down);
},{capture:true});
document.addEventListener('visibilitychange',()=>{if(launched){core._th09_keys_clear();core._th09_touch_cancel();core._th09_loop_pause(+(document.hidden||networkOverlayPending||(options.netplayMode==='lan'&&!netplay?.active&&!netplay?.finished)));if(document.hidden)void save().catch(fatal);}});
window.addEventListener('blur',()=>{core?._th09_keys_clear();core?._th09_touch_cancel();});
// Android sends touches directly to the child; iOS uses the host protocol.
for(const [name,type] of [['pointerdown',0],['pointermove',1],['pointerup',2],['pointercancel',2]])document.body.addEventListener(name,e=>{
 if(!launched||!options.touchEnabled||e.pointerType==='mouse'||e.target.closest('button,input,dialog'))return;
 e.preventDefault();if(type===0)document.body.setPointerCapture(e.pointerId);const b=canvas.getBoundingClientRect();core._th09_touch(type,e.pointerId,(e.clientX-b.left)/b.width,(e.clientY-b.top)/b.height);
});
canvas.addEventListener('webglcontextlost',e=>{e.preventDefault();fatal(Error('图形环境失效，请退出后重新开始。'));});window.addEventListener('pagehide',()=>{if(launched){netplay?.close();core._th09_loop_pause(1);void save();}});
const initialized=(async()=>{
 let last=performance.now(),lastFrames=0;
 core=await createModule({canvas,printErr:console.error,onNetworkRequest:openNetwork,onNetworkResult:()=>netplay?.result(),onNetworkInput:(...args)=>netplay?.input(...args),onNetworkSpectatorFrame:(...args)=>netplay?.publish(...args),onGameFrame(ok,ms){if(!ok){fatal(Error(err()));return;}netplay?.frame();if(!first){first=true;emit('first-frame');}const current=core._th09_storage_revision();if(current!==revision){revision=current;void sync().catch(fatal);}const now=performance.now(),frames=status().title[0];if(now-last>=1000){emit('frame-health',{fps:(frames-lastFrames)*1000/(now-last),maxGapMs:ms});emit('audio-health',{backend:'script',robust:core.SDL3?.audioContext?.state==='running'});last=now;lastFrames=frames;}const title=status().title;if(title[7]===0||(netplay?.finished&&title[1]&&title[2]===1)||(netplay?.spectator&&title[1]&&(title[2]===14||title[2]===1)))void stop().catch(fatal);}});
 window.Module=core;window.FS=core.FS;core.SDL3=core.SDL3||{};if(parent!==window&&parent.__touhouAudioContext)core.SDL3.audioContext=parent.__touhouAudioContext;
 core.FS.mkdirTree(saveRoot);core.FS.mount(core.IDBFS,{},saveRoot);await sync(true);core.FS.mkdirTree(saveRoot+'/replay');core.FS.symlink(saveRoot,'/save');
 if(query.get('managedData')!=='1'||parent===window||typeof parent.__eaglerPrepareManagedRuntimeDataV1!=='function')throw Error('请从 eagler-touhou 启动花映塚');
 const buffer=(await parent.__eaglerPrepareManagedRuntimeDataV1({game,generation:query.get('gameGeneration'),epoch})).buffer;
 if(Object.prototype.toString.call(buffer)!=='[object ArrayBuffer]'||buffer.byteLength<1000000)throw Error('花映塚资源无效');core.FS.writeFile('/th09.dat',new Uint8Array(buffer));
 core.FS.mkdirTree('/fonts');for(const name of ['cp932.bin','blend.bin']){const response=await fetch('./fonts/'+name);if(!response.ok)throw Error('字体表缺失：'+name);core.FS.writeFile('/fonts/'+name,new Uint8Array(await response.arrayBuffer()));}
 emit('ready');if(query.get('standalone')==='1')await command({command:'launch'});
})().catch(e=>{fatal(e);throw e;});
