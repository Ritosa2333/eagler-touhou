import assert from 'node:assert/strict';
import test from 'node:test';
import {functionSource} from './web-input/play-harness.mjs';
import {OwnedNativeTitleRecordStore,validatedOwnedNativeTitleRecord} from '../site/runtime/sound.mjs';
import {createReplayIDB} from './launcher-session/replay-idb.mjs';
import {profileFixture} from './native-score/score-fixture.mjs';
import {practiceReplayMatchesRuntime} from '../site/runtime/thprac128-replay.mjs';

test('a live practice Ending16 hands off to name registration, never straight to Title',async()=>{
  const result={phase:'return-requested',ownerKind:'practice',generation:7};
  const host={owner:{step:async()=>result,input:{release(){}}}};
  const state={endingHost:host,endingTicket:1,endingBusy:false,stageSelection:{thprac:{}},module:{_th128_world_finish_thprac_ending:()=>1}};
  const calls=[];
  const run=new Function('state','finishEndingPostrunReturn','finishOrdinaryEndingReturn','returnToTitle',
    'async '+functionSource('advanceOriginalEnding')+';return advanceOriginalEnding;')(state,
    (h,s)=>calls.push(['name',h,s]),(h,s)=>calls.push(['ordinary',h,s]),()=>{calls.push(['title']);return true;});
  await run();assert.deepEqual(calls,[['name',host,result]]);
});

test('a saved practice score round-trips separately from the ordinary title profile',async()=>{
  const idb=createReplayIDB(),normal=new OwnedNativeTitleRecordStore(idb),practice=new OwnedNativeTitleRecordStore(idb,{practice:true});
  const profile=profileFixture();let digest=2166136261;for(const b of profile)digest=Math.imul(digest^b,16777619)>>>0;
  const native={bound:true,started:true,prepared:true,acknowledged:false,normalClear:false,tainted:true,provenance:'practice-native-postrun',
    generation:7,frames:121,bytes:52232,route:0,rank:2,digest,serial:1,nameCommitted:true};
  const saved=await practice.commit(0,{profile,native,runId:'practice-test-7'});
  assert.equal(saved.strictFullReadback,true);assert.deepEqual((await practice.observe()).profile,profile);
  assert.equal(await normal.observe(),null);
  await assert.rejects(normal.commit(0,{profile,native,runId:'practice-test-7'}),/postrun identity/);
  await assert.rejects(validatedOwnedNativeTitleRecord(saved),/provenance/);
});

test('a practice replay from the prior preview loads only with identical resources and the audited new core',()=>{
  const v122={coreSHA256:'b3c05ff0b1e1016887fd73335ee6fa1443b2cdd6781a470bdf5f0487f39976a3',resourceSHA256:'a'.repeat(64)};
  const v123={...v122,coreSHA256:'54e68d7320a9966feba767e18e481fb0f4f116bcfabeb63da71da68afce59ee1'};
  const v124={...v122,coreSHA256:'1bd134af624d261fb8adeab60c9811cbdbdd8afc030a4b9e851ea60810cc7c33'};
  assert.equal(practiceReplayMatchesRuntime(v122,v123),true);
  assert.equal(practiceReplayMatchesRuntime(v123,v123),true);
  assert.equal(practiceReplayMatchesRuntime(v122,{...v123,resourceSHA256:'b'.repeat(64)}),false);
  assert.equal(practiceReplayMatchesRuntime({...v122,coreSHA256:'c'.repeat(64)},v123),false);
  assert.equal(practiceReplayMatchesRuntime(v122,v124),true);
  assert.equal(practiceReplayMatchesRuntime(v123,v124),true);
  assert.equal(practiceReplayMatchesRuntime(v123,{...v124,resourceSHA256:'b'.repeat(64)}),false);
});

test('older practice replays carry over after updating normal tools with the same resources',()=>{
  const before={coreSHA256:'54e8c73cabdadd627ffd90d0f7566fc8682caf5be0fd7c917d0bb459430f96d9',resourceSHA256:'a'.repeat(64)};
  const normalTools={...before,coreSHA256:'ee582188cf319c996c17d4abc45bd1cb9efd6dea55fb151337b945e690b9dfce'};
  const after={...before,coreSHA256:'a6eeec124b0102d0d25c2b2088f17fef039fdc332626fc92822569911cf72e50'};
  assert.equal(practiceReplayMatchesRuntime(normalTools,after),true);
  assert.equal(practiceReplayMatchesRuntime(before,after),true);
  assert.equal(practiceReplayMatchesRuntime(before,{...after,resourceSHA256:'b'.repeat(64)}),false);
});
