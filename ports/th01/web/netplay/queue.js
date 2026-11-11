export const PROTOCOL='th01-native-lockstep/1';
export const MAX_ROLLBACK=8;
export const VALID_INPUT=4294967295;
// Authoritative inputs are immutable; bounded prediction is repaired before confirmation.
export class FrameQueue{
  constructor(slot,delay,count=2,{rollback=false}={}){
    if(![2,3].includes(count)||!Number.isInteger(slot)||slot<0||slot>=count||!Number.isInteger(delay)||delay<1||delay>12)throw Error('输入缓冲无效');
    this.slot=slot;this.delay=delay;this.frame=0;this.confirmed=delay;this.rollback=rollback;this.dirty=null;this.used=new Map();this.lanes=Array.from({length:count},()=>new Map());
    for(let i=0;i<delay;i++)for(const lane of this.lanes)lane.set(i,0);
  }
  capture(buttons){
    const frame=this.frame+this.delay,lane=this.lanes[this.slot];
    if(lane.has(frame))return null;
    const packet={type:'input',frame,slot:this.slot,buttons:buttons>>>0};
    lane.set(frame,packet.buttons);this.updateConfirmed();return packet;
  }
  receive(frame,buttons,slot){
    if(!Number.isInteger(slot)||slot<0||slot>=this.lanes.length||slot===this.slot||!Number.isSafeInteger(frame)||frame<0||frame>this.frame+120||
      !Number.isInteger(buttons)||buttons<0||buttons>VALID_INPUT)throw Error('输入帧或座位无效');
    if(frame<Math.min(this.frame,this.confirmed)-32)return;
    const lane=this.lanes[slot];
    if(lane.has(frame)&&lane.get(frame)!==buttons)throw Error('同一帧输入冲突');
    lane.set(frame,buttons);
    if(this.rollback)for(const [at,pair] of this.used)if(at>=frame&&pair[slot]!==this.predicted(slot,at)){this.dirty=this.dirty===null?at:Math.min(this.dirty,at);break;}
    this.updateConfirmed();
  }
  updateConfirmed(){while(this.lanes.every(lane=>lane.has(this.confirmed)))this.confirmed++;}
  predicted(slot,frame){
    const lane=this.lanes[slot];if(lane.has(frame))return lane.get(frame);
    for(let age=1;age<=MAX_ROLLBACK;age++)if(lane.has(frame-age))return lane.get(frame-age)&(age<=6?15:12);
    return 0;
  }
  refreshCorrection(){
    if(!this.rollback)return null;
    this.dirty=null;
    for(const [at,pair] of this.used)if(pair.some((v,slot)=>v!==this.predicted(slot,at))){this.dirty=at;break;}
    return this.dirty;
  }
  peek(){
    if(this.lanes.every(lane=>lane.has(this.frame)))return this.lanes.map(lane=>lane.get(this.frame));
    if(!this.rollback||this.frame-this.confirmed>=MAX_ROLLBACK||!this.lanes[this.slot].has(this.frame))return null;
    return this.lanes.map((_,slot)=>this.predicted(slot,this.frame));
  }
  commit(pair){if(this.rollback)this.used.set(this.frame,pair.slice());this.frame++;}
  rewind(frame){this.frame=frame;this.dirty=null;for(const at of this.used.keys())if(at>=frame)this.used.delete(at);}
  prune(){const prefix=Math.min(this.frame,this.confirmed);for(const at of this.used.keys())if(at<prefix)this.used.delete(at);for(const lane of this.lanes)for(const at of lane.keys())if(at<prefix-32)lane.delete(at);}
}
