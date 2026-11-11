import test from 'node:test';
import assert from 'node:assert/strict';
import {createChallengeStocks} from '../browser/pc98-challenge-stocks.mjs';

test('native stock boundary tracks deaths, shared pickups, and score extends',()=>{
  const state=createChallengeStocks(3),initial=state.snapshot();
  assert.deepEqual(state.begin(),[{lives:2,bombs:0},{lives:2,bombs:0},{lives:2,bombs:0}]);
  state.finish([{lives:1,bombs:1},{lives:2,bombs:1},{lives:3,bombs:1}]);
  assert.deepEqual(state.snapshot(),[0,-1,-2]);
  const frame=state.snapshot();state.begin(4);state.finish([{lives:3,bombs:0},{lives:1,bombs:0},{lives:0,bombs:0}]);
  assert.deepEqual(state.snapshot(),[-1,0,-2]);state.restore(frame);assert.deepEqual(state.snapshot(),[0,-1,-2]);
  state.restore(initial);assert.deepEqual(state.snapshot(),[0,0,0]);
});

test('native stock boundaries reject invalid or overlapping operations',()=>{
  const state=createChallengeStocks(2);assert.throws(()=>state.finish([]));state.begin();assert.throws(()=>state.begin());assert.throws(()=>state.finish([{lives:1,bombs:0}]));state.abort();assert.deepEqual(state.snapshot(),[0,0]);
});
