// Native DD5C/DD7B fires only after >=3 released frames: the maximum
// cadence is one pressed frame + three released frames (15 Hz at 60 FPS).
const frameMs=1000/60;
export class FireControl {
  constructor(){this.reset();}
  reset(){this.state={held:false,next:0,until:0,charging:false,selectionHeld:false,selectionDelay:0,selectionRemaining:0};}
  selection(rapid,charge){
    const s=this.state,pressed=rapid||charge;
    // Selection polls only every three ticks and suspends polling during the
    // other player's 18-tick confirmation animation. Retain a fresh press
    // long enough to survive both, including a short tap between polls.
    if(pressed&&!s.selectionHeld){s.selectionDelay=4;s.selectionRemaining=60;}
    s.selectionHeld=pressed;
    s.held=false;s.until=0;s.charging=false;
    if(s.selectionDelay){s.selectionDelay--;return false;}
    if(s.selectionRemaining){s.selectionRemaining--;return true;}
    return pressed;
  }
  sample(rapid,charge,now,rapidEnabled=true){
    this.state.selectionHeld=false;this.state.selectionDelay=0;this.state.selectionRemaining=0;
    if(!rapidEnabled){this.reset();return !!(rapid||charge);}
    const s=this.state;
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
