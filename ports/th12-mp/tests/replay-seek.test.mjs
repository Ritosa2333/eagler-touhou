import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {createReplaySeeker} from '../src/replay-seek.mjs';
import {parseReplay} from '../src/replay.mjs';
import {initializeCandidate,restoreStage,state} from '../tools/replay-verifier/capture-candidate.mjs';

function fixture(inputs,{endAt=Infinity,endCode=2,frames=inputs.length}={}){
  const heap=new Uint8Array(256),hud=new Float32Array(heap.buffer,0,15),ticks=[];
  let starts=0;
  const core={HEAPU8:heap,_malloc:()=>64,_free(){},_th12_hud:()=>0,_th12_hud_size:()=>15,
    _th12_start_replay(){starts++;ticks.length=0;hud.fill(0);return 1;},
    _th12_tick(held,pressed){ticks.push([held,pressed]);hud[0]++;if(ticks.length===endAt)hud[13]=endCode;}};
  const stage={number:1,seed:1234,frames,initial:{x:0,y:0,power:100,lives:2,bombs:2,
    score:0,pointValue:10000,lifeFragments:0,bombFragments:0,ufoColors:[0,0,0],rank:0,nativeGlobals:[0,0,0,0]}};
  const reads=[],replay={character:2,shot:1,difficulty:1,
    input(number,cursor){assert.equal(number,1);reads.push(cursor);return inputs[cursor]??null;}};
  return {core,stage,replay,ticks,reads,starts:()=>starts};
}
const row=(held,pressed=held)=>({held,pressed,released:0});
const sequence=length=>Array.from({length},(_,i)=>row(i%2?545:512,i%2?33:512));

test('seek restarts at original header and consumes exactly the recorded inputs including Ctrl',async()=>{
  const f=fixture([row(0,0),row(512),row(545,33),row(1,0),row(0,0)]),progress=[],observed=[];
  let yields=0,resets=0;
  const seeker=createReplaySeeker(f.core,{batchTicks:2,yieldTask:async()=>{yields++;},
    onReset:core=>{assert.equal(core,f.core);assert.equal(f.ticks.length,0);resets++;},
    onTick:(core,cursor,input)=>observed.push([cursor,input.held])});
  const result=await seeker.seek(f.replay,f.stage,4,{onProgress:p=>progress.push(p.cursor)});
  assert.deepEqual(result,{cursor:4,target:4,previous:1,reason:'target-reached'});
  assert.deepEqual(f.ticks,[[0,0],[512,512],[545,33],[1,0]]);
  assert.deepEqual(f.reads,[0,1,2,3]);assert.equal(yields,1);
  assert.deepEqual(observed,[[1,0],[2,512],[3,545],[4,1]]);assert.deepEqual(progress,[0,2,4]);
  assert.equal(f.starts(),1);assert.equal(resets,1);
});

test('a newer seek cancels the old request before it can consume more inputs',async()=>{
  const f=fixture(sequence(12)),pending=[];
  const seeker=createReplaySeeker(f.core,{batchTicks:2,yieldTask:()=>new Promise(resolve=>pending.push(resolve))});
  const first=seeker.seek(f.replay,f.stage,10);
  assert.deepEqual(f.ticks,[[512,512],[545,33]]);
  const second=await seeker.seek(f.replay,f.stage,1);
  assert.equal(second.cursor,1);assert.equal(second.reason,'target-reached');
  pending.shift()();const old=await first;
  assert.equal(old.reason,'cancelled');assert.equal(old.cursor,2);
  assert.deepEqual(f.ticks,[[512,512]]);assert.equal(f.starts(),2);
});

test('explicit cancellation leaves the last consumed cursor and stops at a yield boundary',async()=>{
  const f=fixture(sequence(12));let resume;
  const seeker=createReplaySeeker(f.core,{batchTicks:3,yieldTask:()=>new Promise(resolve=>{resume=resolve;})});
  const pending=seeker.seek(f.replay,f.stage,10);seeker.cancel();resume();
  const result=await pending;assert.equal(result.cursor,3);assert.equal(result.reason,'cancelled');assert.equal(f.ticks.length,3);
});

test('EOS is never consumed; held-only ffff remains a valid input',async()=>{
  const f=fixture([row(0),row(0xffff,0),{held:0xffff,pressed:0xffff,released:0xffff},row(1)],{frames:8});
  const result=await createReplaySeeker(f.core).seek(f.replay,f.stage,7);
  assert.equal(result.cursor,2);assert.equal(result.reason,'replay-input-ended');
  assert.deepEqual(f.ticks,[[0,0],[0xffff,0]]);assert.deepEqual(f.reads,[0,1,2]);
});

test('Game Over and stage completion include their final tick and do not force playback past it',async()=>{
  for(const [endCode,reason]of [[2,'game-over'],[1,'stage-ended']]){
    const f=fixture(sequence(10),{endAt:3,endCode});
    const result=await createReplaySeeker(f.core).seek(f.replay,f.stage,9);
    assert.deepEqual(result,{cursor:3,target:9,previous:512,reason});assert.equal(f.ticks.length,3);
  }
});

test('target cursor is floored and clamped to the recorded stage range',async()=>{
  const f=fixture(sequence(5)),seeker=createReplaySeeker(f.core);
  assert.deepEqual(await seeker.seek(f.replay,f.stage,-1),{cursor:0,target:0,previous:0,reason:'target-reached'});
  assert.deepEqual(await seeker.seek(f.replay,f.stage,2.9),{cursor:2,target:2,previous:545,reason:'target-reached'});
  assert.deepEqual(await seeker.seek(f.replay,f.stage,999),{cursor:5,target:5,previous:512,reason:'replay-input-ended'});
  await assert.rejects(seeker.seek(f.replay,f.stage,NaN),/位置无效/);
});

test('advancing from the current cursor consumes only one new tick without restarting',async()=>{
  const f=fixture(sequence(10));let resets=0;
  const seeker=createReplaySeeker(f.core,{onReset:()=>resets++});
  await seeker.seek(f.replay,f.stage,3);
  const ticks=f.ticks.length;
  const result=await seeker.seek(f.replay,f.stage,4,{restart:false,fromCursor:3});
  assert.deepEqual(result,{cursor:4,target:4,previous:545,reason:'target-reached'});
  assert.equal(f.ticks.length,ticks+1);assert.deepEqual(f.ticks.at(-1),[545,33]);
  assert.equal(f.starts(),1);assert.equal(resets,1);
  const unchanged=await seeker.seek(f.replay,f.stage,4,{restart:false,fromCursor:4});
  assert.equal(unchanged.previous,545);assert.equal(f.ticks.length,ticks+1);
  await assert.rejects(seeker.seek(f.replay,f.stage,2,{restart:false,fromCursor:4}),/不能倒退/);
  assert.equal(f.ticks.length,ticks+1);
});

test('advancing an already ended core keeps its cursor and never restarts or ticks',async()=>{
  const f=fixture(sequence(10),{endAt:3});let resets=0;
  const seeker=createReplaySeeker(f.core,{onReset:()=>resets++});
  await seeker.seek(f.replay,f.stage,8);
  const result=await seeker.seek(f.replay,f.stage,9,{restart:false,fromCursor:3});
  assert.deepEqual(result,{cursor:3,target:9,previous:512,reason:'game-over'});
  assert.equal(f.ticks.length,3);assert.equal(f.starts(),1);assert.equal(resets,1);
});

const productionReplay=new URL('../site/replays/th12_17.rpy',import.meta.url);
test('production stage1 and stage2 state is identical after rewind and reconstruction',
  {skip:!fs.existsSync(productionReplay)},async()=>{
  const file=productionReplay;
  const replay=parseReplay(fs.readFileSync(file));
  for(const number of [1,2]){
    const stage=replay.stages.find(s=>s.number===number),core=await initializeCandidate(number),target=9000;
    assert.ok(stage);restoreStage(core,replay,stage);
    for(let cursor=0;cursor<target;cursor++){const input=replay.input(number,cursor);core._th12_tick(input.held,input.pressed);}
    const expected=state(core);
    for(let cursor=target;cursor<target+50;cursor++){const input=replay.input(number,cursor);core._th12_tick(input.held,input.pressed);}
    const result=await createReplaySeeker(core,{batchTicks:113,yieldTask:async()=>{}}).seek(replay,stage,target);
    assert.equal(result.reason,'target-reached');assert.equal(result.cursor,target);
    assert.deepEqual(state(core),expected,'full production state after rewinding stage'+number);
  }
});
