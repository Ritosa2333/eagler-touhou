import './vendor/dosbox-x/emulators.js';
import {extractGame,dosboxConfig,prepareFastBoot} from './dosbox-disk.js';
import {LocalAudioOutput} from './dosbox-audio-output.js';
import {createDosboxSnapshots} from './dosbox-snapshots.js';
import {makeMemoryDiff} from './dosbox-memory-diff.js';
import {installFramePresenter} from './frame-presenter.js';
import {dosboxKeyCode} from './dosbox-keys.js';
import {loadLocalCore,usesWriteJournal} from './startup-assets.js';
import {hashNativeMemory} from './netplay/native-hash.js';
import {createExternalReplayHost,inspectExternalMemory} from './replay/external-host.js';
import {loadExternalDosboxFactory} from './replay/external-loader.js';
import {createSoloReplayLifecycle} from './replay/external-lifecycle.js';
export class DOSBoxPC98 {
  static async create(config){const emulator=new DOSBoxPC98(config);await emulator.initialize();return emulator;}
  constructor(config){this.config=config;this.state='loading';this.disks=new Map();this.keys=new Set();this.outputs=new Map();this.confirmedAudio=0;this.frame=null;this.replaying=false;this.tick=0;this.audioChunks=[];this.externalHost=null;}
  async initialize(){
    this.failure=null;
    const net=!!this.config.lockstep;
    const tracked=net&&usesWriteJournal(this.config.rollbackEnabled);
    if(!net&&(typeof WebAssembly.Suspending!=='function'||typeof WebAssembly.promising!=='function'))throw Error('DOSBox-X 性能版需要支持 JSPI 的新版 Chrome / Edge。');
    const context=(this.config.audioContext?.state!=='closed'&&this.config.audioContext)||new AudioContext({sampleRate:44100,latencyHint:'interactive'});
    context.resume().catch(()=>{});this.audio=new LocalAudioOutput(context,1);
    const vendor=new URL('./vendor/dosbox-x/',import.meta.url),emulator=this;
    const [{default:factory},wasmBinary]=await Promise.all([
      import(net?(tracked?'./vendor/dosbox-x/dosbox-rollback-tracked.mjs':'./vendor/dosbox-x/dosbox-rollback.mjs'):'./vendor/dosbox-x/dosbox-jspi.mjs'),
      this.config.wasmBinary||(!net?loadLocalCore():undefined),
    ]);
    const epoch=this.config.lockstepEpoch??946684800000;
    const NativeDate=globalThis.Date;
    class VirtualDate extends NativeDate {constructor(...args){super(...(args.length?args:[epoch+(emulator.module?.virtualNow||0)]));}static now(){return epoch+(emulator.module?.virtualNow||0);}getTimezoneOffset(){return 0;}getFullYear(){return this.getUTCFullYear();}getMonth(){return this.getUTCMonth();}getDate(){return this.getUTCDate();}getDay(){return this.getUTCDay();}getHours(){return this.getUTCHours();}getMinutes(){return this.getUTCMinutes();}getSeconds(){return this.getUTCSeconds();}}
    const queue=[];let receiver;
    this.module={canvas:this.config.canvas,wasmBinary,worker:!!this.config.workerRuntime,SDL2:{audioContext:context},virtualDate:VirtualDate,virtualNow:0,targetNow:0,deterministic:net,nativeScheduling:true,nativeCpu:true,nativePlanar:false,nativeFramePresentation:false,
      mobileRuntime:{core:'dosbox-x',cpuCore:'simple',continuation:net?'Asyncify':'JSPI',writeJournal:tracked},
      locateFile:f=>new URL(f==='wdosbox-x-jspi.wasm'?(net?(tracked?'wdosbox-x-rollback-tracked.wasm':'wdosbox-x-rollback.wasm'):'wdosbox-x-fast.wasm'):f==='wdosbox-x.wasm'?(tracked?'wdosbox-x-rollback-tracked.wasm':'wdosbox-x-rollback.wasm'):f,vendor).href,
      postMessage:(name,props)=>{if(receiver)receiver(name,props);else queue.push([name,props]);},
      onAbort:error=>{this.failure=Error(String(error));this.state='failed';this.config.onError?.(this.failure);}};
    this.module.installFramePresenter=()=>{
      this.module.framePresenter?.dispose();
      this.module.framePresenter=installFramePresenter(this.module,{deferred:net,isReplaying:()=>this.replaying});
    };
    this.transport={sessionId:'th03-dosbox',module:this.module,net:null,
      sendMessageToServer:(name,props)=>this.module.messageHandler({data:{name,props}}),
      initMessageHandler:handler=>{receiver=handler;for(const [name,props]of queue)handler(name,props);queue.length=0;},exit:()=>{}};
    if(this.config.externalReplay&&!net){
      const replay=this.config.externalReplay;
      this.externalHost=createExternalReplayHost({...replay,
        observe:replay.lifecycle?createSoloReplayLifecycle(replay.lifecycle,()=>this.module.HEAPU8):null,
        applyInput:command=>this.applyReplayInput(command),inspect:()=>inspectExternalMemory(this.module),
        onChunk:value=>replay.onChunk?.(value),onEnd:value=>replay.onEnd?.(value),
        onError:error=>{if(replay.onError)replay.onError(error);else this.config.onError?.(error);},
        onRecordingError:error=>replay.onRecordingError?.(error),
      });
      let replayFactory;
      try{replayFactory=await loadExternalDosboxFactory({vendor,module:this.module,host:this.externalHost,wasmBinary});}
      catch(error){
        if(replay.mode!=='record')throw error;
        this.externalHost.dispose();this.externalHost=null;delete this.module.instantiateWasm;
        try{replay.onRecordingError?.(error);}catch{}
        replayFactory=factory;
      }
      await replayFactory(this.module);
    }else await factory(this.module);
    this.module.callMain([this.transport.sessionId]);
    this.installDiskNotifications();
    if(net&&this.config.rollbackEnabled!==false)this.module.memoryDiff=await makeMemoryDiff(this.module);
    this.onSleep=({data})=>{if(!net&&data?.name==='ws-sync-sleep'&&data.props.sessionId===this.transport.sessionId)globalThis.postMessage({name:'wc-sync-sleep',props:data.props},'*');};
    if(!net&&typeof window!=='undefined'&&window.postMessage)window.addEventListener('message',this.onSleep);
    this.onKey=event=>this.key(event.code,event.type==='keydown');
    for(const type of ['keydown','keyup'])this.config.canvas.addEventListener?.(type,this.onKey);
    this.state='ready';
    const m=this.module;
    m.netKey=(type,event)=>this.key(event.code,type==='keydown');
    m.netBeginFrame=(frame,replay)=>{this.frame=frame;this.replaying=!!replay;this.outputs.set(frame,[]);};
    m.netEndFrame=()=>{this.snapshots?.seal();this.frame=null;this.replaying=false;};
    m.netConfirm=(prefix,retainFrom=prefix)=>{this.snapshots?.confirm(retainFrom);for(const [frame,blocks]of this.outputs)if(frame<prefix){if(frame>=this.confirmedAudio)for(const samples of blocks)this.audio.push([samples]);this.outputs.delete(frame);}this.confirmedAudio=Math.max(this.confirmedAudio,prefix);};
    m.netCapture=frame=>{if(!this.snapshots)this.snapshots=createDosboxSnapshots(this);this.snapshots.capture(frame);};
    m.netRestore=frame=>{this.snapshots.restore(frame);for(const key of this.outputs.keys())if(key>=frame)this.outputs.delete(key);};
    m.netSnapshotInfo=()=>this.snapshots?.info()||{snapshotBytes:0,snapshots:0};
    m.netFlushAudio=()=>this.audio.flush();m.netInfo=()=>({tick:this.tick,now:m.virtualNow,...this.audio.info()});
    m.netHash=(start,end)=>hashNativeMemory(m.nativeExports().memory,start,end);
    m.netPresent=()=>m.framePresenter?.flush({redraw:true})||false;
  }
  key(code,down){if(this.module.warmingBoot)return;if(this.externalHost){if(this.externalHost.status.mode==='playback')return;this.externalHost.queue({type:'key',code,down});return;}this.applyKey(code,down);}
  applyKey(code,down){const number=dosboxKeyCode(code);if(number===undefined)return;if(down)this.keys.add(code);else this.keys.delete(code);if(this.config.lockstep)this.ci?.addKey(number,down,this.module.virtualNow);else if(this.externalHost)this.ci?.addKey(number,down,this.externalHost.keyTime());else this.ci?.sendKeyEvent(number,down);}
  applyReplayInput(command){if(command.type==='key')this.applyKey(command.code,command.down);else if(command.type==='assist')this.config.applyReplayInput?.(command.input);else throw Error('单人 Replay 输入类型无效');}
  assist(input){if(this.externalHost){if(this.externalHost.status.mode==='playback')return;this.externalHost.queue({type:'assist',input});}else this.config.applyReplayInput?.(input);}
  finishReplay(){this.externalHost?.finish();}
  fastForwardReplay(value){this.externalHost?.fastForward(value);}
  stopBrowserRecording(){this.externalHost?.disable();}
  pauseReplay(value){if(value)this.pause();else this.run();}
  installDiskNotifications(){
    const m=this.module,ops=m.MEMFS.stream_ops,write=ops.write,emulator=this;
    m.programReads=[];m.stepMetrics={count:0,totalMs:0,maxMs:0,slowSteps:[]};
    const open=m.FS.open;
    m.FS.open=function(file,...args){
      const stream=open.call(this,file,...args);
      if(!emulator.installingDisk&&(stream.flags&3)===0&&/\/(OP|MAINL|MAIN)\.EXE$/i.test(stream.path||'')){
        const name=stream.path.split('/').at(-1).toUpperCase(),previous=m.programReads.at(-1);
        if(previous?.name!==name||previous.tick!==emulator.tick){m.programReads.push({name,tick:emulator.tick,virtualNow:m.virtualNow,wall:performance.now()});if(m.programReads.length>64)m.programReads.shift();}
      }
      return stream;
    };
    ops.write=function(stream,...args){
      const result=write.call(this,stream,...args);
      if(result&&!emulator.installingDisk&&!emulator.config.lockstep&&emulator.extracted?.owns(m.FS.getPath(stream.node)))emulator.config.onDiskChange?.(emulator.diskName);
      return result;
    };
    if(m.MEMFS.ops_table)m.MEMFS.ops_table.file.stream.write=ops.write;
  }
  addDiskImage(name,bytes){this.disks.set(name,new Uint8Array(bytes));}
  async setHdd(drive,name){
    if(drive!==0||!this.disks.has(name))throw Error('无效游戏磁盘');this.diskName=name;
    this.extracted=extractGame(this.disks.get(name));
    const network=!!this.config.lockstep,launcher=prepareFastBoot(this.extracted,{network});
    const init=[...this.extracted.files,launcher,{dosboxConf:dosboxConfig({network,fastBoot:true}),jsdosConf:{version:'8'}}];
    this.installingDisk=true;
    try{this.ci=await globalThis.emulators.backend(init,this.transport,{});}finally{this.installingDisk=false;}
    this.ci.events().onSoundPush(samples=>{this.module.audioPackets=(this.module.audioPackets||0)+1;if(this.externalHost?.status.fastForward||this.module.warmingBoot||(this.config.lockstep&&this.config.independentSfx))return;if(this.config.workerRuntime){this.audioChunks.push(samples.slice());return;}if(this.frame!==null){if(this.frame>=this.confirmedAudio)this.outputs.get(this.frame)?.push(samples.slice());}else this.audio.push([samples]);});
    this.logs=[];this.ci.events().onMessage((type,...messages)=>{this.logs.push([type,...messages]);this.logs=this.logs.slice(-40);if(type==='error'&&messages.some(s=>/\[panic\]|Backend crashed|Aborted\(/i.test(String(s)))){this.failure=Error(messages.join(' '));this.state='failed';this.config.onError?.(this.failure);}});
    this.ci.events().onExit(()=>{if(this.disposed||this.resetting||this.failure||this.state==='failed')return;this.state='exited';this.config.onExit?.();});
    for(const code of this.keys)this.key(code,true);
    if(this.state!=='running'&&!this.config.lockstep)this.ci.pause();
    this.module.SDL2={audioContext:this.audio.context};
    if(!this.module.framePresenter)throw Error('DOSBox-X 直接呈现器未初始化，请刷新页面并检查运行资源版本');
  }
  getDiskImage(name){if(name===this.diskName&&this.extracted)return this.extracted.export(this.module.FS);const disk=this.disks.get(name);if(!disk)throw Error('未知磁盘');return disk.slice();}
  run(){if(this.failure)throw this.failure;this.state='running';this.externalHost?.pause(false);if(!this.config.lockstep)this.ci?.resume();if(!['running','closed'].includes(this.audio.context.state))this.audio.context.resume().catch(()=>{});}
  pause(){this.state='paused';this.externalHost?.pause(true);if(!this.config.lockstep)this.ci?.pause();this.module?.framePresenter?.resetTiming();this.audioChunks=[];this.audio.flush();}
  async reset(){
    const running=this.state==='running',diskName=this.diskName,disks=this.disks;
    this.resetting=true;this.pause();this.externalHost?.dispose();
    if(this.ci){const exiting=this.ci.exit();if(this.config.lockstep){for(let i=0;!this.ci.exited&&i<1000;i++){this.module.virtualNow=Math.max(this.module.virtualNow,this.module.wakeAt);this.module.advanceNative();}}await exiting;}
    this.dispose();this.ci=null;this.extracted=null;this.state='loading';this.keys=new Set();this.outputs=new Map();this.confirmedAudio=0;this.frame=null;this.replaying=false;this.tick=0;this.audioChunks=[];this.snapshots=null;
    this.disposed=false;this.externalHost=null;await this.initialize();this.disks=disks;
    if(diskName)await this.setHdd(0,diskName);
    this.resetting=false;if(running)this.run();
  }
  step(){
    if(!this.config.lockstep||this.state!=='running')throw Error('DOSBox-X 没有进入同步运行状态');
    const began=performance.now(),m=this.module;this.tick++;m.cooperativeYields=0;m.targetNow=Math.max(m.virtualNow,Math.floor(this.tick*1000/60));
    let safety=0;
    while(m.wakeAt<=m.targetNow){if(++safety>4096)throw Error('DOSBox-X 同步步骤未让出执行');if(m.wakeAt>m.virtualNow)m.virtualNow=m.wakeAt;m.advanceNative();}
    m.virtualNow=m.targetNow;
    if(!m.pendingWake)throw Error('DOSBox-X 未在同步边界挂起');
    if(m.stepMetrics){const cost=performance.now()-began;m.stepMetrics.count++;m.stepMetrics.totalMs+=cost;m.stepMetrics.maxMs=Math.max(m.stepMetrics.maxMs,cost);
      if(cost>30){m.stepMetrics.slowSteps.push({at:began,tick:this.tick,virtualNow:m.virtualNow,costMs:cost});if(m.stepMetrics.slowSteps.length>120)m.stepMetrics.slowSteps.shift();}}
  }
  readAudio(){if(!this.audioChunks.length)return null;const length=this.audioChunks.reduce((n,s)=>n+s.length,0),samples=new Float32Array(length);let at=0;for(const block of this.audioChunks.splice(0)){samples.set(block,at);at+=block.length;}return [samples];}
  dispose(){if(this.disposed)return;this.disposed=true;this.externalHost?.disable();this.pause();this.externalHost?.dispose();this.ci?.exit();this.module?.framePresenter?.dispose();this.audio.disconnect();this.audio.context.close().catch(()=>{});for(const type of ['keydown','keyup'])this.config.canvas.removeEventListener?.(type,this.onKey);if(typeof window!=='undefined')window.removeEventListener('message',this.onSleep);}
}
// Source imports keep their legacy class name, while every new instance uses DOSBox-X.
export {DOSBoxPC98 as NP21};
