import {StageRestoreWords as w} from './Th14StageRestoreProtocol.mjs';

export class Th14StageRestoreDelivery {
 constructor({createWorker=url=>new Worker(url,{type:'module'}),onError=()=>{}}={}){
  this.createWorker=createWorker;this.onError=onError;this.current=null;this.generation=0;
 }
 begin(playback){
  this.cancel();
  let worker,task;
  try{
   if(typeof SharedArrayBuffer!=='function')throw new Error('TH14 independent stage restore requires SharedArrayBuffer');
   const buffer=new SharedArrayBuffer(w.length*4),state=new Int32Array(buffer);
   const generation=this.generation=this.generation===0x7fffffff?1:this.generation+1;
   const operations=playback?4:3;
   Atomics.store(state,w.generation,generation);Atomics.store(state,w.count,operations);
   worker=this.createWorker(new URL('Th14StageRestoreWorker.mjs',import.meta.url));
   task={worker,state,generation,operations};this.current=task;
   worker.onmessage=event=>{
    if(this.current!==task)return;
    if(event.data?.error||![2,3,4].includes(event.data?.status)){
     this.abort(task,new Error(event.data?.error??'TH14 stage restore returned invalid status'));return;
    }
    if(event.data.status===4)this.abort(task,new Error('TH14 stage restore producer failed'));
    else if(event.data.status===3&&Atomics.load(state,w.command)!==2)
     this.abort(task,new Error('TH14 stage restore unexpectedly cancelled'));
    else worker.terminate();
   };
   worker.onerror=event=>{if(this.current===task)this.abort(task,new Error(event.message??'TH14 stage restore worker error'));};
   worker.postMessage({buffer,generation,operations,minimumMs:60});
   Atomics.store(state,w.start,1);Atomics.notify(state,w.start);
   return true;
  }catch(error){
   if(task)this.abort(task,error);else{worker?.terminate();this.onError(error);}
   return false;
  }
 }
 abort(task,error){
  if(this.current!==task)return;
  Atomics.store(task.state,w.status,4);
  Atomics.store(task.state,w.command,2);Atomics.store(task.state,w.ack,-1);
  Atomics.notify(task.state,w.command);Atomics.notify(task.state,w.ack);Atomics.notify(task.state,w.start);
  task.worker.terminate();this.onError(error);
 }
 ready(operation){
  const task=this.current;
  if(!task||!Number.isInteger(operation)||operation<0||operation>=task.operations)return 2;
  const s=task.state;
  if(Atomics.load(s,w.generation)!==task.generation||Atomics.load(s,w.command)===2||Atomics.load(s,w.status)>=3||
     Atomics.load(s,w.ack)!==operation)return 2;
  return Atomics.load(s,w.ready)>=operation+1?1:0;
 }
 committed(operation){
  if(this.ready(operation)!==1)return false;
  const s=this.current.state;Atomics.store(s,w.ack,operation+1);Atomics.notify(s,w.ack);return true;
 }
 cancel(){
  const task=this.current;if(!task)return;
  this.current=null;
  Atomics.store(task.state,w.command,2);Atomics.store(task.state,w.ack,-1);
  Atomics.notify(task.state,w.command);Atomics.notify(task.state,w.ack);Atomics.notify(task.state,w.start);
  task.worker.terminate();
 }
}
