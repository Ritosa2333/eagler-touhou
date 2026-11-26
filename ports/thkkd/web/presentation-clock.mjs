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
