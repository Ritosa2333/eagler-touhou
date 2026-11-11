import './vendor/dosbox-x/emulators.js';
import {extractGame,dosboxConfig,prepareFastBoot} from './dosbox-disk.js';
import {LocalAudioOutput} from './netplay/audio-output.js';
import {createDosboxSnapshots} from './dosbox-snapshots.js';
import {makeMemoryDiff} from './dosbox-memory-diff.js';
import {installFramePresenter} from './frame-presenter.js';
import {loadPlayerArt,installPlayerArt} from './player-art.js';
import {createExternalReplayHost,inspectExternalMemory} from './replay/external-host.js';
import {loadExternalDosboxFactory} from './replay/external-loader.js';
import {createSoloReplayLifecycle} from './replay/external-lifecycle.js';
import {createPracticeEngine} from './practice-engine.js';
import {createPracticeClock} from './practice-config.js';
const codes={ArrowUp:265,ArrowDown:264,ArrowLeft:263,ArrowRight:262,KeyZ:90,KeyX:88,ShiftLeft:340,ShiftRight:344,Escape:256,Enter:257,Space:32};
export const supportsWriteJournal=()=>WebAssembly.validate(new Uint8Array([0,97,115,109,1,0,0,0,5,5,2,0,1,0,1]));
export class DOSBoxPC98 {
  static async create(config){const emulator=new DOSBoxPC98(config);try{await emulator.initialize();return emulator;}catch(error){emulator.dispose();throw error;}}
  constructor(config){this.config=config;this.state='loading';this.disks=new Map();this.keys=new Set();this.outputs=new Map();this.frame=null;this.replaying=false;this.tick=0;this.audioChunks=[];this.externalHost=null;this.practiceStandby=!!config.practiceStandby;this.practiceParked=false;this.prepared=new Promise((resolve,reject)=>{this.resolvePrepared=resolve;this.rejectPrepared=reject;});this.prepared.catch(()=>{});}
  async initialize(){
    const net=!!this.config.lockstep;
    const art=this.config.artPatch?await loadPlayerArt(this.config.artPatch):null;
    const tracked=net&&this.config.rollbackEnabled!==false&&supportsWriteJournal();
    if(!net&&(typeof WebAssembly.Suspending!=='function'||typeof WebAssembly.promising!=='function'))throw Error('DOSBox-X 性能版需要支持 JSPI 的新版 Chrome / Edge。');
    const context=(this.config.audioContext?.state!=='closed'&&this.config.audioContext)||new AudioContext({sampleRate:44100,latencyHint:'interactive'});
    context.resume().catch(()=>{});this.audio=new LocalAudioOutput(context,1);
    const vendor=new URL('./vendor/dosbox-x/',import.meta.url),emulator=this;
    const epoch=this.config.lockstepEpoch??946684800000;
    const NativeDate=globalThis.Date;
    class VirtualDate extends NativeDate {constructor(...args){super(...(args.length?args:[epoch+(emulator.module?.virtualNow||0)]));}static now(){return epoch+(emulator.module?.virtualNow||0);}getTimezoneOffset(){return 0;}getFullYear(){return this.getUTCFullYear();}getMonth(){return this.getUTCMonth();}getDate(){return this.getUTCDate();}getDay(){return this.getUTCDay();}getHours(){return this.getUTCHours();}getMinutes(){return this.getUTCMinutes();}getSeconds(){return this.getUTCSeconds();}}
    const queue=[];let receiver;
    this.module={canvas:this.config.canvas,worker:!!this.config.workerRuntime,SDL2:{audioContext:context},virtualDate:VirtualDate,virtualNow:0,targetNow:0,deterministic:net,
      nativeScheduling:net,nativeCpu:net,nativeFramePresentation:false,
      ...(this.config.wasmBinary?{wasmBinary:this.config.wasmBinary}:{}),
      mobileRuntime:{core:'dosbox-x',cpuCore:'normal',configuredCpuCore:'simple',continuation:net?'Asyncify':'JSPI',writeJournal:tracked},
      locateFile:f=>new URL(f==='wdosbox-x-jspi.wasm'?(net?(tracked?'wdosbox-x-rollback-tracked.wasm':'wdosbox-x-rollback.wasm'):'wdosbox-x-fast.wasm'):f==='wdosbox-x.wasm'?(tracked?'wdosbox-x-rollback-tracked.wasm':'wdosbox-x-rollback.wasm'):f,vendor).href,
      postMessage:(name,props)=>{if(receiver)receiver(name,props);else queue.push([name,props]);},
      onAbort:error=>{this.state='failed';this.config.onError?.(Error(String(error)));}};
    this.module.installFramePresenter=()=>{
      this.module.framePresenter?.dispose();
      this.module.framePresenter=installFramePresenter(this.module,{deferred:net,isReplaying:()=>this.replaying});
    };
    this.transport={sessionId:'th02-dosbox',module:this.module,net:null,
      sendMessageToServer:(name,props)=>this.module.messageHandler({data:{name,props}}),
      initMessageHandler:handler=>{receiver=handler;for(const [name,props]of queue)handler(name,props);queue.length=0;},exit:()=>{}};
    let factory;
    if((this.config.externalReplay||this.config.practice)&&!net){
      const replay=this.config.externalReplay||{mode:'record',boundaryMode:'sleep'};
      if(this.config.practice){
        this.practiceClock=createPracticeClock(this.config.practice.options.speed);
        this.practice=createPracticeEngine({mod:this.module,patch:this.config.practicePatch,config:this.config.practice,clock:this.practiceClock});
        this.module.practiceSignal=(port,packed)=>this.practice.signal(port,packed);
      }
      const lifecycle=replay.lifecycle?createSoloReplayLifecycle(replay.lifecycle,()=>this.module.HEAPU8):null;
      this.externalHost=createExternalReplayHost({...replay,
        clock:this.practiceClock,shouldHold:()=>this.holdPreparedPractice(),onBoundary:()=>this.module.observeReplayBoundary?.(),
        observe:()=>{const p=this.practice?.status;if(p?.outcome)return {active:true,stage:p.stage,paused:true,finish:p.outcome==='clear'?'clear':'game-over'};return lifecycle?.();},
        applyInput:command=>this.applyReplayInput(command),inspect:()=>inspectExternalMemory(this.module),
        onChunk:value=>replay.onChunk?.(value),onEnd:value=>replay.onEnd?.(value),
        onError:error=>{if(replay.onError)replay.onError(error);else this.config.onError?.(error);},
        onRecordingError:error=>replay.onRecordingError?.(error),
      });
      try{factory=await loadExternalDosboxFactory({vendor,module:this.module,host:this.externalHost,wasmBinary:this.config.wasmBinary});if(!this.config.externalReplay)this.externalHost.disable();}
      catch(error){
        if(replay.mode!=='record'||this.config.practice)throw error;
        this.externalHost.dispose();this.externalHost=null;delete this.module.instantiateWasm;
        try{replay.onRecordingError?.(error);}catch{}
        factory=(await import('./vendor/dosbox-x/dosbox-jspi.mjs')).default;
      }
    }else factory=(await import(net?(tracked?'./vendor/dosbox-x/dosbox-rollback-tracked.mjs':'./vendor/dosbox-x/dosbox-rollback.mjs'):'./vendor/dosbox-x/dosbox-jspi.mjs')).default;
    await factory(this.module);
    if(art)this.module.playerArt=installPlayerArt(this.module,this.config.artPatch,art);
    this.module.callMain([this.transport.sessionId]);
    if(net&&this.config.rollbackEnabled!==false)this.module.memoryDiff=await makeMemoryDiff(this.module);
    this.onSleep=({data})=>{if(!net&&data?.name==='ws-sync-sleep'&&data.props.sessionId===this.transport.sessionId)globalThis.postMessage({name:'wc-sync-sleep',props:data.props},'*');};
    if(!net&&typeof window!=='undefined'&&window.postMessage)window.addEventListener('message',this.onSleep);
    // Online keys must pass through the input queue, including canvas events.
    this.onKey=event=>{if(!net)this.key(event.code,event.type==='keydown');};
    for(const type of ['keydown','keyup'])this.config.canvas.addEventListener?.(type,this.onKey);
    this.state='ready';
    const m=this.module;
    m.netKey=(type,event)=>this.key(event.code,type==='keydown');
    m.netBeginFrame=(frame,replay)=>{this.frame=frame;this.replaying=!!replay;if(!this.config.independentEffects)this.outputs.set(frame,[]);};
    m.netEndFrame=()=>{this.snapshots?.seal();this.frame=null;this.replaying=false;};
    m.netConfirm=(prefix,retainFrame=prefix)=>{this.snapshots?.confirm(prefix,retainFrame);for(const [frame,blocks]of this.outputs)if(frame<prefix){for(const samples of blocks)this.audio.push([samples]);this.outputs.delete(frame);}};
    m.netDiscardFrame=frame=>this.outputs.delete(frame);
    m.netCapture=(frame,restored=false)=>{if(!this.snapshots)this.snapshots=createDosboxSnapshots(this);this.snapshots.capture(frame,restored);};
    m.netRestore=frame=>{this.snapshots.restore(frame);for(const key of this.outputs.keys())if(key>=frame)this.outputs.delete(key);};
    m.netSnapshotInfo=()=>this.snapshots?.info()||{snapshotBytes:0,snapshots:0};
    m.netFlushAudio=()=>this.audio.flush();m.netInfo=()=>({tick:this.tick,...this.audio.info()});
    m.netPresent=()=>m.framePresenter?.flush({redraw:true})||false;
  }
  key(code,down){if(this.externalHost){if(this.externalHost.status.mode==='playback')return;this.externalHost.queue({type:'key',code,down});return;}this.applyKey(code,down);}
  applyKey(code,down){const number=codes[code];if(number===undefined)return;if(down)this.keys.add(code);else this.keys.delete(code);if(this.config.lockstep)this.ci?.addKey(number,down,this.module.virtualNow);else if(this.externalHost)this.ci?.addKey(number,down,this.externalHost.keyTime());else this.ci?.sendKeyEvent(number,down);}
  applyReplayInput(command){if(command.type==='key')this.applyKey(command.code,command.down);else if(command.type==='assist')this.config.applyReplayInput?.(command.input);else if(command.type==='practice-options'&&this.practice)this.practice.configure(command.options);else throw Error('单人 Replay 输入类型无效');}
  holdPreparedPractice(){
    if(!this.practiceStandby)return false;
    const p=this.practice?.status;if(p?.error)throw Error(p.error);
    const cs=this.practice?.debug.csBase,offset=this.config.practicePatch.entries.mailbox;
    if(!p?.ready||!p.waiting||!(cs>=0)||this.module.HEAPU8[cs+offset+22]!==2)return false;
    if(!this.practiceParked){this.practiceParked=true;this.practiceClock.setPaused(true);this.audio.flush();this.resolvePrepared(p);this.config.onPracticeParked?.(p);}
    return true;
  }
  activatePractice(options){
    if(!this.practiceParked||this.disposed)throw Error('预备练习起点尚未就绪');
    // Record current options on this attempt's original boundary and tape.
    this.practiceOptions(options);this.practiceStandby=false;this.practiceParked=false;this.practiceClock.setPaused(false);this.run();
  }
  practiceOptions(options){if(!this.practice||this.externalHost?.status.mode==='playback')return;if(this.config.externalReplay&&this.externalHost.status.recording)this.externalHost.queue({type:'practice-options',options});else this.practice.configure(options);}
  assist(input){if(this.externalHost){if(this.externalHost.status.mode==='playback')return;this.externalHost.queue({type:'assist',input});}else this.config.applyReplayInput?.(input);}
  finishReplay(){this.externalHost?.finish();}
  fastForwardReplay(value){this.externalHost?.fastForward(value);}
  stopBrowserRecording(){this.externalHost?.disable();}
  pauseReplay(value){if(value)this.pause();else this.run();}
  addDiskImage(name,bytes){this.disks.set(name,new Uint8Array(bytes));}
  async setHdd(drive,name){
    if(drive!==0||!this.disks.has(name))throw Error('无效游戏磁盘');this.diskName=name;
    this.extracted=extractGame(this.disks.get(name));
    const network=!!this.config.lockstep,directLaunch=!!this.config.launchFiles?.some(file=>file.path==='game/HUMAROKU/WEBSTART.COM'),launcher=prepareFastBoot(this.extracted,{network,directLaunch});
    const init=[...this.extracted.files,...(this.config.launchFiles||[]),launcher,{dosboxConf:dosboxConfig({network,fastBoot:true}),jsdosConf:{version:'8'}}];
    this.ci=await globalThis.emulators.backend(init,this.transport,{});
    this.ci.events().onSoundPush(samples=>{this.module.audioPackets=(this.module.audioPackets||0)+1;const p=this.practice?.status;if(this.practiceStandby||this.externalHost?.status.fastForward||this.module.warmingBoot||this.config.independentEffects||p&&(p.preparing||p.waiting||p.outcome))return;if(this.config.workerRuntime){this.audioChunks.push(samples.slice());return;}if(this.frame!==null)this.outputs.get(this.frame)?.push(samples.slice());else this.audio.push([samples]);});
    this.logs=[];this.ci.events().onMessage((type,...messages)=>{this.logs.push([type,...messages]);this.logs=this.logs.slice(-40);if(type==='error'&&messages.some(s=>String(s).includes('[panic]'))){this.state='failed';this.config.onError?.(Error(messages.join(' ')));}});
    this.ci.events().onExit(()=>{if(this.disposed||this.resetting)return;this.state='exited';this.config.onExit?.();});
    for(const code of this.keys)this.ci.sendKeyEvent(codes[code],true);
    if(this.state!=='running'&&!this.config.lockstep)this.ci.pause();
    this.module.SDL2={audioContext:this.audio.context};
    if(!this.module.framePresenter)throw Error('DOSBox-X 直接呈现器未初始化，请刷新页面并检查运行资源版本');
  }
  getDiskImage(name){if(name===this.diskName&&this.extracted)return this.extracted.export(this.module.FS);const disk=this.disks.get(name);if(!disk)throw Error('未知磁盘');return disk.slice();}
  run(){this.state='running';this.externalHost?.pause(false);if(!this.config.lockstep)this.ci?.resume();this.audio.context.resume().catch(()=>{});}
  pause(){this.state='paused';this.externalHost?.pause(true);if(!this.config.lockstep)this.ci?.pause();this.module?.framePresenter?.resetTiming();this.audio.flush();}
  async reset(){
    const running=this.state==='running',diskName=this.diskName,disks=this.disks;
    this.resetting=true;this.pause();this.externalHost?.dispose();
    if(this.ci){const exiting=this.ci.exit();if(this.config.lockstep){for(let i=0;!this.ci.exited&&i<1000;i++){this.module.virtualNow=Math.max(this.module.virtualNow,this.module.wakeAt);this.module.advanceNative();}}await exiting;}
    this.dispose();this.state='loading';this.keys=new Set();this.outputs=new Map();this.frame=null;this.replaying=false;this.tick=0;this.audioChunks=[];this.snapshots=null;
    this.disposed=false;this.externalHost=null;await this.initialize();this.disks=disks;
    if(diskName)await this.setHdd(0,diskName);
    this.resetting=false;if(running)this.run();
  }
  step(){
    if(!this.config.lockstep||this.state!=='running')throw Error('DOSBox-X 没有进入同步运行状态');
    const m=this.module;this.tick++;m.targetNow=Math.max(m.virtualNow,Math.floor(this.tick*1000/60));
    let safety=0;
    while(m.wakeAt<=m.targetNow){if(++safety>1000)throw Error('DOSBox-X 同步步骤未让出执行');if(m.wakeAt>m.virtualNow)m.virtualNow=m.wakeAt;m.advanceNative();}
    m.virtualNow=m.targetNow;
    if(!m.pendingWake)throw Error('DOSBox-X 未在同步边界挂起');
  }
  readAudio(){if(!this.audioChunks.length)return null;const length=this.audioChunks.reduce((n,s)=>n+s.length,0),samples=new Float32Array(length);let at=0;for(const block of this.audioChunks.splice(0)){samples.set(block,at);at+=block.length;}return [samples];}
  dispose(){if(this.disposed)return;this.disposed=true;this.rejectPrepared?.(Error('练习实例已关闭'));this.externalHost?.disable();if(this.audio)this.pause();this.externalHost?.dispose();this.ci?.exit();this.module?.framePresenter?.dispose();this.audio?.disconnect();this.audio?.context.close().catch(()=>{});for(const type of ['keydown','keyup'])this.config.canvas.removeEventListener?.(type,this.onKey);if(typeof window!=='undefined')window.removeEventListener('message',this.onSleep);}
}
// Source imports keep their legacy class name, while every new instance uses DOSBox-X.
export {DOSBoxPC98 as NP21};
