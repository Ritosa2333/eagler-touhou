// Synchronized permission/command state for the native TH04 pause menu.
// Rendering and gameplay suspension belong to MAIN.EXE, not an HTML overlay.
export class PauseMenu {
  constructor(hostSlot,players=2){this.hostSlot=hostSlot;this.previous=Array(players).fill(0);this.paused=false;this.selection=0;this.exited=false;}
  step(inputs){
    const edge=inputs.map((bits,i)=>bits&~this.previous[i]);this.previous=[...inputs];
    if(this.exited)return 'exit';
    if(!this.paused){
      if(edge.some(bits=>bits&128)){this.paused=true;this.selection=0;return 'paused';}
      return 'play';
    }
    const host=edge[this.hostSlot],guest=edge.reduce((bits,value,i)=>i===this.hostSlot?bits:bits|value,0);
    // A guest can only resume, regardless of the host's highlighted item.
    // Explicit resume takes precedence over simultaneous exit/navigation.
    if((guest&(128|256|32|512))||(host&(128|512))){this.paused=false;return 'resume';}
    if(host&3)this.selection^=1;
    if((host&1024)||((host&(256|32))&&this.selection===1)){
      this.exited=true;return 'exit';
    }
    if(host&(256|32)){this.paused=false;return 'resume';}
    return 'paused';
  }
  signature(){return [this.paused?1:0,this.selection,this.exited?1:0,...this.previous];}
}
