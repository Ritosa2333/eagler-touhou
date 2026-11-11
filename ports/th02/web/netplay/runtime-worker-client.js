import {keyboardBits,isBound,isFormTarget,useControlSnapshot,controlsSummary} from '../control-settings.js';
import {LocalSfx} from './local-sfx.js';
import {LocalBgm} from './local-bgm.js';
const PROTOCOL='th02-rollback/6',canvas=document.getElementById('canvas');
let bitmapContext;try{bitmapContext=canvas.getContext('bitmaprenderer',{alpha:false});}catch{bitmapContext=null;}
const ctx=bitmapContext||canvas.getContext('2d',{alpha:false});
if(!ctx)throw Error('浏览器无法创建游戏画面');
if(!bitmapContext)ctx.imageSmoothingEnabled=false;
let worker,serial=0,stopped=false,settings,configuration=[0,0,2],volume=.7,blocked=false,listener=()=>{},keys=new Set();
let context,sfx,bgm,musicTask,musicError='',playedName='',pendingMusic=[],cache={},check=null,queuedFrame=null,preparedFrame=null,raf=0,presentPending=false;
let raw=false,points={focus:true,always:false},lastPoints='',acknowledgedPrefix=0,sentPrefix=0,stagedMode=false,preparedReady=false,bootPromise;
const requests=new Map();
const rpcMetrics={renderer:bitmapContext?'bitmaprenderer':'2d',roundTripMs:0,commands:0,workerExecutionMs:0,loadRoundTripMs:0,loadExecutionMs:0,displayMs:0,displayed:0,replaced:0,expired:0,hiddenDiscarded:0,queuedConfirmations:0,presentationRpcs:0};
function request(type,args=[],extra={}){
 if(stopped)return Promise.reject(Error('TH02 worker 已停止'));
 const id=++serial;return new Promise((resolve,reject)=>{requests.set(id,{resolve,reject,type,started:performance.now()});worker.postMessage({type,args,id,...extra});});
}
function fatal(message){if(stopped)return;window.parent.postMessage({protocol:PROTOCOL,event:'boot-error',message},location.origin);sync.stop();}
function fire(type,args=[]){if(worker)void request(type,args).catch(e=>{if(!stopped)fatal(e.message);});}
function clearKeys(){keys.clear();listener(0);}
function releaseFrame(frame){frame.bitmap.close();worker?.postMessage({type:'frame-ack',sequence:frame.sequence});}
function discardPrepared(){preparedReady=false;if(preparedFrame){releaseFrame(preparedFrame);preparedFrame=null;}}
function discardFrames(){preparedReady=false;if(preparedFrame){releaseFrame(preparedFrame);preparedFrame=null;}if(queuedFrame){releaseFrame(queuedFrame);queuedFrame=null;}if(raf){cancelAnimationFrame(raf);raf=0;}}
function queueForDisplay(frame){if(document.hidden){releaseFrame(frame);rpcMetrics.hiddenDiscarded++;return;}if(queuedFrame){releaseFrame(queuedFrame);rpcMetrics.replaced++;}queuedFrame=frame;if(!raf)raf=requestAnimationFrame(draw);}
function draw(){
 raf=0;if(!queuedFrame||stopped)return;const frame=queuedFrame;queuedFrame=null;
 const hidden=document.hidden,expired=Number.isFinite(frame.producedAt)&&performance.timeOrigin+performance.now()-frame.producedAt>50;
 if(hidden||expired){releaseFrame(frame);rpcMetrics[hidden?'hiddenDiscarded':'expired']++;return;}
 const at=performance.now();
 try{if(bitmapContext)bitmapContext.transferFromImageBitmap(frame.bitmap);else ctx.drawImage(frame.bitmap,0,0,640,400);rpcMetrics.displayMs+=performance.now()-at;rpcMetrics.displayed++;}
 catch(error){fatal(error.message);}
 finally{releaseFrame(frame);}
}
function music(event){
 if(event.name){if(playedName!==event.name){pendingMusic.push([0,event.name]);if(event.ax>>>8)pendingMusic.push([event.ax,event.name]);}else pendingMusic.push([event.ax,event.name]);playedName=event.name;}
 else if(playedName)pendingMusic.push([event.ax,playedName]);
 if(pendingMusic.length>64)pendingMusic.splice(0,pendingMusic.length-64);
 if(bgm?.manifest){for(const [ax,name]of pendingMusic.splice(0))bgm.commandTrack(ax,name);return;}
 if(!musicTask&&!musicError)musicTask=Promise.resolve().then(async()=>{bgm=new LocalBgm(context,()=>{});bgm.setVolume(volume);await bgm.load({warmOpening:false});if(stopped)return;for(const [ax,name]of pendingMusic.splice(0))bgm.commandTrack(ax,name);}).catch(e=>{musicError=e.message;});
}
async function load(config){
 settings=config;context=new AudioContext({sampleRate:44100,latencyHint:'interactive'});await context.resume().catch(()=>{});
 sfx=new LocalSfx(context);await sfx.load();
 if(stopped){sfx.dispose();throw Error('TH02 worker 已停止');}
 worker=new Worker(new URL('./runtime-worker.js',import.meta.url),{type:'module'});
 worker.onerror=e=>fatal(e.message||'TH02 worker 失败');
 worker.onmessage=({data})=>{
  if(data.type==='notify'){window.parent.postMessage(data.message,location.origin);return;}
  if(data.type==='music'){music(data.event);return;}
  if(data.type==='sfx'){if(!document.hidden)sfx.play(data.id);return;}
  if(data.type==='frame'){if(stopped){releaseFrame(data);return;}if(data.staged){if(preparedFrame)releaseFrame(preparedFrame);preparedFrame=data;}else queueForDisplay(data);return;}
  if(data.type==='confirmed-exit'){if(!stopped)window.th02Sync?.onConfirmedExit?.();return;}
  if(data.type==='error'){const pending=requests.get(data.id);requests.delete(data.id);pending?.reject(Error(data.message));fatal(data.message);return;}
  if(data.type!=='result')return;
  cache={...cache,...data.status};const pending=requests.get(data.id);requests.delete(data.id);
  if(pending){const elapsed=performance.now()-pending.started;if(pending.type==='load'){rpcMetrics.loadRoundTripMs+=elapsed;rpcMetrics.loadExecutionMs+=data.executionMs;}else{rpcMetrics.roundTripMs+=elapsed;rpcMetrics.workerExecutionMs+=data.executionMs;rpcMetrics.commands++;}}pending?.resolve(data.result);
 };
 const result=await request('load',[],{settings:config,configure:configuration});check=result.initial;
 await request('setRawPresentation',[raw]);await request('setPointPreferences',[points]);return result;
}
const sync={
 asyncExecution:true,
 configure(...value){configuration=value;},load:value=>bootPromise??=load(value),
 start(){stagedMode=true;fire('start');canvas.focus();},
 stop(){if(stopped)return;stopped=true;clearKeys();discardFrames();worker?.terminate();for(const p of requests.values())p.reject(Error('TH02 worker 已停止'));requests.clear();bgm?.dispose();sfx?.dispose();context?.close().catch(()=>{});},
 async execute(inputs,{frame,replay=false,offlineMask=0,capture=false,hash=false}){if(!replay){stagedMode=true;discardPrepared();}const result=await request('execute',[],{inputs,frame,replay,offlineMask,capture,hash,stagePresentation:!replay});if(!replay)preparedReady=true;if(hash)check=result;return result;},
 async replaySlice(frame,plans){
  stagedMode=true;discardPrepared();const result=await request('replay-slice',[],{frame,plans,stagePresentation:true});
  if(result.applied===plans.length&&plans.at(-1)?.end)preparedReady=true;
  for(const record of result.records||[])if(record.hash)check=record.hash;
  return result;
 },
 capture:frame=>request('capture',[frame]),step:(inputs,options)=>{stagedMode=false;discardPrepared();return request('step',[inputs,options]);},restore:async frame=>{discardFrames();return request('restore',[frame]);},
 async confirm(prefix){if(prefix<=acknowledgedPrefix)return;const result=await request('confirm',[prefix]);acknowledgedPrefix=Math.max(acknowledgedPrefix,prefix);sentPrefix=Math.max(sentPrefix,prefix);return result;},
 confirmQueued(prefix,onExit){if(prefix<=sentPrefix||stopped||!worker)return false;sentPrefix=prefix;worker.postMessage({type:'confirm',args:[prefix],oneWay:true});if(onExit)sync.onConfirmedExit=onExit;rpcMetrics.queuedConfirmations++;return true;},
 replayProof:()=>cache.proof,checksum:()=>check,snapshot:()=>cache.game??null,
 present(){if(stopped)return false;if(stagedMode){if(!preparedReady)return false;preparedReady=false;if(!preparedFrame)return false;const frame=preparedFrame;preparedFrame=null;queueForDisplay(frame);return true;}if(presentPending)return false;rpcMetrics.presentationRpcs++;presentPending=true;request('present').catch(e=>{if(!stopped)fatal(e.message);}).finally(()=>{presentPending=false;});return true;},
 onInput(callback){listener=callback;},onMenuAction(){},
 setMusicVolume(value){volume=Math.max(0,Math.min(1,Number(value)||0));bgm?.setVolume(volume);},
 updateControls(value,editing=false){clearKeys();useControlSnapshot(value);blocked=editing;document.getElementById('controls-summary').textContent=controlsSummary('online');},
 setPointPreferences(value){points={...value};const next=JSON.stringify(value);if(next!==lastPoints){lastPoints=next;fire('setPointPreferences',[value]);}},
 setRawPresentation(value){raw=!!value;fire('setRawPresentation',[raw]);},
 async unlockAudio(){if(context?.state!=='running')await context?.resume();return context?.state==='running';},
 touchContext:()=>cache.touch??{key:'boot',play:false},readyForRollback:()=>!!cache.ready,canPredict:()=>cache.reason==='',predictionBlockReason:()=>cache.reason??'原版菜单或关卡加载',
 rollbackInfo:()=>cache.rollback??{},renderInfo:()=>({...cache.render,worker:true}),
 performanceInfo:()=>({worker:{...cache.performance},page:{...rpcMetrics},slowSteps:cache.slowSteps?.slice()??[],scope:'cumulative wall-clock observations; nested native/step/restore costs overlap; RPC round trip includes queue/transport/page scheduling; not physical input latency'}),
 diagnosticText:()=>`${(cache.diagnostics||'TH02 启动中').split('\n').slice(0,2).join('\n')}\nWorker 独立模拟 · 核心固定 30000 cycles\n浏览器 BGM：${musicError||bgm?.status||'等待确认关卡'} · 本地完整音效 ${sfx?.info().played??0} 次`,
 faultReport:()=>({...cache.fault,game:cache.game,settings,worker:true,performance:sync.performanceInfo(),music:{...cache.fault?.music,error:musicError,status:bgm?.status,track:bgm?.active?.id},sound:{...cache.fault?.sound,...sfx?.info()}})
};
window.addEventListener('keydown',e=>{if(blocked||isFormTarget(e.target)||e.metaKey||e.ctrlKey||e.altKey||!isBound(e.code))return;e.preventDefault();e.stopImmediatePropagation();keys.add(e.code);listener(keyboardBits(keys));},true);
window.addEventListener('keyup',e=>{if(keys.delete(e.code)){e.preventDefault();e.stopImmediatePropagation();listener(keyboardBits(keys));}},true);
window.addEventListener('blur',clearKeys);window.addEventListener('pagehide',()=>sync.stop());
document.addEventListener('visibilitychange',()=>{if(document.hidden){clearKeys();sfx?.stop();discardFrames();}});
canvas.addEventListener('contextlost',()=>fatal('游戏画面上下文已丢失，请重新开始'));
canvas.addEventListener('pointerdown',()=>canvas.focus());document.getElementById('sound').onclick=()=>sync.unlockAudio();
window.th02Sync=sync;
