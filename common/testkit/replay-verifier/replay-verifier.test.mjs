import test from 'node:test';
import assert from 'node:assert/strict';
import {compareTraces} from './compare.mjs';
import {digestCanonicalJson, recordsFromSegments} from './adapter.mjs';
import {corpusReadiness, validateCorpus} from './corpus.mjs';

const digest = value => value.toString(16).padStart(32, '0');
function trace({game = 'th08', ticks = 3, mutate, complete = true, omitCategory} = {}) {
  const records = [{
    type: 'run-start', schema: 'eagler/replay-trace/v1',
    comparisonIdentity: {game, profile: 'jp-1.00d', replaySha256: 'a'.repeat(64), executableSha256: 'b'.repeat(64), resourceSha256: 'c'.repeat(64), stateSchema: 'th08/state/v1', traceCodec: 'jsonl/v1', digestAlgorithm: 'fixture-128/v1'},
    coverage: {requiredCategories: ['rng', 'player'], optionalCategories: []},
    provenance: {provider: 'fixture'},
  }, {type: 'segment-start', sequence: 0, segmentId: 'stage-1#0', route: 'stage-1'}];
  for (let logicalTick = 0; logicalTick < ticks; logicalTick++) {
    const categories = {rng: digest(logicalTick + 1), player: digest(logicalTick + 10)};
    if (omitCategory === logicalTick) delete categories.player;
    const record = {type: 'tick', sequence: records.length - 1, segmentId: 'stage-1#0', logicalTick,
      replaySampleIndex: logicalTick, clockDisposition: 'advanced', appliedInput: logicalTick & 1,
      clocks: {game: logicalTick}, scalars: {score: logicalTick * 10}, categories};
    if (mutate && mutate.tick === logicalTick) mutate.apply(record);
    records.push(record);
  }
  records.push({type: 'segment-end', sequence: records.length - 1, segmentId: 'stage-1#0', logicalTick: ticks, reason: 'route-complete', complete});
  records.push({type: 'run-end', sequence: records.length - 1, reason: 'replay-complete', complete, summary: {segments: 1}});
  return records;
}

test('equal complete traces pass with declared coverage', async () => {
  const result = await compareTraces(trace(), trace());
  assert.equal(result.status, 'PASS');
  assert.equal(result.comparedTicks, 3);
});

test('reports the earliest category divergence without shifting frames', async () => {
  const actual = trace({mutate: {tick: 1, apply: record => { record.categories.rng = digest(999); }}});
  const result = await compareTraces(trace(), actual);
  assert.equal(result.status, 'DIVERGED');
  assert.equal(result.firstDifference.logicalTick, 1);
  assert.equal(result.firstDifference.category, 'rng');
});

test('reports dropped records as incomplete rather than aligning later ticks', async () => {
  const actual = trace(); actual.splice(3, 1);
  const result = await compareTraces(trace(), actual);
  assert.equal(result.status, 'INCOMPLETE');
  assert.equal(result.reason, 'sequence-discontinuity');
  assert.equal(result.side, 'actual');
});

test('rejects a different game/profile identity before comparing ticks', async () => {
  const result = await compareTraces(trace(), trace({game: 'th10'}));
  assert.equal(result.status, 'INCOMPATIBLE');
  assert.equal(result.reason, 'comparison-identity');
});

test('a declared category missing from one tick is a divergence', async () => {
  const result = await compareTraces(trace(), trace({omitCategory: 0}));
  assert.equal(result.status, 'DIVERGED');
  assert.equal(result.reason, 'missing-category');
});

test('a truncated or explicitly incomplete run cannot pass', async () => {
  const truncated = trace(); truncated.pop();
  assert.equal((await compareTraces(trace(), truncated)).status, 'INCOMPLETE');
  assert.equal((await compareTraces(trace(), trace({complete: false}))).status, 'INCOMPLETE');
});

test('rejects logical tick gaps even when both traces contain the same gap', async () => {
  const expected = trace(), actual = trace();
  expected[3].logicalTick = 4; actual[3].logicalTick = 4;
  const result = await compareTraces(expected, actual);
  assert.equal(result.status, 'INCOMPLETE');
  assert.equal(result.reason, 'logical-tick-discontinuity');
});

test('rejects records after a completed run', async () => {
  const expected = trace(), actual = trace();
  actual.push({type: 'run-end', sequence: 6, reason: 'duplicate', complete: true});
  const result = await compareTraces(expected, actual);
  assert.equal(result.status, 'INCOMPLETE');
  assert.equal(result.reason, 'records-after-run-end');
});

test('rejects a duplicate run-start and non-canonical floating JSON values', async () => {
  const duplicated = trace(); duplicated.splice(2, 0, duplicated[0]);
  assert.equal((await compareTraces(duplicated, duplicated)).reason, 'duplicate-run-start');
  const floating = trace(); floating[2].scalars.score = 0.5;
  assert.equal((await compareTraces(trace(), floating)).status, 'ERROR');
});

test('shared adapter frames multiple title-owned segments without hiding incomplete runs', () => {
  const records = [...recordsFromSegments({
    comparisonIdentity: trace()[0].comparisonIdentity,
    coverage: {requiredCategories: ['rng'], optionalCategories: []}, provenance: {provider: 'test'},
    segments: [
      {route: 'stage-1', ticks: [{replaySampleIndex: 1, clockDisposition: 'advanced', appliedInput: 0, clocks: {}, scalars: {}, categories: {rng: digestCanonicalJson([1, 2])}}]},
      {route: 'stage-2', ticks: [], complete: false, reason: 'capture-aborted'},
    ],
  })];
  assert.deepEqual(records.map(record => record.sequence).filter(value => value !== undefined), [0, 1, 2, 3, 4, 5]);
  assert.equal(records.at(-1).complete, false);
  assert.deepEqual(records.at(-1).summary, {segments: 2, ticks: 1});
});

test('corpus contract freezes the game-owned Demo rotation order', () => {
  const corpus = {schema: 'eagler/replay-corpus/v1', game: 'th10', policy: {gameplayMutation: 'forbidden'},
    demo: {rotationOrder: ['demo/demo1.rpy', 'demo/demo2.rpy', 'demo/demo3.rpy', 'demo/demo0.rpy']},
    cases: [
      ...['demo1', 'demo2', 'demo3', 'demo0'].map(name => ({id: name, kind: 'demo', source: `demo/${name}.rpy`, status: 'missing-original-provider'})),
      {id: 'lunatic', kind: 'lunatic', status: 'missing-replay'},
    ]};
  assert.equal(corpusReadiness(corpus).available, 0);
  const reordered = structuredClone(corpus); [reordered.cases[0], reordered.cases[1]] = [reordered.cases[1], reordered.cases[0]];
  assert.throws(() => validateCorpus(reordered), /rotation order/);
});
