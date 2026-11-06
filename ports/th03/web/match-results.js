// Publish only confirmed outcomes; a new character selection starts a new match.
export class ConfirmedMatchResults {
  constructor(publish){this.publish=publish;this.pending=new Map();this.confirmed=0;this.lastGeneration=null;}
  stage(frame,result,selecting=false){if(frame>=this.confirmed)this.pending.set(frame,{result,selecting});}
  rewind(frame){for(const key of this.pending.keys())if(key>=frame)this.pending.delete(key);}
  confirm(prefix){for(const [frame,{result,selecting}] of this.pending)if(frame<prefix){
    if(selecting)this.lastGeneration=null;
    if(result&&result.generation!==this.lastGeneration){this.lastGeneration=result.generation;this.publish({start:frame,frames:1,winner:result.winner});}
    this.pending.delete(frame);
  }this.confirmed=Math.max(this.confirmed,prefix);}
}
