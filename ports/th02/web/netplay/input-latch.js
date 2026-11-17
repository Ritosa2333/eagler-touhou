// Physical events survive between captures and while a logical tick stalls.
// Authoritative packets stay immutable; pending edges clear only on capture.
export class InputLatch {
 constructor(){this.clear();}
 clear(){this.held=0;this.pending=0;this.menu=new Map();this.edges=0;}
 observe(value){
  const pressed=value&~this.held;this.pending|=pressed&~15;
  // A newer edge replaces only its own axis. A horizontal change must not
  // erase a short vertical tap still waiting for an uncaptured tick.
  if(pressed&3)this.pending=(this.pending&~3)|(value&3);
  if(pressed&12)this.pending=(this.pending&~12)|(value&12);
  this.held=value;
  if(pressed)this.edges++;
  for(const bit of [128,256])if(pressed&bit)this.menu.set(bit,2);
 }
 sample(){let bits=this.held|this.pending;for(const [bit,count]of this.menu)if(count)bits|=bit;return bits;}
 consume(){this.pending=0;for(const [bit,count]of this.menu)if(count<=1)this.menu.delete(bit);else this.menu.set(bit,count-1);}
}
