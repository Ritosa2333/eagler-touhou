import {runSharedSceneCameraCopy} from './Th14SharedSceneCopy.mjs';
self.onmessage=event=>{
 const task=event.data;
 try{const status=runSharedSceneCameraCopy(task);self.postMessage({generation:task.generation,status});}
 catch(error){self.postMessage({generation:task?.generation,error:String(error)});}
 finally{self.close();}
};
self.postMessage({type:'prepared'});
