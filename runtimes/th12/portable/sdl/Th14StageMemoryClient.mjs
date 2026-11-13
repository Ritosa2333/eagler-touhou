// Physical C++ stores on shared Wasm memory. A terminal acknowledgement is a
// lifetime boundary: cancellation must join before any pointed-to owner dies.
export const StageMemoryWords=Object.freeze({generation:0,command:1,status:2,packet:3,stack:4,playback:5,steps:6,length:7});
export const stageMemoryTerminal=status=>status===3||status===4||status===5;

export function inspectStageMemoryKernel(bytes){
 bytes=bytes instanceof Uint8Array?bytes:new Uint8Array(bytes);
 if(bytes.length<8||![0,97,115,109,1,0,0,0].every((v,i)=>bytes[i]===v))throw new Error('Invalid stage kernel');
 let at=8,memoryType=null;
 const number=()=>{let value=0,shift=0,byte;do{if(at>=bytes.length||shift>28)throw new Error('Invalid Wasm integer');
  byte=bytes[at++];value|=(byte&127)<<shift;shift+=7;}while(byte&128);return value>>>0;};
 const name=()=>{const length=number(),end=at+length;if(end>bytes.length)throw new Error('Invalid Wasm name');
  const value=new TextDecoder().decode(bytes.subarray(at,end));at=end;return value;};
 while(at<bytes.length){
  const section=bytes[at++],size=number(),end=at+size;if(end>bytes.length)throw new Error('Invalid Wasm section');
  if(section===8)throw new Error('Stage kernel must not run a start function');
  if(section===11&&number()!==0)throw new Error('Stage kernel must not initialize shared data');
  if(section===2){
   if(number()!==1||name()!=='env'||name()!=='memory'||bytes[at++]!==2)throw new Error('Stage kernel imports memory only');
   const flags=number(),minimum=number(),maximum=number();
   if(flags!==3||minimum>16384||maximum!==16384)throw new Error('Invalid shared stage memory limits');
   memoryType={minimum,maximum};
  }
  if(at>end)throw new Error('Invalid stage kernel section contents');at=end;
 }
 if(!memoryType)throw new Error('Missing shared stage memory import');return memoryType;
}

function validatePacket(memory,packet,stack,playback,api){
 const length=memory.buffer.byteLength;
 const range=(pointer,size,alignment=4)=>Number.isInteger(pointer)&&pointer>0&&(pointer&(alignment-1))===0&&pointer+size<=length;
 if(api.stage_memory_packet_size()!==40||!range(packet,40)||!range(stack-65536,65536,16))throw new Error('Invalid stage packet or private stack');
 const p=Array.from(new Uint32Array(memory.buffer,packet,10));
 const regions=[[packet,40],[stack-65536,65536],[p[0],8],[p[1],8]];
 if(playback)regions.push([p[2],api.stage_memory_entry_size()],[p[4],api.stage_memory_economy_size()],[p[5],4],[p[6],4],[p[7],4],[p[8],4],[p[9],4]);
 else regions.push([p[3],4]);
 for(const [pointer,size]of regions)if(!range(pointer,size))throw new Error('Invalid stage target range');
 for(let i=0;i<regions.length;++i)for(let j=0;j<i;++j){
  const [a,as]=regions[i],[b,bs]=regions[j];if(a<b+bs&&b<a+as)throw new Error('Stage targets overlap');
 }
}

export async function runStageMemoryTask({module,memory,buffer,generation,signature},prepared=()=>{}){
 const w=StageMemoryWords;
 if(!(module instanceof WebAssembly.Module)||!(memory instanceof WebAssembly.Memory)||!(memory.buffer instanceof SharedArrayBuffer)||
  !(buffer instanceof SharedArrayBuffer)||buffer.byteLength!==w.length*4||!Number.isInteger(generation)||generation<=0)
  throw new Error('Invalid physical stage worker task');
 const state=new Int32Array(buffer),owns=()=>Atomics.load(state,w.generation)===generation;
 const finish=status=>{Atomics.store(state,w.status,status);Atomics.notify(state,w.status);return status;};
 try{
  if(!owns()||Atomics.load(state,w.command)===2)return finish(4);
  const instance=await WebAssembly.instantiate(module,{env:{memory}}),api=instance.exports;
  if((api.stage_memory_layout()>>>0)!==(signature>>>0))throw new Error('Stage C++ layout mismatch');
  Atomics.store(state,w.status,1);Atomics.notify(state,w.status);prepared();
  while(owns()&&Atomics.load(state,w.command)===0)Atomics.wait(state,w.command,0);
  if(!owns()||Atomics.load(state,w.command)===2)return finish(4);
  const packet=Atomics.load(state,w.packet)>>>0,stack=Atomics.load(state,w.stack)>>>0,playback=Atomics.load(state,w.playback);
  if(playback!==0&&playback!==1)throw new Error('Invalid stage mode');
  validatePacket(memory,packet,stack,playback!==0,api);api.__stack_pointer.value=stack;
  Atomics.store(state,w.status,2);Atomics.notify(state,w.status);
  if(!owns()||Atomics.load(state,w.command)===2)return finish(4);
  // No main-thread ACK, timers, frame numbers or observations between stores.
  // A cancel racing this call joins it; it does not free its objects early.
  if(playback)api.stage_memory_playback(packet);else api.stage_memory_record(packet);
  Atomics.store(state,w.steps,playback?4:3);return finish(3);
 }catch(error){finish(5);throw error;}
}

export class Th14StageMemoryClient {
 constructor({kernelBytes=null,createWorker=url=>new Worker(url,{type:'module'}),onError=()=>{}}={}){
  this.kernelBytes=kernelBytes;this.createWorker=createWorker;this.onError=onError;this.module=null;
  this.signature=null;this.opening=null;this.current=null;this.generation=0;
 }
 async open(expected){
  expected>>>=0;
  if(!this.opening)this.opening=(async()=>{
   const bytes=this.kernelBytes??new Uint8Array(await (await fetch(new URL('./Th14StageMemoryWorker.wasm',import.meta.url))).arrayBuffer());
   const limits=inspectStageMemoryKernel(bytes),module=await WebAssembly.compile(bytes);
   const imports=WebAssembly.Module.imports(module);
   if(imports.length!==1||imports[0].kind!=='memory'||imports[0].module!=='env'||imports[0].name!=='memory')throw new Error('Unexpected stage kernel imports');
   const testMemory=new WebAssembly.Memory({initial:limits.minimum,maximum:limits.maximum,shared:true});
   const api=(await WebAssembly.instantiate(module,{env:{memory:testMemory}})).exports;
   for(const name of ['stage_memory_layout','stage_memory_playback','stage_memory_record','stage_memory_packet_size','stage_memory_entry_size','stage_memory_economy_size'])
    if(typeof api[name]!=='function')throw new Error('Missing stage kernel export: '+name);
   if(!(api.__stack_pointer instanceof WebAssembly.Global)||api.stage_memory_packet_size()!==40)throw new Error('Invalid stage kernel ABI');
   api.__stack_pointer.value=api.__stack_pointer.value;
   const signature=api.stage_memory_layout()>>>0;if(signature!==expected)throw new Error('Stage C++ layout mismatch');
   this.module=module;this.signature=signature;
  })();
  await this.opening;if(this.signature!==expected)throw new Error('Stage client layout changed');
 }
 fail(task,error,noWriter=false){
  if(!task.failed){task.failed=true;task.error=String(error);if(this.current===task)this.onError(error instanceof Error?error:new Error(String(error)));}
  Atomics.store(task.state,StageMemoryWords.command,2);Atomics.notify(task.state,StageMemoryWords.command);
  if(noWriter&&!stageMemoryTerminal(Atomics.load(task.state,StageMemoryWords.status))){
   Atomics.store(task.state,StageMemoryWords.status,5);Atomics.notify(task.state,StageMemoryWords.status);
  }
  task.resolvePrepared(false);
 }
 prepare(memory,expected){
  const w=StageMemoryWords;
  if(!this.module||this.signature!==(expected>>>0)||!(memory instanceof WebAssembly.Memory)||!(memory.buffer instanceof SharedArrayBuffer))return false;
  if(this.current&&!stageMemoryTerminal(Atomics.load(this.current.state,w.status)))return false;
  if(this.generation===0x7fffffff)return false;
  const buffer=new SharedArrayBuffer(w.length*4),state=new Int32Array(buffer),generation=++this.generation;
  Atomics.store(state,w.generation,generation);
  let resolvePrepared;const prepared=new Promise(resolve=>{resolvePrepared=resolve;});
  const task={generation,buffer,state,memory,worker:null,packet:0,stack:0,playback:false,failed:false,prepared,resolvePrepared};this.current=task;
  try{
   task.worker=this.createWorker(new URL('./Th14StageMemoryWorker.mjs',import.meta.url));
   task.worker.onmessage=event=>{
    const message=event.data;if(message?.generation!==generation)return;
    if(message.type==='prepared'){task.resolvePrepared(true);return;}
    if(stageMemoryTerminal(message.status))task.resolvePrepared(false);
    if(message.error)this.fail(task,message.error);
   };
   // An uncaught worker error has unwound its only kernel call, or failed
   // before that call could start. It cannot issue another C++ write.
   task.worker.onerror=event=>this.fail(task,event?.message??'Stage worker failed',true);
   task.worker.postMessage({module:this.module,memory,buffer,generation,signature:this.signature});
   return true;
  }catch(error){this.fail(task,error,true);task.worker?.terminate();return false;}
 }
 poll(packet,stack,playback){
  const task=this.current,w=StageMemoryWords;playback=!!playback;
  if(!task||task.failed)return 2;
  if(!task.packet){
   if(!Number.isInteger(packet)||packet<=0||(packet&3)||packet+40>task.memory.buffer.byteLength||
    !Number.isInteger(stack)||stack<65536||(stack&15)||stack>task.memory.buffer.byteLength){this.fail(task,'Invalid stage packet binding');return 2;}
   task.packet=packet;task.stack=stack;task.playback=playback;
   Atomics.store(task.state,w.packet,packet);Atomics.store(task.state,w.stack,stack);Atomics.store(task.state,w.playback,playback?1:0);
   Atomics.store(task.state,w.command,1);Atomics.notify(task.state,w.command);
  }else if(task.packet!==packet||task.stack!==stack||task.playback!==playback){this.fail(task,'Stage worker owner changed');return 2;}
  const status=Atomics.load(task.state,w.status);
  return status===3?1:status===4||status===5?2:0;
 }
 requestStop(task){Atomics.store(task.state,StageMemoryWords.command,2);Atomics.notify(task.state,StageMemoryWords.command);}
 async cancelAndJoin(){
  const task=this.current;if(!task)return;this.requestStop(task);
  while(!stageMemoryTerminal(Atomics.load(task.state,StageMemoryWords.status)))await new Promise(resolve=>setTimeout(resolve,1));
  task.resolvePrepared(false);
  if(this.current===task)this.current=null;
 }
 cancelAndJoinSync(){
  const task=this.current;if(!task)return;this.requestStop(task);
  for(;;){const status=Atomics.load(task.state,StageMemoryWords.status);if(stageMemoryTerminal(status))break;
   // Node/WASI host only. The browser adapter uses the async join above.
   Atomics.wait(task.state,StageMemoryWords.status,status,50);
  }
  task.resolvePrepared(false);
  if(this.current===task)this.current=null;
 }
}
