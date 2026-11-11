import {keyboardBits,isBound} from './controls.js';
import {isFormTarget,useControlSnapshot,controlsSummary} from '../control-settings.js';
import {LocalSe} from './local-se.js';
import {LocalBgm} from './local-bgm.js';
const PROTOCOL='th04-rollback/4',canvas=document.getElementById('canvas');
let bitmapContext;try{bitmapContext=canvas.getContext('bitmaprenderer',{alpha:false});}catch{bitmapContext=null;}
const ctx=bitmapContext||canvas.getContext('2d',{alpha:false});
if(!ctx)throw Error('浏览器无法创建游戏画面');
if(!bitmapContext)ctx.imageSmoothingEnabled=false;
let worker,serial=0,stopped=false,settings,configuration=[0,0,2],volume=.7,blocked=false,listener=()=>{},keys=new Set();
let context,se,bgm,musicTask,musicError='',seError='',pendingMusic=[],cache={},check=null,queuedFrame=null,preparedFrame=null,raf=0,presentPending=false;
let raw=false,points={focus:true,always:false},lastPoints='',acknowledgedPrefix=0,sentPrefix=0,stagedMode=false,preparedReady=false,bootPromise,lastFaultCpu=null;
let drawn=0,replaced=0,lastDraw=0,intervals=[],executionMs=0,stoppedMusic=null;
const rpcMetrics={renderer:bitmapContext?'bitmaprenderer':'2d',displayMs:0,expired:0,hiddenDiscarded:0,queuedConfirmations:0,presentationRpcs:0};
const requests=new Map();
function request(type,args=[],extra={}){
 if(stopped||!worker)return Promise.reject(Error('TH04 Worker 未运行'));
 const id=++serial;return new Promise((resolve,reject)=>{
  requests.set(id,{resolve,reject});try{worker.postMessage({type,args,id,...extra});}catch(e){requests.delete(id);reject(e);}
 });
}
function notifyRoom(message){if(typeof globalThis.th04RuntimeNotify==='function')globalThis.th04RuntimeNotify(message);}
function fatal(message){if(stopped)return;notifyRoom({protocol:PROTOCOL,event:'runtime-error',message});sync.stop();}
function fire(type,args=[]){if(worker)void request(type,args).catch(e=>{if(!stopped)fatal(e.message);});}
function clearKeys(){keys.clear();listener(0);}
function releaseFrame(frame){frame.bitmap.close();worker?.postMessage({type:'frame-ack',sequence:frame.sequence});}
function discardPrepared(){preparedReady=false;if(preparedFrame){releaseFrame(preparedFrame);preparedFrame=null;}}
function discardFrames(){preparedReady=false;if(preparedFrame){releaseFrame(preparedFrame);preparedFrame=null;}if(queuedFrame){releaseFrame(queuedFrame);queuedFrame=null;}if(raf){cancelAnimationFrame(raf);raf=0;}}
function queueForDisplay(frame){if(document.hidden){releaseFrame(frame);rpcMetrics.hiddenDiscarded++;return;}if(queuedFrame){releaseFrame(queuedFrame);replaced++;}queuedFrame=frame;if(!raf)raf=requestAnimationFrame(draw);}
function draw(){
 raf=0;if(!queuedFrame||stopped)return;
 const frame=queuedFrame;queuedFrame=null;
 const hidden=document.hidden,expired=Number.isFinite(frame.producedAt)&&performance.timeOrigin+performance.now()-frame.producedAt>50;
 if(hidden||expired){releaseFrame(frame);rpcMetrics[hidden?'hiddenDiscarded':'expired']++;return;}
 const began=performance.now();
 try{if(bitmapContext)bitmapContext.transferFromImageBitmap(frame.bitmap);else ctx.drawImage(frame.bitmap,0,0,640,400);drawn++;const now=performance.now();rpcMetrics.displayMs+=now-began;if(lastDraw)intervals.push(now-lastDraw);lastDraw=now;if(intervals.length>120)intervals.shift();}
 catch(error){fatal(error.message);}
 finally{releaseFrame(frame);}
}
function musicInfo(){return {...cache.fault?.music,error:musicError||cache.fault?.music?.error,status:bgm?.status,track:bgm?.active?.id,requested:bgm?.requested?.id};}
function music(event){
 pendingMusic.push([event.ax,event.name]);if(pendingMusic.length>64)pendingMusic.splice(0,pendingMusic.length-64);
 if(bgm?.manifest){for(const [ax,name]of pendingMusic.splice(0))bgm.commandTrack(ax,name);return;}
 if(!musicTask&&!musicError)musicTask=Promise.resolve().then(async()=>{
  if(stopped)return;bgm=new LocalBgm(context,()=>{});bgm.setVolume(volume);
  await bgm.load({...settings,warmOpening:false});if(stopped)return;
  for(const [ax,name]of pendingMusic.splice(0))bgm.commandTrack(ax,name);
 }).catch(e=>{if(!stopped)musicError=e.message;});
}
async function load(config){
 settings=config;context=new AudioContext({sampleRate:44100,latencyHint:'interactive'});await context.resume().catch(()=>{});
 if(context.sampleRate!==44100)throw Error('此浏览器未提供统一的 44100 Hz 音频');
 se=new LocalSe(context);await se.load().catch(e=>{seError='浏览器音效加载失败：'+e.message;});
 if(stopped){se.dispose();throw Error('TH04 Worker 已停止');}
 worker=new Worker(new URL('./runtime-worker.js',import.meta.url),{type:'module'});
 worker.onerror=e=>fatal(e.message||'TH04 Worker 失败');
 worker.onmessageerror=()=>fatal('TH04 Worker 消息读取失败');
 worker.onmessage=({data})=>{
  if(data.type==='frame'&&stopped){releaseFrame(data);return;}
  if(stopped)return;
  if(data.type==='confirmed-exit'){sync.onConfirmedExit?.();return;}
  if(data.type==='notify'){
   if(data.message.fault){cache.fault=data.message.fault;lastFaultCpu=data.message.fault.cpu;}
   notifyRoom(data.message);return;
  }
  if(data.type==='music'){music(data.event);return;}
  if(data.type==='se'){
   if(!seError)try{se.confirm(data.event);}catch(e){seError='浏览器音效错误：'+e.message;se.stop();}return;
  }
  if(data.type==='se-stop'){se.stop();return;}
  if(data.type==='frame'){if(data.staged){if(preparedFrame)releaseFrame(preparedFrame);preparedFrame=data;}else queueForDisplay(data);return;}
  if(data.type==='error'){
   if(data.fault){cache.fault=data.fault;lastFaultCpu=data.fault.cpu;}
   const pending=requests.get(data.id);requests.delete(data.id);pending?.reject(Error(data.message));fatal(data.message);return;
  }
  if(data.type!=='result')return;
  Object.assign(cache,data.status);executionMs=data.executionMs;const pending=requests.get(data.id);requests.delete(data.id);pending?.resolve(data.result);
 };
 const result=await request('load',[],{settings:config,configure:configuration});
 await request('setRawPresentation',[raw]);await request('setPointPreferences',[points]);return result;
}
const sync={
 asyncExecution:true,
 configure(...value){configuration=value;},load:value=>bootPromise??=load(value),
 start(){stagedMode=true;fire('start');canvas.focus();},
 stop(){
  if(stopped)return;stoppedMusic=musicInfo();stopped=true;clearKeys();discardFrames();
  worker?.terminate();for(const p of requests.values())p.reject(Error('TH04 Worker 已停止'));requests.clear();
  bgm?.dispose();se?.dispose();context?.close().catch(()=>{});
 },
 async execute(inputs,{frame,replay=false,offlineMask=0,capture=false,hash=false}){
  if(!replay){stagedMode=true;discardPrepared();}const result=await request('execute',[],{inputs,frame,replay,offlineMask,capture,hash,stagePresentation:!replay});if(!replay)preparedReady=true;if(hash)check=result;return result;
 },
 capture:frame=>request('capture',[frame]),step:(inputs,options)=>{stagedMode=false;discardPrepared();return request('step',[inputs,options]);},restore:async frame=>{discardFrames();return request('restore',[frame]);},
 async replaySlice(frame,plans){
  stagedMode=true;discardPrepared();const result=await request('replay-slice',[],{frame,plans,stagePresentation:true});
  if(result.applied===plans.length&&plans.at(-1)?.end)preparedReady=true;
  for(const record of result.records||[])if(record.hash)check=record.hash;
  return result;
 },
 async confirm(prefix){if(prefix<=acknowledgedPrefix)return;const result=await request('confirm',[prefix]);acknowledgedPrefix=Math.max(acknowledgedPrefix,prefix);sentPrefix=Math.max(sentPrefix,prefix);return result;},
 confirmQueued(prefix,onExit){if(prefix<=sentPrefix||stopped||!worker)return false;sentPrefix=prefix;worker.postMessage({type:'confirm',args:[prefix],oneWay:true});if(onExit)sync.onConfirmedExit=onExit;rpcMetrics.queuedConfirmations++;return true;},
 replayProof:()=>cache.proof,checksum:()=>check,snapshot:()=>cache.game??null,
 present(){if(stopped||!worker)return false;if(stagedMode){if(!preparedReady)return false;preparedReady=false;if(!preparedFrame)return false;const frame=preparedFrame;preparedFrame=null;queueForDisplay(frame);return true;}if(presentPending)return false;rpcMetrics.presentationRpcs++;presentPending=true;request('present').catch(e=>{if(!stopped)fatal(e.message);}).finally(()=>{presentPending=false;});return true;},
 onInput(callback){listener=callback;},onMenuAction(){},
 setMusicVolume(value){volume=Math.max(0,Math.min(1,Number(value)/100||0));bgm?.setVolume(volume);},
 updateControls(value,editing=false){clearKeys();useControlSnapshot(value);blocked=editing;document.getElementById('controls-summary').textContent=controlsSummary('online');},
 setPointPreferences(value){points={...value};const next=JSON.stringify(value);if(next!==lastPoints){lastPoints=next;fire('setPointPreferences',[value]);}},
 setRawPresentation(value){raw=!!value;fire('setRawPresentation',[raw]);},
 async unlockAudio(){if(context?.state!=='running')await context?.resume();return context?.state==='running';},
 touchContext:()=>cache.touch??{key:'boot',play:false},readyForRollback:()=>!!cache.ready,
 canPredict:()=>cache.reason==='',predictionBlockReason:()=>cache.reason??'游戏尚未就绪',
 rollbackInfo:()=>cache.rollback??{},
 renderInfo:()=>({...cache.render,worker:true,pageDrawn:drawn,replaced,pending:!!queuedFrame,lastExecutionMs:executionMs,pageIntervals:intervals.slice(),pageRpc:{...rpcMetrics}}),
 soundInfo:()=>({...se?.info(),error:seError}),
 diagnosticText:()=>`${(cache.diagnostics||'TH04 启动中').split('\n').slice(0,2).join('\n')}\nWorker 独立模拟 · 核心固定 30000 cycles\n${seError||se?.status||'浏览器音效：等待'}\n浏览器音乐：${musicError||bgm?.status||'等待确认关卡'}`,
 faultReport:()=>({...cache.fault,cpu:lastFaultCpu,settings,worker:true,render:sync.renderInfo(),sound:sync.soundInfo(),music:stoppedMusic??musicInfo()}),
 async collectFaultReport(){
  if(stopped)return sync.faultReport();
  const fault=await request('faultReport');lastFaultCpu=fault.cpu;
  return {...fault,...sync.faultReport(),cpu:fault.cpu};
 }
};
window.addEventListener('keydown',e=>{if(blocked||isFormTarget(e.target)||e.metaKey||!isBound(e.code))return;e.preventDefault();e.stopImmediatePropagation();keys.add(e.code);listener(keyboardBits(keys));},true);
window.addEventListener('keyup',e=>{if(keys.delete(e.code)){e.preventDefault();e.stopImmediatePropagation();listener(keyboardBits(keys));}},true);
window.addEventListener('blur',clearKeys);window.addEventListener('pagehide',()=>sync.stop());
document.addEventListener('focusin',e=>{if(isFormTarget(e.target))clearKeys();});
document.addEventListener('visibilitychange',()=>{if(document.hidden){clearKeys();se?.stop();discardFrames();}});
canvas.addEventListener('contextlost',()=>fatal('游戏画面上下文已丢失，请重新开始'));
canvas.addEventListener('pointerdown',()=>canvas.focus());
document.getElementById('sound').onclick=()=>sync.unlockAudio();
document.getElementById('bgm-volume').addEventListener('input',e=>sync.setMusicVolume(e.target.value));
window.th04Sync=sync;
