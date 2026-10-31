import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,string,root,report} from './helpers.mjs';
import {bits,ins,resource} from './ecl-fixtures.mjs';

test('Battle adapters preserve respawn protection and ECL spell-end force cancellation',async()=>{
 const c=await core(),a=c.anm_manager_create(),program=c.program_create(),sht=c.sht_create();let battle=0;
 const dv=()=>new DataView(c.memory.buffer),set=(p,v)=>dv().setInt32(p,v,true),get=p=>dv().getInt32(p,true);
 const load=(name,id)=>{const b=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(b.length);try{memory(c,p,b.length).set(b);assert.equal(c.anm_manager_load(a,id,p,b.length),1,string(c,c.anm_manager_error(a)));}finally{c.deallocate(p);}};
 try{
  load('ascii.anm',2);assert.equal(c.anm_manager_fallback(a,2),1);
  for(const [name,id]of[['pl00.anm',10],['bullet.anm',7],['effect.anm',8],['beast.anm',30]])load(name,id);
  const shot=readFileSync(resolve(root,'reference/assets/pl00b.sht')),sp=c.allocate(shot.length);memory(c,sp,shot.length).set(shot);assert.equal(c.sht_open(sht,sp,shot.length),1);c.deallocate(sp);
  const {bytes}=resource({clearReward:Buffer.concat([ins(615,[bits(640)]),ins(10,[],0,10000)]),clearPlain:Buffer.concat([ins(616,[bits(640)]),ins(10,[],0,10000)])}),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.program_attach(program,p,bytes.length),0);c.deallocate(p);
  const create=()=>{if(battle)c.battle_delete(battle);battle=c.battle_create(a,program,sht,0);assert.equal(c.battle_initialize(battle),1,string(c,c.battle_error(battle)));};
  const bullet=(protectedBullet=true,x=0,y=352)=>{assert.equal(c.battle_emit_test_bullet(battle,x,y),1,string(c,c.battle_error(battle)));const slot=c.battle_bullet_list(battle,0,0)>>>0,view=c.battle_bullet_view(battle,slot);set(c.bullet_state_field(view,24,0),1);set(c.bullet_state_field(view,22,0),protectedBullet?600:0);return c.bullet_state_field(view,24,0);};
  for(const special of [false,true]){
   create();const phase=bullet(),age=c.battle_player_field(battle,2);set(c.battle_player_field(battle,1),0);set(age,29);set(age+4,30);dv().setFloat32(age+8,30,true);
   if(special){set(c.battle_player_field(battle,19),1);set(c.battle_player_field(battle,19)+12,100);}
   assert.equal(c.battle_step(battle,0,0,1,0,1),1,string(c,c.battle_error(battle)));
   assert.equal(get(phase),special?4:1,special?'Final spell respawn forces cancellation':'Normal respawn keeps protected bullets');
  }
  for(const name of ['clearReward','clearPlain']){
   create();set(c.battle_player_field(battle,1),1);const phase=bullet(),np=c.allocate(name.length+1),rp=c.allocate(84);memory(c,np,name.length+1).set(Buffer.from(name+'\0'));memory(c,rp,84).fill(0);dv().setFloat32(rp+4,120,true);set(rp+20,100000);
   try{assert.equal(c.battle_spawn(battle,np,rp),1,string(c,c.battle_error(battle)));assert.equal(c.battle_step(battle,0,0,1,0,4),1,string(c,c.battle_error(battle)));assert.equal(get(phase),4,name+' clears protected fixed bullet');}finally{c.deallocate(rp);c.deallocate(np);}
  }
  for(const protectedBullet of [false,true]){
   create();set(c.battle_player_field(battle,1),1);for(let i=0;i<5;i++)assert.equal(c.battle_collect_spirit(battle,2),1,string(c,c.battle_error(battle)));
   const phase=bullet(protectedBullet,90,368);assert.equal(c.battle_step(battle,0,0,1,0,64),1,string(c,c.battle_error(battle)));assert.equal(get(phase),protectedBullet?1:4,'Otter barrier honors bullet protection');
  }
  report('battle-cancellation-regression',{passed:true,cases:6,scope:'Production GameBattle adapters, real player lifecycle, otter barrier and ECL opcodes 615/616 against live pooled bullets, including protection on/off. Native service calls are independently checked by enemy-scene-commands and roaring-frame.'});
 }finally{if(battle)c.battle_delete(battle);c.program_delete(program);c.sht_delete(sht);c.anm_manager_delete(a);}
});
