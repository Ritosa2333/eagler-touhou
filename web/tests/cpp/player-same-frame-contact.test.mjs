import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report} from './helpers.mjs';
test('A real player hit is visible to subsequent contacts in the same callback',async()=>{
 const c=await core(),m=await oracle(),manager=c.anm_manager_create(),sht=c.sht_create(),owner=m.allocate(0x2c100),hud=m.allocate(0x200),position=m.allocate(8),size=m.allocate(8);
 let player=0,checks=0,nativeHits=0;
 m.u32(0x4e9bb8,owner);m.u32(0x4e9a8c,hud);
 // The original death transition is compared separately with its real ANM.
 // Here observe its immediate live fields without replacing contact predicates.
 m.replace(0x456540,'observe original hit boundary',()=>{nativeHits++;m.i32(owner+0x16220,4);m.i32(owner+0x16284,6);return 0;});
 const load=(name,id)=>{const b=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(b.length);memory(c,p,b.length).set(b);assert.equal(c.anm_manager_load(manager,id,p,b.length),1,string(c,c.anm_manager_error(manager)));c.release(p);};
 try{
  load('ascii.anm',2);assert.equal(c.anm_manager_fallback(manager,2),1);load('effect.anm',8);load('pl00.anm',9);
  const b=readFileSync(resolve(root,'reference/assets/pl00.sht')),p=c.allocate(b.length);memory(c,p,b.length).set(b);assert.equal(c.sht_open(sht,p,b.length),1);c.release(p);
  player=c.player_create(sht,manager,0);assert.equal(c.player_initialize(player),1,string(c,c.player_error(player)));
  new DataView(c.memory.buffer).setInt32(c.player_field(player,1),1,true);
  assert.equal(c.player_update(player,0,0,7,0,1,0,0,0),1,string(c,c.player_error(player)));
  const dv=new DataView(c.memory.buffer),at=c.player_field(player,9),x=dv.getFloat32(at,true),y=dv.getFloat32(at+4,true);
  m.write(owner+0x618,Buffer.from(memory(c,at,8)));m.write(owner+0x2bfb0,Buffer.from(memory(c,c.player_field(player,30),24)));m.write(owner+0x2bfc8,Buffer.from(memory(c,c.player_field(player,42),8)));m.i32(owner+0x16220,1);m.i32(owner+0x16284,0);m.f32(position,x);m.f32(position+4,y);m.f32(size,2);m.f32(size+4,2);
  const before=c.player_life_event_count(player),expected=m.call(0x455be0,{args:[position,size,0]})|0;
  assert.equal(expected,1);assert.equal(c.player_contact(player,0,x,y,2,2,0),expected);assert.equal(c.player_life_event_count(player),before+3);assert.equal(new DataView(c.memory.buffer).getInt32(c.player_field(player,1),true),4);checks+=4;
  for(let i=0;i<24;i++){
   assert.equal(m.call(0x455be0,{args:[position,size,0]})|0,0);
   for(let kind=0;kind<3;kind++)assert.equal(c.player_contact(player,kind,x,y,kind===2?10:2,2,0),0,'subsequent contact '+i+'/'+kind);
   assert.equal(c.player_life_event_count(player),before+3,'only one hit sound/effect');checks+=5;
  }
  assert.equal(nativeHits,1);assert.equal(string(c,c.player_error(player)),'');
  report('player-same-frame-contact',{passed:true,checks,nativeHits,scope:'Real Player and ANM hit transition followed by rectangle/circle/laser contacts before the next player update. Original rectangle predicates read the live death state; only the native hit service boundary is observed. Full-world demo1 separately verifies actual projectile iteration and both RNG streams.'});
 }finally{if(player)c.player_delete(player);c.sht_delete(sht);c.anm_manager_delete(manager);m.close();}
});
