// Keep fractional time between animation frames; 90/120/144 Hz displays must
// advance exactly the same 60 Hz simulation as a 60 Hz display.
export class FrameClock {
 constructor(){this.reset(0);}
 reset(now){this.last=now;this.pending=0;}
 advance(now){
  const elapsed=Math.max(0,Math.min(250,now-this.last));this.last=now;
  this.pending=Math.min(this.pending+elapsed,1000/60*4);
  const count=Math.floor((this.pending+1e-7)/(1000/60));
  this.pending=Math.max(0,this.pending-count*(1000/60));return count;
 }
}
export function keyboardBits(keys){
 let held=0;
 for(const [codes,bit] of [
  [['KeyZ'],1],[['KeyX'],2],[['ShiftLeft','ShiftRight'],8],
  [['ArrowUp'],16],[['ArrowDown'],32],[['ArrowLeft'],64],[['ArrowRight'],128],
  [['Enter','NumpadEnter'],256],[['ControlLeft','ControlRight'],512],[['KeyC'],1024]
 ])if(codes.some(code=>keys.has(code)))held|=bit;
 return held;
}
