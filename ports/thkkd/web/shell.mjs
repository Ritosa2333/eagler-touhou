import createModule from './thkkd.mjs';
import {Platform} from './platform.mjs';
import {Input} from './input.mjs';
import {createSaveSync} from './save-sync.mjs';
import {createDirectInputBridge} from './direct-input-bridge.mjs';
import {createLocalReplay} from './local-replay.mjs';
import {decodeReplay,encodeExtended} from './replay.mjs';
import {PresentationClock,ReplayPresentationClock} from './presentation-clock.mjs';
import {installDisplay} from './display.mjs';
const game='thkkd',protocol='eagler-touhou/1',query=new URLSearchParams(location.search),epoch=Number(query.get('runtimeEpoch'));
const canvas=document.querySelector('canvas'),loading=document.querySelector('#loading'),errorBox=document.querySelector('#error');
const saveRoot='/savesthkkd',identity={protocol,game,epoch,origin:location.origin};
let core,platform,configured=false,launched=false,stopping=false,first=false,options={},launchSerial,raf=0,lastTime=null,accumulator=0,lastDisplay=0,healthTime=0,logicFrames=0,displayFrames=0,queue=Promise.resolve(),savedQueue=Promise.resolve(),replayBridge,activeReplay,playbackState,replayHasStepped=false;
const display=installDisplay(canvas,()=>platform?.fitFPS());
const emit=(event,fields={})=>parent.postMessage({protocol,game,epoch,event,...fields},location.origin);
const input=new Input(identity,()=>launched&&!document.hidden&&!stopping);
input.isGameplay=()=>!!core&&launched&&core._thkkd_word(0xa8ce58)===100&&core._thkkd_word(0xa78e2c)===0&&core._thkkd_word(0xa73060)===0;
input.isDialogue=()=>!!core&&launched&&core._thkkd_word(0xa8ce58)===100&&core._thkkd_word(0xa78e2c)===0&&core._thkkd_word(0xa73060)!==0;
input.bindings=()=>Array.from({length:9},(_,index)=>core._thkkd_word(0xa733dc+index*4));
input.onRestart=()=>{if(core?._thkkd_restart()===1)input.resetRecording();};
const sync=createSaveSync(()=>core);
const presentationClock=new PresentationClock();
const replayPresentationClock=new ReplayPresentationClock();
const nativeError=()=>core.UTF8ToString(core._thkkd_error());
function fatal(error){launched=false;cancelAnimationFrame(raf);const message=error?.message||String(error);errorBox.textContent=message;emit('error',{error:message});void platform?.suspend();}
function path(value){
  if(typeof value!=='string')throw Error('用户文件路径无效');
  let name=value.replaceAll('\\','/');if(name.startsWith(saveRoot+'/'))name=name.slice(saveRoot.length+1);
  if(!/^(?:config\.txt|th_kaikeidou(?:01|_score00|_score10|_spell01|_spell10)\.dat|replay\/(?:[\p{L}\p{N} _().-]+\/)*[\p{L}\p{N} _().-]+\.(?:rpy|thkkdr))$/iu.test(name)||name.split('/').some(p=>p==='..'||p==='.'))throw Error('用户文件路径超出允许范围');return name;
}
function listFiles(){const files=[];const visit=dir=>{for(const name of core.FS.readdir(saveRoot+dir)){if(name==='.'||name==='..')continue;const full=saveRoot+dir+'/'+name,stat=core.FS.stat(full);if(core.FS.isDir(stat.mode))visit(dir+'/'+name);else{const relative=(dir+'/'+name).slice(1);try{path(relative);files.push({path:relative,size:stat.size,mtime:stat.mtime.getTime()});}catch{}}}};visit('');return files;}
async function stop(status='exited'){
  if(stopping)return;stopping=true;launched=false;cancelAnimationFrame(raf);replayBridge?.close();replayBridge=null;activeReplay=null;input.replaying=false;input.replayMotion=null;input.clear();core?._thkkd_stop();await savedQueue;await sync(false);platform.resetGame();await platform.suspend();emit('exit',{code:0,status});stopping=false;
}
function presented(time){
  display.check();
  if(!presentationClock.due(time,options.limitPresentationTo60))return;
  platform.renderer.present(replayPresentationClock.alpha(time,playbackState,options.limitPresentationTo60));displayFrames++;if(!first&&platform.presented){first=true;loading.textContent='';emit('first-frame',launchSerial===undefined?{}:{launchSerial});}
}
function health(time){if(time-healthTime>=2000){const elapsed=(time-healthTime)/1000;emit('frame-health',{fps:displayFrames/elapsed,logicFps:logicFrames/elapsed,frameMs:elapsed*1000/Math.max(1,displayFrames)});emit('audio-health',{backend:'Web Audio / HTML Audio',...platform.audioHealth(),robust:true});healthTime=time;logicFrames=0;displayFrames=0;}}
function replayFrame(time){if(!launched||stopping)return;try{if(!document.hidden&&replayHasStepped)presented(time);health(time);raf=requestAnimationFrame(replayFrame);}catch(error){fatal(error);}}
function getReplay(){
  if(replayBridge)return replayBridge;
  replayBridge=createLocalReplay({
    inspect:async name=>(await decodeReplay(core.FS.readFile(saveRoot+'/'+path(name)))).description,
    async open(name,stage){
      if(stage!==0)throw Error('海惠堂录像按原作完整输入流播放');
      activeReplay=await decodeReplay(core.FS.readFile(saveRoot+'/'+path(name)));input.clear();input.resetRecording();input.unlimitedUsed=activeReplay.description.slowdown===100;input.replaying=true;input.replayMotion=activeReplay.motion;
      core._thkkd_stop();platform.resetGame();if(core._thkkd_start()<0)throw Error(nativeError());
      const temporary='replay/.thkkd-playback.rpy';core.FS.writeFile(saveRoot+'/'+temporary,activeReplay.native);
      const nameBytes=new TextEncoder().encode(temporary+'\0');core.HEAPU8.set(nameBytes,core._thkkd_replay_path());
      try{if(core._thkkd_replay_open()<0)throw Error(nativeError());if(core._thkkd_word(0xb4310c)!==1||core._thkkd_word(0xb43a08)!==activeReplay.count)throw Error('原生录像输入缓冲加载失败');}finally{core.FS.unlink(saveRoot+'/'+temporary);}
      first=false;launched=true;replayHasStepped=false;replayPresentationClock.reset();healthTime=performance.now();logicFrames=0;displayFrames=0;raf=requestAnimationFrame(replayFrame);
    },
    step(){const result=core._thkkd_step();if(result<0)throw Error(nativeError());logicFrames++;replayHasStepped=true;replayPresentationClock.stepped(performance.now(),core._thkkd_word(0xb43110)>=activeReplay.count-1);platform.seekAudioTick();return result===0;},
    state(){const frame=core._thkkd_word(0xb43110),total=Math.max(0,(activeReplay?.count||1)-1);return {frame:Math.min(frame,total),total,ended:frame>=total,stage:0,startStage:0};},
    stopLoop(){cancelAnimationFrame(raf);lastTime=null;accumulator=0;presentationClock.reset();replayPresentationClock.reset();},
    async relocate(from,to){const a=core.FS.readFile(saveRoot+'/'+from),b=core.FS.readFile(saveRoot+'/'+to);if(a.length!==b.length||a.some((value,i)=>value!==b[i]))throw Error('目标录像与当前录像内容不一致');activeReplay.path=to;},
    audioState({paused,hidden,seeking,ended}){
      const stop=paused||hidden||ended;
      platform.playbackPaused=stop;
      platform.setSeeking(seeking);
      if(stop&&platform.context.state==='running')void platform.suspend();
      else if(!stop&&!seeking&&platform.context.state!=='running')void platform.resume();
      return seeking||stop||platform.context.state==='running';
    },
    onState(playback){playbackState=playback;replayPresentationClock.setPlayback(playback);emit('runtime-info',{playback});},onError:fatal,
  });return replayBridge;
}
function frame(time){
  if(!launched||stopping)return;
  try{
    if(document.hidden){lastTime=null;accumulator=0;raf=requestAnimationFrame(frame);return;}
    if(lastTime===null)lastTime=time;
    accumulator+=Math.min(250,time-lastTime);lastTime=time;
    let ticks=0;
    while(accumulator+1e-7>=1000/60&&ticks++<15){const result=core._thkkd_step();if(result<0)throw Error(nativeError());if(result===1){void stop();return;}accumulator=Math.max(0,accumulator-1000/60);logicFrames++;}
    if(presentationClock.due(time,options.limitPresentationTo60)){
      display.check();
      platform.renderer.present(options.limitPresentationTo60?0:accumulator/(1000/60));displayFrames++;lastDisplay=time;
      if(!first&&platform.presented){first=true;loading.textContent='';emit('first-frame',launchSerial===undefined?{}:{launchSerial});}
    }
    health(time);
    raf=requestAnimationFrame(frame);
  }catch(error){fatal(error);}
}
async function command(m){
  if(input.command(m))return {};
  switch(m.command){
  case 'configure':if(launched)throw Error('游戏已启动');if(!['ogg','none'].includes(m.music))throw Error('音乐模式无效');options={limitPresentationTo60:true,touchEnabled:false,...m.options};if(options.touchSensitivity!==undefined&&(!Number.isFinite(options.touchSensitivity)||options.touchSensitivity<100||options.touchSensitivity>300))throw Error('触控灵敏度无效');input.configure(options);platform.renderer.setSmoothing(!options.limitPresentationTo60);core._thkkd_set_high_refresh(options.limitPresentationTo60?0:1);platform.music=m.music==='ogg';configured=true;return {};
  case 'resources':if(!Array.isArray(m.resources))throw Error('资源列表无效');if(m.resources.length)throw Error('此版本资源由完整 DATA 包提供');return {};
  case 'launch':if(!configured)throw Error('请先配置游戏');if(launched)throw Error('游戏已经运行');input.clear();input.resetRecording();presentationClock.reset();if(core._thkkd_start()<0)throw Error(nativeError());await platform.resume();launchSerial=m.launchSerial;launched=true;first=false;lastTime=null;accumulator=1000/60;healthTime=performance.now();logicFrames=0;displayFrames=0;raf=requestAnimationFrame(frame);emit('runtime-info',{renderer:'WebGL2 / C++ WebAssembly',architecture:protocol,version:'thkkd-web-1'});return {};
  case 'list':return {files:listFiles()};
  case 'read':return {bytes:Array.from(core.FS.readFile(saveRoot+'/'+path(m.path)))};
  case 'write':{const name=path(m.path);if(!Array.isArray(m.bytes)&&!(m.bytes instanceof Uint8Array))throw Error('文件内容无效');if(m.bytes.length>64*1024*1024||Array.from(m.bytes).some(n=>!Number.isInteger(n)||n<0||n>255))throw Error('文件内容无效');const full=saveRoot+'/'+name;core.FS.mkdirTree(full.slice(0,full.lastIndexOf('/')));core.FS.writeFile(full,Uint8Array.from(m.bytes));await sync(false);return {};}
  case 'remove':core.FS.unlink(saveRoot+'/'+path(m.path));await sync(false);return {};
  case 'sync':await savedQueue;await sync(false);return {};
  case 'playback':return getReplay().command({...m,...(m.path!==undefined?{path:path(m.path)}:{}),...(m.target!==undefined?{target:path(m.target)}:{})});
  case 'retry-music':await platform.resume();return {};
  default:throw Error('不支持的 Runtime 命令：'+m.command);
  }
}
globalThis.__eaglerDirectInputBridge=createDirectInputBridge({...identity,ready:()=>launched&&!document.hidden&&!stopping,dispatch:m=>input.command(m)});
addEventListener('message',event=>{
  const m=event.data;if(event.source!==parent||event.origin!==location.origin||m?.protocol!==protocol||m.game!==game||m.epoch!==epoch||typeof m.command!=='string')return;
  const run=async()=>{try{const result=await command(m);if(m.request!==undefined)emitResponse(m,{ok:true,...result});}catch(error){if(m.request!==undefined)emitResponse(m,{ok:false,error:error.message});else fatal(error);}};
  queue=queue.then(run).catch(fatal);
});
function emitResponse(m,result){parent.postMessage({protocol,game,epoch,request:m.request,...result},location.origin);}
for(const [name,down]of [['keydown',true],['keyup',false]])addEventListener(name,event=>{if(input.keyboard.mapNative(event,down)||input.keyboard.event(event,down)){event.preventDefault();}if(down&&(!replayBridge?.active||!playbackState?.paused))void platform?.context.resume();});
const clear=()=>{input.clear();lastTime=null;accumulator=0;presentationClock.reset();replayPresentationClock.reset();};
addEventListener('blur',clear);
document.addEventListener('visibilitychange',()=>{clear();if(document.hidden)void platform?.suspend();else void platform?.resume();});
addEventListener('pagehide',()=>{clear();launched=false;cancelAnimationFrame(raf);display.dispose();void sync(false);void platform?.destroy();});
const initialized=(async()=>{
  if(!Number.isSafeInteger(epoch)||epoch<=0)throw Error('Runtime 会话代次无效');
  core=await createModule({print:()=>{},printErr:message=>console.error(message)});globalThis.Module=core;
  platform=new Platform(canvas,input,emit);core.thkkdPlatform=platform;core.eaglerSetAudioLevels=(music,effects)=>{platform.bgmGain.gain.value=Math.max(0,Math.min(1,music));platform.sfxGain.gain.value=Math.max(0,Math.min(1,effects));};
  platform.hitbox=()=>{
    const word=address=>core._thkkd_word(address);
    if(!options.alwaysHitbox||word(0xa8ce58)!==100||word(0xa8cd68)===1||word(0xa72208+word(0xa733f4)*4)>0)return null;
    const real=address=>{const bytes=new DataView(new ArrayBuffer(8));bytes.setUint32(0,word(address),true);bytes.setUint32(4,word(address+4),true);return bytes.getFloat64(0,true);};
    return {graph:word(0xa74f48),x:real(0xa8cdf8)+32+real(0xa7e600),y:real(0xa8ce00)+16+real(0xa7e608),life:word(0xa8cd7c)};
  };
  platform.onFileSaved=name=>{
    const convert=name.endsWith('.rpy')&&input.extended,native=convert?core.FS.readFile(name):null,motion=convert?new Map(input.recordedMotion):null,unlimited=input.unlimitedUsed;
    savedQueue=savedQueue.then(async()=>{if(convert){const bytes=await encodeExtended(native,motion,unlimited);core.FS.writeFile(name.replace(/\.rpy$/,'.thkkdr'),bytes);core.FS.unlink(name);}await sync(false);}).catch(fatal);
  };
  core.FS.mkdirTree(saveRoot);core.FS.mount(core.IDBFS,{},saveRoot);await sync(true);core.FS.mkdirTree(saveRoot+'/replay');
  let buffer;
  if(parent!==window&&typeof parent.__eaglerPrepareManagedRuntimeDataV1==='function')buffer=(await parent.__eaglerPrepareManagedRuntimeDataV1({game,generation:query.get('gameGeneration'),epoch})).buffer;
  else if(query.get('standalone')==='1'){const response=await fetch(query.get('data')||'./thkkd.data');if(!response.ok)throw Error('DATA 加载失败');buffer=await response.arrayBuffer();}
  else throw Error('请从 eagler-touhou 启动東方海恵堂');
  await platform.load(buffer,value=>loading.textContent=`正在准备東方海恵堂资源 ${Math.round(value*100)}%`);
  await document.fonts.load('16px "Thkkd Unifont"');await document.fonts.ready;
  globalThis.__thkkdRuntime={core,platform,input,command,stop,status:()=>({launched,first,logicFrames,displayFrames})};
  emit('ready',{saveRoot});
  if(query.get('standalone')==='1'){await command({command:'configure',music:'ogg',options:{}});await command({command:'launch'});}
})().catch(fatal);
