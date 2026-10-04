// Platform shell for the upstream eagler-touhou/1 Launcher contract.
// Game construction, input, timing, rendering, text and sound belong to C++.
// Mirrors th10/th20 shell.mjs and imports the shared eagler-host transport.
import createModule from './th11-sdl.mjs';
import {scanCodes} from './keyboard.mjs';
import {createBrowserKeyboard} from './directory-keyboard.mjs';
import {validateMotionReplay} from './motion-replay.mjs';
import {bindOutsideTouches,normalizeOptions,applyTouchOptions,touchControls,suspendRuntimeAudio,resumeRuntimeAudio,directTouch,installResources as installHostResources,observeMusicWrites,mountManagedData,isSupersededRuntimeError} from './eagler-host.mjs';
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
function apply(){Module.eaglerOptions=options;applyTouchOptions(core,options);core.th11_music_enabled(+music);}
let thpracKeyboardBits=0;
function thpracKey(code,down){const bit=code==='Backspace'?1:code==='Tab'?1<<8:code==='F12'?1<<9:/^F[1-7]$/.test(code)?1<<Number(code.slice(1)):0;if(!bit||!options.thpracEnabled)return false;if(down)thpracKeyboardBits|=bit;else thpracKeyboardBits&=~bit;(Module.eaglerControls??={}).thpracKeyboardBits=thpracKeyboardBits;return true;}
function clearPracticeKeys(){thpracKeyboardBits=0;if(Module)(Module.eaglerControls??={}).thpracKeyboardBits=0;}
const keyboard=createBrowserKeyboard({
 accept:code=>!!scanCodes[code],
 send(code,down){if(!core)return;if(thpracKey(code,down))return;core.th11_key(scanCodes[code],+down);},
 onClear:clearPracticeKeys
});
function clearKeyboard(){keyboard.clear();core?.th11_keys_clear();}
async function resumeForegroundAudio(forcePause=false){
 if(!Module||!core||!launched||document.hidden)return false;
 if(forcePause)core.sdl_loop_pause(1);
 return resumeRuntimeAudio(Module,core,()=>!!core&&launched&&!document.hidden);
}
function closeAudio(){
 // The launcher owns the shared context. SDL owns and must disconnect its
 // stream, but closing that stream must not close the launcher's context.
 const s=Module.SDL3,context=s?.audioContext,borrowed=parent!==window&&context===parent.__touhouAudioContext;
 if(borrowed)s.audioContext=undefined;
 try{core.th11_audio_close();}finally{if(borrowed)s.audioContext=context;}
}
async function stop(){if(stopping)return;stopping=true;try{clearKeyboard();core.th11_loop_stop();await save();closeAudio();launched=false;emit('exit',{code:0,status:'success'});}finally{stopping=false;}}
function path(value){const name=String(value).replaceAll('\\','/').toLowerCase().replace(/^\/savesth11\//,'').replace(/^\//,'');if(!/^(?:scoreth11\.dat|th11\.cfg|replay\/th11_(?:\d{2}|ud[a-z0-9]{4})\.rpyx?)$/.test(name))throw Error('存档路径无效');return name;}
function launch(){
 if(launched)return;clearKeyboard();
 if(!core.th11_initialize())throw Error(coreError());
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
 case 'touch-controls':touchControls(core,options,m);return {};
 case 'launch':launch();return {};
 case 'sync':await save();return {};
 case 'list':{const files=[];for(const dir of ['','/replay'])for(const name of Module.FS.readdir('/savesth11'+dir)){const n=(dir+'/'+name).replace(/^\//,'');try{path(n);}catch{continue;}const s=Module.FS.stat('/savesth11/'+n);if(Module.FS.isFile(s.mode)){files.push({path:n,size:s.size});}}return {files};}
 case 'read':return {bytes:Array.from(Module.FS.readFile('/savesth11/'+path(m.path)))};
 case 'write':{
  if(launched)throw Error('请先退出游戏');const target=path(m.path),bytes=new Uint8Array(m.bytes||[]);
  if(!bytes.length||bytes.length>64*1024*1024)throw Error('文件大小无效');
  const p=core.malloc(bytes.length);if(!p)throw Error('文件导入内存不足');let valid=false;try{Module.HEAPU8.set(bytes,p);valid=!!core.th11_validate_file(target==='scoreth11.dat'?0:target==='th11.cfg'?2:1,p,bytes.length);}finally{core.free(p);}
  if(!valid)throw Error('文件不是有效的地灵殿存档或录像');
  if(target.startsWith('replay/')&&!validateMotionReplay(bytes))throw Error('录像数据无效');
  Module.FS.writeFile('/savesth11/'+target,bytes);await sync(false);return {};
 }
 case 'remove':{if(launched)throw Error('请先退出游戏');Module.FS.unlink('/savesth11/'+path(m.path));await sync(false);return {};}
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
