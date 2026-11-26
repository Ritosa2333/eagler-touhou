export class PresentationClock {
  reset(){this.next=undefined;}
  due(time,limited){
    if(!limited){this.next=undefined;return true;}
    if(this.next===undefined)this.next=time;
    if(time+.01<this.next)return false;
    const interval=1000/60;
    this.next+=Math.max(1,Math.floor((time-this.next)/interval)+1)*interval;
    return true;
  }
}

export class ReplayPresentationClock {
  reset(){this.stepTime=undefined;}
  stepped(time){this.stepTime=time;}
  alpha(time,playback,limited){
    if(limited||this.stepTime===undefined||!playback||playback.paused||playback.seeking||playback.ended||!(playback.rate>0))return 0;
    return Math.max(0,Math.min(1,(time-this.stepTime)*playback.rate*60/1000));
  }
}
