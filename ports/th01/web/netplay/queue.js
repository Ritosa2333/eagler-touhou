import {Pc98RollbackCore} from '../../../../common/pc98/rollback-client.mjs';
export const PROTOCOL='th01-native-lockstep/1';
export const MAX_ROLLBACK=8;
export const VALID_INPUT=4294967295;
// Authoritative inputs are immutable; bounded prediction is repaired before confirmation.
export class FrameQueue{
  constructor(slot,delay,count=2,{rollback=false,coreModule}={}){
    if(![2,3].includes(count)||!Number.isInteger(slot)||slot<0||slot>=count||!Number.isInteger(delay)||delay<1||delay>12)throw Error('输入缓冲无效');
    this.slot=slot;this.delay=delay;this.frame=0;this.confirmed=delay;this.rollback=rollback;this.dirty=null;this.used=new Map();this.lanes=Array.from({length:count},()=>new Map());
    this.core=new Pc98RollbackCore(coreModule,{players:count,slot,limit:8,held:15,directions:3,directionFrames:6});
    for(let i=0;i<delay;i++)for(let player=0;player<count;player++){this.lanes[player].set(i,0);this.core.submit(player,i,this.decode(0));}
  }
  decode(value){const active=Math.floor(value/2**30)%2===1,begin=Math.floor(value/2**31)%2===1,raw=Math.floor(value/65536)%16384;return {buttons:value&65535,mode:active?(begin?4:3):0,x:active?(raw>=8192?raw-16384:raw):0,y:0,flags:0};}
  encode(value){return (value.buttons+(value.mode?2**30+(value.mode===4?2**31:0)+(value.x&16383)*65536:0))>>>0;}
  capture(buttons){
    const frame=this.frame+this.delay,lane=this.lanes[this.slot];
    if(lane.has(frame))return null;
    const packet={type:'input',frame,slot:this.slot,buttons:buttons>>>0};
    lane.set(frame,packet.buttons);this.core.submit(this.slot,frame,this.decode(packet.buttons));this.updateConfirmed();return packet;
  }
  receive(frame,buttons,slot){
    if(!Number.isInteger(slot)||slot<0||slot>=this.lanes.length||slot===this.slot||!Number.isSafeInteger(frame)||frame<0||frame>this.frame+120||
      !Number.isInteger(buttons)||buttons<0||buttons>VALID_INPUT)throw Error('输入帧或座位无效');
    if(frame<Math.min(this.frame,this.confirmed)-32)return;
    const lane=this.lanes[slot];
    if(lane.has(frame)&&lane.get(frame)!==buttons)throw Error('同一帧输入冲突');
    if(!lane.has(frame)){lane.set(frame,buttons);this.core.submit(slot,frame,this.decode(buttons));}
    this.dirty=this.core.dirty;
    this.updateConfirmed();
  }
  updateConfirmed(){while(this.lanes.every(lane=>lane.has(this.confirmed)))this.confirmed++;}
  predicted(slot,frame){
    const lane=this.lanes[slot];if(lane.has(frame))return lane.get(frame);
    for(let age=1;age<=MAX_ROLLBACK;age++)if(lane.has(frame-age))return lane.get(frame-age)&(age<=6?15:12);
    return 0;
  }
  refreshCorrection(){this.core.refresh();this.dirty=this.core.dirty;return this.dirty;}
  peek(){
    if(!this.rollback&&!this.lanes.every(lane=>lane.has(this.frame)))return null;
    const inputs=this.core.prepare(this.frame);return inputs?inputs.map(value=>this.encode(value)):null;
  }
  commit(pair){this.core.commit(this.frame);this.frame++;}
  rewind(frame){this.core.rewind(frame);this.frame=frame;this.dirty=null;}
  prune(){const prefix=Math.min(this.frame,this.confirmed);this.core.discard(prefix);for(const at of this.used.keys())if(at<prefix)this.used.delete(at);for(const lane of this.lanes)for(const at of lane.keys())if(at<prefix-32)lane.delete(at);}
}
