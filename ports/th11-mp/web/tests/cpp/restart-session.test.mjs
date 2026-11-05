import test from 'node:test';import assert from 'node:assert/strict';import{readFileSync}from'node:fs';import{resolve}from'node:path';import{core,memory,root,report}from'./helpers.mjs';
test('TH11 continue and retry rebuild live gameplay with the native counter resets',async()=>{
 const c=await core(),archive=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(archive.length),source=c.resources_create(),s=c.game_session_create(),draw=c.comp_create(),name=c.allocate(9),out=c.allocate(1000000),r=c.replay_create();memory(c,data,archive.length).set(archive);assert.equal(c.resources_open(source,data,archive.length),1);memory(c,name,9).set(Buffer.from('CONTINUE\0'));
 const dv=()=>new DataView(c.memory.buffer),get=p=>dv().getInt32(p,true),error=()=>{const p=c.game_session_error(s),b=memory(c,p,512);return new TextDecoder().decode(b.subarray(0,b.indexOf(0)));};
 const tick=(held=0,n=1)=>{for(let i=0;i<n;++i){assert.equal(c.game_session_update(s,held,0,0,0,0,0),1,error());assert.equal(c.game_session_render(s,draw),1,error());}},press=held=>{tick(held);tick();};
 const replayHeader=stage=>{const n=c.game_session_save_replay(s,name,out,1000000);assert.ok(n);assert.equal(c.replay_open(r,out,n),1);return c.replay_data(r)+dv().getUint32(c.replay_stage(r,stage)+4,true);};
 try{for(let shot=0;shot<6;++shot){
  assert.equal(c.game_session_begin(s,source,1,Math.floor(shot/3),shot%3,1,0),1,error());tick(0,60);assert.equal(c.game_session_complete(s,0),1);tick(0,152);assert.equal(c.game_session_value(s,2),2);
  c.game_session_loss_probe(s);tick();assert.equal(c.game_session_value(s,0),3);tick(0,24);
  for(let i=0;i<12&&c.game_session_pause_state(s)===18;++i)press(1);
  assert.equal(c.game_session_pause_state(s),14,'game-over choices');press(1);tick(0,14);assert.equal(c.game_session_value(s,0),1);assert.equal(c.game_session_value(s,2),2);
  const e=c.battle_data(s,0);assert.equal(get(e),0);assert.equal(get(e+16),2);assert.equal(get(e+4),get(e+32));assert.equal(get(e+28),-512);assert.equal(get(replayHeader(2)+40),1,'continue count in saved native header');
  assert.equal(c.game_session_pause(s),1);tick(0,12);press(0x200000);tick(0,14);assert.equal(c.game_session_value(s,0),1);assert.equal(c.game_session_value(s,2),1,'retry returns to the original first stage');
  const reset=c.battle_data(s,0);assert.equal(get(reset),0);assert.equal(get(reset+4),0);assert.equal(get(reset+16),2);assert.equal(get(reset+28),0);assert.equal(get(replayHeader(1)+40),0);
 }report('restart-session',{passed:true,selections:6,scope:'Original 41fe10/41f8f0 reset rules through real result Continue and pause R shortcut: score, power, lives, rank, recorded continue counter and first-stage retention. Stage clear/game-over are fixture triggers; their gameplay conditions are checked separately.'});
 }finally{c.replay_delete(r);c.comp_delete(draw);c.game_session_delete(s);c.resources_delete(source);for(const p of[data,name,out])c.release(p);}
});
