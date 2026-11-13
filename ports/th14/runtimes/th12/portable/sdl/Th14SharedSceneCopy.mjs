// Capture the live C++ camera directly from shared Wasm memory. The worker
// writes only its private result buffer, so cancellation cannot write freed
// C++ owners or a successor's state. No per-word main-thread acknowledgements.
export const SceneCopyWords=Object.freeze({generation:0,command:1,status:2,copied:3,data:4,length:79});
const validLayout=offsets=>Array.isArray(offsets)&&offsets.length===75&&new Set(offsets).size===75&&
 offsets.every(value=>Number.isInteger(value)&&value>=0&&value<300&&(value&3)===0);
function validate(memory,source,offsets,result,generation){
 if(!(memory instanceof SharedArrayBuffer)||!(result instanceof SharedArrayBuffer)||
  result.byteLength!==SceneCopyWords.length*4||!Number.isInteger(generation)||generation<=0||generation>0x7fffffff||
  !Number.isInteger(source)||source<=0||source>0xffffffff||(source&3)||source+300>memory.byteLength||!validLayout(offsets))
  throw new Error('Invalid TH14 shared camera capture');
}
export function runSharedSceneCameraCopy({memory,source,offsets,result,generation}){
 validate(memory,source,offsets,result,generation);
 const w=SceneCopyWords,control=new Int32Array(result),output=new Uint32Array(result),input=new Uint32Array(memory);
 const owns=()=>Atomics.load(control,w.generation)===generation&&Atomics.load(control,w.command)===0;
 if(!owns()){Atomics.store(control,w.status,3);return 3;}
 Atomics.store(control,w.status,1);
 for(let index=0;index<75;++index){
  if(!owns()){Atomics.store(control,w.status,3);return 3;}
  const value=Atomics.load(input,(source+offsets[index])>>>2);
  Atomics.store(output,w.data+index,value);Atomics.store(control,w.copied,index+1);
 }
 const status=owns()?2:3;Atomics.store(control,w.status,status);return status;
}

export class Th14SharedSceneCopy {
 constructor({createWorker=url=>new Worker(url,{type:'module'}),onError=()=>{}}={}){
  this.createWorker=createWorker;this.onError=onError;this.current=null;this.generation=0;
 }
 fail(task,error){
  if(this.current!==task)return;
  task.failed=true;
  Atomics.store(task.state,SceneCopyWords.status,4);task.resolvePrepared(false);
  this.onError(error instanceof Error?error:new Error(String(error)));
 }
 prepare(){
  this.cancel();
  if(typeof SharedArrayBuffer!=='function'){this.onError(new Error('TH14 scene copy requires shared memory'));return false;}
  if(this.generation===0x7fffffff){this.onError(new Error('TH14 scene generation exhausted'));return false;}
  const generation=++this.generation,result=new SharedArrayBuffer(SceneCopyWords.length*4),state=new Int32Array(result);
  Atomics.store(state,SceneCopyWords.generation,generation);
  let resolvePrepared;const prepared=new Promise(resolve=>{resolvePrepared=resolve;});
  const task={generation,result,state,source:0,offsets:null,worker:null,prepared,resolvePrepared,failed:false};this.current=task;
  try{
   task.worker=this.createWorker(new URL('./Th14SharedSceneWorker.mjs',import.meta.url));
   task.worker.onmessage=event=>{
    if(this.current!==task)return;
    const message=event.data;
    if(message?.type==='prepared'){task.resolvePrepared(true);return;}
    if(message?.generation!==generation)return;
    if(message.error)this.fail(task,message.error);
    else if(message.status!==2&&message.status!==3)this.fail(task,'Unexpected TH14 scene worker status');
   };
   task.worker.onerror=event=>this.fail(task,event?.message??'TH14 scene worker failed');
   return true;
  }catch(error){this.fail(task,error);task.worker?.terminate();return false;}
 }
 read(memory,source,offsets,index,offset){
  const task=this.current,w=SceneCopyWords;
  if(!task||task.failed)return 2;
  if(!Number.isInteger(index)||index<0||index>=75){this.fail(task,'Invalid TH14 camera word');return 2;}
  if(task.source===0){
   try{
    if(index!==0)throw new Error('TH14 scene capture did not begin at word0');
    validate(memory,source,offsets,task.result,task.generation);
    task.source=source;task.offsets=offsets.slice();
    task.worker.postMessage({memory,source,offsets:task.offsets,result:task.result,generation:task.generation});
   }catch(error){this.fail(task,error);return 2;}
  }
  // A grown SharedArrayBuffer still aliases the original memory range. The
  // C++ source address remains stable; do not restart an in-flight capture.
  if(task.source!==source||task.offsets[index]!==offset){this.fail(task,'TH14 scene source owner changed');return 2;}
  const status=Atomics.load(task.state,w.status);
  return status===2?1:status===3||status===4?2:0;
 }
 value(index){
  const task=this.current,w=SceneCopyWords;
  if(!task||task.failed||!Number.isInteger(index)||index<0||index>=75||Atomics.load(task.state,w.status)!==2)
   throw new Error('TH14 camera result is not ready');
  return Atomics.load(new Uint32Array(task.result),w.data+index);
 }
 cancel(){
  const task=this.current;this.current=null;if(!task)return;
  Atomics.store(task.state,SceneCopyWords.command,1);task.resolvePrepared(false);task.worker?.terminate();
 }
}
