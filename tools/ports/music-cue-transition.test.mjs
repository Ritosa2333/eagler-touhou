import test from 'node:test';
import assert from 'node:assert/strict';
import {collectMusicCueTransition,assessMusicCueTransition} from './music-cue-transition.mjs';

const runtime={epoch:1,pointer:12,generation:3},cueSource={asset:'stage-message',bytes:200,hash:{algorithm:'fnv1a64',value:'0123456789abcdef'},entryCount:2};
const expected={stage:0,entry:0,beforeFile:'stage.ogg',afterFile:'boss.ogg',acceptedStatus:1,maxPolls:3,cueSource,
  releaseInputs:['fire','skip'],maxCleanupPolls:3};
function fixture(){
  const observation=(frame,file,serial=1)=>({runtime:{...runtime},frame,file,request:{serial,entry:0,statusCode:1},
    music:{pending:false,soundReady:true,playing:true,outputPaused:false},inputs:{fire:serial===1&&frame<4,skip:serial===1&&frame<4}});
  return{schema:'eagler-touhou/music-cue-transition/2',source:{runtime:{...runtime},stage:0,identity:{...cueSource},entries:[0]},
    before:observation(1,'stage.ogg',0),request:{accepted:true,entry:0,statusCode:1},
    samples:[observation(2,'stage.ogg'),observation(3,'boss.ogg')],cleanup:{releaseCompleted:true,samples:[observation(4,'boss.ogg')]}};
}

test('shared cue verdict binds real progress, source bytes and exactly one native request',()=>{
  const record=fixture();assert.equal(assessMusicCueTransition(record,expected).passed,true);
  for(const mutate of [
    r=>r.source.identity.hash={...r.source.identity.hash,value:'other'},r=>r.source.entries=[],r=>r.before.file='boss.ogg',
    r=>r.request.statusCode=0,r=>r.samples[1].runtime.epoch++,r=>r.samples[1].request.serial++,
    r=>r.samples[1].file='other.ogg',r=>r.samples[1].frame=1,r=>r.cleanup.releaseCompleted=false,
  ]){const changed=structuredClone(record);changed.passed=true;mutate(changed);assert.equal(assessMusicCueTransition(changed,expected).passed,false);}
});

function collectorFixture({sticky=false}={}){
  const state={polls:0,requested:false,releaseAt:null,releaseCalls:0};
  const io={readSource:async()=>fixture().source,observe:async()=>{
    const held=state.requested&&(state.releaseAt===null||sticky||state.polls<=state.releaseAt+1);
    return{runtime:{...runtime},frame:state.polls+1,file:state.requested&&state.polls>=2?'boss.ogg':'stage.ogg',
      request:{serial:state.requested?1:0,entry:0,statusCode:1},inputs:{fire:held,skip:held},
      music:{pending:false,soundReady:true,playing:true,outputPaused:false}};
  },request:async entry=>{state.requested=true;return{accepted:true,statusCode:1,entry};},wait:async()=>{state.polls++;},
    release:async()=>{state.releaseCalls++;state.releaseAt=state.polls;return{released:true};}};
  return{state,io};
}

test('shared collector observes held inputs until a later native frame actually releases them',async()=>{
  const {state,io}=collectorFixture(),result=await collectMusicCueTransition(io,expected);
  assert.equal(result.samples.length,2);assert.equal(state.releaseCalls,1);
  assert.equal(result.cleanup.samples.length,2);assert.equal(result.cleanup.samples[0].inputs.fire,true);
  assert.deepEqual(result.cleanup.samples[1].inputs,{fire:false,skip:false});
  assert.equal(assessMusicCueTransition(result,expected).passed,true);
});

test('a successful release callback cannot certify native inputs that remain held',async()=>{
  const {state,io}=collectorFixture({sticky:true});
  await assert.rejects(collectMusicCueTransition(io,expected),error=>{
    assert.match(error.message,/inputs remain held/);assert.equal(error.evidence.cleanup.releaseCompleted,true);
    assert.equal(error.evidence.cleanup.samples.length,expected.maxCleanupPolls);return true;
  });assert.equal(state.releaseCalls,1);
});

test('a rejected cue still releases its trigger and retains cleanup observations',async()=>{
  const {state,io}=collectorFixture();io.request=async()=>({accepted:false});
  await assert.rejects(collectMusicCueTransition(io,expected),error=>{
    assert.match(error.message,/request was rejected/);assert.equal(error.evidence.cleanup.samples.length,1);return true;
  });assert.equal(state.releaseCalls,1);
});

test('cleanup errors preserve the original request failure and the native observations',async()=>{
  const {state,io}=collectorFixture();
  io.request=async()=>{state.requested=true;throw Error('request branch failed');};
  io.release=async()=>{state.releaseCalls++;throw Error('release branch failed');};
  await assert.rejects(collectMusicCueTransition(io,expected),error=>{
    assert.equal(error.message,'request branch failed');assert.equal(error.evidence.cleanupError,'release branch failed');
    assert.equal(error.evidence.cleanup.releaseCompleted,false);assert.equal(error.evidence.cleanup.samples.length,3);return true;
  });assert.equal(state.releaseCalls,1);
});

test('native cleanup evidence rejects missing inputs, stale frames, foreign instances and cached release flags',()=>{
  for(const mutate of [
    row=>{row.cleanup.released=true;row.cleanup.samples[0].inputs.fire=true;},
    row=>{row.cleanup.released=true;row.cleanup.samples[0].inputs.skip=true;},
    row=>delete row.cleanup.samples[0].inputs.skip,
    row=>row.cleanup.samples[0].inputs.fire=0,
    row=>row.cleanup.samples[0].frame=3,
    row=>row.cleanup.samples[0].runtime.epoch++,
    row=>row.cleanup.samples[0].request.serial++,
    row=>row.cleanup.samples[0].file='other.ogg',
    row=>row.cleanup.samples[0].music.playing=false,
    row=>row.cleanup.samples=[],
    row=>row.samples.forEach(sample=>sample.inputs.fire=false),
    row=>row.cleanupObservationError='raw read failed',
    row=>row.schema='eagler-touhou/music-cue-transition/1',
  ]){
    const record=fixture();record.passed=true;mutate(record);
    assert.equal(assessMusicCueTransition(record,expected).passed,false);
  }
  for(const releaseInputs of [[],['fire','fire'],['missing']])
    assert.equal(assessMusicCueTransition(fixture(),{...expected,releaseInputs}).passed,false);
});

test('stopping and opening are observed states while pending names never certify a finished cue',()=>{
  const record=fixture(),pending=structuredClone(record.samples[0]);pending.file=null;
  pending.music={pending:true,soundReady:false,playing:false,outputPaused:false};
  const named=structuredClone(pending);named.file='boss.ogg';named.frame=3;
  const established=structuredClone(record.samples[1]);established.frame=4;
  record.cleanup.samples[0].frame=5;
  record.samples=[pending,named,established];assert.equal(assessMusicCueTransition(record,expected).passed,true);
  record.samples.pop();assert.equal(assessMusicCueTransition(record,expected).passed,false);
  record.samples=[structuredClone(pending),established];record.samples[0].music={...established.music};
  assert.equal(assessMusicCueTransition(record,expected).passed,false,'A missing name with a ready source is inconsistent');
});
