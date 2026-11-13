import assert from 'node:assert/strict';
import {evaluateFrameWork, installFrameWorkCapture} from './frame-work.mjs';

const prior = [];
const module = {onGameWorkFrame: row => prior.push(row.sequence)};
const stream = installFrameWorkCapture(module);
for (let i = 0; i < 40; ++i) {
  const begin = i * 20;
  const phase = ['loading', 'suspended', 'paused'][i] ?? 'active';
  module.onGameWorkFrame({sequence: i + 1, begin, end: begin + 2, milliseconds: 2,
    stage: i < 20 ? 1 : 2, endStage: i < 20 ? 1 : 2, phase, source: i % 3 + 1,
    simulated: i === 0 ? 0 : i === 4 ? 3 : 1, rollback: i === 4 ? 2 : 0,
    restores: i === 4 ? 1 : 0, presentations: i === 0 ? 0 : 1,
    presentationAt: i === 0 ? null : begin + 1, ok: true});
}
const samples = stream.stop();
const options = {expectedStages: [1, 2], expectedPhases: ['active', 'loading', 'paused', 'suspended'],
  expectedFirstSequence: 1, requireZero: true, requireCatchup: true, requireRollback: true,
  capture: {...stream.capture}};
assert.equal(prior.length, 40);
assert.equal(evaluateFrameWork(samples, options).passed, true);
assert.equal(evaluateFrameWork(samples, options).presentationIntervals.count, 38);
const reject = (rows, opts = options) => assert.equal(evaluateFrameWork(rows, opts).passed, false);
reject([]);
reject(samples.slice(1));
reject(samples.slice(1), {...options, capture: {first: 2, last: 40, count: 39}});
reject(samples.slice(0, -1));
reject(samples.filter((_, i) => i !== 10));
reject(samples.flatMap((row, i) => i === 10 ? [row, {...row}] : [row]));
reject(samples.map((row, i) => i === 10 ? {...row, ok: false} : row));
reject(samples.map(row => ({...row, rollback: 0, restores: 0})));
reject(samples.map(row => ({...row, phase: row.phase === 'loading' ? 'active' : row.phase})));
reject(samples, {...options, expectedStages: [1, 2, 3]});
reject(samples.map(row => row.stage === 2 ? {...row, milliseconds: 20, end: row.begin + 20} : row));
reject(samples.map(row => ({...row, milliseconds: 20, end: row.begin + 20})));
reject(samples.map((row, i) => i === 5 ? {...row, presentationAt: row.end + 1} : row));
// Synchronous loading callbacks are whole work records in the same sequence.
// They may submit without exposing the exact presentation timestamp.
const startupIndices = [0, 1, 7, 8, 9, 10];
const startup = samples.map((row, i) => startupIndices.includes(i)
  ? {...row, source: startupIndices.indexOf(i) + 4, phase: 'loading',
    simulated: 0, rollback: 0, restores: 0, presentationAt: null}
  : i === 11 ? {...row, phase: 'suspended'} : row);
assert.equal(evaluateFrameWork(startup, options).passed, true);
reject(startup.map((row, i) => i === 0 ? {...row, source: 11} : row));
reject(startup.map((row, i) => i === 0 ? {...row, phase: 'active'} : row));
reject(startup.map((row, i) => i === 1 ? {...row, simulated: 1} : row));
reject(startup.map((row, i) => i === 7 ? {...row, begin: startup[6].begin} : row));
reject(startup.map((row, i) => i === 8 ? {...row, ok: false} : row));
const readyPoll = startup.map((row, i) => i === 10 ? {...row, source: 10} : row);
assert.equal(evaluateFrameWork(readyPoll, options).passed, true);
// One expensive actual initialization remains visible in its source, even
// when it does not change the mandated overall/stage p95.
const slowStartup = readyPoll.map((row, i) => i === 9 ? {...row, milliseconds: 19, end: row.begin + 19} : row);
assert.equal(evaluateFrameWork(slowStartup, options).sources['8'].max, 19);
const ambiguous = samples.map((row, i) => i === 5 ? {...row, presentations: 2, presentationAt: null} : row);
assert.equal(evaluateFrameWork(ambiguous, options).ambiguousPresentationIntervals, 1);
assert.throws(() => stream.stop());
const preWorld = samples.map((row, i) => i === 0
  ? {...row, stage: 0, endStage: 1, phase: 'loading', source: 4,
      milliseconds: 19, end: row.begin + 19} : row);
const preWorldResult = evaluateFrameWork(preWorld, options);
assert.equal(preWorldResult.passed, true);
assert.equal(preWorldResult.stages['0'].max, 19);
assert.equal(preWorldResult.total.max, 19);
reject(preWorld.map((row, i) => i === 0 ? {...row, phase: 'active', source: 1} : row));
// Startup work is never removed or exempted from the overall hard limit.
const slowPreWorld = evaluateFrameWork(preWorld.map((row, i) => i >= 5 && i < 9 ? {...row, stage: 0, endStage: 0,
  phase: 'loading', source: 4, simulated: 0, rollback: 0, restores: 0,
  milliseconds: 19, end: row.begin + 19} : row), options);
assert.deepEqual(slowPreWorld.errors, ['Overall work p95 exceeds limit']);
assert(slowPreWorld.total.p95 > 16.67);
const sparseSlowStage = evaluateFrameWork(samples.map((row, i) => i >= 20 && i < 22
  ? {...row, milliseconds: 19, end: row.begin + 19} : row), options);
assert(sparseSlowStage.total.p95 <= 16.67);
assert.deepEqual(sparseSlowStage.errors, ['Stage 2 work p95 exceeds limit']);
reject(preWorld, {...options, expectedStages: [1, 2, 3]});
console.log('frame-work: positive and 13 corruption/coverage controls passed');
