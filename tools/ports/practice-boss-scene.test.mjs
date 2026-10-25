import test from 'node:test';
import assert from 'node:assert/strict';
import {TH08_PRACTICE_ADAPTER} from '../../tests/adaptation/adapters/titles/th08-practice.mjs';
import {assertPracticeBossEffects} from './practice-boss-scene.mjs';
const contract=TH08_PRACTICE_ADAPTER.bossEffects;
function fixture(){
  const e={schema:'eagler-touhou/practice-boss-effects/1',cleanupErrors:[]};
  for(const [name,mask,frame] of [['locked',81,100],['damage',17,300],['unlocked',65,500]]){
    e[name]=Array.from({length:40},(_,i)=>{
      const w=Array(48).fill(0);w[0]=w[1]=w[19]=w[41]=w[44]=1;w[2]=w[18]=frame+i;w[4]=mask;
      w[25]=1;w[26]=name==='damage'?6000-i*5:6000;w[27]=name==='unlocked'?5000:4000+i;
      w[28]=200+i;w[29]=5;w[30]=0x4a;w[31]=2000;
      return{raw:{available:true,words:w},identity:{app:123,documentTimeOrigin:45,url:'https://localhost/runtime'}};
    });
  }return e;
}
test('Boss checks require real positive damage and opposite life/time consumers',()=>{
  const e=fixture();assert.equal(assertPracticeBossEffects(e,contract).timeLock,true);
  for(const change of [e=>e.locked.forEach(r=>r.raw.words[29]=0),e=>e.locked[3].raw.words[26]--,
    e=>e.locked[3].raw.words[27]--,e=>e.unlocked[3].raw.words[27]++,e=>e.damage.forEach(r=>r.raw.words[26]=6000),
    e=>e.locked[3].raw.words[28]--,e=>e.damage[3].raw.words[31]++,e=>e.unlocked[2].identity.app++,
    e=>e.locked[3].raw.words[15]=1,e=>e.locked[3].raw.words[8]=1]){
    const bad=structuredClone(e);change(bad);assert.throws(()=>assertPracticeBossEffects(bad,contract));
  }
});

test('Version 2 Boss evidence binds actual lock timer text and both hiding controls',()=>{
  const e=fixture();e.schema='eagler-touhou/practice-boss-effects/2';const f=contract.lockTimer.fields;
  const display=(row,enabled,visible)=>{const w=Array(1024).fill(0),frame=row.raw.words[2];w.splice(0,8,1,1,frame,frame,0,1,0,0);w[f.frame]=frame;w[f.generation]=frame;w[f.option]=Number(enabled);w[f.visible]=Number(visible);w[f.consumer]=frame;
    if(visible){const text=Math.fround(Math.fround(frame)/60).toFixed(2);w[f.frames]=frame;w[f.length]=text.length;w[f.vertices]=text.length*4;for(const [i,ch] of [...text].entries())w[f.text+i]=ch.charCodeAt(0);}
    row.displayRaw={available:true,words:w};};
  for(const name of ['locked','damage','unlocked'])for(const row of e[name])display(row,name!=='damage',name==='locked');
  e.timerOff=structuredClone(e.locked.slice(0,4));for(const row of e.timerOff)display(row,false,false);
  assert.equal(assertPracticeBossEffects(e,contract).lockTimerText,true);
  for(const mutate of [e=>e.locked[0].displayRaw.words[f.text]++,e=>e.locked[1].displayRaw.words[f.vertices]=0,
    e=>e.locked[2].displayRaw.words[f.frames]++,e=>e.damage[1].displayRaw.words[f.visible]=1,e=>e.unlocked[1].displayRaw.words[f.option]=0,
    e=>e.timerOff.pop(),e=>delete e.locked[3].displayRaw]){
    const bad=structuredClone(e);mutate(bad);assert.throws(()=>assertPracticeBossEffects(bad,contract));}
});
