import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,memory,root,report} from './helpers.mjs';
test('TH11 original demos traverse the live C++ gameplay managers without failed callbacks',async()=>{
 const c=await core(),archive=readFileSync(resolve(root,'../[th11] 东方地灵殿 (汉化版+日文版)/th11.dat')),data=c.allocate(archive.length),source=c.resources_create(),session=c.game_session_create();
 memory(c,data,archive.length).set(archive);assert.equal(c.resources_open(source,data,archive.length),1);let frames=0;const cases=[];
 const error=()=>{const p=c.game_session_error(session);let n=0;while(memory(c,p+n,1)[0])++n;return Buffer.from(memory(c,p,n)).toString();};
 try{for(let i=0;i<4;++i){
  const raw=readFileSync(resolve(root,`reference/assets/demo${i}.rpy`)),p=c.allocate(raw.length);memory(c,p,raw.length).set(raw);
  assert.equal(c.game_session_replay(session,source,p,raw.length,0,1),1,error());c.release(p);
  const snapshots=[];let frame=0;
  for(;frame<6000&&c.game_session_value(session,0)===1;++frame){
   assert.equal(c.game_session_update(session,0,0,0,0,0,0),1,`demo${i} frame${frame}: ${error()}`);
   if(frame%300===0){const e=c.battle_data(session,0),dv=new DataView(c.memory.buffer);snapshots.push({frame,score:dv.getInt32(e,true),lives:dv.getInt32(e+16,true),enemies:c.game_session_value(session,7),bullets:c.game_session_value(session,8)});}
  }
  cases.push({file:`demo${i}.rpy`,frames:frame,phase:c.game_session_value(session,0),snapshots});frames+=frame;
  assert.ok(frame<6000,'recording ends');assert.equal(c.game_session_value(session,0),4,'reaches original terminal marker');
 }
 report('session-replays',{passed:true,frames,cases,scope:'Original demo data through live managers; NOT yet a full-game native state comparison. No immunity or skipped gameplay callbacks.'});
 }finally{c.game_session_delete(session);c.resources_delete(source);c.release(data);}
});
