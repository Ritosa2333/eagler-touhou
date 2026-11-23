// th16 启动器壳：eagler-touhou/1 协议（与 ports/th20 同一传输层）。游戏构造、输入、计时、绘制、文字与声音都在 C++。
//   数据：启动器经 __eaglerPrepareManagedRuntimeDataV1 交付 th16.dat → MEMFS /game/th16.dat；resources.json 列出 OGG 与共享字体
//   存档：/savesth16/<jp|chs> 挂 IDBFS（scoreth16.dat / th16.cfg / replay/th16_XX.rpy），每次 C++ 写盘后回写
//   输入：启动器托管键（keyboard 命令 / 页面 keydown）→ sdl_key(DOM code)；触屏按钮 → sdl_touch_controls
import {createBrowserKeyboard} from './directory-keyboard.mjs';
import createModule from './th16-sdl.mjs';
import {bindOutsideTouches,normalizeOptions,applyTouchOptions,touchControls,suspendRuntimeAudio,resumeRuntimeAudio,directTouch,ensureSharedFontAlias,installResources as installHostResources,observeMusicWrites,mountManagedData,isSupersededRuntimeError} from './eagler-host.mjs';
const protocol='eagler-touhou/1',game='th16',query=new URLSearchParams(location.search),canvas=document.querySelector('canvas');
const epoch=Number(query.get('runtimeEpoch'));
const validEpoch=Number.isSafeInteger(epoch)&&epoch>0;
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...fields},location.origin);
let Module,core,app=0,launched=false,first=false,closing=false,language=query.get('language')==='lang_zh-hans'?'chs':'jp',options={},music=true;
const keyboard=createBrowserKeyboard({hostContext:{protocol,game,epoch,origin:location.origin,ready:()=>launched&&!document.hidden&&!closing,reset:clearKeyboard},
 send(code,down){if(!core)return;cstring(code,p=>core.sdl_key(p,+down));},
});
function clearKeyboard(){keyboard.clear();core?.sdl_keys_clear();}
let frames=0,lastHealth=0,lastFrame=0,maxGap=0,lastPresented=0,saveTimer=null;
const cancelTouches=bindOutsideTouches(document,canvas,()=>core,()=>launched&&options.touchEnabled);
const error=reason=>{const message=reason?.stack||String(reason);document.querySelector('#error').textContent=message;emit('error',{message,error:message});console.error(reason);};
const u32=(ptr,count)=>new Uint32Array(core.memory.buffer,ptr,count);
const cstring=(text,fn)=>{const bytes=new TextEncoder().encode(text+'\0'),p=core.graphics_allocate(bytes.length);try{new Uint8Array(core.memory.buffer,p,bytes.length).set(bytes);return fn(p);}finally{core.graphics_free(p);}};
const root=()=>'/savesth16/'+language;
let storageSync=Promise.resolve();
const sync=populate=>{const current=storageSync.then(()=>new Promise((resolve,reject)=>Module.FS.syncfs(populate,e=>e?reject(e):resolve())));storageSync=current.catch(()=>{});return current;};
function relativeSave(path){
 if(typeof path!=='string'||path.length>200)throw Error('Invalid save path');
 path=path.replaceAll('\\','/').toLowerCase();
 if(path.startsWith('/savesth16/'))path=path.slice('/savesth16/'.length).replace(/^(?:jp|chs)\//,'');
 else path=path.replace(/^\//,'');
 if(!/^(?:scoreth16\.dat|th16\.cfg|replay\/th16_(?:\d{2}|ud[a-z0-9]{4})\.rpy)$/.test(path))throw Error('Invalid save path: '+path);
 return path;
}
async function mountData(){await mountManagedData(Module,{game,parentWindow:parent,query,emit});}
async function installResources(resources=[]){return installHostResources(Module,resources,{game,emit});}
// 0x459240 文字：CP932 → Unicode 表由浏览器 Shift_JIS 解码器生成（FontPainter 读 /fonts/cp932.bin，131072 字节）
function installCodePage(){
 const decoder=new TextDecoder('shift_jis'),mapping=new Uint16Array(65536);
 for(let n=0;n<65536;++n){const v=decoder.decode(n<256?Uint8Array.of(n):Uint8Array.of(n>>8,n&255));mapping[n]=v.length===1&&v!=='\ufffd'?v.charCodeAt(0):0x30fb;}
 Module.FS.mkdirTree('/fonts');Module.FS.writeFile('/fonts/cp932.bin',new Uint8Array(mapping.buffer));
}
function applyOptions(){applyTouchOptions(core,options);}
function status(){return Array.from(new Int32Array(core.memory.buffer,core.sdl_game_status(),10));}
function save(){if(app)core.application_save(app);return sync(false);}
async function resumeForegroundAudio(forcePause=false){
 if(!core||!launched||document.hidden)return false;
 if(forcePause)core.sdl_loop_pause(1);
 return resumeRuntimeAudio(Module,core,()=>!!core&&launched&&!document.hidden);
}
async function stop(){if(closing)return;closing=true;clearKeyboard();try{core.sdl_loop_stop();await save();core.sdl_game_close();await sync(false);app=0;launched=false;emit('exit',{code:0,status:'success'});}finally{closing=false;}}
async function launch(){
 if(launched)return;clearKeyboard();
 ensureSharedFontAlias(Module,language);installCodePage();
 const mode=Module.touhouMusicMode||'none';music=mode!=='none';core.sdl_ogg_decode_mode(options.oggDecodeMode==='full');core.sdl_music_enabled(music);
 app=core.sdl_game_open(language==='chs'?1:0,Date.now()&65535);if(!app)throw Error('C++ game initialization failed: '+Module.UTF8ToString(core.th16_error()));
 core.sdl_music_enabled(music);applyOptions();launched=true;first=false;lastPresented=0;lastHealth=performance.now();lastFrame=0;frames=0;maxGap=0;
 canvas.focus({preventScroll:true});core.sdl_loop_pause(1);if(!document.hidden)void resumeForegroundAudio();core.sdl_loop_start(app);
 emit('runtime-info',{renderer:'SDL3 / WebGL2 / C++',architecture:'eagler-touhou/1',version:'1.0.0-sdl3'});
}
async function command(message){
 switch(message.command){
 case 'configure':if(launched)throw Error('Cannot configure a running game');language=message.language==='lang_zh-hans'?'chs':'jp';options=normalizeOptions(message.options);if(!['ogg','none'].includes(message.music))throw Error('Invalid music mode');Module.touhouMusicMode=message.music;Module.eaglerOptions=options;music=message.music!=='none';await installResources(message.sharedResources);await installResources(message.runtimeResources);await installResources(message.resources);applyOptions();return {};
 case 'resources':await installResources(message.resources);return {};
 case 'keyboard':if(launched&&!document.hidden&&!closing)keyboard.event(message,!!message.down,'hosted');return {};
 case 'keyboard-clear':clearKeyboard();return {};
 case 'touch-cancel':cancelTouches();return {};
 case 'direct-touch':directTouch(core,canvas,message,{width:innerWidth,height:innerHeight});return {};
 case 'touch-controls':touchControls(core,options,message);return {};
 case 'launch':await launch();return {};
 case 'sync':await save();return {};
 case 'list':{const files=[];for(const dir of ['','/replay'])for(const name of Module.FS.readdir(root()+dir)){const path=(dir+'/'+name).replace(/^\//,'');try{relativeSave(path);}catch{continue;}const full=root()+'/'+path,s=Module.FS.stat(full);if(Module.FS.isFile(s.mode))files.push({path,size:s.size});}return {files};}
 case 'read':{const path=relativeSave(message.path);return {bytes:Array.from(Module.FS.readFile(root()+'/'+path))};}
 case 'write':{if(!Array.isArray(message.bytes)||message.bytes.length>16*1024*1024||message.bytes.some(b=>!Number.isInteger(b)||b<0||b>255))throw Error('Invalid save bytes');const bytes=new Uint8Array(message.bytes);Module.FS.mkdirTree(root()+'/replay');Module.FS.writeFile(root()+'/'+relativeSave(message.path),bytes);await sync(false);return {};}
 case 'remove':{const path=relativeSave(message.path);Module.FS.unlink(root()+'/'+path);await sync(false);return {};}
 default:throw Error('Unsupported runtime command: '+message.command);
 }
}
let queue=Promise.resolve();
async function dispatchCommand(m){
 try{const result=await command(m);if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:true,...result},location.origin);}
 catch(e){if(typeof m.request==='string')parent.postMessage({protocol,game,epoch,request:m.request,ok:false,error:String(e),errno:e?.errno},location.origin);else error(e);}
}
window.addEventListener('message',event=>{const m=event.data;if(!validEpoch||event.source!==parent||event.origin!==location.origin||m?.protocol!==protocol||m.game!==game||m.epoch!==epoch||typeof m.command!=='string')return;
 if(m.command==='keyboard'||m.command==='keyboard-clear'){void dispatchCommand(m);return;}
 queue=queue.then(async()=>{if(await initialized===false)return;await dispatchCommand(m);}).catch(error);
});
document.addEventListener('visibilitychange',()=>{if(!core||!launched)return;clearKeyboard();cancelTouches();if(document.hidden){suspendRuntimeAudio(Module,core);queue=queue.then(save).catch(error);}else void resumeForegroundAudio(true);});
window.addEventListener('blur',()=>{if(core){clearKeyboard();cancelTouches();}});
window.addEventListener('pagehide',()=>{clearKeyboard();cancelTouches();if(core&&launched){suspendRuntimeAudio(Module,core);void save().catch(console.error);}});
window.addEventListener('pageshow',()=>{if(core&&launched&&!document.hidden)void resumeForegroundAudio(true);});
canvas.addEventListener('webglcontextlost',event=>{event.preventDefault();clearKeyboard();core?.sdl_loop_pause(1);error('图形环境已失效，请退出后重新开始。');});
for(const name of ['pointerdown','keydown'])window.addEventListener(name,()=>{if(Module?.SDL3?.audioContext?.state!=='running')void resumeForegroundAudio(true);},{capture:true});
for(const name of ['keydown','keyup'])window.addEventListener(name,event=>{
 if(!core||!launched||closing||document.hidden)return;
 if(keyboard.mapNative(event,name==='keydown')){event.preventDefault();event.stopImmediatePropagation();return;}
 keyboard.event(event,name==='keydown');
},{capture:true});
const initialized=(async()=>{
 let audioContext;try{audioContext=parent.__touhouAudioContext;}catch{}
 Module=await createModule({canvas,noInitialRun:true,...(audioContext?{SDL3:{audioContext}}:{}),print:console.log,printErr:console.error,
  instantiateWasm(imports,ready){return WebAssembly.instantiateStreaming(fetch('./th16-sdl.wasm'),imports).then(({instance,module})=>{core=instance.exports;ready(instance,module);return core;});}
 });
 window.Module=Module;window.FS=Module.FS;observeMusicWrites(Module,core,game);Module.FS.mkdirTree('/savesth16');Module.FS.mount(Module.IDBFS,{},'/savesth16');await sync(true);
 for(const lang of ['jp','chs'])Module.FS.mkdirTree('/savesth16/'+lang+'/replay');await mountData();cstring('#canvas',core.sdl_canvas);
 Module.runtimePrepare=()=>!document.hidden;
 Module.runtimeFinish=(result,duration)=>{
  const now=performance.now(),p=u32(core.sdl_stats(),6)[5];if(p!==lastPresented){frames++;if(lastFrame)maxGap=Math.max(maxGap,now-lastFrame);lastFrame=now;lastPresented=p;if(!first){first=true;emit('first-frame');}}
  if(result||core.application_error(app)){if(core.application_error(app)){error('Game error: '+Module.UTF8ToString(core.th16_error()));core.sdl_loop_pause(1);}else queueMicrotask(()=>void stop().catch(error));}
  if(now-lastHealth>=1000){emit('frame-health',{fps:frames*1000/(now-lastHealth),maxGapMs:maxGap,frameMs:duration});const a=u32(core.sdl_audio_stats(),12);emit('audio-health',{queuedMs:a[5]*1000/44100,minQueuedMs:a[7]*1000/44100,backend:'script',underruns:0,robust:true});frames=0;maxGap=0;lastHealth=now;}
 };
 Module.runtimeFileChanged=()=>{if(saveTimer!==null)return;saveTimer=setTimeout(()=>{saveTimer=null;queue=queue.then(()=>sync(false)).catch(error);},0);};
 Module.runtimeStopped=()=>{};Module.callMain=launch;
 window.__th16Runtime={core,Module,get app(){return app;},status,launch,stop,command};
 emit('ready');
})().catch(e=>{if(isSupersededRuntimeError(e)){console.debug('Runtime navigation superseded');return false;}error(e);throw e;});
