import {MAX_INPUT} from '../touch-input.js';
export const PROTOCOL='th02-rollback/6';
export const TICK_MS=1000/60;
export const BOOT_DELAY=2;
export const MAX_ROLLBACK=12;
export const MAX_INPUT_DELAY=12;
export const MAX_ROLLBACK_INPUT_DELAY=6;
export const HYBRID_RESERVE=2,PREDICTION_JITTER_STEPS=2;
export function rollbackRttForSamples(samples){
  if(!Array.isArray(samples)||samples.length!==3||samples.some(n=>!Number.isFinite(n)||n<0||n>30000))throw Error('无效开局延迟采样');
  return [...samples].sort((a,b)=>a-b)[1];
}
// Split transit into fixed local delay D and a bounded prediction reserve P.
// Keep direction persistence independent from the speculative work limit.
export function rollbackPolicyForRtt(rtt,rollbackEnabled=true){
  if(!Number.isFinite(rtt)||rtt<0||rtt>30000||typeof rollbackEnabled!=='boolean')throw Error('无效网络延迟');
  const rttMs=Math.ceil(rtt),transitSteps=rollbackEnabled&&rttMs>20?Math.ceil(rttMs/2/TICK_MS):0;
  const predictionSteps=Math.min(HYBRID_RESERVE,Math.max(0,transitSteps-1));
  const inputDelay=Math.min(MAX_ROLLBACK_INPUT_DELAY,transitSteps-predictionSteps);
  const predictionLead=rollbackEnabled?predictionSteps+PREDICTION_JITTER_STEPS:0;
  return {schema:2,rollbackEnabled,rttMs,transitSteps,inputDelay,predictionSteps,predictionLead};
}
export function validRollbackPolicy(policy){
  if(!policy)return false;
  try{const expected=rollbackPolicyForRtt(policy.rttMs,policy.rollbackEnabled);return Object.keys(policy).length===Object.keys(expected).length&&Object.entries(expected).every(([key,value])=>policy[key]===value);}
  catch{return false;}
}
export function rollbackInputDelayForRtt(rtt){return rollbackPolicyForRtt(rtt).inputDelay;}
// One-way transit plus one scheduling step. The three-player host includes
// all directed peer measurements; using a whole RTT doubles ordinary delay.
export function lockstepDelayForRtt(rtt){
  if(!Number.isFinite(rtt)||rtt<0)throw Error('无效网络延迟');
  return Math.min(MAX_INPUT_DELAY,Math.max(BOOT_DELAY,Math.ceil(rtt/2/TICK_MS)+1));
}
export const VALID_INPUT=MAX_INPUT;
const HELD=32|64|2048,DIRECTIONS=15;

// confirmed is an exclusive contiguous prefix of authoritative inputs, not
// the largest frame received. Prediction never advances it.
export class RollbackQueue {
  constructor(slot,players=2,{inputDelay=0,directionPrediction=3,predictionLead=directionPrediction,rollbackEnabled=true}={}){
    if(![2,3].includes(players)||!Number.isInteger(slot)||slot<0||slot>=players)throw Error('无效玩家槽位');
    // A longer rollback lead must be negotiated and prefilled before activation.
    if(!Number.isInteger(inputDelay)||inputDelay<0||inputDelay>MAX_ROLLBACK_INPUT_DELAY||
       !Number.isInteger(directionPrediction)||directionPrediction<0||directionPrediction>MAX_ROLLBACK||
       !Number.isInteger(predictionLead)||predictionLead<0||predictionLead>MAX_ROLLBACK||
       typeof rollbackEnabled!=='boolean')throw Error('无效预测策略');
    this.inputDelay=inputDelay;this.directionPrediction=directionPrediction;this.predictionLead=predictionLead;this.rollbackEnabled=rollbackEnabled;
    this.lockstepDelay=BOOT_DELAY;
    this.slot=slot;this.players=players;this.frame=0;this.confirmed=BOOT_DELAY;
    this.activation=null;this.dirty=null;this.skippedCorrections=0;this.inputs=Array.from({length:players},()=>new Map());
    this.used=new Map();
    this.offline=new Map();
    for(const lane of this.inputs)for(let n=0;n<BOOT_DELAY;n++)lane.set(n,0);
  }
  get active(){return this.activation!==null&&this.frame>=this.activation;}
  setLockstepDelay(delay){
    if(this.frame!==0||this.activation!==null||!Number.isInteger(delay)||delay<BOOT_DELAY||delay>MAX_INPUT_DELAY||this.inputs[this.slot].size!==BOOT_DELAY)throw Error('无效开局输入缓冲');
    this.lockstepDelay=delay;
    // Other peers may already have received run on a different connection.
    // Preserve their early packets rather than replacing the queue.
    for(const lane of this.inputs)for(let n=BOOT_DELAY;n<delay;n++){
      if(lane.has(n)&&lane.get(n)!==0)throw Error('开局输入缓冲冲突');
      lane.set(n,0);
    }
    this.updateConfirmed();
  }
  setInputDelay(delay){
    if(this.frame!==0||this.activation!==null||!Number.isInteger(delay)||delay<0||
       delay>MAX_ROLLBACK_INPUT_DELAY||delay>this.lockstepDelay)throw Error('无效回滚输入缓冲');
    this.inputDelay=delay;
  }
  setPredictionLead(lead){
    if(this.frame!==0||this.activation!==null||!Number.isInteger(lead)||lead<0||lead>MAX_ROLLBACK)throw Error('无效预测领先额度');
    this.predictionLead=lead;
  }
  arm(frame){
    if(!this.rollbackEnabled||this.activation!==null||!Number.isSafeInteger(frame)||frame<this.frame||frame>this.frame+240)throw Error('无效回滚启用帧');
    if(this.inputDelay>this.lockstepDelay)throw Error('回滚输入缓冲尚未预填');
    this.activation=frame;
  }
  capture(buttons,frontier=this.frame){
    // A repair can sample its old live frontier without assigning today's
    // physical input to a historical frame or changing a published packet.
    if(!Number.isSafeInteger(frontier)||frontier<this.frame||frontier>this.frame+MAX_ROLLBACK||
       (frontier!==this.frame&&!this.active))throw Error('无效输入采样前沿');
    const frame=frontier+(this.active?this.inputDelay:this.lockstepDelay),lane=this.inputs[this.slot];
    if(lane.has(frame))return null;
    if(!Number.isSafeInteger(buttons)||buttons<0||buttons>VALID_INPUT)throw Error('无效本机输入');
    const packet={type:'input',frame,slot:this.slot,buttons};
    lane.set(frame,packet.buttons);this.updateConfirmed();return packet;
  }
  predicted(slot,frame){
    if(this.isOffline(slot,frame))return 0;
    const lane=this.inputs[slot];
    if(lane.has(frame))return lane.get(frame);
    for(let age=1;age<=MAX_ROLLBACK;age++)if(lane.has(frame-age)){
      return lane.get(frame-age)&(HELD|(age<=this.directionPrediction?DIRECTIONS:0));
    }
    return 0;
  }
  receive(frame,buttons,slot){
    if(!Number.isInteger(slot)||slot<0||slot>=this.players||slot===this.slot||
       !Number.isSafeInteger(frame)||frame<0||frame>this.frame+120||
       !Number.isInteger(buttons)||buttons<0||buttons>VALID_INPUT)throw Error('收到越界输入');
    const lane=this.inputs[slot];
    if(this.isOffline(slot,frame))return;
    if(frame<Math.min(this.frame,this.confirmed)-32)return;
    if(lane.has(frame)){if(lane.get(frame)!==buttons)throw Error('同一帧收到冲突输入');return;}
    lane.set(frame,buttons);
    // A new matching input can extend the bounded direction prediction
    // horizon, so compare subsequent predictions too, not only this frame.
    for(const [at,pair]of this.used)if(at>=frame&&pair[slot]!==this.predicted(slot,at)){
      this.dirty=this.dirty===null?at:Math.min(this.dirty,at);break;
    }
    this.updateConfirmed();
  }
  refreshCorrection(force=false){
    if(this.dirty===null&&!force)return null;
    const correctionPending=this.dirty!==null;
    // Several packets can change a prediction and then restore the exact
    // input already simulated. Recheck the complete used history once, just
    // before restoring native state, rather than replaying a transient gap.
    this.dirty=null;
    for(const [at,pair]of this.used){
      if(pair.some((value,slot)=>value!==this.predicted(slot,at))){
        this.dirty=this.dirty===null?at:Math.min(this.dirty,at);
      }
    }
    if(this.dirty===null&&correctionPending)this.skippedCorrections++;
    return this.dirty;
  }
  isOffline(slot,frame){return this.offline.has(slot)&&frame>=this.offline.get(slot);}
  offlineMask(frame=this.frame){let mask=0;for(const [slot,at]of this.offline)if(frame>=at)mask|=1<<slot;return mask;}
  membershipReport(){
    const prefix=Math.min(this.frame,this.confirmed);
    return {frame:this.frame,prefix,inputs:this.inputs.map(lane=>[...lane].filter(([at])=>at>=prefix-32))};
  }
  applyDeparture({frame,slots,inputs}){
    if(!Number.isSafeInteger(frame)||frame<Math.min(this.frame,this.confirmed)||frame>this.frame+120||
       !Array.isArray(slots)||slots.includes(this.slot)||!Array.isArray(inputs)||inputs.length!==this.players)
      throw Error('无效离线同步边界');
    for(const slot of slots){
      if(!Number.isInteger(slot)||slot<0||slot>=this.players)throw Error('无效离线座位');
      this.offline.set(slot,frame);
      for(const at of this.inputs[slot].keys())if(at>=frame)this.inputs[slot].delete(at);
    }
    for(let slot=0;slot<this.players;slot++)for(const [at,value]of inputs[slot]){
      if(slot===this.slot){
        if(this.inputs[slot].get(at)!==value)throw Error('离线同步试图修改本机输入');
      }else this.receive(at,value,slot);
    }
    // Dropped lanes become authoritative neutral input, forever. Their last
    // uncertain pre-boundary steps are supplied by the host's merged report.
    this.updateConfirmed();
  }
  updateConfirmed(){while(this.inputs.every((lane,slot)=>this.isOffline(slot,this.confirmed)||lane.has(this.confirmed)))this.confirmed++;}
  peek(allowPrediction=true){
    this.waitReason='';
    if(this.active&&this.frame-this.confirmed>=MAX_ROLLBACK){this.waitReason='window';return null;}
    if(this.inputs.every((lane,slot)=>this.isOffline(slot,this.frame)||lane.has(this.frame)))return this.inputs.map((lane,slot)=>this.isOffline(slot,this.frame)?0:lane.get(this.frame));
    if(!this.active){this.waitReason='boot';return null;}
    if(!allowPrediction){this.waitReason='prediction-disabled';return null;}
    if(!this.inputs[this.slot].has(this.frame)){this.waitReason='local-input';return null;}
    return this.inputs.map((_,slot)=>this.predicted(slot,this.frame));
  }
  commit(pair){this.used.set(this.frame,pair.slice());this.frame++;}
  rewind(frame){
    this.frame=frame;this.dirty=null;
    for(const at of this.used.keys())if(at>=frame)this.used.delete(at);
  }
  prune(confirmedPrefix=this.confirmed){
    const prefix=Math.min(this.frame,this.confirmed,confirmedPrefix);
    for(const at of this.used.keys())if(at<prefix)this.used.delete(at);
    for(const lane of this.inputs)for(const at of lane.keys())if(at<prefix-32)lane.delete(at);
  }
}
