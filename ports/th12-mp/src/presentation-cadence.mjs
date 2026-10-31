// Interpolation is useful only when presentation outruns the fixed game tick.
// This never throttles rAF or changes the core accumulator/tick duration.
export function needsPresentationInterpolation(interval,step=1000/60){
  return Number.isFinite(interval)&&interval>0&&Number.isFinite(step)&&step>0&&interval<step*.9;
}
export class PresentationCadence {
  constructor(){this.fastFrames=0;}
  observe(interval,step=1000/60){
    this.fastFrames=needsPresentationInterpolation(interval,step)?Math.min(4,this.fastFrames+1):0;
    return this.fastFrames===4;
  }
}

// Carry the fractional refresh remainder. Resetting to each accepted rAF would
// turn a 60 Hz cap into 48 Hz at 144 Hz or 55 Hz at 165 Hz.
export class PresentationLimiter {
  constructor(){this.next=null;}
  allows(now,enabled=true,step=1000/60){
    if(!enabled){this.next=null;return true;}
    if(this.next===null){this.next=now+step;return true;}
    if(now<this.next-.5)return false;
    this.next+=Math.max(1,Math.floor((now-this.next+.5)/step)+1)*step;
    return true;
  }
}
export const needsPresentationCapture=(remaining,step,interpolate)=>remaining<step*(interpolate?2:1);
