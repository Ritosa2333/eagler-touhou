import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { nativeTimerStep } from '../tools/replay/native-timer.mjs';
import { nativeRandomStep } from '../tools/replay/junowen-format.mjs';
const fixture = JSON.parse(readFileSync(new URL('./fixtures/native-primitives.json', import.meta.url), 'utf8'));

test('timer model matches 120 original x86 instruction results', () => {
  assert.equal(fixture.exeSha256, '8918fb39d511c9906ef769c6dd2761e7cb44ded5b7c4c1cd99c8992d12c078e9');
  assert.equal(fixture.timer.length,120);
  for (const vector of fixture.timer) {
    assert.deepEqual(nativeTimerStep(vector.input, vector.speed), vector.output, JSON.stringify(vector));
  }
});

test('RNG model matches 156 original x86 instruction results', () => {
  assert.equal(fixture.rng.length,156);
  for (const vector of fixture.rng) {
    assert.deepEqual(nativeRandomStep(vector.state, vector.range), vector.output, JSON.stringify(vector));
  }
});

test('fractional clock may retain or skip integer indices; a network tick is not a native frame', () => {
  let state={previous:0xffffffff,frame:0,fraction:0,selector:0};
  const indices=[];
  for (let i=0;i<4;i++) {state=nativeTimerStep(state,0.5);indices.push(state.frame);}
  assert.deepEqual(indices,[0,1,1,2]);
  state=nativeTimerStep(state,2);
  assert.equal(state.frame,4);
});
