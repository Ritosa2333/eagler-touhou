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
