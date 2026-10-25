import test from 'node:test';
import assert from 'node:assert/strict';
import {assertReplayStateFields} from './state-fields.mjs';
import {compareTraces} from './compare.mjs';
import {digestCanonicalJson, recordsFromSegments} from './adapter.mjs';
import {corpusReadiness, validateCorpus} from './corpus.mjs';
import {formatResult} from './report.mjs';
import {replayMethodControls} from './method-controls.mjs';
import {createHash} from 'node:crypto';

const digest = value => value.toString(16).padStart(32, '0');

test('raw state field validation rejects missing, sparse, nonfinite and out-of-range values before hashing',()=>{
  const row={frame:0xffffffff,point:[-192.5,384],state:[0,255]},shape={uint32:['frame'],vectors:{point:2},byteArrays:{state:2}};
  assert.equal(assertReplayStateFields(row,shape),row);
  for(const mutate of [r=>delete r.frame,r=>r.frame=-1,r=>r.frame=0x100000000,r=>r.frame=1.5,
    r=>r.point[0]=NaN,r=>r.point[0]=Infinity,r=>delete r.point[0],r=>r.point.pop(),
    r=>r.state.pop(),r=>r.state[0]=256,r=>r.state[0]=-1,r=>delete r.state[0]]){
    const changed=structuredClone(row);mutate(changed);assert.throws(()=>assertReplayStateFields(changed,shape));
  }
});
function trace({game = 'th08', ticks = 3, mutate, complete = true, omitCategory,
  coverage = {requiredCategories: ['rng', 'player'], optionalCategories: []}} = {}) {
  const records = [{
    type: 'run-start', schema: 'eagler/replay-trace/v1',
    comparisonIdentity: {game, profile: 'jp-1.00d', replaySha256: 'a'.repeat(64), executableSha256: 'b'.repeat(64), resourceSha256: 'c'.repeat(64), stateSchema: 'th08/state/v1', traceCodec: 'jsonl/v1', digestAlgorithm: 'fixture-128/v1'},
    coverage,
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
  records.push({type: 'run-end', sequence: records.length - 1, reason: 'replay-complete', complete, summary: {segments: 1, ticks}});
  return records;
}

test('equal complete traces pass with declared coverage', async () => {
  const result = await compareTraces(trace(), trace());
  assert.equal(result.status, 'PASS');
  assert.equal(result.comparedTicks, 3);
});

function methodFixture({ticks=3,wrongGame=false,wrongReplay=false,closeError=false}={}){
  const replayBytes=Buffer.from('method-control-replay'),replaySha256=createHash('sha256').update(replayBytes).digest('hex');
  const records=trace({ticks});records[0].comparisonIdentity.replaySha256=wrongReplay?'0'.repeat(64):replaySha256;
  if(wrongGame)records[0].comparisonIdentity.game='th10';
  let closed=0,returned=false;
  return{records,state:()=>({closed,returned}),fixture:{id:'real-window-fixture',replayBytes,replaySha256},requiredCategories:['rng','player'],
    adapter:{describe:()=>({adapterApiVersion:1,game:'th08',adapterVersion:'method-fixture/1',features:{original:true,candidate:true}}),
      inspectReplay:()=>({}),openCandidate:()=>{throw Error('Method controls must not open the game candidate');},openOriginal:async()=>({records:(async function*(){try{yield* records;}finally{returned=true;}})(),
        close:()=>{closed++;if(closeError)throw Error('fixture close failure');}})}};
}

test('real-provider method controls cover inputs, every required state category and all supplied clocks/scalars without certifying a prefix',async()=>{
  const f=methodFixture(),before=structuredClone(f.records),result=await replayMethodControls(f);
  assert.equal(result.passed,true,result.error);assert.equal(result.unchangedPrefix.status,'INCOMPLETE');
  assert.equal(result.unchangedPrefix.comparedTicks,3);assert.deepEqual(result.controls.map(row=>row.id),['input','state:rng','state:player','clocks:game','scalars:score','drop','duplicate','shift']);
  assert.deepEqual(f.records,before);assert.deepEqual(f.state(),{closed:1,returned:true});
});

for(const [name,options]of Object.entries({short:{ticks:2},'other game':{wrongGame:true},'other Replay':{wrongReplay:true},'close failure':{closeError:true}}))
  test('method controls fail on '+name+' rather than publishing a passed method',async()=>{
    const f=methodFixture(options),result=await replayMethodControls(f);assert.equal(result.passed,false);assert.equal(f.state().closed,1);
  });

test('cancelled method controls do not open a reference session',async()=>{
  const f=methodFixture(),result=await replayMethodControls({...f,signal:AbortSignal.abort()});
  assert.equal(result.passed,false);assert.equal(f.state().closed,0);
});

test('reports the earliest category divergence without shifting frames', async () => {
  const actual = trace({mutate: {tick: 1, apply: record => { record.categories.rng = digest(999); }}});
  const result = await compareTraces(trace(), actual);
  assert.equal(result.status, 'DIVERGED');
  assert.equal(result.firstDifference.logicalTick, 1);
  assert.equal(result.firstDifference.category, 'rng');
});

test('a filtered trace reports its native sample and clocks separately from its comparison row',async()=>{
  const expected=trace(),actual=trace({mutate:{tick:1,apply:row=>{row.categories.rng=digest(999);}}});
  for(const records of [expected,actual])for(const row of records.filter(row=>row.type==='tick')){
    row.replaySampleIndex=6052+row.logicalTick;row.clocks={replayFrame:row.replaySampleIndex,gameFrames:5692+row.logicalTick};
  }
  const result=await compareTraces(expected,actual);
  assert.equal(result.firstDifference.logicalTick,1);
  assert.equal(result.firstDifference.replaySampleIndex,6053);
  assert.deepEqual(result.firstDifference.clocks,{replayFrame:6053,gameFrames:5693});
  const human=formatResult(result);assert.match(human,/first difference: stage-1#0:1/);
  assert.match(human,/Replay sample: 6053/);assert.match(human,/"gameFrames":5693/);
});

test('an explicitly incomplete prefix retains earlier divergence and never passes as a complete run',async()=>{
  const prefix=trace({ticks:1,complete:false});
  const result=await compareTraces(trace({ticks:3}),prefix);
  assert.equal(result.status,'INCOMPLETE');assert.equal(result.comparedTicks,1);
  assert.equal(result.reason,'candidate-capture-incomplete');
  const changed=trace({ticks:1,complete:false,mutate:{tick:0,apply:row=>{row.scalars.score=99;}}});
  const divergent=await compareTraces(trace({ticks:3}),changed);
  assert.equal(divergent.status,'DIVERGED');assert.equal(divergent.firstDifference.logicalTick,0);
  assert.equal((await compareTraces(trace({ticks:3}),trace({ticks:1}))).status,'DIVERGED',
    'A declared complete early ending remains a divergence');
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

test('run-end summary is optional, but a present summary must match the trace it closes', async () => {
  const expected = trace(), actual = trace();
  expected.at(-1).summary = {segments: 99, ticks: 999};
  actual.at(-1).summary = {segments: 99, ticks: 999};
  const invalid = await compareTraces(expected, actual);
  assert.equal(invalid.status, 'INCOMPLETE');
  assert.equal(invalid.reason, 'run-end-summary-mismatch');

  const withoutSummaryExpected = trace(), withoutSummaryActual = trace();
  delete withoutSummaryExpected.at(-1).summary;
  delete withoutSummaryActual.at(-1).summary;
  assert.equal((await compareTraces(withoutSummaryExpected, withoutSummaryActual)).status, 'PASS');

  const oneSummaryExpected = trace(), oneSummaryActual = trace();
  delete oneSummaryExpected.at(-1).summary;
  assert.equal((await compareTraces(oneSummaryExpected, oneSummaryActual)).status, 'PASS',
    'A valid optional summary on one trace does not make otherwise equal traces diverge');
});

test('coverage declarations must match and caller requirements only add declared categories', async () => {
  const incompatible = await compareTraces(trace(), trace({coverage: {requiredCategories: ['rng'], optionalCategories: []}}));
  assert.equal(incompatible.status, 'INCOMPATIBLE');
  assert.equal(incompatible.reason, 'coverage');

  const expected = trace();
  const actual = trace({mutate: {tick: 0, apply: record => { record.categories.player = digest(999); }}});
  const narrowed = await compareTraces(expected, actual, {requiredCategories: ['rng']});
  assert.equal(narrowed.status, 'DIVERGED', 'an override cannot turn off header-required player coverage');
  assert.equal(narrowed.firstDifference.category, 'player');

  const optionalCoverage = {requiredCategories: ['rng'], optionalCategories: ['player']};
  const optionalExpected = trace({coverage: optionalCoverage});
  const optionalActual = trace({coverage: optionalCoverage,
    mutate: {tick: 1, apply: record => { record.categories.player = digest(999); }}});
  assert.equal((await compareTraces(optionalExpected, optionalActual)).status, 'PASS');
  const requestedOptional = await compareTraces(optionalExpected, optionalActual, {requiredCategories: ['player']});
  assert.equal(requestedOptional.status, 'DIVERGED');
  assert.equal(requestedOptional.firstDifference.category, 'player');
  assert.equal((await compareTraces(optionalExpected, optionalActual, {requiredCategories: ['not-declared']})).status, 'INCOMPATIBLE');

  const legacyCoverage = {requiredCategories: ['rng', 'player']};
  assert.equal((await compareTraces(trace({coverage: legacyCoverage}), trace({coverage: legacyCoverage}))).status, 'PASS',
    'An omitted optional category list means no optional categories in Replay v1');
});

test('a zero-tick run passes only with a closed segment lifecycle', async () => {
  const expected = trace({ticks: 0}), actual = trace({ticks: 0});
  const result = await compareTraces(expected, actual);
  assert.equal(result.status, 'PASS');
  assert.equal(result.comparedTicks, 0);

  const noSegments = [trace()[0], {type: 'run-end', sequence: 0, reason: 'replay-complete', complete: true, summary: {segments: 0, ticks: 0}}];
  const invalidLifecycle = await compareTraces(noSegments, noSegments);
  assert.equal(invalidLifecycle.status, 'INCOMPLETE');
  assert.equal(invalidLifecycle.reason, 'invalid-run-end-lifecycle');
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
  const overlappingCoverage = trace(); overlappingCoverage[0].coverage.optionalCategories = ['rng'];
  assert.equal((await compareTraces(overlappingCoverage, overlappingCoverage)).status, 'ERROR');
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
