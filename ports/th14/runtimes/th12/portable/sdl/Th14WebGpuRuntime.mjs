import {Th14WebGpuHost} from './Th14WebGpuHost.mjs';
import {bindTh14WebGpuCore} from './Th14WebGpuCore.mjs';
import {ArchiveDecodeClient} from './Th14ArchiveDecodeClient.mjs';
import {ArchiveReadAhead} from './Th14ArchiveReadAhead.mjs';
import {Th14StageRestoreDelivery} from './Th14StageRestoreDelivery.mjs';
import {Th14SharedSceneCopy} from './Th14SharedSceneCopy.mjs';
import {Th14StageMemoryClient} from './Th14StageMemoryClient.mjs';

// Only one suspended Wasm stack may exist. Frame and loading callbacks share
// this queue; the next RAF is requested after the preceding callback returns.
export class Th14WebGpuRuntime {
  constructor(module,{createHost,requestFrame=globalThis.requestAnimationFrame?.bind(globalThis),setTimer=globalThis.setTimeout.bind(globalThis),createArchiveWorker=url=>new Worker(url,{type:'module'})}={}) {
    this.module=module;this.createHost=createHost??(()=>Th14WebGpuHost.create({canvas:module.canvas,onError:error=>this.fail(error)}));
    this.requestFrame=requestFrame;this.setTimer=setTimer;this.tail=Promise.resolve();this.failure=null;this.host=null;this.opening=null;
    this.completedCallbacks=0;this.pendingCallbacks=0;
    this.createArchiveWorker=createArchiveWorker;this.archive=null;this.archiveOpening=null;
    this.stageRestore=new Th14StageRestoreDelivery({onError:error=>this.fail(error)});
    this.sceneCopy=new Th14SharedSceneCopy({onError:error=>this.fail(error)});
    this.stageMemory=new Th14StageMemoryClient({onError:error=>this.fail(error)});
  }
  fail(error) {if (!this.failure) {this.failure=error;this.module.onWebGpuError?.(error);}}
  run(action) {
    ++this.pendingCallbacks;
    const result=this.tail.then(()=>{if(this.failure)throw this.failure;return action();});
    this.tail=result.catch(error=>this.fail(error)).finally(()=>{--this.pendingCallbacks;++this.completedCallbacks;});
    return result;
  }
  async open() {
    if (!this.opening) this.opening=Promise.resolve().then(this.createHost).then(host=>this.host=host)
      .catch(error=>{this.fail(error);throw error;});
    await this.opening;
  }
  call(name,returnType,types,args) {return this.module.ccall(name,returnType,types,args,{async:true});}
  bindCore(raw) {return bindTh14WebGpuCore(this.module,raw);}
  openArchive(bytes) {
    if(this.archive)throw new Error('Archive decoder already opened');
    const decoder=new ArchiveDecodeClient(this.createArchiveWorker(new URL('Th14ArchiveDecodeWorker.mjs',import.meta.url)));
    // open() copies/transfers before its first await. Never retain a HEAP view
    // across memory growth, and never transfer the game's original DAT owner.
    const opening=decoder.open(bytes);this.archiveOpening=opening;
    void opening.catch(()=>{});
    this.archive=new ArchiveReadAhead({
      decode:async names=>{await opening;return decoder.decode(names);},
      close:()=>decoder.close(),
    },this);
  }
  cancelArchive() {this.archive?.cancel();}
  archiveStatus() {return this.archive?.batch?.error?0:1;}
  archiveSize(name) {
    try{return this.archive?.size(name)??-1;}catch{return -2;}
  }
  archiveCopy(name,destination) {
    try{return this.archive?.copy(name,destination)===destination.byteLength?1:0;}catch{return 0;}
  }
  archiveAnmSize(name) {
    try{return this.archive?.anmSize(name)??-1;}catch{return -2;}
  }
  archiveAnmCopy(name,destination) {
    try{return this.archive?.anmCopy(name,destination)===destination.byteLength?1:0;}catch{return 0;}
  }
  archiveParsedSize(kind,name) {
    try{if(kind!==1&&kind!==2&&kind!==3)return -2;return this.archive?.parsedSize(kind===1?'std':kind===2?'ecl':'sht',name)??-1;}catch{return -2;}
  }
  archiveParsedCopy(kind,name,destination) {
    try{if(kind!==1&&kind!==2&&kind!==3)return 0;return this.archive?.parsedCopy(kind===1?'std':kind===2?'ecl':'sht',name,destination)===destination.byteLength?1:0;}catch{return 0;}
  }
  scheduleArchive(callback,data,names) {
    const invoke=()=>this.call('th14_webgpu_invoke',null,['number','number'],[callback,data]);
    if(!this.archive)throw new Error('Archive decoder is unavailable');
    const job=this.archive.schedule(names,invoke);
    void job.delivered.catch(error=>this.fail(error));
    return job;
  }
  schedule(callback,data,delay) {
    const deliver=()=>{void this.run(()=>this.call('th14_webgpu_invoke',null,['number','number'],[callback,data])).catch(()=>{});};
    if (delay<0) {
      if (!this.requestFrame) throw new Error('requestAnimationFrame unavailable');
      this.requestFrame(deliver);
    } else this.setTimer(deliver,delay);
  }
  startFrames(callback,data) {
    if (!this.requestFrame) throw new Error('requestAnimationFrame unavailable');
    const tick=time=>{
      void this.run(()=>this.call('th14_webgpu_frame','number',['number','number','number'],[callback,time,data]))
        .then(again=>{if(again&&!this.failure)this.requestFrame(tick);},()=>{});
    };
    return this.requestFrame(tick);
  }
}
