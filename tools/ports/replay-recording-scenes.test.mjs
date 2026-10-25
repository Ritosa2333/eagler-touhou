import test from 'node:test';
import assert from 'node:assert/strict';
import {TH08_REPLAY_SEMANTICS} from '../../tests/adaptation/adapters/titles/th08-replay-format.mjs';
import {assessReplayPlaybackWindow,assessReplayPointerConsumption,assessReplayRecordingInputWindow,
  assessReplayInputIsolation,createReplayRecordingScenes} from './replay-recording-scenes.mjs';

const hash='a'.repeat(64);
const plan=createReplayRecordingScenes({semantics:TH08_REPLAY_SEMANTICS,stage:4,windowTicks:12});
const sample=(sequence,changes={})=>({sequence,boundary:1,stage:4,controlFrame:sequence,
  pollSequence:sequence,liveInput:0,recording:{active:true,stage:4,frame:sequence,cursor:sequence*2,physical:0},
  playback:{loaded:true,stage:4,frame:sequence,cursor:sequence*2,current:0,previous:0},
  consumer:{sequence,controlFrame:sequence,stage:4,current:0,previous:0,validMask:2,phase:1},
  motion:{playing:true,stage:4,touchCursor:0,touchCount:3,pointCount:0,point:null},...changes});

test('Replay scene plan exposes every required route and a bounded pointer trajectory',()=>{
  assert.equal(plan.status,'ready');
  assert.deepEqual(plan.requiredCases,['record-keyboard','record-virtual-gamepad','record-touch-controls',
    'record-pointer-trajectory','playback-baseline','playback-live-keyboard-direction',
    'playback-live-keyboard-shoot','playback-live-virtual-gamepad-shoot','playback-live-touch-fire']);
  const pointer=plan.cases.find(item=>item.id==='record-pointer-trajectory');
  assert.deepEqual(pointer.trajectory.map(item=>item.action),['down','move','up']);
  assert.equal(pointer.requiredVersion,2);
  assert.equal(pointer.pointObservation.coordinates,'Float32 normalized [0,1]');
  assert.deepEqual(plan.finalizer,{export:'save_replay',app:true,allocateExport:'allocate',freeExport:'deallocate',
    slot:1,name:'ADAPTEST',path:'replay/th8_01.rpy',
    source:'ports/th08/web/cpp/platform/BrowserExports.cpp | browser_save_replay | GameApplication::finalize_replay'});
  assert.equal(plan.replayPath,'/savesth08/replay/th8_01.rpy');
  assert.equal(plan.cases.find(item=>item.id==='playback-baseline').path,plan.replayPath);
  const perturb=plan.cases.find(item=>item.id==='playback-live-keyboard-direction');
  assert.equal(perturb.probe.logicalMask,128);assert.equal(perturb.exemptionMask,8);
  const touchProbe=plan.cases.find(item=>item.id==='playback-live-touch-fire');
  assert.deepEqual(touchProbe.ingress,TH08_REPLAY_SEMANTICS.isolation.probes.find(item=>item.name==='touch-fire').ingress);
  const noTouchReceipt=createReplayRecordingScenes({semantics:{...TH08_REPLAY_SEMANTICS,
    nativeTrace:{...TH08_REPLAY_SEMANTICS.nativeTrace,inputReceipts:{}}},stage:4,windowTicks:12});
  assert.equal(noTouchReceipt.cases.find(item=>item.id==='playback-live-touch-fire').status,'blocked');
  const unsupported=createReplayRecordingScenes({semantics:{game:'th99',variant:'normal',inputs:[]}});
  assert.equal(unsupported.status,'blocked');
  assert.deepEqual(unsupported.cases.map(item=>item.status),Array(7).fill('blocked'));
  const missingFinalizer=createReplayRecordingScenes({semantics:{...TH08_REPLAY_SEMANTICS,finalizer:null}});
  assert.equal(missingFinalizer.cases.find(item=>item.id==='record-keyboard').status,'blocked');
  assert.equal(missingFinalizer.cases.find(item=>item.id==='playback-baseline').status,'blocked');
});

test('recording input window must align native poll, recorder, independent raw bytes, and release',()=>{
  const scene=plan.cases.find(item=>item.id==='record-keyboard');
  const recorded=[0,1,1,0,0].map((buttons,index)=>({frame:index+1,cursor:(index+1)*2,buttons}));
  const frames=[{frame:0,cursor:0,buttons:0},...recorded];
  const samples=recorded.map((raw,index)=>sample(10+index,{stage:4,pollSequence:50+index,liveInput:raw.buttons,
    recording:{active:true,stage:4,frame:raw.frame,cursor:raw.cursor,physical:raw.buttons},
    consumer:{sequence:10+index,controlFrame:10+index,stage:4,phase:1,validMask:2,
      current:frames[index].buttons,previous:frames[index-1]?.buttons??0}}));
  const good=assessReplayRecordingInputWindow({samples,scene,rawFrames:{sha256:hash,stride:2,frames},expectedSha256:hash});
  assert.equal(good.status,'passed');assert.equal(good.completeReplay,false);
  const wrongBoundary=samples.map(item=>({...item,boundary:0}));
  assert.equal(assessReplayRecordingInputWindow({samples:wrongBoundary,scene,rawFrames:{sha256:hash,stride:2,frames},expectedSha256:hash}).status,'incomplete');
  assert.equal(assessReplayRecordingInputWindow({samples,scene,rawFrames:{sha256:'b'.repeat(64),stride:2,frames},expectedSha256:hash}).status,'incomplete');
  assert.equal(assessReplayRecordingInputWindow({samples:samples.slice(2),scene,rawFrames:{sha256:hash,stride:2,frames},expectedSha256:hash}).status,'incomplete',
    'recorded window without the required pressed dwell is incomplete');
  const broken=samples.map(item=>structuredClone(item));broken[1].recording.physical=0;
  assert.equal(assessReplayRecordingInputWindow({samples:broken,scene,rawFrames:{sha256:hash,stride:2,frames},expectedSha256:hash}).status,'failed');
  const gap=samples.map(item=>structuredClone(item));gap[2].sequence++;
  assert.equal(assessReplayRecordingInputWindow({samples:gap,scene,rawFrames:{sha256:hash,stride:2,frames},expectedSha256:hash}).status,'incomplete');
  for(const mutate of [
    rows=>{rows[2].consumer.current=0;},rows=>{rows[3].consumer.previous=0;},
    rows=>{rows[4].consumer.current=1;},rows=>{rows[2].consumer.sequence=rows[1].consumer.sequence;},
  ]){
    const rows=structuredClone(samples);mutate(rows);
    assert.equal(assessReplayRecordingInputWindow({samples:rows,scene,rawFrames:{sha256:hash,stride:2,frames},expectedSha256:hash}).status,'failed',
      'matching poll/recorder/file cannot hide an incorrect actual player consumer');
  }
  const missing=structuredClone(samples);delete missing[2].consumer;
  assert.equal(assessReplayRecordingInputWindow({samples:missing,scene,rawFrames:{sha256:hash,stride:2,frames},expectedSha256:hash}).status,'incomplete');
});

test('playback window requires raw-frame identity and a real aligned player-consumer receipt',()=>{
  const rawFrames=[{frame:0,buttons:0},{frame:1,buttons:1},{frame:2,buttons:128}];
  const samples=[1,2].map((frame,index)=>sample(80+index,{controlFrame:600+index,
    playback:{loaded:true,stage:4,frame,cursor:frame*2,current:rawFrames[frame-1].buttons,previous:0},
    consumer:{sequence:400+index,controlFrame:600+index,stage:4,current:rawFrames[frame-1].buttons,
      previous:0,validMask:2,phase:1}}));
  const args={samples,rawFrames:{sha256:hash,stride:2,frames:rawFrames},expectedSha256:hash,stage:4,rawIndexOffset:-1,boundary:1};
  assert.equal(assessReplayPlaybackWindow(args).status,'passed');
  const wrongStage=samples.map(item=>structuredClone(item));wrongStage[0].playback.stage=3;
  assert.equal(assessReplayPlaybackWindow({...args,samples:wrongStage}).status,'failed');
  const notConsumed=samples.map(item=>structuredClone(item));notConsumed[0].consumer.phase=2;
  assert.equal(assessReplayPlaybackWindow({...args,samples:notConsumed}).status,'incomplete');
  const wrongConsumer=samples.map(item=>structuredClone(item));wrongConsumer[1].consumer.current=0;
  assert.equal(assessReplayPlaybackWindow({...args,samples:wrongConsumer}).status,'failed');
  assert.equal(assessReplayPlaybackWindow({...args,rawFrames:{...args.rawFrames,sha256:'b'.repeat(64)}}).status,'incomplete');
  const gap=samples.map(item=>structuredClone(item));gap[1].sequence+=2;
  assert.equal(assessReplayPlaybackWindow({...args,samples:gap}).status,'incomplete');
});

test('paired playback isolation binds the live probe to unchanged stream and player-consumed frames',()=>{
  const scene=plan.cases.find(item=>item.id==='playback-live-keyboard-direction');
  const makeWindow=(perturbed,{controlOrigin=900,consumerOrigin=500}={})=>Array.from({length:12},(_,index)=>{
    const frame=index+1,controlFrame=controlOrigin+index,liveInput=perturbed&&index>=3&&index<=5?128:0;
    return sample(index+1,{controlFrame,pollSequence:200+index,liveInput,
      playback:{loaded:true,stage:4,frame,cursor:frame*2,current:frame%2,previous:(frame-1)%2},
      consumer:{sequence:consumerOrigin+index,controlFrame,stage:4,current:frame%2,previous:(frame-1)%2,validMask:2,phase:1}});
  });
  const baseline=makeWindow(false,{controlOrigin:900,consumerOrigin:500});
  const perturbed=makeWindow(true,{controlOrigin:1900,consumerOrigin:1500});
  const rawFrames={sha256:hash,stride:2,frames:Array.from({length:12},(_,index)=>({frame:index,buttons:(index+1)%2}))};
  const args={baseline,perturbed,scene,rawFrames,expectedSha256:hash,baselineSha256:hash,perturbedSha256:hash};
  const good=assessReplayInputIsolation(args);
  assert.equal(good.status,'passed');assert.equal(good.completeReplay,false);
  assert.equal(good.observedPollSamples,3);
  assert.equal(assessReplayInputIsolation({...args,baselineSha256:'b'.repeat(64)}).status,'incomplete');

  const noRelease=makeWindow(true,{controlOrigin:1900,consumerOrigin:1500});for(const item of noRelease)item.liveInput=128;
  assert.equal(assessReplayInputIsolation({...args,perturbed:noRelease}).status,'incomplete');
  const wrongCursor=makeWindow(true);wrongCursor[7].playback.cursor+=2;
  assert.equal(assessReplayInputIsolation({...args,perturbed:wrongCursor}).status,'failed');
  const wrongConsumer=makeWindow(true,{controlOrigin:1900,consumerOrigin:1500});wrongConsumer[7].consumer.sequence+=10;
  assert.equal(assessReplayInputIsolation({...args,perturbed:wrongConsumer}).status,'incomplete','a consumer sequence gap invalidates the paired evidence');
  const wrongControlFrame=makeWindow(true,{controlOrigin:1900,consumerOrigin:1500});
  wrongControlFrame[7].controlFrame+=10;wrongControlFrame[7].consumer.controlFrame+=10;
  assert.equal(assessReplayInputIsolation({...args,perturbed:wrongControlFrame}).status,'failed');
  const contaminatedBaseline=makeWindow(false);contaminatedBaseline[4].liveInput=128;
  assert.equal(assessReplayInputIsolation({...args,baseline:contaminatedBaseline}).status,'failed');
  const unrelatedInput=makeWindow(true);unrelatedInput[8].liveInput|=4;
  assert.equal(assessReplayInputIsolation({...args,perturbed:unrelatedInput}).status,'failed');
});

test('touch-control suppression uses a native ingress receipt while Replay poll and consumer stay file-derived',()=>{
  const scene=plan.cases.find(item=>item.id==='playback-live-touch-fire');
  const rawFrames={sha256:hash,stride:2,frames:Array.from({length:12},(_,index)=>({frame:index,buttons:(index+1)%2}))};
  const makeWindow=(perturbed,{fireAt=new Set([3,4]),context=3,pollLeak=false,controlOrigin=900,consumerOrigin=500}={})=>
    Array.from({length:12},(_,index)=>{
      const frame=index+1,controlFrame=controlOrigin+index,current=(frame)%2,previous=(frame-1)%2;
      return sample(index+1,{controlFrame,pollSequence:200+index,liveInput:pollLeak&&perturbed&&index===3?1:0,
        playback:{loaded:true,stage:4,frame,cursor:frame*2,current,previous},
        consumer:{sequence:consumerOrigin+index,controlFrame,stage:4,current,previous,validMask:2,phase:1},
        ingress:{schema:'eagler-touhou/replay-native-input-receipt/1',status:'passed',receiptId:'touchControl',
          fields:{context,fire:perturbed&&fireAt.has(index)?1:0}}});
    });
  const baseline=makeWindow(false,{controlOrigin:900,consumerOrigin:500});
  const perturbed=makeWindow(true,{controlOrigin:1900,consumerOrigin:1500});
  const args={baseline,perturbed,scene,rawFrames,expectedSha256:hash,baselineSha256:hash,perturbedSha256:hash};
  const good=assessReplayInputIsolation(args);
  assert.equal(good.status,'passed');assert.equal(good.observedPollSamples,0);
  assert.equal(good.observedIngressSamples,2);assert.equal(good.ingressStrategy,'native-touch-control-suppressed');

  const missing=makeWindow(true);delete missing[4].ingress;
  assert.equal(assessReplayInputIsolation({...args,perturbed:missing}).status,'incomplete');
  const noFire=makeWindow(true,{fireAt:new Set()});
  assert.equal(assessReplayInputIsolation({...args,perturbed:noFire}).status,'incomplete');
  const noRelease=makeWindow(true,{fireAt:new Set([3,4,5,6,7,8,9,10,11])});
  assert.equal(assessReplayInputIsolation({...args,perturbed:noRelease}).status,'incomplete');
  const wrongContext=makeWindow(true,{context:2});
  assert.equal(assessReplayInputIsolation({...args,perturbed:wrongContext}).status,'incomplete');
  const wrongBool=makeWindow(true);wrongBool[3].ingress.fields.fire=2;
  assert.equal(assessReplayInputIsolation({...args,perturbed:wrongBool}).status,'incomplete');
  const leaked=makeWindow(true,{pollLeak:true});
  assert.equal(assessReplayInputIsolation({...args,perturbed:leaked}).status,'failed');
  const baselineHeld=makeWindow(false);baselineHeld[2].ingress.fields.fire=1;
  assert.equal(assessReplayInputIsolation({...args,baseline:baselineHeld}).status,'failed');
  const changedStream=makeWindow(true);changedStream[6].consumer.current^=1;
  assert.equal(assessReplayInputIsolation({...args,perturbed:changedStream}).status,'failed');
});

test('THMOTION pointer evidence requires native cursor advancement across parsed v2 events',()=>{
  const events=[{tick:1,id:7101,x:.42,y:.64,action:0},{tick:2,id:7101,x:.48,y:.61,action:1},
    {tick:3,id:7101,x:.52,y:.58,action:2}];
  const extension={version:2,stage:4,sha256:hash,touch:[{stage:4,entries:events}]};
  const states=[
    {touchCursor:0,pointCount:0,point:null},
    {touchCursor:1,pointCount:1,point:{id:7101,x:.42,y:.64,released:false}},
    {touchCursor:2,pointCount:1,point:{id:7101,x:.48,y:.61,released:false}},
    {touchCursor:3,pointCount:1,point:{id:7101,x:.52,y:.58,released:true}},
  ];
  const samples=states.map((state,index)=>sample(120+index,{motion:{playing:true,stage:4,touchCount:3,...state}}));
  const scene=plan.cases.find(item=>item.id==='record-pointer-trajectory');
  assert.equal(assessReplayPointerConsumption({samples,extension,scene,expectedSha256:hash}).status,'passed');
  const noAdvance=samples.slice(1).map(item=>structuredClone(item));
  assert.equal(assessReplayPointerConsumption({samples:noAdvance,extension,scene,expectedSha256:hash}).status,'incomplete');
  const mismatched=samples.map(item=>structuredClone(item));mismatched[1].motion.touchCount=4;
  assert.equal(assessReplayPointerConsumption({samples:mismatched,extension,scene,expectedSha256:hash}).status,'failed');
  const wrongPoint=samples.map(item=>structuredClone(item));wrongPoint[2].motion.point.x=.9;
  assert.equal(assessReplayPointerConsumption({samples:wrongPoint,extension,scene,expectedSha256:hash}).status,'failed');
  const noRelease=samples.map(item=>structuredClone(item));noRelease[3].motion.point.released=false;
  assert.equal(assessReplayPointerConsumption({samples:noRelease,extension,scene,expectedSha256:hash}).status,'failed');
  const nativeRemoval=samples.map(item=>structuredClone(item));nativeRemoval[3].motion.pointCount=0;nativeRemoval[3].motion.point=null;
  assert.equal(assessReplayPointerConsumption({samples:nativeRemoval,extension,scene,expectedSha256:hash}).status,'passed');
  assert.equal(assessReplayPointerConsumption({samples,extension:{...extension,version:1},scene,expectedSha256:hash}).status,'incomplete');
  assert.equal(assessReplayPointerConsumption({samples,extension:{...extension,sha256:'b'.repeat(64)},scene,expectedSha256:hash}).status,'incomplete');
  assert.equal(assessReplayPointerConsumption({samples,extension,scene:{...scene,trajectory:scene.trajectory.map(item=>({...item,x:.9}))},expectedSha256:hash}).status,'failed');
  assert.equal(assessReplayPointerConsumption({samples,extension,scene,expectedSha256:hash}).status,'passed');
});
