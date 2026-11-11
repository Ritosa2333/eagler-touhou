// Native DD5C/DD7B fires only after >=3 released frames: the maximum
// cadence is one pressed frame + three released frames (15 Hz at 60 FPS).
const frameMs=1000/60;
export class FireControl {
  constructor(){this.reset();}
  reset(){this.state={held:false,next:0,until:0,charging:false};}
  sample(rapid,charge,now){
    const s=this.state;
    // Dialogue also waits for a fresh press: keep pulsing Z outside combat.
    if(charge){s.held=false;s.until=0;s.charging=true;return true;}
    // Always let the native key go up after charge, even with rapid held.
    if(s.charging){s.charging=false;s.next=now+3*frameMs;s.until=0;s.held=rapid;return false;}
    if(rapid&&(!s.held||now>=s.next-0.001)){
      s.until=now+frameMs;s.next=now+4*frameMs;
    }
    s.held=rapid;
    return now<s.until-0.001;
  }
}
