// A presentation callback owns a bounded slice of fixed simulation ticks.
// Seeking runs existing input again; it never integrates a negative delta.
export class PlaybackClock {
  constructor({budgetMs=8,maxSteps=4096,now=()=>performance.now()}={}) {
    this.budgetMs=budgetMs;this.maxSteps=maxSteps;this.now=now;
    this.rate=1;this.paused=false;this.target=null;this.last=null;this.credit=0;
  }
  reset() {this.last=null;this.credit=0;this.target=null;}
  speed(rate) {if(![0,.5,1,2,4,8].includes(rate))throw Error('Invalid playback speed');this.rate=rate;this.last=null;this.credit=0;}
  pause(value) {this.paused=!!value;this.last=null;this.credit=0;}
  seek(frame) {if(!Number.isSafeInteger(frame)||frame<0)throw Error('Invalid playback position');this.target=frame;this.last=null;this.credit=0;}
  tick(time,{position,step,ended=()=>false}) {
    const delta=this.last===null?0:Math.min(.25,Math.max(0,(time-this.last)/1000));this.last=time;
    if(ended()||(this.paused&&this.target===null))return 0;
    if(this.target!==null&&position()>=this.target){this.target=null;this.credit=0;return 0;}
    const seeking=this.target!==null,unlimited=seeking||this.rate===0;
    if(!unlimited)this.credit=Math.min(240,this.credit+delta*60*this.rate);
    const allowed=unlimited?this.maxSteps:Math.min(this.maxSteps,Math.floor(this.credit+1e-9));
    const started=this.now();let count=0;
    while(count<allowed&&!ended()&&(count===0||this.now()-started<this.budgetMs)) {
      if(this.target!==null&&position()>=this.target)break;
      if(step(count,count+1===allowed)===false)throw Error('Replay simulation failed');++count;
    }
    if(!unlimited)this.credit=Math.max(0,this.credit-count);
    if(this.target!==null&&(position()>=this.target||ended())){this.target=null;this.credit=0;}
    return count;
  }
}
