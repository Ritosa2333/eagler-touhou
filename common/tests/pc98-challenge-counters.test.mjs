import test from 'node:test';
import assert from 'node:assert/strict';
import {createChallengeCounters} from '../browser/pc98-challenge-counters.mjs';

test('challenge counts each player independently and allows negative values',()=>{
  const counters=createChallengeCounters(3);
  counters.miss(0);counters.miss(0);counters.miss(2);counters.item(1);counters.extend([0,1,2]);
  assert.deepEqual(counters.snapshot(),[1,-2,0]);
  const saved=counters.snapshot();counters.miss(1);counters.restore(saved);
  assert.deepEqual(counters.snapshot(),[1,-2,0]);counters.reset();assert.deepEqual(counters.snapshot(),[0,0,0]);
});
test('challenge validates player indices and checkpoint contents',()=>{
  const counters=createChallengeCounters(2);assert.throws(()=>counters.miss(2));assert.throws(()=>counters.restore([0,NaN]));assert.throws(()=>counters.extend([0,2]));assert.deepEqual(counters.snapshot(),[0,0]);
});


test('same-frame hits and rewards survive rollback without partial mutations',()=>{
  const c=createChallengeCounters(3), checkpoint=c.snapshot();
  const events=[{kind:'miss',slots:[0]},{kind:'bomb',slots:[0,1,2]},{kind:'extend',slots:[0,1,2]}];
  c.applyFrame(events);assert.deepEqual(c.snapshot(),[-1,-2,-2]);
  c.restore(checkpoint);c.applyFrame(events);assert.deepEqual(c.snapshot(),[-1,-2,-2]);
  assert.throws(()=>c.applyFrame([{kind:'miss',slots:[0]},{kind:'life',slots:[5]}]));assert.deepEqual(c.snapshot(),[-1,-2,-2]);
});
