import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdtemp, rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {TH08_REPLAY_SEMANTICS} from '../../tests/adaptation/adapters/titles/th08-replay-format.mjs';
import {runReplayRecordingGroup} from './replay-recording-runner.mjs';

const STAGE=4;
const REPLAY_PATH='replay/th8_01.rpy';
const FILE=Buffer.from('production replay bytes for finite-window runner test');
const HASH=createHash('sha256').update(FILE).digest('hex');

function makeIo(mode='normal'){
  const state={phase:null,recordFrame:0,playbackFrame:0,traceSequence:0,pending:null,live:{keys:new Set(),gamepad:new Set(),controls:false},
    frames:new Map([[0,0]]),pointerEvents:[],file:Buffer.from(FILE),releaseCalls:0,traceEnableCalls:0,openedHashes:[],
    bootstrapTicks:0,probeDispatches:0,receiptReads:{recording:0,playback:0}};
  const liveInput=()=>{
    let bits=0;
    if(!(mode==='queueOnly'&&state.phase==='recording'))
      for(const code of state.live.keys){if(code==='KeyZ')bits|=1;if(code==='ArrowRight')bits|=128;}
    if(state.live.gamepad.has(0))bits|=1;
    if(state.live.controls&&state.phase!=='playback')bits|=1;
    return bits;
  };
  const pointAtCursor=cursor=>{
    if(cursor<=0)return{pointCount:0,point:null};
    const event=state.pointerEvents[cursor-1];
    return{pointCount:1,point:{id:event.id,x:event.x,y:event.y,released:event.action==='up'}};
  };
  const makeSample=()=>{
    const input=liveInput();
    if(state.phase==='recording'){
      state.recordFrame++;
      state.frames.set(state.recordFrame,input);
      return{sequence:state.traceSequence,boundary:1,stage:STAGE,controlFrame:state.recordFrame,
        pollSequence:state.recordFrame,liveInput:input,
        recording:{active:true,stage:STAGE,frame:state.recordFrame,cursor:state.recordFrame*2,physical:input},
        motion:{playing:false,stage:STAGE,tick:state.recordFrame,movementCursor:0,movementCount:0,
          touchCursor:0,touchCount:state.pointerEvents.length,pointCount:0,point:null}};
    }
    if(mode==='bootstrap80'||mode==='noPlaybackConsumer'){
      state.bootstrapTicks++;
      if(mode==='noPlaybackConsumer'||state.bootstrapTicks<=80){
        return{sequence:state.traceSequence,boundary:1,stage:0,controlFrame:state.bootstrapTicks,
          pollSequence:state.bootstrapTicks,liveInput:input,playback:{loaded:false,stage:0,frame:0,cursor:0,current:0,previous:0},
          consumer:null,motion:{playing:false,stage:0,tick:state.bootstrapTicks,movementCursor:0,movementCount:0,
            touchCursor:0,touchCount:state.pointerEvents.length,pointCount:0,point:null}};
      }
    }
    state.playbackFrame++;
    const frame=state.playbackFrame,rawIndex=frame-1,current=state.frames.get(rawIndex)??0;
    const previous=state.frames.get(rawIndex-1)??0;
    const touchCursor=state.pointerEvents.filter(event=>event.tick<=frame).length;
    const point=pointAtCursor(touchCursor);
    const consumerCurrent=mode==='liveLeak'&&input?current|input:current;
    return{sequence:state.traceSequence,boundary:1,stage:STAGE,controlFrame:frame,pollSequence:frame,liveInput:input,
      playback:{loaded:true,stage:STAGE,frame,cursor:frame*2,current,previous},
      consumer:{sequence:frame,controlFrame:frame,stage:STAGE,current:consumerCurrent,previous,
        validMask:2,phase:1},motion:{playing:true,stage:STAGE,tick:frame,movementCursor:0,movementCount:0,
        touchCursor,touchCount:state.pointerEvents.length,pointCount:point.pointCount,point:point.point}};
  };
  const io={
    titleParser:{
      parseFrames(bytes,{stage,expectedSha256,firstFrame,frameCount}){
        assert.equal(stage,STAGE);assert.equal(expectedSha256,HASH);assert.equal(firstFrame,0);
        return{sha256:expectedSha256,stride:2,frames:Array.from({length:frameCount},(_,frame)=>({frame,cursor:frame*2,
          byteOffset:frame*2,buttons:state.frames.get(frame)??0}))};
      },
      parseExtension(bytes,{expectedSha256,expectedGame}){
        assert.equal(expectedSha256,HASH);assert.equal(expectedGame,8);
        return{version:2,sha256:expectedSha256,touch:[{stage:STAGE,entries:state.pointerEvents.map(event=>({tick:event.tick,
          id:event.id,x:event.x,y:event.y,action:event.action==='down'?0:event.action==='move'?1:2}))}]};
      },
    },
    async prepare({phase}){
      state.phase=phase;
      if(phase==='recording'){state.recordFrame=0;state.frames=new Map([[0,0]]);state.pointerEvents=[];}
      if(phase==='playback'){state.playbackFrame=0;state.bootstrapTicks=0;}
      state.live={keys:new Set(),gamepad:new Set(),controls:false};
      return{status:'passed'};
    },
    async controlTrace({enabled,reset}){
      if(reset){state.traceSequence=0;state.pending=null;}
      if(enabled)state.traceEnableCalls++;
      state.traceEnabled=enabled;
      return{status:'passed',enabled,reset};
    },
    async key({code,pressed}){
      if(state.phase==='playback'&&pressed)state.probeDispatches++;
      if(pressed)state.live.keys.add(code);else state.live.keys.delete(code);
      return{status:'passed'};
    },
    async gamepad({bit,pressed}){
      if(state.phase==='playback'&&pressed)state.probeDispatches++;
      if(pressed)state.live.gamepad.add(bit);else state.live.gamepad.delete(bit);
      return{status:'passed'};
    },
    async controls({enabled}){if(state.phase==='playback'&&enabled)state.probeDispatches++;state.live.controls=enabled;return{status:'passed'};},
    async directTouch(event){
      if(!event.cleanup){
        state.pointerEvents.push({...event,tick:state.recordFrame+1});
      }
      return{status:'passed'};
    },
    async step(){
      if(!state.traceEnabled)return{status:'incomplete',reason:'trace disabled'};
      state.traceSequence++;
      state.pending=makeSample();
      return{status:'passed'};
    },
    async readTrace({afterSequence}={}){
      if(mode==='traceGap'&&state.traceSequence===1)return{status:'incomplete',gap:true,samples:[],nextSequence:2};
      if(!state.pending||afterSequence!==undefined&&state.pending.sequence<=afterSequence)
        return{status:'passed',gap:false,samples:[],nextSequence:state.traceSequence+1};
      return{status:'passed',gap:false,samples:[state.pending],nextSequence:state.traceSequence+1};
    },
    async releaseAll(){state.releaseCalls++;state.live={keys:new Set(),gamepad:new Set(),controls:false};return{status:'passed'};},
    async finalize(){return{status:'passed',path:REPLAY_PATH,sha256:HASH};},
    async readFile(){
      if(mode==='savedByteMismatch')return Buffer.concat([state.file,Buffer.from(' changed')]);
      return Buffer.from(state.file);
    },
    async openPlayback({bytes,sha256}){
      state.openedHashes.push(sha256);
      if(createHash('sha256').update(bytes).digest('hex')!==sha256)return{status:'failed',reason:'wrong bytes'};
      return{status:'passed',sha256};
    },
  };
  if(mode!=='receiptMissing')io.readInputReceipt=async probe=>{
    assert.equal(probe.ingress.receipt,'touchControl');
    state.receiptReads[state.phase]=(state.receiptReads[state.phase]??0)+1;
    const inconsistent=mode==='receiptInconsistent';
    return{schema:'eagler-touhou/replay-input-receipt/1',status:'passed',receiptId:'touchControl',
      fields:{context:3,fire:mode==='receiptAlwaysFalse'?0:inconsistent?1:Number(state.phase==='playback'&&state.live.controls)}};
  };
  return{io,state};
}

async function run(mode='normal'){
  const outputDir=await mkdtemp(join(tmpdir(),'replay-recording-runner-'));
  const {io,state}=makeIo(mode);
  const report=await runReplayRecordingGroup({semantics:TH08_REPLAY_SEMANTICS,io,outputDir,stage:STAGE,windowTicks:12});
  return{report,state,outputDir};
}

test('one recording group saves a shared Replay and validates finite browser/native playback windows',async()=>{
  const {report,state,outputDir}=await run();
  try {
    assert.equal(report.status,'passed');
    assert.deepEqual(report.counts,{passed:9,failed:0,incomplete:0,blocked:0});
    assert.equal(report.completeReplay,false);
    assert.equal(report.replay.sha256,HASH);
    assert.equal(report.cases.find(item=>item.id==='record-pointer-trajectory').playbackExtension.status,'passed');
    assert.equal(state.openedHashes.length,1+1+8,'baseline, pointer, and fresh paired playback sessions all load the same file');
    assert.ok(state.openedHashes.every(value=>value===HASH));
    assert.ok(state.releaseCalls>=1);
    assert.ok(state.traceEnableCalls>=1);
    assert.equal(state.traceEnabled,false);
  } finally {
    await rm(outputDir,{recursive:true,force:true});
  }
});

test('queue-only input never passes the recording route',async()=>{
  const {report,outputDir}=await run('queueOnly');
  try {
    const keyboard=report.cases.find(item=>item.id==='record-keyboard');
    assert.notEqual(keyboard.status,'passed');
    assert.notEqual(report.status,'passed');
  } finally {await rm(outputDir,{recursive:true,force:true});}
});

test('trace loss is incomplete and cannot be promoted to a route pass',async()=>{
  const {report,outputDir}=await run('traceGap');
  try {
    assert.equal(report.cases.find(item=>item.id==='record-keyboard').status,'incomplete');
    assert.notEqual(report.status,'passed');
  } finally {await rm(outputDir,{recursive:true,force:true});}
});

test('saved-byte identity drift fails every dependent Replay case',async()=>{
  const {report,outputDir}=await run('savedByteMismatch');
  try {
    assert.equal(report.status,'failed');
    assert.ok(report.cases.some(item=>item.status==='failed'));
    assert.match(report.parseError.reason,/changed after finalization/);
  } finally {await rm(outputDir,{recursive:true,force:true});}
});

test('live input leaking into playback consumption fails paired isolation',async()=>{
  const {report,outputDir}=await run('liveLeak');
  try {
    const direction=report.cases.find(item=>item.id==='playback-live-keyboard-direction');
    assert.equal(direction.status,'failed');
    assert.notEqual(report.status,'passed');
  } finally {await rm(outputDir,{recursive:true,force:true});}
});

test('touch-fire playback attaches native context/fire receipts to baseline and perturbed manual samples',async()=>{
  const {report,state,outputDir}=await run();
  try {
    const touchFire=report.cases.find(item=>item.id==='playback-live-touch-fire');
    assert.equal(touchFire.status,'passed');
    assert.ok(state.receiptReads.playback>=touchFire.baseline.samples.length+touchFire.perturbed.samples.length);
    for(const sample of [...touchFire.baseline.samples,...touchFire.perturbed.samples])
      assert.equal(sample.ingress?.receiptId,'touchControl');
    assert.ok(touchFire.baseline.samples.every(sample=>sample.ingress.fields.fire===0));
    assert.ok(touchFire.perturbed.samples.some(sample=>sample.ingress.fields.fire===1));
    assert.ok(touchFire.perturbed.samples.some(sample=>sample.ingress.fields.fire===0));
    assert.ok([...touchFire.baseline.samples,...touchFire.perturbed.samples].every(sample=>(sample.liveInput&1)===0),
      'touch control is observed at its native ingress while remaining suppressed from Replay live polling');
  } finally {await rm(outputDir,{recursive:true,force:true});}
});

test('missing native touch receipt remains incomplete with its captured diagnostic',async()=>{
  const {report,outputDir}=await run('receiptMissing');
  try {
    const touchFire=report.cases.find(item=>item.id==='playback-live-touch-fire');
    assert.equal(touchFire.status,'incomplete');
    assert.equal(touchFire.baseline.samples[0].ingress.status,'incomplete');
    assert.match(touchFire.baseline.samples[0].ingress.reason,/readInputReceipt/);
  } finally {await rm(outputDir,{recursive:true,force:true});}
});

test('native touch receipt that stays released cannot prove a fire perturbation',async()=>{
  const {report,outputDir}=await run('receiptAlwaysFalse');
  try {
    const touchFire=report.cases.find(item=>item.id==='playback-live-touch-fire');
    assert.equal(touchFire.status,'incomplete');
    assert.ok(touchFire.perturbed.samples.every(sample=>sample.ingress.fields.fire===0));
  } finally {await rm(outputDir,{recursive:true,force:true});}
});

test('native touch receipt contradicting the released baseline fails the scene',async()=>{
  const {report,outputDir}=await run('receiptInconsistent');
  try {
    const touchFire=report.cases.find(item=>item.id==='playback-live-touch-fire');
    assert.equal(touchFire.status,'failed');
    assert.match(touchFire.reason,/baseline already held/);
  } finally {await rm(outputDir,{recursive:true,force:true});}
});

test('playback bootstrap preserves loading samples and starts its consumer window at native frame one',async()=>{
  const {report,outputDir}=await run('bootstrap80');
  try {
    assert.equal(report.status,'passed');
    const baseline=report.cases.find(item=>item.id==='playback-baseline');
    assert.equal(baseline.bootstrapSamples.length,81,'the marker sample is retained with the 80 loading samples');
    assert.equal(baseline.readiness.playbackFrame,1);
    assert.equal(baseline.samples[0].playback.frame,1,'the first valid consumer sample is not discarded');
    assert.equal(baseline.samples.length,20,'the finite playback window continues through the saved pointer tick');
    const pointer=report.cases.find(item=>item.id==='record-pointer-trajectory');
    assert.equal(pointer.bootstrapSamples.length,81);
    assert.equal(pointer.samples[0].playback.frame,1);
  } finally {await rm(outputDir,{recursive:true,force:true});}
});

test('playback with no same-stage consumer marker is incomplete and applies no probe input',async()=>{
  const {report,state,outputDir}=await run('noPlaybackConsumer');
  try {
    assert.notEqual(report.status,'passed');
    const baseline=report.cases.find(item=>item.id==='playback-baseline');
    assert.equal(baseline.status,'incomplete');
    assert.equal(baseline.bootstrapSamples.length,600);
    assert.equal(baseline.samples.length,0);
    assert.equal(baseline.readiness,null);
    assert.equal(state.probeDispatches,0,'live perturbations only begin after the readiness marker');
  } finally {await rm(outputDir,{recursive:true,force:true});}
});
