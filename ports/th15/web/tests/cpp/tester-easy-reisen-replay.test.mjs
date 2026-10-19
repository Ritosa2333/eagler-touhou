import test from 'node:test';
import assert from 'node:assert/strict';
import {existsSync,readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,string,root,report,sha,target} from './helpers.mjs';
import {preloadStage} from './stage-assets-fixture.mjs';
const replayFile=resolve(root,'reference/replays/tester-easy-reisen.rpy');
const traceFile=resolve(root,'reference/native/tester-easy-reisen-frames.bin');
test('TH15 supplied Reisen Easy Legacy replay preserves original executable gameplay state through all six stages',{},async()=>{
 const bytes=readFileSync(replayFile),raw=readFileSync(traceFile);
 assert.equal(sha(bytes),'5d43f9d2b21b04386af99313fb690aff968560cb236693e13b22d78089444e24','capture belongs to supplied replay');
 assert.equal(sha(raw),'7c15fa004580f63aaef3ab7759f3d37e76bd60ff5085cefbbb1cc699d9cac437','atomic original-process capture');
 assert.equal(target.sha256,'67a642357c8777089f468aab9c7a0ae346ebdb62849d842a7b7b18d1e6910364','original executable used for capture');
 const stages=new Map();for(let at=0;at<raw.length;at+=80){const s=Array.from({length:20},(_,j)=>raw.readUInt32LE(at+j*4));if(!stages.has(s[0]))stages.set(s[0],new Map());stages.get(s[0]).set(s[1],s);}
 const c=await core(),p=c.allocate(bytes.length),out=c.allocate(64),metadata=c.replay_create(),results=[];
 memory(c,p,bytes.length).set(bytes);assert.equal(c.replay_open(metadata,p,bytes.length),1);
 try{for(let stage=1;stage<=6;stage++){
  const stageRecord=c.replay_stage(metadata,stage),limit=new DataView(c.memory.buffer).getUint32(stageRecord+8,true),samples=stages.get(stage);
  assert.ok(samples);const frames=Math.min(limit,Math.max(...samples.keys()));const asset=preloadStage(c,stage,c.replay_field(metadata,1)),run=c.run_session_create(asset.fixture);
  try{c.stage_assets_viewport(asset.fixture,128,16,320,16);assert.equal(c.run_session_load(run,stage,c.replay_field(metadata,1),p,bytes.length),1,string(c,c.run_session_error(run)));
   let checks=0;const presentationCountDifferences=[];
   for(let frame=1;frame<=frames;frame++){assert.equal(c.run_session_step_world(run),1,string(c,c.run_session_error(run)));const native=samples.get(frame);if(!native||(native[15]&0x800))continue;c.run_session_world_state(run,out);const actual=Array.from(new Uint32Array(c.memory.buffer,out,14));
    assert.deepEqual(actual,native.slice(1,15),'stage '+stage+' frame '+frame+' input/player/score/stock/power/game RNG/enemies');const v=new DataView(c.memory.buffer),player=c.run_gameplay_field(run,1),score=c.run_gameplay_field(run,2);assert.deepEqual([v.getInt32(player+4,true),v.getInt32(player+40,true),v.getInt32(score+40,true)],native.slice(17,20),'stage '+stage+' frame '+frame+' pieces/tier');checks++;
    if(actual[13]!==native[14])presentationCountDifferences.push({frame,cpp:actual[13],native:native[14]});
   }assert.ok(checks>14000);results.push({stage,frames,checks,presentationCountDifferences});
  }finally{c.run_session_delete(run);asset.close();}
 }report('tester-easy-reisen-replay',{passed:true,replaySha256:sha(bytes),originalExecutableSha256:target.sha256,traceSha256:sha(raw),results,checks:results.reduce((n,s)=>n+s.checks,0),scope:'Atomic end-of-frame captures from an isolated real original 1.00b process with update/draw callbacks retained. Only presentation waits and device Present were bypassed for diagnostic speed. Comparison covers all 14 gameplay fields: replay input, exact player position/state, score, deaths, life/Bomb stock, power, gameplay RNG seed/call count, enemies and visible bullets; life/Bomb pieces and extend tier are also exact. Initial/transition loading and out-of-stream stage labels are excluded. Browser lifecycle and live rendering are validated separately.'});
 }finally{c.replay_delete(metadata);c.release(out);c.release(p);}
});
