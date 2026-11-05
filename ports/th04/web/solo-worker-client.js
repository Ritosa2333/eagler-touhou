import {packTouch,unpackTouch} from './touch-input.js';
export const supportsSoloWorker=()=>typeof Worker==='function'&&typeof OffscreenCanvas==='function'&&typeof HTMLCanvasElement.prototype.transferControlToOffscreen==='function';

export class WorkerNP21 {
  static async create(config,setup){
    const emulator=new WorkerNP21(config,setup);
    try{await emulator.initialize();return emulator;}catch(error){emulator.dispose();throw error;}
  }
  constructor(config,setup){
    this.config=config;this.setup=setup;this.state='loading';this.isWorker=true;
    this.pending=new Map();this.serial=0;this.closed=false;this.sources=new Set();this.audioEnd=0;this.audioEpoch=0;
    this.snapshot={state:null,markers:[]};this.practiceParked=false;this.prepared=new Promise((resolve,reject)=>{this.resolvePrepared=resolve;this.rejectPrepared=reject;});this.prepared.catch(()=>{});
    this.context=new AudioContext();this.context.resume().catch(()=>{});
    this.module={heapBytes:0,SDL2:{audioContext:this.context},executionThread:'worker'};
    this.worker=new Worker(new URL('./solo-worker.js',import.meta.url),{type:'module',name:'TH'+setup.game+' simulation'});
    this.worker.onmessage=({data})=>this.receive(data);
    this.worker.onerror=event=>this.fail(Error(event.message||'模拟线程启动失败'));
    this.worker.onmessageerror=()=>this.fail(Error('模拟线程通信失败'));
    this.onKey=event=>{
      if(!['ready','running'].includes(this.state))return;
      this.send('key',[{type:event.type,code:event.code,key:event.key,keyCode:event.keyCode,which:event.which,
        charCode:event.charCode,repeat:event.repeat,location:event.location,ctrlKey:event.ctrlKey,altKey:event.altKey,shiftKey:event.shiftKey,metaKey:event.metaKey}]);
    };
    for(const type of ['keydown','keyup','keypress'])config.canvas.addEventListener(type,this.onKey);
    this.onVisibility=()=>{if(document.hidden)this.flushAudio();};
    document.addEventListener('visibilitychange',this.onVisibility);
    this.onPageHide=event=>{if(!event.persisted)this.dispose();};
    window.addEventListener('pagehide',this.onPageHide);
    this.audioTimer=setInterval(()=>this.pumpAudio(),25);
  }
  async initialize(){
    if(!supportsSoloWorker())throw Error('浏览器不支持后台模拟，请选择兼容模式');
    const {canvas,onDiskChange,onExit,onError:configOnError,onPracticeParked,applyReplayInput,...config}=this.config;
    const replay=config.externalReplay;this.externalReplayCallbacks=replay||null;
    if(replay)config.externalReplay={mode:replay.mode,boundaryMode:replay.boundaryMode,endReason:replay.endReason,endPhase:replay.endPhase,timeline:replay.timeline,lifecycle:replay.lifecycle,
      ...(replay.clocks?{clocks:replay.clocks.slice()}:{}),...(replay.events?{events:replay.events}:{}),...(replay.endBoundary!==undefined?{endBoundary:replay.endBoundary}:{})};
    const offscreen=canvas.transferControlToOffscreen();
    const {onError,...setup}=this.setup;
    const transfer=[offscreen];if(config.externalReplay?.clocks)transfer.push(config.externalReplay.clocks.buffer);
    const result=await this.call('init',[{...setup,config,canvas:offscreen,sampleRate:this.context.sampleRate}],transfer);
    this.module.heapBytes=result.heapBytes;this.module.mobileRuntime=result.runtime;this.state='ready';
  }
  call(method,args=[],transfer=[]){
    if(this.closed)return Promise.reject(Error('模拟线程已关闭'));
    const id=++this.serial;
    return new Promise((resolve,reject)=>{
      const timeout=setTimeout(()=>{this.pending.delete(id);reject(Error('模拟线程响应超时：'+method));},60000);
      this.pending.set(id,{resolve,reject,timeout});
      try{this.worker.postMessage({id,method,args},transfer);}catch(error){clearTimeout(timeout);this.pending.delete(id);reject(error);}
    });
  }
  send(method,args=[]){if(!this.closed)this.worker.postMessage({method,args});}
  receive(data){
    if(this.closed)return;
    if(data.type==='practice-parked'){this.practiceParked=true;this.snapshot.practice=data.practice;this.resolvePrepared(data.practice);return;}
    if(data.type==='external-replay-chunk'){this.externalReplayCallbacks?.onChunk?.(data.chunk);return;}
    if(data.type==='external-replay-end'){this.externalReplayCallbacks?.onEnd?.(data.result);return;}
    if(data.type==='external-replay-error'){(this.externalReplayCallbacks?.onError||this.externalReplayCallbacks?.onRecordingError)?.(Error(data.message));return;}
    if(data.name==='ws-sync-sleep'){this.worker.postMessage({name:'wc-sync-sleep',props:data.props});return;}
    if(data.type==='error'){
      if(data.diagnostics)this.lastDiagnostics=data.diagnostics;
      const pending=this.pending.get(data.id);
      if(pending){clearTimeout(pending.timeout);this.pending.delete(data.id);pending.reject(Error(data.message));}
      else this.fail(Error(data.message));
    }else if(data.type==='reply'){
      const pending=this.pending.get(data.id);if(!pending)return;
      clearTimeout(pending.timeout);this.pending.delete(data.id);pending.resolve(data.result);
    }else if(data.type==='snapshot'){
      this.send('snapshotAck');this.snapshot=data;this.module.localFrameCount=data.frames;this.module.frameUploads=data.uploads;
      if(data.heapBytes)this.module.heapBytes=data.heapBytes;
      if(data.render)this.module.renderMetrics=data.render;
      for(const cost of data.costs)this.module.observeLocalFrame?.(cost);
      for(const now of data.drawTimes||[])this.module.observeFrameDraw?.(now);
      for(const [ax,hash]of data.events)this.onMusic?.(ax,hash);
    }else if(data.type==='diskChange')this.config.onDiskChange?.();
    else if(data.type==='exit'){this.state='exited';this.flushAudio();this.config.onExit?.();}
  }
  async pumpAudio(){
    if(this.closed||this.audioPending||this.state!=='running'||document.hidden||this.context.state!=='running')return;
    if(this.audioEnd-this.context.currentTime>(this.config.nativeSoloAudio?0.18:0.025))return;
    this.audioPending=true;const epoch=this.audioEpoch;
    try{
      const channels=await this.call('audio');
      if(this.closed||epoch!==this.audioEpoch||!channels||this.state!=='running'||this.context.state!=='running')return;
      this.module.audioPackets=(this.module.audioPackets||0)+1;
      const buffer=this.context.createBuffer(channels.length,channels[0].length,this.context.sampleRate);
      channels.forEach((channel,i)=>buffer.copyToChannel(channel,i));
      const source=this.context.createBufferSource();source.buffer=buffer;source.connect(this.context.destination);
      this.sources.add(source);source.onended=()=>{this.sources.delete(source);source.disconnect();};
      const start=Math.max(this.audioEnd,this.context.currentTime+0.015);source.start(start);this.audioEnd=start+buffer.duration;
    }catch(error){if(!this.closed)this.fail(error);}finally{this.audioPending=false;}
  }
  flushAudio(){
    this.audioEpoch++;this.audioEnd=0;
    for(const source of this.sources){try{source.stop();}catch{}source.disconnect();}this.sources.clear();
  }
  run(){this.state='running';this.send('run');this.context.resume().catch(()=>{});}
  pause(){if(this.state==='running'){this.state='paused';this.clearInput();this.send('pause');this.flushAudio();}}
  reset(){this.clearInput();this.snapshot={state:null,markers:[]};this.flushAudio();this.send('reset');}
  addDiskImage(name,data){return this.call('addDiskImage',[name,data],[data.buffer]);}
  setHdd(drive,name){return this.call('setHdd',[drive,name]);}
  getDiskImage(name){return this.call('getDiskImage',[name]);}
  setInput(input){
    const old=this.queuedInput;
    if(old&&old.generation===input.generation){
      const merge=(a,b)=>b?.active&&!b.begin&&a?.active&&a.unlimited===b.unlimited?{...b,begin:!!a.begin,x:a.x+b.x,y:a.y+b.y}:b;
      if(Array.isArray(input.touch))input.touch=input.touch.map((value,i)=>packTouch(value&4095,merge(unpackTouch(old.touch[i]||0),unpackTouch(value))));
      else input.touch=merge(old.touch,input.touch);
    }
    this.queuedInput=input;this.flushInput();
  }
  async flushInput(){
    if(this.inputPending||!this.queuedInput||this.closed)return;
    const input=this.queuedInput;this.queuedInput=null;this.inputPending=true;
    try{await this.call('input',[input]);}catch(error){if(!this.closed)this.fail(error);}
    finally{this.inputPending=false;if(!this.closed)this.flushInput();}
  }
  clearInput(){this.queuedInput=null;this.send('clear');}
  finishReplay(){this.send('externalReplayFinish');}
  async activatePractice(options){await this.call('practiceActivate',[options]);this.practiceParked=false;this.config.practiceStandby=false;this.state='running';this.context.resume().catch(()=>{});}
  practiceOptions(options){this.send('practiceOptions',[options]);}
  fastForwardReplay(value){this.send('externalReplayFast',[!!value]);}
  stopBrowserRecording(){this.send('externalReplayDisable');}
  pauseReplay(value){this.state=value?'paused':'running';this.send('externalReplayPause',[!!value]);if(value){this.flushAudio();this.context.suspend().catch(()=>{});}else this.context.resume().catch(()=>{});}
  fail(error){if(this.closed)return;console.error(error);this.failedError=error;this.dispose();this.setup.onError?.(error);}
  dispose(){
    if(this.closed)return;this.closed=true;this.rejectPrepared(Error('模拟线程已关闭'));this.state='exited';clearInterval(this.audioTimer);this.flushAudio();this.worker.terminate();
    for(const pending of this.pending.values()){clearTimeout(pending.timeout);pending.reject(this.failedError||Error('模拟线程已关闭'));}this.pending.clear();
    for(const type of ['keydown','keyup','keypress'])this.config.canvas.removeEventListener(type,this.onKey);
    document.removeEventListener('visibilitychange',this.onVisibility);window.removeEventListener('pagehide',this.onPageHide);
    this.context.close().catch(()=>{});
  }
}

// Keep the existing TH03 controls on the page; native memory is worker-owned.
export function workerAssistBridge(emulator){
  let focus=0;
  return {
    update:()=>emulator.snapshot.state,
    markers:()=>emulator.snapshot.markers,
    reset:()=>{focus=0;emulator.clearInput();},
    setFocus:value=>{focus=value;},
    setTouch:touch=>emulator.setInput({generation:emulator.snapshot.state?.generation,focus,touch}),
  };
}

export function mountWorkerChoice(parent){
  const label=document.createElement('label');label.textContent='运行方式 ';
  const select=document.createElement('select');select.id='runtime-mode';
  select.innerHTML='<option value="worker">后台模拟（建议）</option><option value="main">兼容模式</option>';
  try{const saved=localStorage.getItem('solo-runtime-mode');if(saved==='main')select.value=saved;}catch{}
  if(!supportsSoloWorker()){select.value='main';select.options[0].disabled=true;}
  select.onchange=()=>{try{localStorage.setItem('solo-runtime-mode',select.value);}catch{}};
  label.append(select);parent.append(label);return select;
}
