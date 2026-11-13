import {runStageMemoryTask,StageMemoryWords} from './Th14StageMemoryClient.mjs';
self.onmessage=async event=>{
 self.onmessage=null;const task=event.data;
 try{
  const status=await runStageMemoryTask(task,()=>self.postMessage({generation:task.generation,type:'prepared'}));
  self.postMessage({generation:task.generation,status});
 }catch(error){
  if(task?.buffer instanceof SharedArrayBuffer&&task.buffer.byteLength===StageMemoryWords.length*4){
   const state=new Int32Array(task.buffer);Atomics.store(state,StageMemoryWords.status,5);Atomics.notify(state,StageMemoryWords.status);
  }
  self.postMessage({generation:task?.generation,error:String(error)});
 }finally{self.close();}
};
