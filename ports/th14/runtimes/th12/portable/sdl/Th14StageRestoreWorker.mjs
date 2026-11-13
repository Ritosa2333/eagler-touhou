import {runStageRestoreProducer} from './Th14StageRestoreProtocol.mjs';
self.onmessage=event=>{
 try{runStageRestoreProducer(event.data,status=>self.postMessage({status}));}
 catch(error){self.postMessage({error:String(error)});}
 finally{self.close();}
};
