export class FrameClock {
 constructor(){this.reset();}
 reset(){this.last=null;this.credit=0;this.steps=0;}
 add(time){const gap=this.last===null?0:Math.max(0,(time-this.last)/1000);this.last=time;this.credit=Math.min(.25,this.credit+gap);this.steps=0;return gap*1000;}
 consume(period){if(!Number.isFinite(period)||period<=0)throw Error('Invalid TH05 frame period');if(this.credit+1e-9<period||this.steps>=16)return false;this.credit=Math.max(0,this.credit-period);this.steps++;return true;}
}
export class PresentationClock {
 constructor(){this.next=-Infinity;}
 reset(){this.next=-Infinity;}
 present(time,limit){if(!Number.isFinite(this.next))this.next=time;if(limit&&time+.001<this.next)return false;this.next=limit?Math.max(this.next+1000/60,time-100):time+1000/60;return true;}
}
