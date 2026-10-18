import test from 'node:test';import assert from 'node:assert/strict';import{readFileSync}from'node:fs';import{resolve}from'node:path';
import{core,memory,root,report}from'./helpers.mjs';
test('TH11 saved sessions reproduce all six players, keyboard edges and continuous touch',async()=>{
 const c=await core(),archive=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(archive.length),resources=c.resources_create(),output=c.allocate(2000000),name=c.allocate(16);memory(c,data,archive.length).set(archive);assert.equal(c.resources_open(resources,data,archive.length),1);memory(c,name,5).set(Buffer.from('TEST\0'));
 const snapshot=s=>Buffer.concat([Buffer.from(memory(c,c.battle_data(s,0),48)),Buffer.from(memory(c,c.battle_data(s,2),128)),Buffer.from(memory(c,c.anm_env_rng(c.game_session_animations(s),0),8))]);let frames=0;
 try{for(let selection=0;selection<6;++selection){const session=c.game_session_create(),expected=[];
  assert.equal(c.game_session_begin(session,resources,1,Math.floor(selection/3),selection%3,1,0),1);
  const touch=selection%2;for(let frame=0;frame<1800;++frame){
   const held=1|((frame%180<110)?8:0)|(frame%240<60?64:frame%240<120?128:0);
   c.game_session_touch_motion(session,touch&&frame%90<75?1:0,Math.sin(frame/75)*150,330+Math.cos(frame/90)*50);
   assert.equal(c.game_session_update(session,held,0,0,0,0,0),1,`record ${selection}/${frame}`);expected.push(snapshot(session));
  }
  const n=c.game_session_save_replay(session,name,output,2000000);assert.ok(n);const first=Buffer.from(memory(c,output,n));assert.equal(c.game_session_save_replay(session,name,output,2000000),n);assert.deepEqual(Buffer.from(memory(c,output,n)),first,'repeated save is idempotent');
  assert.equal(c.game_session_replay(session,resources,output,n,1,0),1);
  for(let frame=0;frame<expected.length;++frame){assert.equal(c.game_session_update(session,0,0,0,0,0,0),1,`playback ${selection}/${frame}`);assert.deepEqual(snapshot(session),expected[frame],`selection${selection} touch${touch} frame${frame}`);++frames;}
  assert.equal(c.game_session_update(session,0,0,0,0,0,0),1);assert.equal(c.game_session_value(session,0),4);c.game_session_delete(session);
 }report('recorded-session',{passed:true,frames,scope:'Six live C++ sessions saved and played through all game managers; every tick checks economy, complete movement and script RNG. Alternating keyboard/continuous-touch samples, normal original encrypted data with optional browser USER touch block, and idempotent saves. This is not a native full-world comparison.'});
 }finally{for(const p of[data,output,name])c.release(p);c.resources_delete(resources);}
});
