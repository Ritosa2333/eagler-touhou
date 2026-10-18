import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,root,report} from './helpers.mjs';

test('TH11 session owns deterministic stage lifetime and frame boundary',async()=>{
 const c=await core();
 const raw=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat'));
 const data=c.allocate(raw.length),source=c.resources_create(),session=c.game_session_create();
 memory(c,data,raw.length).set(raw); assert.equal(c.resources_open(source,data,raw.length),1);
 try {
  const sessionError=()=>{const p=c.game_session_error(session),b=memory(c,p,256),n=b.indexOf(0);return new TextDecoder().decode(b.subarray(0,n<0?256:n));};
  assert.equal(c.game_session_begin(session,source,1,0,0,1,0),1,sessionError());
  assert.equal(c.game_session_value(session,0),1); // stage
  assert.equal(c.game_session_value(session,2),1);
  assert.equal(c.game_session_value(session,4),1); // ECL battle adapters attached
  for(let frame=0;frame<120;++frame) assert.equal(c.game_session_update(session,0,0,0,0,0,0),1,`${sessionError()} battle=${c.game_session_value(session,5)} player=${c.game_session_value(session,6)}`);
  assert.equal(c.game_session_value(session,1),120);
  assert.ok(c.game_session_value(session,7)>=0,'stage ECL enemy manager remained live');
  assert.equal(c.game_session_pause(session),1);
  assert.equal(c.game_session_value(session,0),2); // paused
  assert.equal(c.game_session_update(session,0,0,0,0,0,0),1);
  assert.equal(c.game_session_value(session,1),120);
  assert.equal(c.game_session_resume(session),1);
  assert.equal(c.game_session_title(session),1);
  assert.equal(c.game_session_value(session,0),0); // title
  assert.equal(c.game_session_value(session,2),0);
  assert.equal(c.game_session_update(session,0,0,0,0,0,0),1);
  assert.ok(c.game_session_value(session,3)>0,'title ANM remains visible after returning to title');
  const tick=(held=0,n=1)=>{for(let i=0;i<n;++i)assert.equal(c.game_session_update(session,held,0,0,0,0,0),1,sessionError());};
  tick(0,90);tick(1);tick(0,31);tick(1);tick(0,24);tick(1);tick(0,24);tick(1);tick(0,40);
  assert.equal(c.game_session_value(session,0),1,'original title selection starts actual stage');
  assert.equal(c.game_session_value(session,2),1);
  assert.equal(c.game_session_title(session),1);
  for(const demo of[3,2,1,0]){
   let idle=0;while(c.game_session_value(session,0)===0&&idle++<1800)tick();assert.equal(c.game_session_value(session,0),1,'idle title starts original demonstration');
   const raw=readFileSync(resolve(root,`reference/assets/demo${demo}.rpy`)),bytes=c.allocate(raw.length),r=c.replay_create();memory(c,bytes,raw.length).set(raw);assert.equal(c.replay_open(r,bytes,raw.length),1);const stage=new DataView(c.memory.buffer).getUint16(c.replay_data(r)+112,true);assert.equal(c.game_session_value(session,2),stage,'original demo rotation '+demo);c.replay_delete(r);c.release(bytes);
   tick(1);assert.equal(c.game_session_value(session,0),0,'confirm leaves demo');tick();
  }
  report('game-session',{passed:true,checks:9,frames:120,scope:'Atomic core/stage resource lifetime, title/stage/pause transitions and 60 Hz ANM frame boundary; battle callbacks are intentionally reported as a separate integration milestone.'});
 } finally { c.game_session_delete(session); c.resources_delete(source); c.release(data); }
});
