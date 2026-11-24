// Owns a single epoch of simulation. The host owns transport, presentation and
// engine lifetime. Engine callbacks run synchronously, without external effects.
const frameLimit=0x7fffffff;
// A seat can have at most its scheduled input delay plus one sampled frame
// beyond its current World. Leave room for that tail, never replay a backlog.
const restartCatchUpLimit=16;
export const completeConfirmedRound=Symbol('complete confirmed round');
function sync(value,label){
  if(value && typeof value.then==='function')throw new TypeError(`${label} must be synchronous`);
  return value;
}
function confirmedRow(row){
  return Object.freeze({frame:row.frame,inputs:Object.freeze(row.inputs.map(input=>Object.freeze({...input,predicted:false})))});
}

export function createMultiplayerRound({session,localPlayer,engine,sendInput,onConfirmed=()=>{},maxFrames=frameLimit}){
  if(localPlayer!==0&&localPlayer!==1)throw new TypeError('Invalid local seat');
  if(!Number.isSafeInteger(maxFrames)||maxFrames<1||maxFrames>frameLimit)throw new RangeError('Invalid round frame limit');
  if(!engine||['checkpoint','restore','step'].some(name=>typeof engine[name]!=='function')||
    (engine.release!==undefined&&typeof engine.release!=='function')||
    typeof sendInput!=='function'||typeof onConfirmed!=='function')throw new TypeError('Round requires synchronous engine and host callbacks');
  const {epoch,rollbackWindow}=session.rules;
  if(session.inputStatus().nextFrame!==0)throw new Error('Round must adopt a fresh epoch at frame zero');
  const pending=new Map();
  let queued=-1,nextCommit=0,replayedFrames=0,closed=false,busy=false,fault=null,restartFence=null,completed=false;
  const status=()=>Object.freeze({epoch,committedThrough:nextCommit-1,replayedFrames,pendingFrames:pending.size,closed,fault});
  const active=()=>{
    if(closed||session.rules.epoch!==epoch||session.status().closed)throw new Error('Round epoch is no longer active');
    if(fault!==null)throw new Error(`Round stopped: ${fault}`);
    if(busy)throw new Error('Reentrant round callback');
  };
  function release(record){
    if(record?.checkpoint!==undefined)sync(engine.release?.(record.checkpoint),'Checkpoint release');
  }
  function simulate(row,confirmedThrough){
    const speculative=row.frame>confirmedThrough;
    const checkpoint=speculative?sync(engine.checkpoint(),'Checkpoint'):undefined;
    if(speculative&&checkpoint===undefined)throw new Error('Speculative frame requires an engine checkpoint');
    try{
      // Prediction is scheduling metadata. The native step gets identical data
      // for the same held inputs whether first run or re-simulated.
      const effects=sync(engine.step(confirmedRow(row)),'Engine step');
      pending.set(row.frame,{row,effects,checkpoint});
    }catch(error){release({checkpoint});throw error;}
  }
  function settle(){
    if(completed)return;
    const {confirmedThrough}=session.inputStatus();
    const plan=session.prepareCorrection();
    if(plan){
      const base=pending.get(plan.fromFrame);
      if(plan.fromFrame<nextCommit||base?.checkpoint===undefined)throw new Error('Correction has no live engine checkpoint');
      sync(engine.restore(base.checkpoint),'Engine restore');
      for(const row of plan.frames){
        const old=pending.get(row.frame);
        pending.delete(row.frame);release(old);
        simulate(row,confirmedThrough);
      }
      session.acceptCorrection(plan);
      replayedFrames+=plan.frames.length;
    }
    const through=Math.min(confirmedThrough,session.inputStatus().nextFrame-1,restartFence===null?frameLimit:restartFence-1);
    while(nextCommit<=through){
      const record=pending.get(nextCommit);
      if(!record)throw new Error('Confirmed history has a gap');
      const result=sync(onConfirmed(confirmedRow(record.row),record.effects),'Confirmation consumer');
      pending.delete(nextCommit++);release(record);
      if(result===completeConfirmedRound){
        // Both native seats are held at Game Over. A later speculative row can
        // have consumed a logical slot, but is not part of the completed tape.
        completed=true;
        for(const suffix of pending.values())release(suffix);
        pending.clear();return;
      }
    }
  }
  function operate(callback){
    active();busy=true;
    try{return callback();}
    catch(error){fault=error instanceof Error?error.message:String(error);throw error;}
    finally{busy=false;}
  }
  function tick(held,{allowPrediction=true}={}){
    return operate(()=>{
      settle();
      if(completed)return {advanced:false,reason:'round-completed'};
      const {phase}=session.status(),{nextFrame}=session.inputStatus();
      if(phase==='preparing'||phase==='ready')return {advanced:false,reason:'restart-preparing'};
      if(nextFrame>=maxFrames)return {advanced:false,reason:'round-limit'};
      if(queued!==nextFrame){
        const {packet}=session.queueLocal(typeof held==='function'?held():held);
        queued=nextFrame;sync(sendInput(packet),'Input transport');
      }
      const {confirmedThrough}=session.inputStatus();
      if(nextFrame>confirmedThrough+(allowPrediction?rollbackWindow:0))return {advanced:false,reason:'waiting-input'};
      const row=session.consume();simulate(row,confirmedThrough);settle();
      return {advanced:true,frame:row.frame};
    });
  }
  function receiveInput(packet,sourcePlayer){
    active();
    if(sourcePlayer!==1-localPlayer)throw new Error('Input must come from the bound remote seat');
    return session.receiveInput(packet,sourcePlayer);
  }
  function freezeForRestart(){
    return operate(()=>{
      if(restartFence!==null)throw new Error('Restart frame boundary is already frozen');
      settle();return restartFence=nextCommit;
    });
  }
  function finishRestartAt(frameCount){
    active();
    if(restartFence===null||!Number.isSafeInteger(frameCount)||frameCount<restartFence||frameCount>maxFrames||
      frameCount-restartFence>restartCatchUpLimit)throw new Error('Restart tail exceeds the bounded catch-up window');
    const {confirmedThrough}=session.inputStatus();
    if(confirmedThrough+1<frameCount)return false;
    return operate(()=>{
      restartFence=frameCount;settle();
      while(!completed&&session.inputStatus().nextFrame<frameCount)simulate(session.consumeRestartFrame(),confirmedThrough);
      settle();if(nextCommit!==frameCount)throw new Error('Restart tail lacks continuous confirmed frames');
      return true;
    });
  }
  function resumeAfterRestart(){restartFence=null;}
  function close(){
    if(closed)return;if(busy)throw new Error('Reentrant round callback');
    closed=true;
    try{for(const record of pending.values())release(record);}finally{pending.clear();}
  }
  return Object.freeze({status,tick,receiveInput,flush:()=>operate(()=>{settle();return status();}),close,
    freezeForRestart,finishRestartAt,resumeAfterRestart});
}
