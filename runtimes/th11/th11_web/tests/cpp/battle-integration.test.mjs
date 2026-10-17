import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,root,report} from './helpers.mjs';

test('TH11 battle connects six selections, items, shots, collision, graze and title lifetime',async()=>{
 const c=await core(),raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
 const data=c.allocate(raw.length),source=c.resources_create(),s=c.game_session_create();
 memory(c,data,raw.length).set(raw);assert.equal(c.resources_open(source,data,raw.length),1);
 const get=(p)=>new DataView(c.memory.buffer).getInt32(p,true),put=(p,x)=>new DataView(c.memory.buffer).setInt32(p,x,true),flt=p=>new DataView(c.memory.buffer).getFloat32(p,true);
 const error=()=>{const p=c.game_session_error(s),b=memory(c,p,256);return new TextDecoder().decode(b.subarray(0,b.indexOf(0)));};
 const step=held=>assert.equal(c.game_session_update(s,held,0,0,0,0,0),1,error());
 let frames=0,checks=0;
 try {
  for(let selection=0;selection<6;++selection){
   const character=Math.floor(selection/3),subtype=selection%3;
   assert.equal(c.game_session_begin(s,source,1,character,subtype,1,0),1,error());
   assert.equal(c.battle_value(s,5),1,'stage ANM slots bind their own files');
   for(let frame=0;frame<125;++frame){step(0);++frames;}
   assert.equal(c.battle_value(s,0),character);assert.equal(c.battle_value(s,1),subtype);
   const motion=c.battle_data(s,2),before=flt(motion);step(0x80);
   assert.ok(flt(motion)>before,'right input moves selected player');assert.equal(c.battle_value(s,2),0x80);step(0x80);assert.equal(c.battle_value(s,2),0,'held is not repeated press');
   for(const kind of [4,5,6])assert.deepEqual(Buffer.from(memory(c,c.battle_data(s,kind),12)),Buffer.from(memory(c,motion,12)),'all collision worlds follow player');
   const economy=c.battle_data(s,0),state=c.battle_data(s,1);
   const power=get(economy+4),powerStep=get(economy+36);assert.equal(c.battle_item(s,4),1);assert.equal(get(economy+4),power+powerStep,'large power item awards selected SHT step');
   assert.ok(c.battle_value(s,7)>0,'power award rebuilds selected options');assert.equal(c.battle_value(s,3),1,'item uses bullet ANM');
   const lives=get(economy+16);for(let i=0;i<5;++i)assert.equal(c.battle_item(s,5),1);assert.equal(get(economy+16),lives+1);assert.equal(get(economy+20),0);
   let lifeEvent=false;for(let i=0;i<c.battle_value(s,4);++i)if(c.battle_event(s,i,0)===3&&c.battle_event(s,i,1)===lives+1)lifeEvent=true;assert.ok(lifeEvent,'HUD life event retained');
   assert.ok(c.battle_damage(s)>0,'player shot damages actual enemy adapter');assert.equal(c.battle_laser_cancel(s),1,'laser cancellation creates rewards');
   for(const callback of [0,1])for(const enemyAdvanced of [0,1]){assert.equal(c.battle_damage_gate(s,callback,0,enemyAdvanced),0,'stationary player timer suppresses damage independently of enemy timer');assert.ok(c.battle_damage_gate(s,callback,1,enemyAdvanced)>0,'advancing player timer enables damage independently of enemy timer');}
   const graze=get(economy+40);put(economy+12,0);assert.equal(c.battle_graze(s,0),1);assert.equal(get(economy+40),graze+1);assert.equal(get(economy+12),500);
   const x=flt(motion),y=flt(motion+4);put(state+48,0);new DataView(c.memory.buffer).setFloat32(state+52,0,true);
   assert.equal(c.battle_collision(s,3,x,y,8,32),1);assert.equal(get(state),1,'warning beam never triggers hit');
   assert.equal(c.battle_collision(s,selection%3,x,y,8,32),1);assert.equal(get(state),4,'collision enters deathbomb window');
   const shared=c.battle_data(s,7);put(shared,7);put(shared+8,19);assert.equal(c.battle_enemy_death(s),1);assert.equal(get(shared),8);assert.equal(get(shared+8),0);
   assert.equal(c.game_session_title(s),1);assert.equal(c.game_session_value(s,4),0);assert.equal(c.game_session_value(s,0),0);
   step(0);assert.ok(c.game_session_value(s,3)>0,'title survives active animation teardown');checks+=25;
  }
  report('battle-integration',{passed:true,selections:6,frames,checks,scope:'Live adapter regression checks, not an original full-game replay comparison. Full stage, spells, bombs, dialogue, audio and renderer remain separate acceptance gates.'});
 }finally{c.game_session_delete(s);c.resources_delete(source);c.release(data);}
});
