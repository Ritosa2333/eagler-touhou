import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {recordsFromSegments} from './adapter.mjs';
import {runReplaySuite} from './suite.mjs';

const digest = bytes => createHash('sha256').update(bytes).digest('hex');
const identity = replaySha256 => ({game:'th08',profile:'original/v1',replaySha256,
  executableSha256:'a'.repeat(64),resourceSha256:'b'.repeat(64),stateSchema:'state/v1',
  traceCodec:'jsonl/v1',digestAlgorithm:'sha256-truncated-128/canonical-json-v1'});
function trace(replaySha256, value, complete = true) {
  return [...recordsFromSegments({comparisonIdentity:identity(replaySha256),
    coverage:{requiredCategories:['world'],optionalCategories:[]},provenance:{provider:'fixture'},
    segments:[{route:'stage-0',ticks:[{replaySampleIndex:1,clockDisposition:'advanced',appliedInput:0,
      clocks:{gameFrame:1},scalars:{stage:0},categories:{world:digest(Buffer.from(value)).slice(0,32)}}],complete}],
    runReason:complete?'natural-terminal':'capture-incomplete'})];
}

function adapter({candidateTrace, inspect = () => ({format:'fixture'}), originalClose = () => {}, candidateClose = () => {}} = {}) {
  return {describe:()=>({adapterApiVersion:1,game:'th08',adapterVersion:'test/v1',features:{original:true,candidate:true}}),
    inspectReplay:inspect,
    async openOriginal({fixture}) {return {records:trace(fixture.replaySha256,'same'),close:originalClose,provenance:{provider:'immutable-fixture'}};},
    async openCandidate({fixture}) {return {records:candidateTrace?.(fixture)??trace(fixture.replaySha256,'same'),close:candidateClose,provenance:{provider:'candidate-fixture'}};}};
}

function fixture(id, replayBytes = Buffer.from(id)) {
  return {id,status:'ready',replayBytes,replaySha256:digest(replayBytes)};
}

test('shared Replay suite verifies every selected fixture through the common tick comparator',async()=>{
  let closed=0,inspected=0;
  const cases=[fixture('demo0'),fixture('demo1')];
  const result=await runReplaySuite({adapter:adapter({inspect:()=>{inspected++;return{format:'fixture'};},
    originalClose:()=>{closed++;},candidateClose:()=>{closed++;}}),cases,expectedIds:['demo0','demo1']});
  assert.equal(result.status,'PASS');assert.equal(result.complete,true);
  assert.deepEqual(result.results.map(item=>item.comparison.comparedTicks),[1,1]);
  assert.equal(inspected,2);assert.equal(closed,4);
});

test('missing, withheld, and wrong-identity fixture bytes cannot silently shrink a selected lane',async()=>{
  const cases=[fixture('demo0'),{id:'demo1',status:'missing-replay',reason:'private input absent'}];
  const result=await runReplaySuite({adapter:adapter(),cases,expectedIds:['demo0','demo1','demo2']});
  assert.equal(result.status,'INCOMPLETE');assert.equal(result.complete,false);
  assert.equal(result.results[1].reason,'private input absent');
  assert.equal(result.results[2].reason,'Selected Replay fixture has no case record.');
  const bad=fixture('demo3');bad.replayBytes=Buffer.from('different bytes');
  const wrong=await runReplaySuite({adapter:adapter(),cases:[bad],expectedIds:['demo3']});
  assert.equal(wrong.results[0].status,'INCOMPATIBLE');
});

test('full state divergence and shortened candidate traces are preserved as non-passing results',async()=>{
  const item=fixture('demo0');
  const mismatch=await runReplaySuite({adapter:adapter({candidateTrace:fixture=>trace(fixture.replaySha256,'changed')}),
    cases:[item],expectedIds:['demo0']});
  assert.equal(mismatch.status,'DIVERGED');
  assert.equal(mismatch.results[0].comparison.reason,'category-digest');
  const truncated=await runReplaySuite({adapter:adapter({candidateTrace:fixture=>trace(fixture.replaySha256,'same',false)}),
    cases:[item],expectedIds:['demo0']});
  assert.equal(truncated.status,'INCOMPLETE');
  assert.equal(truncated.results[0].comparison.reason,'segment-not-complete');
});

test('inspection and session failures stay visible and opened sessions are closed',async()=>{
  const item=fixture('demo0');let closed=0;
  const invalidInspection=await runReplaySuite({adapter:adapter({inspect:()=>null}),cases:[item],expectedIds:['demo0']});
  assert.equal(invalidInspection.status,'ERROR');
  assert.match(invalidInspection.results[0].message,/inspection/);
  const candidateFailure=adapter({candidateClose:()=>{closed++;}});
  candidateFailure.openCandidate=async()=>{throw Error('candidate missing');};
  const result=await runReplaySuite({adapter:candidateFailure,cases:[item],expectedIds:['demo0']});
  assert.equal(result.status,'ERROR');assert.equal(closed,0);
});

test('title terminal metadata can reject a naturally completed capture as a state divergence',async()=>{
  let opened=false;
  const titleAdapter=adapter();
  titleAdapter.validateCandidate=()=>({status:'DIVERGED',reason:'terminal-state',expected:{score:10},actual:{score:9}});
  titleAdapter.openOriginal=async()=>{opened=true;throw Error('must not open after terminal mismatch');};
  const result=await runReplaySuite({adapter:titleAdapter,cases:[fixture('demo0')],expectedIds:['demo0']});
  assert.equal(result.status,'DIVERGED');assert.equal(result.results[0].reason,'terminal-state');assert.equal(opened,false);
});

test('a golden reader and candidate provider are both mandatory',async()=>{
  const item=fixture('demo0');
  const noOriginal=adapter();noOriginal.describe=()=>({adapterApiVersion:1,game:'th08',adapterVersion:'bad/v1',features:{original:false}});
  await assert.rejects(runReplaySuite({adapter:noOriginal,cases:[item],expectedIds:['demo0']}),/immutable original trace/);
  await assert.rejects(runReplaySuite({adapter:adapter(),cases:[item,fixture('demo1')],expectedIds:['demo0']}),/outside the selected lane/);
});
