import test from 'node:test';
import assert from 'node:assert/strict';
import {assessReplayTerminalSignal,createReplayProgressTracker,playbackObservation,replayProtocolFailures} from './replay-browser-semantics.mjs';

test('Replay fails on native error, abort, or rejected protocol response and preserves original event data',()=>{
  const events=[{event:'ready',protocol:'eagler-touhou/1'},{event:'error',error:'native failed'},
    {event:'abort',reason:'stopped'},{command:'playback',ok:false,error:'not loaded'}];
  const failures=replayProtocolFailures(events);
  assert.deepEqual(failures,[events[1],events[2],events[3]]);
  assert.strictEqual(failures[0],events[1]);
  assert.deepEqual(replayProtocolFailures(null),[]);
});

test('TH06 and TH07 retain their native cursor/total self-consistency check without claiming full input consumption',()=>{
  for(const game of ['th06','th07']){
    const result=assessReplayTerminalSignal({game,state:{frame:4856,total:4856,ended:true}});
    assert.equal(result.status,'observed');assert.equal(result.nativeFormatConsistency.status,'passed');
    assert.deepEqual(result.terminalSignalObserved,{status:'passed',value:true,field:'ended'});
    assert.equal(result.fullInputConsumption.status,'unproven');
    assert.equal(assessReplayTerminalSignal({game,state:{frame:4855,total:4856,ended:true}}).status,'failed');
    assert.equal(assessReplayTerminalSignal({game,state:{frame:4856,total:0,ended:true}}).status,'failed');
  }
});

test('TH08 reports the native terminal signal without imposing a declared total and keeps the candidate counter separate',()=>{
  const reference={kind:'original-executable-presentation-row-count',counter:{name:'presentationRows',value:10607},
    replaySha256:'a'.repeat(64),selectedReplaySha256:'a'.repeat(64),counterBoundToSelectedReplay:true,comparableToNativeCursor:false,
    countDefinition:'Original presentation rows and native consumed-input cursor have no proven mapping.'};
  const result=assessReplayTerminalSignal({game:'th08',state:{frame:11047,total:0,ended:true},candidateCounterReference:reference});
  assert.equal(result.status,'observed');assert.equal(result.nativeCounterObservation.total,0);
  assert.deepEqual(result.candidateCounterObservation,{kind:reference.kind,name:'presentationRows',referenceValue:10607,nativeFrame:11047,
    numericDelta:440,referenceReplaySha256:'a'.repeat(64),persistedReplaySha256:null,selectedReplaySha256:'a'.repeat(64),sameReplayBytes:true,counterBoundToSelectedReplay:true,
    comparableToNativeCursor:false,semanticComparison:'unproven',countDefinition:reference.countDefinition});
  assert.equal(assessReplayTerminalSignal({game:'th08',state:{frame:11047,total:10607,ended:true}}).status,'observed');
});

test('TH09 reports the native terminal signal and leaves an unbound persisted comparison as a candidate count',()=>{
  const reference={kind:'original-process-and-cpp-session-replay-comparison',counter:{name:'comparedTicks',value:7996},
    replaySha256:'b'.repeat(64),persistedReplaySha256:null,selectedReplaySha256:'b'.repeat(64),
    counterBoundToSelectedReplay:false,comparableToNativeCursor:false,
    countDefinition:'Persisted result does not include the selected Replay hash.'};
  const result=assessReplayTerminalSignal({game:'th09',state:{frame:8004,total:0,ended:true},candidateCounterReference:reference});
  assert.equal(result.status,'observed');assert.equal(result.candidateCounterObservation.numericDelta,8);
  assert.equal(result.candidateCounterObservation.semanticComparison,'unproven');
  assert.equal(result.candidateCounterObservation.counterBoundToSelectedReplay,false);
});

test('TH11 preserves cursor, total and metadata while leaving full input consumption unproven',()=>{
  const stages=[9095,10726,15607,17513,22280,25831].map(frames=>({frames}));
  const reference={kind:'original-retail-replay-golden',counter:{name:'gameplayUpdatesBeforeTerminalMarker',value:100672},
    replaySha256:'c'.repeat(64),selectedReplaySha256:'c'.repeat(64),counterBoundToSelectedReplay:true,
    comparableToNativeCursor:false,countDefinition:'Gameplay updates before marker; no mapping to cross-stage native cursor.'};
  const pass=assessReplayTerminalSignal({game:'th11',state:{frame:100680,total:101052,stage:6,ended:true},candidateCounterReference:reference,inspectionStages:stages});
  assert.equal(pass.status,'observed');assert.equal(pass.nativeFormatConsistency.metadataTotal,101052);
  assert.deepEqual(pass.terminalSignalObserved,{status:'passed',value:true,field:'ended'});
  assert.equal(pass.observation.frame,100680);assert.equal(pass.observation.total,101052);
  assert.equal(pass.fullInputConsumption.status,'unproven');
  assert.match(pass.fullInputConsumption.reason,/game_over.*StageExit.*end_marker/);
  assert.equal(pass.candidateCounterObservation.numericDelta,8);assert.equal(pass.candidateCounterObservation.semanticComparison,'unproven');
  assert.equal(assessReplayTerminalSignal({game:'th11',state:{frame:100680,total:101051,stage:6,ended:true},inspectionStages:stages}).status,'failed');
  assert.equal(assessReplayTerminalSignal({game:'th11',state:{frame:100680,total:101052,stage:6,ended:true},inspectionStages:[]}).status,'failed');
});

test('TH10 and TH15 require rate-window progress and report terminal signals without full input proof',()=>{
  const th10=assessReplayTerminalSignal({game:'th10',state:{frame:0,total:0,stage:1,ended:true},maximumFrame:120});
  assert.equal(th10.status,'observed');assert.match(th10.terminalSignalScope,/does not prove/);
  assert.equal(th10.fullInputConsumption.status,'unproven');
  assert.equal(th10.comparison.observedFrame,0);assert.equal(assessReplayTerminalSignal({game:'th10',state:{ended:true},maximumFrame:119}).status,'failed');
  const th15=assessReplayTerminalSignal({game:'th15',state:{frame:124742,total:0,ended:true},maximumFrame:124742});
  assert.equal(th15.status,'observed');assert.match(th15.terminalSignalScope,/terminal signal/);
  assert.equal(assessReplayTerminalSignal({game:'th15',state:{frame:0,total:0,ended:false},maximumFrame:124742}).status,'failed');
});

test('terminal-signal progress records raw phase and updates last advance only when frame or stage changes',()=>{
  const tracker=createReplayProgressTracker({startedAt:1000,initialState:{kind:'replay',frame:120,total:1000,stage:1,ended:false}});
  const paused=tracker.sample({kind:'replay',frame:120,total:1000,stage:1,phase:'paused'},3000);
  assert.equal(paused.lastAdvanceAt,1000);assert.equal(paused.phase,'paused');assert.equal(paused.playbackPhase,'paused');
  const nextFrame=tracker.sample({kind:'replay',frame:121,total:1000,stage:1},5000);
  assert.equal(nextFrame.lastAdvanceAt,5000);assert.equal(nextFrame.lastAdvanceAgeMs,0);
  const nextStage=tracker.sample({kind:'replay',frame:0,total:1000,stage:2},7000);
  assert.equal(nextStage.lastAdvanceAt,7000);assert.equal(tracker.maximumFrame,121);
  const resetTracker=createReplayProgressTracker({startedAt:8000,initialState:{kind:'replay',frame:0,total:1000,ended:true},initialMaximumFrame:123});
  assert.equal(resetTracker.maximumFrame,123,'a title reset must preserve the maximum observed during the natural-rate window');
  assert.deepEqual(playbackObservation({kind:'replay',ended:true,frame:1}),{
    frame:1,total:null,ended:true,stage:null,startStage:null,phase:null,playbackPhase:'ended',phaseSource:'inferred-from-replay-state',
    preparing:false,paused:false,seeking:false,target:null,rate:null,
  });
});
