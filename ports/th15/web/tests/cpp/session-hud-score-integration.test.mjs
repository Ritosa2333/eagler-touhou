import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report} from './helpers.mjs';
import {preloadStage} from './stage-assets-fixture.mjs';
test('Actual original replay session advances displayed score and high score in the native callback order',async()=>{
 const c=await core(),m=await oracle(),asset=preloadStage(c,1,2),run=c.run_session_create(asset.fixture),bytes=readFileSync(resolve(root,'reference/assets/demo2.rpy')),p=c.allocate(bytes.length),hud=m.allocate(0x2d4);let checks=0,earned=0;
 try{
  memory(c,p,bytes.length).set(bytes);assert.equal(c.run_session_load(run,1,2,p,bytes.length),1,string(c,c.run_session_error(run)));m.u32(0x4e9a8c,hud);m.view(hud,0x2d4).fill(0);
  const dv=()=>new DataView(c.memory.buffer),progress=c.run_gameplay_field(run,0),player=c.run_gameplay_field(run,1),score=c.run_gameplay_field(run,2),display=c.run_session_hud_score(run);
  m.i32(hud+0x160,dv().getInt32(display,true));m.i32(hud+0x164,dv().getInt32(display+4,true));
  for(let frame=0;frame<1800;frame++){
   // Native 43cc50 calls 439c90 at priority 15, before shots and pickups earn
   // this frame's score. No C++ state is modified by this comparison.
   m.i32(0x4e740c,dv().getInt32(score,true));m.i32(0x4e75bc,dv().getInt32(progress+40,true));m.i32(0x4e75c0,dv().getInt32(progress+44,true));m.i32(0x4e7414,dv().getInt32(progress+28,true));m.u32(0x4e7794,dv().getUint32(player+24,true));m.call(0x439c90);
   assert.equal(c.run_session_step_world(run),1,'frame '+frame+' '+string(c,c.run_session_error(run)));
   const actual=[dv().getInt32(display,true),dv().getInt32(display+4,true),dv().getInt32(progress+40,true),dv().getInt32(progress+44,true),dv().getUint32(player+24,true)],expected=[m.i32(hud+0x160),m.i32(hud+0x164),m.i32(0x4e75bc),m.i32(0x4e75c0),m.u32(0x4e7794)];assert.deepEqual(actual,expected,'actual session/native score callback '+frame);checks+=5;earned=Math.max(earned,dv().getInt32(score,true));
  }
  assert.ok(earned>0,'actual original replay earns score');assert.ok(dv().getInt32(display,true)>0,'HUD advances earned score');report('session-hud-score-integration',{passed:true,frames:1800,checks,earned,displayed:dv().getInt32(display,true),highScore:dv().getInt32(progress+40,true),originalFunctions:['0x43cc50','0x439c90'],scope:'Actual demo2 original input, original resources, full SessionGameplay scheduler and unmodified player/collision behavior. Original 439c90 independently checks animated HUD score, increment, high score, continuation digit and player flags at the exact pre-projectile callback phase. Platform audio/rendering and native complete-world parity remain separate.'});
 }finally{c.release(p);c.run_session_delete(run);asset.close();m.close();}
});
