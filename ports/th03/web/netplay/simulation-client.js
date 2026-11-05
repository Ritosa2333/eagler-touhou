export function supportsNetworkWorker(){
  return typeof Worker==='function'&&typeof OffscreenCanvas==='function'&&typeof OffscreenCanvas.prototype.transferToImageBitmap==='function';
}
export class WorkerNetworkSimulation {
  static async create(config,bridge){
    const client=new WorkerNetworkSimulation(config);
    try{
      const {canvas,onError,onExit,onDiskChange,audioContext,...plain}=config;
      const result=await client.call('init',[{config:plain,bridge,sampleRate:client.context.sampleRate}]);
      client.module.mobileRuntime=result.runtime;client.state='ready';return client;
    }catch(error){client.dispose();throw error;}
  }
  constructor(config){
    this.config=config;this.isWorker=true;this.isNetworkWorker=true;this.state='loading';this.tick=0;this.serial=0;this.pending=new Map();this.snapshot={};this.closed=false;
    this.context=new AudioContext({sampleRate:44100,latencyHint:'interactive'});this.context.resume().catch(()=>{});
    this.audio={context:this.context};this.module={SDL2:{audioContext:this.context},executionThread:'worker',heapBytes:0,
      netSnapshotInfo:()=>this.snapshot.snapshots||{snapshotBytes:0,snapshots:0},netKey(){}};
    this.displayMetrics={expired:0,hiddenDiscarded:0,replaced:0};this.module.displayMetrics=this.displayMetrics;
    try{this.worker=new Worker(new URL('./simulation-worker.js',import.meta.url),{type:'module',name:'TH03 network simulation'});}
    catch(error){this.context.close().catch(()=>{});throw error;}
    this.worker.onmessage=({data})=>this.receive(data);
    this.worker.onerror=event=>this.fail(Error(event.message||'联机模拟线程失败'));
    this.worker.onmessageerror=()=>this.fail(Error('联机模拟线程通信失败'));
    this.onPageHide=()=>this.dispose();window.addEventListener('pagehide',this.onPageHide);
    this.onVisibility=()=>{if(document.hidden)this.clearFrames();};document.addEventListener('visibilitychange',this.onVisibility);
    this.onContextLost=()=>this.fail(Error('游戏画面上下文已丢失，请重新开始'));config.canvas.addEventListener('contextlost',this.onContextLost);
  }
  call(method,args=[]){
    if(this.closed)return Promise.reject(Error('联机模拟线程已关闭'));
    const id=++this.serial;this.rpcCount=(this.rpcCount||0)+1;
    return new Promise((resolve,reject)=>{
      const timer=setTimeout(()=>this.fail(Error('联机模拟线程响应超时：'+method)),['init','setHdd','boot'].includes(method)?600000:60000);
      this.pending.set(id,{resolve,reject,timer});
      try{this.worker.postMessage({id,method,args});}catch(error){this.fail(error);}
    });
  }
  receive(data){
    if(data.type==='frame'){
      if(this.closed){data.bitmap.close();return;}
      if(document.hidden){this.releaseFrame(data,true);this.displayMetrics.hiddenDiscarded++;return;}
      if(data.staged){
        this.discardPreparedFrame();this.preparedFrame=data;return;
      }
      if(this.queuedFrame){this.releaseFrame(this.queuedFrame);this.displayMetrics.replaced++;}
      this.queuedFrame=data;
      if(!this.drawHandle)this.drawHandle=requestAnimationFrame(()=>this.draw());return;
    }
    if(this.closed)return;
    if(data.type==='fatal'||data.type==='error'){this.fail(Error(data.message));return;}
    if(data.type==='progress'){this.onProgress?.(data.message);return;}
    if(data.type==='exit'){this.state='exited';this.config.onExit?.();this.onExit?.();return;}
    if(data.type==='confirmed-audio'){for(const event of data.events||[])this.onAudio?.(...event);return;}
    if(data.type!=='reply')return;
    const pending=this.pending.get(data.id);if(!pending)return;
    if(data.status){
      const previous=this.snapshot;this.snapshot={...previous,...data.status};
      if(data.status.stepMetrics)this.snapshot.stepMetrics={...previous.stepMetrics,...data.status.stepMetrics};
      const s=this.snapshot;this.state=s.state;this.tick=s.tick;
      Object.assign(this.module,{virtualNow:s.virtualNow,heapBytes:s.heapBytes,frameUploads:s.frameUploads});
      for(const key of ['stepMetrics','programReads','nativeCore','pauseSearch','render','snapshots'])
        if(Object.hasOwn(data.status,key))this.module[key]=s[key];
    }
    for(const event of data.events||[])this.onAudio?.(...event);
    this.executionMs=(this.executionMs||0)+(data.executionMs||0);
    clearTimeout(pending.timer);this.pending.delete(data.id);pending.resolve(data.result);
  }
  releaseFrame(frame,discarded=!!frame.staged){frame.bitmap.close();if(!this.closed)this.worker.postMessage({type:'frame-ack',sequence:frame.sequence,discarded});}
  discardPreparedFrame(){if(this.preparedFrame){this.releaseFrame(this.preparedFrame);this.preparedFrame=null;}}
  clearFrames(){
    this.discardPreparedFrame();
    if(this.drawHandle)cancelAnimationFrame(this.drawHandle);this.drawHandle=0;
    if(this.queuedFrame){this.releaseFrame(this.queuedFrame,true);this.queuedFrame=null;}
  }
  draw(){
    this.drawHandle=0;const frame=this.queuedFrame;this.queuedFrame=null;if(!frame)return;
    const hidden=document.hidden,expired=Number.isFinite(frame.producedAt)&&performance.timeOrigin+performance.now()-frame.producedAt>50;
    if(hidden||expired){this.releaseFrame(frame,true);this.displayMetrics[hidden?'hiddenDiscarded':'expired']++;return;}
    try{if(!this.closed){this.onDraw?.(frame.bitmap);this.drawn=(this.drawn||0)+1;}}catch(error){this.fail(error);}finally{this.releaseFrame(frame);}
  }
  present(due){
    if(this.closed)return;this.onSnapshot?.(this.snapshot);
    if(this.preparedPresentation){
      if(!this.preparedFrame||(due&&!due(performance.now())))return;
      if(this.queuedFrame)this.releaseFrame(this.queuedFrame);
      this.queuedFrame=this.preparedFrame;this.queuedFrame.staged=false;this.preparedFrame=null;
      if(!this.drawHandle)this.drawHandle=requestAnimationFrame(()=>this.draw());return;
    }
    const key=`${this.snapshot.frameUploads||0}:${this.snapshot.renderEpoch||0}`;
    if(this.presentPending||this.presentedKey===key)return;
    // Idle pumps and an in-flight transfer must not consume the display slot
    // before there is a new frame that can actually be requested.
    if(due&&!due(performance.now()))return;
    this.presentPending=true;
    this.call('present').then(drawn=>{if(drawn)this.presentedKey=key;}).catch(error=>{if(!this.closed)this.fail(error);}).finally(()=>{this.presentPending=false;});
  }
  execute(inputs,options){this.preparedPresentation=true;return this.call('execute',[inputs,{...options,prepareFrame:true}]);}
  replaySlice(frame,plans){this.preparedPresentation=true;return this.call('replay-slice',[frame,plans,true]);}
  restore(frame){this.clearFrames();return this.call('restore',[frame]);}
  confirmQueued(prefix,retainFrom){
    if(this.closed)throw Error('联机模拟线程已关闭');
    this.worker.postMessage({id:++this.serial,method:'confirm',args:[prefix,retainFrom],notify:true});
  }
  confirm(prefix,retainFrom){return this.call('confirm',[prefix,retainFrom]);}
  hash(){return this.call('hash');}
  async boot(progress){this.onProgress=progress;try{return await this.call('boot');}finally{this.onProgress=null;}}
  addDiskImage(name,bytes){return this.call('addDiskImage',[name,bytes]);}
  setHdd(drive,name){return this.call('setHdd',[drive,name]);}
  getDiskImage(name){return this.call('getDiskImage',[name]);}
  run(){if(this.closed)return;this.state='running';this.context.resume().catch(()=>{});this.call('run').catch(error=>{if(!this.closed)this.fail(error);});}
  pause(){
    if(this.closed)return;this.state='paused';
    this.call('pause').catch(error=>{if(!this.closed)this.fail(error);});
  }
  fail(error){if(this.closed)return;this.failure=error;this.dispose();this.config.onError?.(error);}
  dispose(){
    if(this.closed)return;this.closed=true;this.state='exited';
    this.clearFrames();
    this.worker.terminate();for(const pending of this.pending.values()){clearTimeout(pending.timer);pending.reject(this.failure||Error('联机模拟线程已关闭'));}this.pending.clear();
    window.removeEventListener('pagehide',this.onPageHide);this.context.close().catch(()=>{});
    document.removeEventListener('visibilitychange',this.onVisibility);this.config.canvas.removeEventListener('contextlost',this.onContextLost);
  }
}
export function workerNetworkPause(client){
  return {get selecting(){return !!client.snapshot.selecting;},get pending(){return client.snapshot.pending||null;},
    update:()=>client.snapshot.nativeState||null,markers:()=>client.snapshot.markers||[],setFocus(){},setTouch(){},reset(){client.snapshot={...client.snapshot,nativeState:null,markers:[],pending:null};},request:()=>false};
}
