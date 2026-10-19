import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,root,report} from './helpers.mjs';

test('TH11 battle reads the original player protection predicate for final-stage damage',async()=>{
 const c=await core(),m=await oracle(),raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
 const data=c.allocate(raw.length),source=c.resources_create(),s=c.game_session_create(),np=m.allocate(0x8d40);let checks=0;
 memory(c,data,raw.length).set(raw);assert.equal(c.resources_open(source,data,raw.length),1);m.u32(0x4a8eb4,np);
 try{for(let selection=0;selection<6;++selection){
  assert.equal(c.game_session_begin(s,source,6,Math.floor(selection/3),selection%3,3,0),1);
  const state=c.battle_data(s,1),motion=c.battle_data(s,2);
  for(const timer of [-1,0,1,30,300])for(const flags of [0,2,4,6]){
   const dv=new DataView(c.memory.buffer);dv.setInt32(state+48,timer,true);dv.setUint32(motion+116,flags,true);
   m.i32(np+0x8bb4,timer);m.u32(np+0x8bc4,flags);
   assert.equal(c.battle_damage_protection(s),m.call(0x410690),`selection ${selection}, timer ${timer}, flags ${flags}`);++checks;
  }
 }report('damage-protection',{passed:true,checks,scope:'Real GameBattle synchronization versus original 410690: all six selections, zero/nonzero invincibility and player flags. Prevents substituting Nitori Bomb secondary timing for the native protection predicate.'});
 }finally{c.game_session_delete(s);c.resources_delete(source);c.release(data);m.close();}
});
