import test from 'node:test';
import assert from 'node:assert/strict';
import {validatePlayback} from './playback-validation.mjs';

const expected = {sha256: 'a'.repeat(64), character: 1, shot: 1, difficulty: 3,
  lastStage: 8, score: 3000, stages: [{stage: 1, score: 0}, {stage: 2, score: 1000}]};
function result() {
  const identity = {character: 1, shot: 1, difficulty: 3};
  return {replay: {sha256: expected.sha256},
    transitions: expected.stages.map(row => ({...identity, ...row, replayMode: 2})),
    final: {...identity, replayMode: 1, stage: 2, score: 3000}};
}
test('complete checkpoint match is explicitly not deterministic equivalence', () => {
  assert.deepEqual(validatePlayback(expected, result()), {
    passed: true, coverage: 'original-playback-route-and-score-checkpoints', deterministicEquivalence: false,
  });
});
for (const [name, mutate] of [
  ['early return to title', value => { value.transitions.pop(); }],
  ['wrong final score', value => { value.final.score--; }],
  ['wrong stage entry score', value => { value.transitions[1].score++; }],
  ['wrong character', value => { value.transitions[0].character = 0; }],
  ['wrong fixture', value => { value.replay.sha256 = 'b'.repeat(64); }],
  ['still playing', value => { value.final.replayMode = 2; }],
  ['other terminal state', value => { value.final.replayMode = 0; }],
]) test(`reject ${name}`, () => {
  const value = result(); mutate(value);
  assert.throws(() => validatePlayback(expected, value));
});
