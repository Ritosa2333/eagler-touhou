import assert from 'node:assert/strict';
import test from 'node:test';
import {mapDesktopSfxStartJournal} from './runtime-desktop-audio.mjs';
import {evaluateDesktopGameplayReadiness, incompleteGameplayPrecondition,
  incompleteTh09TouchPrecondition, th09TouchFireEligibility,correlateInputSfxWindow,correlateSfxWindow} from './runtime-desktop-readiness.mjs';

const gameplay = () => ({
  capabilities: {input:true,position:true,phase:true,pause:true},
  input:{value:0},position:{x:200,y:300},phase:{name:'match'},pause:{active:false},
  nativeObservation:{version:1},localSeat:0,frames:{simulation:40},
});
const makeSfxVoice=(id,started,captureSequence,overrides={})=>{
  const pcm=overrides.pcm??{nonZero:80,peakAbs:.4,rms:.2},fromFrame=Math.round(started*48000),toFrame=fromFrame+128,
    outputSourceIdentity=overrides.outputSourceIdentity??{nodeId:200+captureSequence,nodeType:'GainNode',role:'runtime-sfx-voice-output'},
    outputTapIndex=overrides.outputTapIndex??1;
  return{id,playing:true,node:true,started,outputTapIndex,tapMatchCount:1,contextIndex:0,contextSampleRate:48000,observedAtContextTime:started+.01,captureSequence,
    sourceNodeIdentity:{nodeId:100+captureSequence,nodeType:'AudioBufferSourceNode',role:'runtime-sfx-buffer-source'},outputSourceIdentity,
    sourceStartFrame:fromFrame,sourceEndFrame:toFrame,sourceIntervalAvailable:true,sourceIntervalReason:null,computedPlaybackRate:1,
    contentDuration:128/48000,requestedDuration:null,historyTapIdentity:{tapId:outputTapIndex,sourceNodeId:outputSourceIdentity.nodeId,historyHandleId:captureSequence+1},
    tapHistoryId:captureSequence+1,
    pcmHistory:{available:true,covered:true,coverageReason:null,fromFrame,toFrame,frames:toFrame-fromFrame,renderedThroughFrame:toFrame,retainedStartFrame:fromFrame,
      gaps:0,gapRangeLimit:64,gapRanges:[],overlappingGapRanges:[],gapDrops:0,overwrittenFrames:0,capacityFrames:4096,
      nonZeroFrames:pcm.nonZero,firstNonZeroFrame:pcm.nonZero?fromFrame:null,peakAbs:pcm.peakAbs},
    outputRoute:{available:true,connected:true,fromFrame,toFrame,drops:0,events:[{connected:true,contextFrame:fromFrame}]},pcm,...overrides};
};
const makeStartJournalEvent=(voice,index,sessionId)=>({sessionId,sequence:index+1,game:'th08',voiceId:String(voice.id),
  sessionSequence:index+2,
  contextIndex:voice.contextIndex??0,sampleRate:voice.contextSampleRate??48000,startCallContextTime:voice.startCallContextTime??voice.started,
  startContextTime:voice.started,
  observedAtContextTime:voice.observedAtContextTime,voiceStarted:voice.voiceStarted??voice.started,
  sourceNodeIdentity:voice.sourceNodeIdentity,outputSourceIdentity:voice.outputSourceIdentity,
  tapMatchCount:voice.tapMatchCount??1,outputTapIndex:voice.outputTapIndex,startFrame:voice.sourceStartFrame,endFrame:voice.sourceEndFrame,
  intervalEndFrame:voice.sourceEndFrame,
  stopEventCount:voice.stopEventCount??1,stopDrops:0,ended:voice.ended??true,
  sourceIntervalAvailable:voice.sourceIntervalAvailable,sourceIntervalReason:voice.sourceIntervalReason,
  computedPlaybackRate:voice.computedPlaybackRate,contentDuration:voice.contentDuration,requestedDuration:voice.requestedDuration,
  offset:voice.offset??0,playbackRate:voice.playbackRate??1,detune:voice.detune??0,
  tapHistoryId:voice.historyTapIdentity.historyHandleId,historyTapIdentity:voice.historyTapIdentity,pcmHistory:voice.pcmHistory,outputRoute:voice.outputRoute});
const mapStartJournal=(inputCapture,voices)=>{
  const activeObservations=voices.map(voice=>({...voice,started:voice.voiceStarted??voice.started,
    voiceStarted:voice.voiceStarted??voice.started}));
  return mapDesktopSfxStartJournal(inputCapture,{activeObservations});
};
const consumerSnapshot=(sequence,fire,activePlayerShots,stage=2)=>({available:true,complete:true,phase:'consumer-run',sequence,stage,
  consumed:{fire},activePlayerShots});
const consumerReceipt=()=>({baseline:consumerSnapshot(10,false,4),held:consumerSnapshot(11,true,5),released:consumerSnapshot(12,false,5)});
const makeHistoryReadiness=(sessionId,tapIds,readyTime)=>{
  const sampleRate=48000,contextIndex=0,readyFrame=Math.floor(readyTime*sampleRate),armTime=readyTime-.05,armFrame=Math.floor(armTime*sampleRate);
  const barrierBasis=tapIds.map(tapId=>({tapId,firstFrame:armFrame,lastProcessStartFrame:armFrame,lastProcessEndFrame:armFrame+128,
    contextTime:(armFrame+128)/sampleRate,contextFrame:armFrame+128}));
  const barrierFrame=Math.max(...barrierBasis.map(row=>row.lastProcessEndFrame));
  return{available:true,status:'ready',reason:null,source:'shared-audioworklet-history-status',sessionId,contextIndex,sampleRate,
    arm:{state:'running',currentTime:armTime,contextFrame:armFrame},barrierFrame,barrierBasis,ready:{state:'running',currentTime:readyTime,contextFrame:readyFrame},
    elapsedMs:2,pollCount:1,checks:[{poll:1,elapsedMs:2,armFrame,readyFrame,barrierFrame,renderedTapCount:tapIds.length,
      barrierCoveredTapCount:tapIds.length,minRenderedThroughFrame:readyFrame,reason:null}],
    taps:tapIds.map((tapId,index)=>({tapId,historyHandleId:index+1,available:true,reason:null,firstFrame:readyFrame-128,
      renderedThroughFrame:readyFrame,processCount:1,lastProcessStartFrame:readyFrame-128,lastProcessEndFrame:readyFrame,
      currentFrame:readyFrame-128,gaps:0,gapDrops:0,overwrittenFrames:0,contextState:'running',contextIndex,sampleRate,
      contextTime:readyTime,contextFrame:readyFrame}))};
};
const makeSfxWindow=({start,end,down,up,trusted=true,consumer=consumerReceipt(),journalVoices=[]})=>{
  const startClock={available:true,contextIndex:0,sampleRate:48000,currentTime:start,state:'running',source:'SDL3.audioContext'};
  const endClock={available:true,contextIndex:0,sampleRate:48000,currentTime:end,state:'running',source:'SDL3.audioContext.currentTime'};
  const sourceEndFrames=journalVoices.map(voice=>voice.sourceEndFrame).filter(Number.isSafeInteger);
  const requiredCloseFrame=Math.max(Math.floor(end*48000),...sourceEndFrames),closeContextTime=Math.max(end,(requiredCloseFrame+1)/48000),closeFrame=Math.floor(closeContextTime*48000);
  const edge=(type,contextTime,sessionSequence)=>({sessionSequence,type,code:'KeyZ',isTrusted:trusted,repeat:false,
    contextIndex:0,sampleRate:48000,contextTime,contextState:'running'});
  const sessionId=77;
  const tapIds=[...new Set(journalVoices.map(voice=>voice.outputTapIndex))];if(!tapIds.length)tapIds.push(1);
  const readiness=makeHistoryReadiness(sessionId,tapIds,start-.01);
  const inputCapture={available:true,game:'th08',sessionId,code:'KeyZ',drops:0,historyReadiness:readiness,historyTaps:tapIds,
    events:[edge('keydown',down,1),edge('keyup',up,journalVoices.length+2)],
    sfxStartJournal:{available:true,sessionId,game:'th08',code:'KeyZ',active:false,stopHookAvailable:true,stopHookError:null,
      historyReady:true,historyError:null,historyReadiness:readiness,historyTapCount:tapIds.length,
      closeBoundary:{available:true,reason:null,contextIndex:0,sampleRate:48000,contextTime:closeContextTime,closeFrame},
      events:journalVoices.map((voice,index)=>({...makeStartJournalEvent(voice,index,sessionId),closeFrame})),
      drops:0,unattributed:0,errors:0}};
  return{source:'SDL3.audioContext.currentTime',game:'th08',sessionId,code:'KeyZ',startContextTime:start,endContextTime:end,contextIndex:0,sampleRate:48000,
    historyReadiness:readiness,
    clockEvidence:{start:startClock,end:endClock},releaseEvidence:{source:'readRuntimeObservation.input',baselineValue:0,consumedValue:1,nativeValue:0,
      contextIndex:0,currentTime:end,consumer},inputCapture};
};
const makeInputSfxAction=(voices,{start,end,down,up,trusted=true,consumer=consumerReceipt()}={})=>{
  const actionWindow=makeSfxWindow({start,end,down,up,trusted,consumer,journalVoices:voices});
  return{actionWindow,nativeSfx:{available:true,...mapStartJournal(actionWindow.inputCapture,voices)}};
};

test('desktop gameplay readiness requires native input, position, live phase, local seat, and simulation progress',()=>{
  assert.deepEqual(evaluateDesktopGameplayReadiness(gameplay(),'th10'),{ready:true,reasons:[]});
  for(const [mutate,reason] of [
    [state=>{state.input.value=null;},'native-input-unavailable'],
    [state=>{state.position.x=NaN;},'native-player-position-unavailable'],
    [state=>{state.phase.name='game_over';},'native-gameplay-phase-unavailable'],
    [state=>{state.pause.active=true;},'gameplay-not-unpaused'],
    [state=>{state.localSeat=1;},'local-seat-unavailable'],
    [state=>{state.frames.simulation=null;},'native-simulation-frame-unavailable'],
  ]){
    const state=gameplay();mutate(state);
    assert.ok(evaluateDesktopGameplayReadiness(state,'th10').reasons.includes(reason),reason);
    const incomplete=incompleteGameplayPrecondition(state,'th10');
    assert.equal(incomplete.checkStatus,'incomplete');
    assert.equal(incomplete.precondition,state,'the original native observation must remain available');
  }
});

test('TH08 readiness uses its title-specific native supervisor state when no phase ABI exists',()=>{
  const state={capabilities:{input:true,position:true,phase:false},input:{value:0},position:{x:200,y:300},
    status:{supervisorActive:2,active:1,loading:false}};
  assert.deepEqual(evaluateDesktopGameplayReadiness(state,'th08'),{ready:true,reasons:[]});
  state.status.loading=true;
  assert.ok(evaluateDesktopGameplayReadiness(state,'th08').reasons.includes('native-gameplay-phase-unavailable'));
});

test('TH09 touch eligibility is grounded in native context, player readiness, and title state',()=>{
  const eligible={touchInput:{context:'gameplay',ready:true,inTitle:false,active:false}};
  assert.deepEqual(th09TouchFireEligibility(eligible),{ready:true,reasons:[]});
  assert.equal(incompleteTh09TouchPrecondition(eligible),null);
  for(const [touch,reason] of [
    [{context:'inactive',ready:true,inTitle:false},'touch-context-not-gameplay'],
    [{context:'dialogue-or-ending',ready:true,inTitle:false},'touch-context-not-gameplay'],
    [{context:'gameplay',ready:false,inTitle:false},'native-touch-player-not-ready'],
    [{context:'gameplay',ready:true,inTitle:true},'title-context-active'],
    [{context:null,ready:null,inTitle:null},'touch-context-not-gameplay'],
  ]){
    const state={touchInput:touch};
    const result=incompleteTh09TouchPrecondition(state);
    assert.equal(result.checkStatus,'incomplete');
    assert.equal(result.precondition,state);
    assert.ok(result.eligibility.reasons.includes(reason));
  }
});

test('TH09 movement requires its actual native ABI together with touch eligibility',()=>{
  const eligible=()=>({...gameplay(),nativeObservation:{version:1,validMask:0x1bf},
    touchInput:{context:'gameplay',ready:true,inTitle:false,active:false}});
  assert.deepEqual(evaluateDesktopGameplayReadiness(eligible(),'th09'),{ready:true,reasons:[]});
  for(const [mutate,reason] of [
    [state=>{state.capabilities.input=false;state.input=null;state.nativeObservation.validMask&=~2;},'native-input-unavailable'],
    [state=>{state.capabilities.position=false;state.position=null;state.nativeObservation.validMask&=~4;},'native-player-position-unavailable'],
    [state=>{state.touchInput.ready=false;},'native-touch-player-not-ready'],
    [state=>{state.touchInput.context='dialogue-or-ending';},'touch-context-not-gameplay'],
  ]){
    const state=eligible();mutate(state);
    const result=incompleteGameplayPrecondition(state,'th09');
    assert.equal(result.checkStatus,'incomplete');assert.ok(result.eligibility.reasons.includes(reason));
  }
});

test('shared SFX scene correlates fresh voice PCM only on that voice’s own output tap',()=>{
  const voice=(id,started,sequence,overrides={})=>makeSfxVoice(id,started,sequence,{outputTapIndex:4,
    observedAtContextTime:started+.025,...overrides});
  const baseline={nativeSfx:{available:true,snapshots:{end:[]}}};
  const fresh=voice('new-shot',12.432,1,{elapsedMs:444,observedAtContextTime:12.457});
  const afterRelease=voice('new-shot',12.931,2,{elapsedMs:931,observedAtContextTime:12.94});
  const action=makeInputSfxAction([fresh,afterRelease],{start:12.24,end:12.597,down:12.25,up:12.5});
  const result=correlateInputSfxWindow(baseline,action);
  assert.equal(result.status,'passed');
  assert.equal(result.newVoiceActivations[0].activationCount,1);
  assert.equal(result.newVoiceActivations[0].activations[0].voice.started,12.432);
  assert.equal(result.newVoiceActivations[0].id,'new-shot');
  assert.equal(result.correlatedVoice.id,'new-shot');
  assert.equal(result.correlatedPcm.tapIndex,4);
  assert.equal(result.correlatedPcm.nonZeroSamples,80,
    'a large unrelated tap cannot replace this voice’s own PCM samples');
  assert.equal(result.newVoiceActivations[0].activations[0].voice.started,12.432,
    'The 931 ms retrigger remains outside the trusted keydown/keyup context-clock interval.');
});

test('shared SFX scene excludes a voice after trusted keyup even while native clear is still pending',()=>{
  const afterKeyup=makeSfxVoice('new-shot',12.55,1,{observedAtContextTime:12.557});
  const baseline={nativeSfx:{available:true,snapshots:{end:[]}}};
  const action=makeInputSfxAction([afterKeyup],{start:12.24,end:12.6,down:12.25,up:12.5});
  const result=correlateInputSfxWindow(baseline,action);
  assert.equal(result.status,'failed','outer native-clear time cannot extend the trusted key interval');
  assert.equal(result.newVoiceActivations.length,0);
  assert.deepEqual(result.inputEdgeWindow,{startContextTime:12.25,endContextTime:12.5});
});

test('shared SFX scene excludes a voice that starts after capture begins but before trusted keydown',()=>{
  const beforeKeydown=makeSfxVoice('new-shot',12.245,1,{observedAtContextTime:12.25});
  const result=correlateInputSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},
    makeInputSfxAction([beforeKeydown],{start:12.24,end:12.6,down:12.25,up:12.5}));
  assert.equal(result.status,'failed','the outer capture start cannot make a pre-key voice attributable to the key');
  assert.equal(result.newVoiceActivations.length,0);
});

test('same-quantum starts are ordered against trusted key edges instead of inferred from equal clock samples',()=>{
  const baseline={nativeSfx:{available:true,snapshots:{end:[]}}},voice=makeSfxVoice('same-quantum',.4,1);
  const sameClock=makeInputSfxAction([voice],{start:.2,end:.6,down:.4,up:.4});
  assert.equal(correlateInputSfxWindow(baseline,sameClock).status,'passed',
    'A start recorded after keydown and before keyup remains attributable when the audio clock has not advanced');

  const beforeDown=makeInputSfxAction([voice],{start:.2,end:.6,down:.4,up:.4});
  beforeDown.actionWindow.inputCapture.events[0].sessionSequence=2;
  beforeDown.actionWindow.inputCapture.events[1].sessionSequence=3;
  beforeDown.actionWindow.inputCapture.sfxStartJournal.events[0].sessionSequence=1;
  beforeDown.nativeSfx={available:true,...mapStartJournal(beforeDown.actionWindow.inputCapture,[voice])};
  assert.equal(correlateInputSfxWindow(baseline,beforeDown).status,'failed',
    'An equal-time source call ordered before keydown cannot be attributed to the shot');

  const afterUp=makeInputSfxAction([voice],{start:.2,end:.6,down:.4,up:.4});
  afterUp.actionWindow.inputCapture.events[0].sessionSequence=1;
  afterUp.actionWindow.inputCapture.events[1].sessionSequence=2;
  afterUp.actionWindow.inputCapture.sfxStartJournal.events[0].sessionSequence=3;
  afterUp.nativeSfx={available:true,...mapStartJournal(afterUp.actionWindow.inputCapture,[voice])};
  assert.equal(correlateInputSfxWindow(baseline,afterUp).status,'failed',
    'An equal-time source call ordered after keyup cannot be attributed to the shot');
});

test('same voice and tap preserve repeated starts and each PCM capture as one firing source',()=>{
  const outputSourceIdentity={nodeId:201,nodeType:'GainNode',role:'runtime-sfx-voice-output'},
    first=makeSfxVoice('voice-1',12.432,1,{outputSourceIdentity,observedAtContextTime:12.457}),
    second=makeSfxVoice('voice-1',12.5,2,{outputSourceIdentity,observedAtContextTime:12.51});
  const result=correlateInputSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},
    makeInputSfxAction([first,second],{start:12.24,end:12.597,down:12.25,up:12.55}));
  assert.equal(result.status,'passed');
  assert.equal(result.correlatedVoice.activationCount,2);
  assert.equal(result.newVoiceActivations[0].activations.length,2);
  assert.equal(result.correlatedPcm.captures,2);
  assert.equal(result.correlatedPcm.nonZeroSamples,160);
});

test('keyup-following same-tap starts are included in interval isolation',()=>{
  const outputSourceIdentity={nodeId:250,nodeType:'GainNode',role:'runtime-sfx-voice-output'},
    first=makeSfxVoice('shot',.4,1,{outputSourceIdentity}),afterKeyup=makeSfxVoice('shot',.5,2,{outputSourceIdentity});
  first.sourceEndFrame=Math.round(.6*48000);first.pcmHistory={...first.pcmHistory,toFrame:first.sourceEndFrame,
    frames:first.sourceEndFrame-first.sourceStartFrame,renderedThroughFrame:first.sourceEndFrame};
  first.outputRoute={...first.outputRoute,toFrame:first.sourceEndFrame};
  const action=makeInputSfxAction([first,afterKeyup],{start:.2,end:.7,down:.21,up:.45});
  action.actionWindow.inputCapture.sfxStartJournal.events[1].sessionSequence=5;
  action.nativeSfx={available:true,...mapStartJournal(action.actionWindow.inputCapture,[first,afterKeyup])};
  assert.equal(action.nativeSfx.activationEvents.length,1,'only the first start is inside the trusted key interval');
  assert.equal(action.nativeSfx.sourceIntervals.length,2,'the source journal retains the later same-session start');
  const result=correlateInputSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},action);
  assert.equal(result.status,'incomplete');
  assert.match(result.reason,/overlap/,'the later voice cannot lend its PCM to a source interval that still owns the tap');
});

test('same-frame production disconnect and reconnect are ambiguous rather than a proven route failure',()=>{
  const voice=makeSfxVoice('route-change',.4,1);
  voice.outputRoute.events=[{sequence:1,connected:true,contextFrame:voice.sourceStartFrame},
    {sequence:2,connected:false,contextFrame:voice.sourceStartFrame+32},
    {sequence:3,connected:true,contextFrame:voice.sourceStartFrame+32}];
  const result=correlateInputSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},
    makeInputSfxAction([voice],{start:.2,end:.6,down:.21,up:.59}));
  assert.equal(result.status,'incomplete');
  assert.match(result.reason,/route is not completely observed/);
});

test('different new voices remain incomplete and missing or mismatched clocks cannot pass',()=>{
  const first=makeSfxVoice('shot',.4,1),second=makeSfxVoice('other',.45,2,{outputTapIndex:3});
  const baseline={nativeSfx:{available:true,snapshots:{end:[]}}};
  const multi=correlateInputSfxWindow(baseline,makeInputSfxAction([first,second],{start:.2,end:.6,down:.21,up:.59}));
  assert.equal(multi.status,'incomplete');
  assert.match(multi.reason,/Multiple new SFX voices/);
  const missingAction=makeInputSfxAction([first],{start:.2,end:.6,down:.21,up:.59});
  missingAction.nativeSfx.activationEvents[0].contextIndex=null;
  const missingClock=correlateInputSfxWindow(baseline,missingAction);
  assert.equal(missingClock.status,'incomplete');
  const malformedAction=makeInputSfxAction([first],{start:.2,end:.6,down:.21,up:.59});malformedAction.actionWindow.clockEvidence.end.currentTime=.61;
  const malformed=correlateInputSfxWindow(baseline,malformedAction);
  assert.equal(malformed.status,'incomplete');
});

test('only trusted down/up edges ordered inside the send-to-native-clear clock window are accepted',()=>{
  const baseline={nativeSfx:{available:true,snapshots:{end:[]}}},voice=makeSfxVoice('shot',.4,1);
  const action=makeInputSfxAction([voice],{start:.2,end:.6,down:.21,up:.59,trusted:false});
  assert.equal(action.nativeSfx.available,false,'The mapper refuses to derive activation evidence from an untrusted input session');
  const result=correlateInputSfxWindow(baseline,action);
  assert.equal(result.status,'incomplete');
});

test('consumer receipt must show one same-stage shot between neutral and released snapshots',()=>{
  const baseline={nativeSfx:{available:true,snapshots:{end:[]}}},voice=makeSfxVoice('shot',.4,1);
  const validAction=makeInputSfxAction([voice],{start:.2,end:.6,down:.21,up:.59});
  assert.equal(correlateInputSfxWindow(baseline,validAction).status,'passed');
  const cases=[
    ['missing raw snapshots',null],
    ['wrong stage',{...consumerReceipt(),released:consumerSnapshot(12,false,5,3)}],
    ['non-monotonic consumer sequence',{...consumerReceipt(),held:consumerSnapshot(10,true,5)}],
    ['no held fire consumption',{...consumerReceipt(),held:consumerSnapshot(11,false,5)}],
    ['fire remains held after release',{...consumerReceipt(),released:consumerSnapshot(12,true,5)}],
    ['no shot created',{...consumerReceipt(),held:consumerSnapshot(11,true,4)}],
    ['incomplete consumer snapshot',{...consumerReceipt(),held:{...consumerSnapshot(11,true,5),complete:false}}],
  ];
  for(const [label,consumer] of cases){
    const action=makeInputSfxAction([voice],{start:.2,end:.6,down:.21,up:.59,consumer});
    const result=correlateInputSfxWindow(baseline,action);
    assert.equal(result.status,'incomplete',label);
    assert.match(result.reason,/snapshots/);
  }
});

test('a fully captured zero output interval remains failed; missing rendered history is incomplete',()=>{
  const baseline={nativeSfx:{available:true,snapshots:{end:[]}}};
  const silent=makeSfxVoice('shot',.4,1,{pcm:{nonZero:0,peakAbs:0,rms:0}});
  const failed=correlateInputSfxWindow(baseline,makeInputSfxAction([silent],{start:.2,end:.6,down:.21,up:.59}));
  assert.equal(failed.status,'failed');
  const absent=makeSfxVoice('shot',.4,1,{pcmHistory:{available:true,covered:false,fromFrame:19200,toFrame:19328,frames:128,
    renderedThroughFrame:19264,retainedStartFrame:0,gaps:0,overwrittenFrames:0,capacityFrames:4096,nonZeroFrames:0,firstNonZeroFrame:null,peakAbs:0}});
  const incomplete=correlateInputSfxWindow(baseline,makeInputSfxAction([absent],{start:.2,end:.6,down:.21,up:.59}));
  assert.equal(incomplete.status,'incomplete');
});

test('PCM gaps outside a source interval remain usable while intersecting or unindexed gaps are incomplete',()=>{
  const baseline={nativeSfx:{available:true,snapshots:{end:[]}}},voice=makeSfxVoice('shot',.4,1),base=voice.pcmHistory;
  const outside={...base,gaps:1,gapRanges:[{fromFrame:19000,toFrame:19100}],overlappingGapRanges:[]};
  const outsideResult=correlateInputSfxWindow(baseline,makeInputSfxAction([{...voice,pcmHistory:outside}],
    {start:.2,end:.6,down:.21,up:.59}));
  assert.equal(outsideResult.status,'passed','a gap wholly outside the source frames does not invalidate its captured PCM');

  const overlappingRange={fromFrame:base.fromFrame+16,toFrame:base.fromFrame+48};
  const overlapping={...base,covered:false,coverageReason:'render-frame-gap-overlaps-range',gaps:1,
    gapRanges:[overlappingRange],overlappingGapRanges:[overlappingRange]};
  const overlapResult=correlateInputSfxWindow(baseline,makeInputSfxAction([{...voice,pcmHistory:overlapping}],
    {start:.2,end:.6,down:.21,up:.59}));
  assert.equal(overlapResult.status,'incomplete');
  assert.equal(overlapResult.firstIssue.code,'pcm-gap-intersects-source');
  assert.equal(overlapResult.firstIssue.voiceId,'shot');
  assert.equal(overlapResult.firstIssue.tapId,1);
  assert.deepEqual(overlapResult.firstIssue.overlappingGapRanges,[overlappingRange]);

  const overflow={...base,covered:false,coverageReason:'gap-index-overflow',gaps:65,
    gapRanges:Array.from({length:64},(_,index)=>({fromFrame:index*2,toFrame:index*2+1})),gapDrops:1};
  const overflowResult=correlateInputSfxWindow(baseline,makeInputSfxAction([{...voice,pcmHistory:overflow}],
    {start:.2,end:.6,down:.21,up:.59}));
  assert.equal(overflowResult.status,'incomplete');
  assert.equal(overflowResult.firstIssue.code,'pcm-gap-index-overflow');
  assert.equal(overflowResult.firstIssue.gapDrops,1);
});

test('a stop with no rendered frames is an identified incomplete interval, not a silent product failure',()=>{
  const voice=makeSfxVoice('shot',.4,1),start=voice.sourceStartFrame;
  const zeroFrame={...voice,sourceEndFrame:start,stopEventCount:1,ended:true,
    pcmHistory:{available:false,reason:'SFX output interval contains no rendered frames'}};
  const result=correlateInputSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},
    makeInputSfxAction([zeroFrame],{start:.2,end:.6,down:.21,up:.59}));
  assert.equal(result.status,'incomplete');
  assert.equal(result.firstIssue.code,'zero-render-interval');
  assert.equal(result.firstIssue.voiceId,'shot');
  assert.equal(result.firstIssue.sourceNodeId,101);
  assert.equal(result.firstIssue.outputNodeId,201);
  assert.equal(result.firstIssue.tapId,1);
  assert.equal(result.firstIssue.fromFrame,start);
  assert.equal(result.firstIssue.toFrame,start);
  assert.match(result.firstIssue.detail,/stopEventCount=1 ended=true/);
});

test('touch-window SFX requires full source-interval PCM; sparse samples stay observations',()=>{
  const voice={id:'touch-fire',playing:true,node:true,started:8,elapsedMs:160,outputTapIndex:2,
    pcm:{nonZero:80,peakAbs:.2,rms:.1}};
  const sparseNonZero=correlateSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},
    {nativeSfx:{available:true,activationEvents:[voice],activeObservations:[voice]},
      actionWindow:{startElapsedMs:100,endElapsedMs:200}});
  assert.equal(sparseNonZero.status,'incomplete');
  assert.equal(sparseNonZero.correlated,false,'one sampled tap window cannot prove a complete source interval');
  assert.equal(sparseNonZero.observedSparsePcm[0].samples[0].nonZero,80,
    'sparse nonzero PCM remains available as observed evidence');

  const completeVoice=makeSfxVoice('touch-fire',8,1,{elapsedMs:160,outputTapIndex:2,
    pcm:{nonZero:80,peakAbs:.2,rms:.1}});
  const completeAction={nativeSfx:{available:true,activationEvents:[completeVoice],activeObservations:[completeVoice]},
    actionWindow:{startElapsedMs:100,endElapsedMs:200}};
  assert.equal(correlateSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},completeAction).status,'passed');

  const completeSilent={...completeVoice,pcmHistory:{...completeVoice.pcmHistory,nonZeroFrames:0,firstNonZeroFrame:null,peakAbs:0},
    pcm:{nonZero:0,peakAbs:0,rms:0}};
  const silent=correlateSfxWindow({nativeSfx:{available:true,snapshots:{end:[]}}},
    {nativeSfx:{available:true,activationEvents:[completeSilent],activeObservations:[completeSilent]},
      actionWindow:{startElapsedMs:100,endElapsedMs:200}});
  assert.equal(silent.status,'failed','only a complete, connected, silent source interval establishes a product failure');
  assert.match(silent.reason,/complete rendered source interval/);
});
