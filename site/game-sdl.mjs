// Browser boundary for the shared SDL3 application. No game/menu/render loop.
import createTH12SDL from './runtime/sdl/touhou12.mjs';
import {RuntimeAssets} from '../src/managed-data.mjs';
import {ManagedInput,keyboardCode} from '../src/managed-input.mjs';
import {TouchInput} from '../src/touch-input.mjs';
import {installManagedShell} from '../src/managed-shell.mjs';
import {openStore,allowedPath} from '../src/storage.mjs';
import {prepareLocalization,isLocalizationResourcePath} from '../src/localization/loader.mjs';
import {mountSdlContent} from '../src/sdl-content.mjs';
import {sdlStopError} from '../src/sdl-lifecycle.mjs';
import {sdlPointerPosition} from '../src/sdl-input.mjs';
import {parseReplay} from '../src/replay.mjs';

const canvas=document.querySelector('#screen'),notice=document.querySelector('#notice');
const assets=new RuntimeAssets(),input=new ManagedInput(),touch=new TouchInput(),keys=new Map();
const encoder=new TextEncoder();
let core,store,localization,emit=()=>{},launched=false,terminated=false,firstFrame=false;
let options={limitPresentationTo60:true},music='ogg',state={kind:'menu',key:'inactive',ready:false};
let healthAt=0,frames=0,lastFrame=0,maxGap=0,syncedAt=0,syncQueue=Promise.resolve(),oldFiles=new Map();
let viewer=null;const failedMusic=new Set(),preparedMusic=new Set(); // Existing files and in-flight requests, retained for this core.
let lastFailure='',nativeStarted=false;const startupLog=[];
const copy=value=>new Uint8Array(value.buffer,value.byteOffset,value.byteLength).slice();
const equal=(a,b)=>a?.length===b.length&&a.every((v,i)=>v===b[i]);
function fail(error){globalThis.__eaglerCrashReporterV1?.fatal(error,{source:'sdl-runtime'});lastFailure=error?.message??String(error);notice.textContent=lastFailure;emit({event:'error',error:lastFailure});}
function musicFailed(track,error){
  failedMusic.add(track);
  const message='音乐加载失败：'+track+'：'+(error?.message??String(error));
  notice.textContent=message;emit({event:'error',code:'audio-resource',error:message});
}
async function installResources(items){
  await assets.install(items,message=>emit(message));
  for(const track of failedMusic)preparedMusic.delete(track);
  failedMusic.clear();
}
function vk(code){
  if(/^Key[A-Z]$/.test(code))return code.charCodeAt(3);
  if(/^Digit[0-9]$/.test(code))return code.charCodeAt(5);
  if(/^Numpad[0-9]$/.test(code))return 96+Number(code.slice(6));
  if(/^F(?:[1-9]|1[0-2])$/.test(code))return 111+Number(code.slice(1));
  return ({ArrowLeft:37,ArrowUp:38,ArrowRight:39,ArrowDown:40,Enter:13,Escape:27,Tab:9,Backspace:8,Space:32,Delete:46,Insert:45,Home:36,End:35,PageUp:33,PageDown:34,Minus:189,Period:190,NumpadSubtract:109,NumpadDecimal:110,ShiftLeft:16,ShiftRight:16,ControlLeft:17,ControlRight:17})[code]??0;
}
function key(code,down,source='host'){
  const n=vk(code);if(!n)return;
  const id=source+':'+code;if(down)keys.set(id,n);else keys.delete(id);
  const held=[...keys.values()].includes(n);core?._th12_host_key(n,Number(held));
}
function clearKeyboard(){const codes=new Set(keys.values());keys.clear();input.clear();for(const code of codes)core?._th12_host_key(code,0);core?._th12_host_clear(0);}
const pendingPracticePointers=new Map();
function cancelTouch(){pendingPracticePointers.clear();touch.cancel();core?._th12_host_controls(0,0,0,0);core?._th12_host_clear(1);}
function stringAt(pointer){return core.UTF8ToString(pointer);}
function withString(text,fn){const size=encoder.encode(text).length+1,pointer=core._malloc(size);try{core.stringToUTF8(text,pointer,size);return fn(pointer);}finally{core._free(pointer);}}
function beforeFrame(){
  const pad=input.pad()??0;let arrows=!!(pad&240),keyFocused=false;
  for(const code of keys.values()){if(code>=37&&code<=40)arrows=true;if(code===16)keyFocused=true;}
  const sample=touch.sample(state,{arrows,keyFocused});
  const m=sample.motion;core._th12_host_controls(sample.held|pad,m?.mode??0,m?.x??0,m?.y??0);
  core._th12_host_pressed(sample.pressed);
}
function frame(){
  const now=performance.now();const raw=JSON.parse(stringAt(core._th12_host_state()));state={...raw,key:raw.kind==='menu'?'menu:'+raw.menuSerial:raw.kind+':'+raw.instance};
  if(viewer&&!state.seeking&&state.scene==='replay-playing')viewer.target=null;
  if(!firstFrame){firstFrame=true;notice.textContent='';emit({event:'first-frame'});healthAt=now;}
  frames++;if(lastFrame)maxGap=Math.max(maxGap,now-lastFrame);lastFrame=now;
  if(now-healthAt>=1000){emit({event:'frame-health',fps:state.fps??frames*1000/(now-healthAt),maxGapMs:maxGap});emit({event:'audio-health',backend:'SDL3',musicMode:music});healthAt=now;frames=0;maxGap=0;}
  if(music!=='none'&&state.track&&!preparedMusic.has(state.track)){
    const track=state.track;preparedMusic.add(track);
    if(!core.FS.analyzePath('/game/music/'+track+'.ogg').exists)
      assets.fetch('/assets/music/'+track+'.ogg').then(async response=>{if(!response.ok)throw Error('音乐加载失败：'+track);core.FS.writeFile('/game/music/'+track+'.ogg',new Uint8Array(await response.arrayBuffer()));core._th12_host_refresh_music();}).catch(error=>musicFailed(track,error));
  }
  if(viewer&&now-viewer.reportedAt>=100){const part=viewer.parts.find(p=>p.stage===state.stage);emit({event:'runtime-info',playback:{kind:'replay',path:viewer.path,frame:(part?.start??0)+(state.cursor??0),total:viewer.total,stage:state.stage,startFrame:viewer.startFrame,startStage:viewer.startStage,difficulty:viewer.replay.difficulty,paused:state.paused,rate:viewer.rate,seeking:!!state.seeking,target:viewer.target,preparing:state.scene==='loading',ended:state.scene!=='replay-playing'&&state.scene!=='loading',description:viewer.description}});viewer.reportedAt=now;}
  if(now-syncedAt>5000){syncedAt=now;syncRuntime().catch(fail);}
}
function paths(FS,directory){
  return FS.readdir(directory).filter(name=>name!=='.'&&name!=='..').flatMap(name=>{const path=directory+'/'+name;return FS.isDir(FS.stat(path).mode)?paths(FS,path):[path];});
}
function mountSave(path,bytes){if(!core)return;core.FS.mkdirTree(path.slice(0,path.lastIndexOf('/')));core.FS.writeFile(path,bytes);}
async function loadSaves(){oldFiles=new Map();for(const path of await store.list()){const bytes=await store.read(path);if(bytes){oldFiles.set(path,copy(bytes));mountSave(path,bytes);}}}
function syncRuntime(){
  syncQueue=syncQueue.catch(()=>{}).then(async()=>{
    if(!core||!nativeStarted){await store?.sync();return;}
    if(!terminated)core._th12_host_flush();const current=new Set();
    for(const path of paths(core.FS,'/savesth12')){
      try{allowedPath(path);}catch{continue;}
      current.add(path);const bytes=core.FS.readFile(path);
      if(!equal(oldFiles.get(path),bytes)){await store.write(path,bytes);oldFiles.set(path,bytes.slice());}
    }
    for(const path of oldFiles.keys())if(!current.has(path)){await store.remove(path);oldFiles.delete(path);}
    await store.sync();
  });return syncQueue;
}
function practicePointer(message){
  if(!core)return false;
  const type=({down:0,move:1,up:2,cancel:3})[message.type]??3,id=message.id??0;
  if(type===0)pendingPracticePointers.delete(id);
  const route=core._th12_host_pointer(type,id,message.x,message.y,Number(message.pointerType==='mouse'));
  if(route===2&&type===0)pendingPracticePointers.set(id,{...message});
  if(route===3){const down=pendingPracticePointers.get(id);pendingPracticePointers.delete(id);if(down)touch.pointer(down,state);}
  if(type>=2)pendingPracticePointers.delete(id);
  return route===1||route===2;
}
function pointer(message){if(!practicePointer(message))touch.pointer(message,state);}
function hostedPointer(message,normalized){return sdlPointerPosition(message,canvas.getBoundingClientRect(),{width:innerWidth,height:innerHeight},normalized);}

async function configure(settings){
  const installed=new Map(),pack=settings.runtimePack;let total=0;
  if(pack){
    if(!Array.isArray(pack.files)||pack.files.length>256)throw Error('语言包安装列表无效');
    for(const file of pack.files){
      if(!file||!isLocalizationResourcePath(file.path)||installed.has(file.path)||!ArrayBuffer.isView(file.bytes))throw Error('语言包内容无效');
      const bytes=copy(file.bytes);total+=bytes.length;if(!bytes.length||total>96*1024*1024)throw Error('语言包安装大小无效');installed.set(file.path,bytes);
    }
  }
  if(settings.language!=='ja'&&!assets.fonts.has('/unifont.otf'))throw Error('语言字体尚未安装');
  localization?.deactivate();localization=(await prepareLocalization({language:settings.language,manifest:pack?.manifest,readFile:path=>installed.get(path),fontReady:Promise.resolve()})).activate();
  assets.localization=localization;options=settings.options;music=settings.music;touch.configure(options);
}
async function playback(message){
  if(!core||!launched)throw Error('请先启动游戏');
  if(['inspect','open'].includes(message.action)){
    const bytes=await store.read(message.path);if(!bytes)throw Error('录像不存在');const replay=parseReplay(bytes);
    const length=part=>{const last=replay.input(part.number,part.frames-1);return part.frames-Number(last?.held===65535&&last.pressed===65535&&last.released===65535);};
    const description={name:replay.name,character:replay.character,shot:replay.shot,difficulty:replay.difficulty,timestamp:replay.timestamp,score:replay.scoreStored*10+replay.continues,slowdown:replay.slowdown,lastStage:replay.finalStage,mode:replay.practice?'practice':'story',stages:replay.stages.map((s,i)=>({id:s.number,label:s.number===7?'Extra':'Stage '+s.number,score:(replay.stages[i+1]?.initial.score??replay.scoreStored)*10,frames:length(s)}))};
    if(message.action==='inspect')return {description};
    mountSave(message.path,bytes);const relative=message.path.slice('/savesth12/replay/'.length);
    const stage=message.stage??replay.stages[0].number;if(!replay.stages.some(s=>s.number===stage))throw Error('录像没有此关卡');
    let total=0;const parts=replay.stages.map(s=>{const p={stage:s.number,start:total,frames:length(s)};total+=p.frames;return p;});
    viewer={path:message.path.slice('/savesth12/'.length),relative,replay,description,parts,total,startStage:stage,startFrame:parts.find(p=>p.stage===stage).start,rate:1,target:null,reportedAt:-Infinity};
    if(!withString(relative,p=>core._th12_host_playback(0,p,stage)))throw Error('录像打开失败');
  }else{
    if(!viewer)throw Error('请先打开录像');
    if(message.action==='seek'){
      if(message.value<viewer.startFrame||message.value>viewer.total)throw Error('录像位置无效');
      const part=viewer.parts.find(p=>message.value>=p.start&&message.value<p.start+p.frames)??viewer.parts.at(-1);
      // The application queues stage loading; seek is installed atomically with it.
      if(!withString(viewer.relative,p=>core._th12_host_playback(5,p,part.stage)))throw Error('录像关卡定位失败');
      core._th12_host_playback(6,0,Math.min(part.frames,message.value-part.start));viewer.target=message.value;return {};
    }
    const action=({pause:1,play:2,seek:3,rate:4})[message.action],value=message.action==='rate'?Math.round(message.value*1000):message.value??0;
    if(!core._th12_host_playback(action,0,value))throw Error('录像操作未完成');
    if(message.action==='rate')viewer.rate=message.value;
  }
  return {};
}
async function launch(){
  if(terminated)throw Error('请从启动器重新打开此会话');if(launched)throw Error('游戏已启动');launched=true;
  try{
    localization??=(await prepareLocalization()).activate();assets.localization=localization;
    core=await createTH12SDL({canvas,noInitialRun:true,printErr:line=>{startupLog.push(String(line));if(startupLog.length>40)startupLog.shift();console.info(line);},onAbort:reason=>{terminated=true;fail(Error(String(reason)));},th12BeforeFrame:()=>{try{beforeFrame();}catch(error){fail(error);}},th12Frame:()=>{try{frame();}catch(error){fail(error);}},th12Started:()=>{nativeStarted=true;},th12Error:message=>{lastFailure=message;},th12Stopped:(code=0,reason='',details)=>{terminated=true;launched=false;const error=sdlStopError(code,reason,details,{started:nativeStarted,lastFailure,logs:startupLog});if(error){fail(error);return;}syncRuntime().then(()=>emit({event:'exit',code:0,status:'success'})).catch(fail);}});
    core.eaglerOptions=options;window.Module=core;
    globalThis.__eaglerCrashReporterV1?.bindCore(core,{context:()=>({session:state.stage?state:null,host:state,heldKeys:[...keys.keys()]})});
    await mountSdlContent(core,assets,{progress:()=>{notice.textContent='少女祈祷中...';}});
    const practiceLocale=options.thpracLocale??(localization.language==='ja'?'ja-JP':localization.language.startsWith('lang_zh')?'zh-CN':'en-US');
    await loadSaves();const locale=practiceLocale==='ja-JP'?2:practiceLocale==='en-US'?1:0;
    core._th12_host_options(Number(options.limitPresentationTo60!==false),Number(!!options.alwaysHitbox),Number(options.thpracEnabled!==false),locale,Number(music!=='none'));
    core._th12_host_music_mode(Number(options.oggDecodeMode==='full'));
    const exitCode=core.callMain(['--data-dir','/game','--save-dir','/savesth12']);
    if(terminated||exitCode)throw sdlStopError(exitCode??1,lastFailure,{source:'call-main'},{started:false,logs:startupLog});
    window.__th12Runtime={core,state:()=>JSON.parse(stringAt(core._th12_host_state()))};
    canvas.focus();
    for(const n of new Set(keys.values()))core._th12_host_key(n,1);
    if(options.replayPath)await playback({action:'open',path:options.replayPath.startsWith('/savesth12/')?options.replayPath:'/savesth12/'+options.replayPath});
    emit({event:'runtime-info',renderer:'SDL3 / GLES 3',architecture:'shared C++20 application / WebAssembly',integration:'sdl3-gles',completeAdapter:true});
  }catch(error){launched=false;if(core)terminated=true;throw error;}
}
async function bootstrap(){
  store=await openStore();
  if(assets.managed){
    document.documentElement.dataset.managed='true';
    const boundaryStore={list:()=>store.list(),read:path=>store.read(path),sync:()=>store.sync(),write:async(path,bytes)=>{await store.write(path,bytes);mountSave(path,bytes);oldFiles.set(path,copy(bytes));},remove:async path=>{await store.remove(path);if(core?.FS.analyzePath(path).exists)core.FS.unlink(path);oldFiles.delete(path);}};
    emit=installManagedShell({store:boundaryStore,running:()=>launched,replayViewer:()=>!!options.replayViewer,configure,resources:installResources,launch,playback,
      key:(code,down,message)=>{if(!input.key(message,down,message.inputSource==='physical'?'physical':'virtual',key))key(keyboardCode(message),down);},
      clearKeyboard,cancelTouch,touch:message=>{touch.controls(message,state);core?._th12_host_mobile(Number(!!message.thpracMobilePanelsEnabled),Number(!!message.thpracMobilePanelsVisible));},pointer:message=>pointer(hostedPointer(message,true)),practicePointer:message=>practicePointer(hostedPointer({...message,pointerType:message.pointerType??'mouse'},false)),sync:syncRuntime},window,{announceReady:false});
    await assets.prepare();emit.ready();
  }else{
    notice.textContent='按“开始”进入 SDL3 版本';
    document.querySelector('#start').addEventListener('click',()=>launch().catch(fail),{once:true});
  }
}
for(const type of ['keydown','keyup'])window.addEventListener(type,event=>{
  if(type==='keydown'&&(event.altKey||event.metaKey))return;
  if(!input.key(event,type==='keydown','physical',key))key(keyboardCode(event),type==='keydown','physical');
  if(vk(keyboardCode(event)))event.preventDefault();
});
window.addEventListener('blur',()=>{clearKeyboard();cancelTouch();});
document.addEventListener('visibilitychange',()=>{clearKeyboard();cancelTouch();core?._th12_host_audio(Number(document.hidden));if(document.hidden)syncRuntime().catch(fail);});
window.addEventListener('pagehide',()=>{syncRuntime().catch(fail);});
for(const type of ['pointerdown','pointermove','pointerup','pointercancel'])canvas.addEventListener(type,event=>{
  const rect=canvas.getBoundingClientRect(),message={type:({pointerdown:'down',pointermove:'move',pointerup:'up',pointercancel:'cancel'})[type],id:event.pointerId,pointerType:event.pointerType,x:(event.clientX-rect.left)*640/rect.width,y:(event.clientY-rect.top)*480/rect.height};
  if(type==='pointerdown')canvas.setPointerCapture(event.pointerId);
  if(event.pointerType==='mouse')practicePointer(message);else pointer(message);
  event.preventDefault();
});
canvas.addEventListener('wheel',event=>{const rect=canvas.getBoundingClientRect();core?._th12_host_wheel((event.clientX-rect.left)*640/rect.width,(event.clientY-rect.top)*480/rect.height,-event.deltaX/100,-event.deltaY/100);event.preventDefault();},{passive:false});
document.querySelector('#fullscreen').addEventListener('click',()=>canvas.requestFullscreen().catch(fail));
bootstrap().catch(fail);
