export const PROTOCOL='th01-native-lockstep/1';
export const VALID_INPUT=4294967295;
// Every input is immutable once sampled. No prediction or wall-clock game state.
export class FrameQueue{
  constructor(slot,delay,count=2){
    if(![2,3].includes(count)||!Number.isInteger(slot)||slot<0||slot>=count||!Number.isInteger(delay)||delay<1||delay>12)throw Error('输入缓冲无效');
    this.slot=slot;this.delay=delay;this.frame=0;this.lanes=Array.from({length:count},()=>new Map());
    for(let i=0;i<delay;i++)for(const lane of this.lanes)lane.set(i,0);
  }
  capture(buttons){
    const frame=this.frame+this.delay,lane=this.lanes[this.slot];
    if(lane.has(frame))return null;
    const packet={type:'input',frame,slot:this.slot,buttons:buttons>>>0};
    lane.set(frame,packet.buttons);return packet;
  }
  receive(frame,buttons,slot){
    if(!Number.isInteger(slot)||slot<0||slot>=this.lanes.length||slot===this.slot||!Number.isSafeInteger(frame)||frame<0||frame>this.frame+120||
      !Number.isInteger(buttons)||buttons<0||buttons>VALID_INPUT)throw Error('输入帧或座位无效');
    if(frame<this.frame)return;
    const lane=this.lanes[slot];
    if(lane.has(frame)&&lane.get(frame)!==buttons)throw Error('同一帧输入冲突');
    lane.set(frame,buttons);
  }
  peek(){return this.lanes.every(lane=>lane.has(this.frame))?this.lanes.map(lane=>lane.get(this.frame)):null;}
  commit(){for(const lane of this.lanes)lane.delete(this.frame);this.frame++;}
}
