import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import test from 'node:test';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const source=fs.readFileSync(path.join(root,'site/play.mjs'),'utf8');
const start=source.indexOf('  replayWorldInputs:');
const end=source.indexOf('  advanceWorld:',start);
assert.ok(start>=0&&end>start,'replayWorldInputs must remain a top-level debug method');
const snippet=source.slice(start,end).trim();
assert.ok(snippet.startsWith('replayWorldInputs:'));
const guardStart=source.indexOf('function assertSynchronousWorldDrainAllowed('),guardEnd=source.indexOf('\n}',guardStart)+2;
assert(guardStart>=0&&guardEnd>guardStart);
const guard=source.slice(guardStart,guardEnd);
const makeReplay=new Function('state','clock','drainWorld','gameplayInputMask','resetHighFrameratePresentation','syncDebugRespawnUI',`${guard}\nreturn ({${snippet}}).replayWorldInputs;`);

function runtime(gameplayInputMask=()=>761){
  const calls=[],ticks=[],drains=[];
  let backgroundTicks=0;
  const state={
    worldMode:true,paused:false,pressed:new Set(['KeyZ']),worldSnapshot:null,
    background:{tick(){++backgroundTicks;}},
    module:{
      _th128_world_input(holding,pressed,released){calls.push([holding,pressed,released]);return 1;},
      _th128_world_tick(){ticks.push(ticks.length);return 1;},
      _th128_world_error(){return '';},
      _th128_world_state(){return JSON.stringify({flow:{},ticks:ticks.length});},
      UTF8ToString(value){return value;},
    },
  };
  const clock={resetCount:0,reset(){++this.resetCount;}};
  return {
    state,clock,calls,ticks,drains,
    get backgroundTicks(){return backgroundTicks;},
    replay:makeReplay(state,clock,()=>{drains.push(drains.length);},gameplayInputMask,()=>{},()=>{}),
  };
}

function assertNoInputMutation(run){
  assert.deepEqual(run.calls,[]);
  assert.deepEqual(run.ticks,[]);
  assert.deepEqual(run.drains,[]);
  assert.equal(run.backgroundTicks,0);
  assert.equal(run.state.paused,false);
  assert.equal(run.clock.resetCount,0);
  assert.equal(run.state.worldSnapshot,null);
}

test('default replay keeps the legacy press/hold/release sequence',()=>{
  const rows=[[2,1,0],[3,1,0]];
  const run=runtime();
  const result=run.replay(rows);
  assert.deepEqual(run.calls,[[1,1,0],[1,0,0],[0,0,1]]);
  assert.equal(run.ticks.length,5);
  assert.equal(run.backgroundTicks,5);
  assert.equal(run.drains.length,5);
  assert.equal(result.lastHolding,0);
  assert.equal(run.state.paused,true);
  assert.equal(run.clock.resetCount,1);
  assert.equal(run.state.pressed.size,0);
  assert.deepEqual(result.snapshot,{flow:{},ticks:5});
  assert.equal(result.error,'');

  const explicit=runtime();
  const explicitResult=explicit.replay(rows,{initialHolding:0,releaseAtEnd:true});
  assert.deepEqual(explicit.calls,run.calls);
  assert.deepEqual(explicit.ticks,run.ticks);
  assert.equal(explicitResult.lastHolding,result.lastHolding);
});

test('chunked contiguous input is equivalent to one batch and creates no boundary edge',()=>{
  const rows=[[2,1,0],[3,1,0],[1,1,0]];
  const single=runtime();
  single.replay(rows,{initialHolding:0,releaseAtEnd:true});

  const chunked=runtime();
  const first=chunked.replay(rows.slice(0,2),{initialHolding:0,releaseAtEnd:false});
  assert.deepEqual(chunked.calls,[[1,1,0],[1,0,0]]);
  assert.equal(first.lastHolding,1);
  const second=chunked.replay(rows.slice(2),{initialHolding:first.lastHolding,releaseAtEnd:true});
  assert.equal(second.lastHolding,0);

  assert.deepEqual(chunked.calls,single.calls);
  assert.deepEqual(chunked.ticks,single.ticks);
  assert.equal(chunked.backgroundTicks,single.backgroundTicks);
  assert.equal(chunked.drains.length,single.drains.length);
  assert.deepEqual(chunked.calls,[[1,1,0],[1,0,0],[1,0,0],[0,0,1]]);
});

test('a real row release is preserved and an active continuation has no fake keyup',()=>{
  const released=runtime();
  const releaseResult=released.replay([[1,1,0],[1,0,0]],{initialHolding:0,releaseAtEnd:false});
  assert.deepEqual(released.calls,[[1,1,0],[0,0,1]]);
  assert.equal(releaseResult.lastHolding,0);
  assert.equal(released.ticks.length,2);

  const continued=runtime();
  const continuedResult=continued.replay([[1,1,0]],{initialHolding:1,releaseAtEnd:false});
  assert.deepEqual(continued.calls,[[1,0,0]]);
  assert.equal(continuedResult.lastHolding,1);
  assert.equal(continued.ticks.length,1);
});

test('releaseAtEnd with an initial prefix releases once at the end',()=>{
  const run=runtime();
  const result=run.replay([],{initialHolding:1,releaseAtEnd:true});
  assert.deepEqual(run.calls,[[0,0,1]]);
  assert.equal(result.lastHolding,0);
  assert.equal(run.ticks.length,0);
});

test('invalid options and rows reject before pausing or touching world input',()=>{
  for(const options of [null,[],{initialHolding:-1},{initialHolding:762},{initialHolding:256},{initialHolding:2},
      {initialHolding:1.5},{initialHolding:NaN},{releaseAtEnd:0},{releaseAtEnd:'true'}]){
    const run=runtime();
    assert.throws(()=>run.replay([[1,1,0]],options),/Invalid replay/i);
    assertNoInputMutation(run);
    assert.equal(run.state.pressed.has('KeyZ'),true);
  }
  const run=runtime();
  assert.throws(()=>run.replay([[1,1,2]]),/Invalid recorded diagnostic input range/);
  assertNoInputMutation(run);
  assert.equal(run.state.pressed.has('KeyZ'),true);
});

test('capability-gated diagnostic mask accepts X bit2 and rejects unknown bits',()=>{
  const run=runtime(()=>763);
  const result=run.replay([[1,2,0]],{initialHolding:2,releaseAtEnd:false});
  assert.deepEqual(run.calls,[[2,0,0]]);
  assert.equal(result.lastHolding,2);
  assert.deepEqual(run.ticks,[0]);

  const unknown=runtime(()=>763);
  assert.throws(()=>unknown.replay([[1,764,0]]),/Invalid recorded diagnostic input range/);
  assertNoInputMutation(unknown);
  assert.throws(()=>unknown.replay([],{initialHolding:764}),/Invalid replay initial holding/);
  assertNoInputMutation(unknown);
});
