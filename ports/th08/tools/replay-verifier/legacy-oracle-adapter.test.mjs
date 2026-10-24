import test from 'node:test';
import assert from 'node:assert/strict';
import {compareTraces} from '../../../../common/testkit/replay-verifier/compare.mjs';
import * as common from '../../../../common/testkit/replay-verifier/adapter.mjs';
import {recordsFromLongRows} from './legacy-oracle-adapter.mjs';

const fixture = {replaySha256: 'a'.repeat(64)};
const identity = {executableSha256: 'b'.repeat(64), resourceSha256: 'c'.repeat(64)};

function row(frame, gameFrames, {flags = 0xc, rng = frame, rngCalls = frame * 2} = {}) {
  return {
    frame, stage: 7, gameFrames, controlFrames: frame, flags, rng, rngCalls,
    player: [192, 384], playerState: 3, keys: 0, numbers: Array(228).fill(0),
  };
}

function trace(rows, provider) {
  return recordsFromLongRows({fixture, rows, provider, identity, complete: true}, common);
}

test('long traces ignore route provenance and frozen Present cadence', async () => {
  const expected = [row(1, 10, {flags: 0x80c}), row(2, 10), row(3, 10), row(4, 11, {flags: 0x80c})];
  const actual = [row(1, 10), row(4, 11)];
  const result = await compareTraces(trace(expected, 'retail'), trace(actual, 'browser'));
  assert.equal(result.status, 'PASS');
  assert.equal(result.comparedTicks, 2);
});

test('RNG consumed while frozen is exposed at the next gameplay tick', async () => {
  const expected = [row(1, 10), row(2, 10, {rngCalls: 40}), row(3, 11, {rng: 9, rngCalls: 42})];
  const actual = [row(1, 10), row(3, 11, {rng: 8, rngCalls: 44})];
  const result = await compareTraces(trace(expected, 'retail'), trace(actual, 'browser'));
  assert.equal(result.status, 'DIVERGED');
  assert.equal(result.firstDifference.difference.path, 'scalars.rngCalls');
});

test('title content availability is profile metadata while every simulation flag stays authoritative',async()=>{
  const expected=[row(1,10),row(2,11)],actual=[row(1,10,{flags:0x3800c}),row(2,11,{flags:0x3800c})];
  const result=await compareTraces(trace(expected,'retail'),trace(actual,'browser'));
  assert.equal(result.status,'PASS');
  for(const bit of [0x1,0x4,0x8,0x4000]){
    const changed=actual.map(item=>({...item,flags:item.flags^bit}));
    const failed=await compareTraces(trace(expected,'retail'),trace(changed,'browser'));
    assert.equal(failed.status,'DIVERGED');assert.equal(failed.firstDifference.difference.path,'scalars.flags');
  }
  assert.throws(()=>[...trace([row(1,10),row(2,11,{flags:0x3800c})],'bad-profile')],/availability changed/);
});
