// Local presentation time. Never written into native state, checkpoints or
// the network hash; rollback and input delays still share one logical clock.
export class SpellWallClock {
  constructor(){this.reset();}
  reset(){this.start=null;this.card=null;this.completed=null;}
  observe(spell,now){
    if(!spell||!Number.isFinite(now))return spell;
    if(spell.flags&1){
      if(this.start===null||this.card!==spell.cardId){this.start=now;this.card=spell.cardId;}
    }
    const result=spell.lastResult;
    if(!(spell.flags&1)&&result?.kind&&result.cardId===this.card&&this.start!==null){
      const duration=Math.max(0,(now-this.start)/1000),remainder=duration%.0167,
        rounded=duration-remainder+(remainder>=.00835?.0167:0),whole=Math.floor(rounded),
        seconds=Math.min(whole,999),hundredths=Math.trunc((rounded-whole)*100);
      this.completed={card:this.card,id:spell.resultAnimationId,
        encoded:((seconds+hundredths+22)*1000+(seconds+66)%1000)*100+(hundredths+33)%100};
      this.start=null;
    }
    const completed=this.completed;
    return completed&&result?.cardId===completed.card&&spell.resultAnimationId===completed.id
      ?{...spell,lastResult:{...result,realTimeEncoded:completed.encoded}}:spell;
  }
}
