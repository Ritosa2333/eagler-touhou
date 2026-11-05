export const BOOT_DELAY=2,INPUT_DELAY=0,TICK_MS=1000/60,MAX_ROLLBACK=12;
export const DIRECTION_PREDICTION=6;
export const neutral=()=>({actions:[],touch:0,focusEnabled:true,points:true,commands:[]});
const HELD=new Set(['shot','rapid','attack','focus']);
const DIRECTIONS=new Set(['up','down','left','right']);
export function validInput(input){
  return input&&Array.isArray(input.actions)&&input.actions.every(a=>['up','down','left','right','shot','rapid','attack','focus'].includes(a))
    &&Number.isSafeInteger(input.touch)&&input.touch>=0&&input.touch<2**45
    &&typeof input.focusEnabled==='boolean'&&typeof input.points==='boolean'
    &&Array.isArray(input.commands)&&input.commands.length<=16
    &&input.commands.every(c=>['pause','resume','round','match','up','down','confirm','escape','hidden','visible'].includes(c));
}
const same=(a,b)=>JSON.stringify(a)===JSON.stringify(b);
export class FrameQueue {
  constructor(slot,{rollbackEnabled=false,directionPrediction=DIRECTION_PREDICTION,predictionLead=directionPrediction,inputDelay=INPUT_DELAY}={}){
    if(![0,1].includes(slot)||typeof rollbackEnabled!=='boolean'||!Number.isInteger(directionPrediction)||directionPrediction<0||directionPrediction>MAX_ROLLBACK||!Number.isInteger(inputDelay)||inputDelay<0||inputDelay>6||(!rollbackEnabled&&inputDelay!==0))throw Error('无效预测策略');
    if(!Number.isInteger(predictionLead)||predictionLead<0||predictionLead>MAX_ROLLBACK)throw Error('无效预测领先额度');
    // Choose before sampling. Starting with at least this much neutral input
    // avoids a hole when the future activation switches to rollback timing.
    Object.defineProperties(this,{inputDelay:{value:inputDelay,enumerable:true},predictionLead:{value:predictionLead,enumerable:true},bootDelay:{value:Math.max(BOOT_DELAY,inputDelay),enumerable:true}});
    this.slot=slot;this.rollbackEnabled=rollbackEnabled;this.directionPrediction=directionPrediction;this.frame=0;this.confirmed=this.bootDelay;
    this.activation=null;this.dirty=null;this.waitReason='';this.inputs=[new Map(),new Map()];this.used=new Map();
    for(const lane of this.inputs)for(let n=0;n<this.bootDelay;n++)lane.set(n,neutral());
  }
  get active(){return this.rollbackEnabled&&this.activation!==null&&this.frame>=this.activation;}
  arm(frame){
    if(!this.rollbackEnabled||this.activation!==null||!Number.isSafeInteger(frame)||frame<this.frame||frame>this.frame+240)throw Error('无效回滚启用帧');
    this.activation=frame;
  }
  needsCapture(frontier=this.frame){const frame=frontier+(this.active?this.inputDelay:this.bootDelay);return !this.inputs[this.slot].has(frame);}
  capture(input,frontier=this.frame){
    if(!Number.isSafeInteger(frontier)||frontier<this.frame||frontier>this.frame+MAX_ROLLBACK||
       (frontier!==this.frame&&!this.active))throw Error('无效输入采样前沿');
    const frame=frontier+(this.active?this.inputDelay:this.bootDelay),lane=this.inputs[this.slot];
    if(lane.has(frame))return null;
    if(!validInput(input))throw Error('本机帧输入无效');
    lane.set(frame,structuredClone(input));this.updateConfirmed();return {type:'frame',frame,input:structuredClone(input)};
  }
  predicted(slot,frame){
    const lane=this.inputs[slot];if(lane.has(frame))return lane.get(frame);
    for(let age=1;age<=MAX_ROLLBACK;age++)if(lane.has(frame-age)){
      const source=lane.get(frame-age);
      return {...source,touch:0,actions:source.actions.filter(action=>HELD.has(action)||(age<=this.directionPrediction&&DIRECTIONS.has(action))),commands:[]};
    }
    return neutral();
  }
  receive(frame,input){
    if(!Number.isSafeInteger(frame)||frame<0||frame>this.frame+120||!validInput(input))throw Error('收到越界输入');
    if(frame<Math.min(this.frame,this.confirmed)-32)return;
    const lane=this.inputs[this.slot^1];
    if(lane.has(frame)){if(!same(lane.get(frame),input))throw Error('同一帧收到冲突输入');return;}
    lane.set(frame,structuredClone(input));
    for(const [at,pair] of this.used)if(at>=frame&&!same(pair[this.slot^1],this.predicted(this.slot^1,at))){this.dirty=this.dirty===null?at:Math.min(this.dirty,at);break;}
    this.updateConfirmed();
  }
  updateConfirmed(){while(this.inputs.every(lane=>lane.has(this.confirmed)))this.confirmed++;}
  peek(allowPrediction=true){
    this.waitReason='';
    if(this.active&&this.frame-this.confirmed>=MAX_ROLLBACK){this.waitReason='window';return null;}
    if(this.inputs.every(lane=>lane.has(this.frame)))return this.inputs.map(lane=>lane.get(this.frame));
    if(!this.active){this.waitReason='boot';return null;}
    if(!allowPrediction){this.waitReason='prediction-disabled';return null;}
    if(!this.inputs[this.slot].has(this.frame)){this.waitReason='local-input';return null;}
    return this.inputs.map((lane,slot)=>lane.has(this.frame)?lane.get(this.frame):this.predicted(slot,this.frame));
  }
  commit(pair){this.used.set(this.frame,pair.map(value=>structuredClone(value)));this.frame++;}
  rewind(frame){this.frame=frame;this.dirty=null;for(const at of this.used.keys())if(at>=frame)this.used.delete(at);}
  prune(acknowledgedPrefix=this.confirmed){const prefix=Math.min(this.frame,this.confirmed,acknowledgedPrefix);for(const at of this.used.keys())if(at<prefix)this.used.delete(at);for(const lane of this.inputs)for(const at of lane.keys())if(at<prefix-32)lane.delete(at);}
}
