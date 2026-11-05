import {GameRuntime} from './runtime.js';
import {PROTOCOL} from './queue.js';
import {sha256} from './sha256.js';
import {stageSelection} from '../stage-select.js';
import {assetBytes,freshJSON} from '../preload-cache.js';

export const supportsSimulationWorker=()=>typeof Worker==='function'&&typeof OffscreenCanvas==='function'&&typeof OffscreenCanvas.prototype.transferToImageBitmap==='function';
export class WorkerGameRuntime extends GameRuntime{
  constructor(...args){
    super(...args);this.asyncExecution=true;this.serial=0;this.requests=new Map();this.urls=[];this.cache=null;this.simulationBackend='worker';
    this.performance={commands:0,roundTripMs:0,executionMs:0};this.audioState={end:0,start:0,cue:2,events:[]};
    this.audioCore={th_get:n=>n===11?this.audioState.end:n===79?this.audioState.cue:0,th_sound_event:i=>this.audioState.events[i-this.audioState.start]||0};
  }
  async load(settings,progress=()=>{}){
    this.settings={...settings};progress('加载并校验原生游戏与联机线程…');
    const version=globalThis.th01RoomPreload?.version??await freshJSON('netplay/runtime.json');
    if(version.protocol!==PROTOCOL||version.tickNumerator!==440000||version.tickDenominator!==24830)throw Error('联机版本不匹配，请刷新页面');
    const sources=new Map();
    await Promise.all(Object.entries(version.files).map(async([name,hash])=>sources.set(name,await assetBytes(name,{hash}))));
    for(const name of ['game.wasm','stage-select.js','netplay/runtime-core.js','netplay/simulation-worker.js','netplay/renderer.js'])if(!sources.has(name))throw Error('联机线程清单不完整，请刷新页面');
    if(this.stopped)throw Error('联机会话已关闭');
    const module=globalThis.th01RoomPreload?.module??await WebAssembly.compile(sources.get('game.wasm'));
    if(WebAssembly.Module.imports(module).length)throw Error('游戏核心含外部导入');
    if(this.stopped)throw Error('联机会话已关闭');
    const blob=source=>{const url=URL.createObjectURL(new Blob([source],{type:'text/javascript'}));this.urls.push(url);return url;};
    const decode=name=>new TextDecoder().decode(sources.get(name));
    const stageURL=blob(sources.get('stage-select.js'));
    const coreURL=blob(decode('netplay/runtime-core.js').replace("'../stage-select.js'",JSON.stringify(stageURL)));
    const workerURL=blob(decode('netplay/simulation-worker.js').replace("'./runtime-core.js'",JSON.stringify(coreURL)));
    const rendererURL=blob(sources.get('netplay/renderer.js'));
    this.worker=new Worker(workerURL,{type:'module',name:'TH01 network simulation'});
    this.worker.onmessage=({data})=>this.receive(data);
    this.worker.onerror=event=>this.failWorker(Error(event.message||'联机模拟线程失败'));
    this.worker.onmessageerror=()=>this.failWorker(Error('联机模拟线程通信失败'));
    try{this.apply(await this.call('init',{module,rendererURL,settings:this.settings,hostSlot:this.hostSlot}));}
    catch(error){this.stop();throw error;}
    this.renderBackend='worker';this.requestPresent();
    return sha256(new TextEncoder().encode(JSON.stringify({version,settings,hostSlot:this.hostSlot})));
  }
  call(type,extra={}){
    if(this.stopped)return Promise.reject(Error('联机模拟线程已停止'));
    const id=++this.serial;return new Promise((resolve,reject)=>{
      const timer=setTimeout(()=>this.failWorker(Error('联机模拟线程响应超时：'+type)),15000);
      this.requests.set(id,{resolve,reject,timer,started:performance.now(),type});
      try{this.worker.postMessage({type,id,...extra});}catch(error){this.failWorker(error);}
    });
  }
  receive(data){
    if(data.type==='frame'){
      if(this.stopped){data.bitmap.close();return;}
      if(data.frame<=(this.presentedFrame??-1)||this.queuedFrame&&data.frame<=this.queuedFrame.frame){this.releaseFrame(data);return;}
      if(this.queuedFrame)this.releaseFrame(this.queuedFrame);this.queuedFrame=data;
      if(!this.presentRequest)this.presentRequest=requestAnimationFrame(()=>this.draw());return;
    }
    if(this.stopped)return;
    if(data.type==='error'){this.failWorker(Error(data.message));return;}
    if(data.type!=='result')return;
    const pending=this.requests.get(data.id);if(!pending)return;
    clearTimeout(pending.timer);this.requests.delete(data.id);
    if(pending.type!=='init'){this.performance.commands++;this.performance.roundTripMs+=performance.now()-pending.started;this.performance.executionMs+=data.executionMs;}
    pending.resolve(data.result);
  }
  apply(result){
    this.cache=result;this.frame=result.frame;this.audioState=result.audio;this.audio.update(this.audioCore);
    if(result.scores&&!stageSelection(this.settings.startStage).practice)try{localStorage.setItem('th01-native-net-scores-v1',btoa(String.fromCharCode(...result.scores)));}catch{this.scoreSaveError=true;}
  }
  async execute(inputs,frame){const result=await this.call('execute',{inputs,frame});if(!this.stopped)this.apply(result);return result;}
  state(){return this.cache?.state??null;}
  checksum(){return this.cache?.checksum;}
  replayProof(){return JSON.stringify(this.checksum());}
  requestPresent(){
    if(this.stopped||!this.cache||document.hidden||this.presentPending||this.presentedFrame===this.frame)return;
    this.presentPending=true;this.call('present',{localSlot:this.localSlot}).catch(error=>{if(!this.stopped)this.failWorker(error);}).finally(()=>{this.presentPending=false;});
  }
  releaseFrame(frame){frame.bitmap.close();this.worker?.postMessage({type:'frame-ack',sequence:frame.sequence});}
  draw(){
    this.presentRequest=null;const frame=this.queuedFrame;this.queuedFrame=null;if(!frame)return;
    const at=performance.now();
    try{if(!this.stopped&&!document.hidden&&(this.presentedFrame===undefined||frame.frame>this.presentedFrame)){this.context?.drawImage(frame.bitmap,0,0);this.presentedFrame=frame.frame;this.displayFrames++;this.renderLatency=frame.latency;this.recordRenderCost(frame.cost,performance.now()-at);}}
    catch(error){this.failWorker(error);}finally{this.releaseFrame(frame);}
  }
  failWorker(error){if(this.stopped)return;this.stop();this.onFailure?.(error);}
  stop(){
    if(this.stopped)return;super.stop();
    if(this.queuedFrame){this.releaseFrame(this.queuedFrame);this.queuedFrame=null;}
    this.worker?.terminate();for(const pending of this.requests.values()){clearTimeout(pending.timer);pending.reject(Error('联机模拟线程已停止'));}this.requests.clear();
    for(const url of this.urls)URL.revokeObjectURL(url);this.urls=[];
  }
}
