// Data-free readiness transport. Only the main C++ owner writes game state.
// Each task owns its buffer. Cancellation cannot expose a successor's buffer.
export const StageRestoreWords=Object.freeze({generation:0,command:1,ready:2,ack:3,status:4,start:5,count:6,length:7});
export function runStageRestoreProducer({buffer,generation,operations,minimumMs=60},terminal=()=>{}) {
 const w=StageRestoreWords,state=new Int32Array(buffer);
 if(state.length!==w.length||!Number.isInteger(generation)||generation<=0||
    ![3,4].includes(operations)||!Number.isFinite(minimumMs)||minimumMs<0||minimumMs>60000)
  throw new Error('Invalid TH14 stage restore task');
 const owns=()=>Atomics.load(state,w.generation)===generation&&Atomics.load(state,w.command)!==2;
 const finish=status=>{Atomics.store(state,w.status,status);terminal(status);return status;};
 while(owns()&&Atomics.load(state,w.start)===0)Atomics.wait(state,w.start,0);
 if(!owns())return finish(3);
 if(Atomics.load(state,w.count)!==operations)return finish(4);
 Atomics.store(state,w.status,1);
 // A timer wake may be spurious; preserve the established loader lower bound.
 const until=performance.now()+minimumMs;
 while(owns()){
  const remaining=until-performance.now();if(remaining<=0)break;
  Atomics.wait(state,w.command,0,remaining);
 }
 for(let operation=1;operation<=operations&&owns();++operation){
  Atomics.store(state,w.ready,operation);Atomics.notify(state,w.ready);
  while(owns()){
   const acknowledged=Atomics.load(state,w.ack);
   if(acknowledged>=operation)break;
   // No game-time timeout: a hidden or paused page may stop its main loop.
   Atomics.wait(state,w.ack,acknowledged);
  }
 }
 return finish(owns()?2:3);
}
