import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createRequire} from 'node:module';
import {assessReplayTerminalSignal,createReplayProgressTracker,gateReplayRecordOnFinalErrors,playbackObservation,
  replayOriginalImportCheck,replayProtocolFailures,replayScenarioPassedTargetIds} from './replay-browser-semantics.mjs';

function successfulReplayRecord(){
  const sha256='a'.repeat(64);
  return{id:'th06-normal',game:'th06',variant:'normal',status:'passed',finalErrorAudit:{status:'passed'},
    checks:[
      {id:'import-current-replay-through-shell',status:'passed',value:{path:'replay/th6_udabcd.rpy',bytes:128,sha256}},
      {id:'native-replay-plays-and-advances',status:'passed',value:{startFrame:0,frame:120,naturalFrames:120,observed:[{frame:120}]}},
      {id:'native-terminal-signal-observed',status:'passed'},
    ],
    playback:{status:'observed',scenarioStatus:'passed',fixture:{path:'replay/th6.rpy',bytes:128,sha256},
      terminalSignalObserved:{status:'passed',value:true},
      fullInputConsumption:{status:'unproven',reason:'Native ended does not prove all blocks were consumed.'}}};
}

test('the production Replay terminal call selects its product mode and retains the parsed endpoint assessment',()=>{
  const source=readFileSync(new URL('./replay-browser.mjs',import.meta.url),'utf8');
  const {parse}=createRequire(new URL('../../launcher/package.json',import.meta.url))('acorn');
  const calls=[];
  function visit(node){
    if(!node||typeof node!=='object')return;
    if(node.type==='CallExpression'&&node.callee?.name==='assessReplayTerminalSignal')calls.push(node);
    for(const child of Object.values(node)){
      if(Array.isArray(child))child.forEach(visit);else if(child?.type)visit(child);
    }
  }
  visit(parse(source,{ecmaVersion:'latest',sourceType:'module',allowHashBang:true}));
  assert.equal(calls.length,1,'The browser driver must have one identifiable terminal assessment call');
  const call=source.slice(calls[0].start,calls[0].end);
  const evaluate=new Function('assessReplayTerminalSignal','target','finalState','record','description','tracker',
    'nativeInputConsumption',`return ${call};`);
  const consumption={status:'incomplete',reason:'Accepted original Replay has no independent natural endpoint mapping.'};
  const assess=variant=>evaluate(assessReplayTerminalSignal,{game:'th08',variant},{frame:120,total:0,ended:true},
    {},null,{maximumFrame:120},consumption);
  const normal=assess('normal');
  assert.equal(normal.status,'incomplete');assert.deepEqual(normal.fullInputConsumption,consumption);
  assert.equal(normal.reason,consumption.reason);
  const multiplayer=assess('multiplayer');
  assert.equal(multiplayer.fullInputConsumption.status,'unproven');
  assert.notEqual(multiplayer.fullInputConsumption.reason,consumption.reason);
});

test('Replay fails on native error, abort, or rejected protocol response and preserves original event data',()=>{
  const events=[{event:'ready',protocol:'eagler-touhou/1'},{event:'error',error:'native failed'},
    {event:'abort',reason:'stopped'},{command:'playback',ok:false,error:'not loaded'}];
  const failures=replayProtocolFailures(events);
  assert.deepEqual(failures,[events[1],events[2],events[3]]);
  assert.strictEqual(failures[0],events[1]);
  assert.deepEqual(replayProtocolFailures(null),[]);
});

test('Replay final audit turns a late same-origin protocol abort into a failed record after playback passed',()=>{
  const rawEvent={protocol:'eagler-touhou/1',game:'th06',epoch:1,event:'abort',reason:'runtime iframe closed with an active resource'};
  const channels={pageErrors:[],consoleErrors:[],httpErrors:[],protocolErrors:[rawEvent],responseErrors:[],teardownErrors:[]};
  const record={status:'passed',playback:{scenarioStatus:'passed',terminalSignalObserved:{status:'passed',value:true},
    scenarioEvidence:{browserAndRuntimeErrors:'pending-final-audit'}}};
  const audit=gateReplayRecordOnFinalErrors(record,channels);
  assert.equal(audit.status,'failed');assert.deepEqual(audit.failedChannels,['protocolErrors']);
  assert.strictEqual(audit.channels.protocolErrors[0],rawEvent);
  assert.equal(record.status,'failed');
  assert.equal(record.playback.preTeardownScenarioStatus,'passed');
  assert.equal(record.playback.scenarioStatus,'failed');
  assert.equal(record.playback.scenarioEvidence.finalErrorAudit,'failed');
  assert.equal(record.playback.scenarioEvidence.browserAndRuntimeErrors,'present');
  assert.deepEqual(record.playback.terminalSignalObserved,{status:'passed',value:true});
});

test('Replay coverage excludes teardown-failed and incomplete records from passed scenarios',()=>{
  const records=[
    {id:'th06-normal',status:'failed',playback:{scenarioStatus:'failed',preTeardownScenarioStatus:'passed'}},
    {id:'th07-normal',status:'passed',playback:{scenarioStatus:'passed'}},
    {id:'th08-normal',status:'incomplete',playback:{scenarioStatus:'passed'}},
  ];
  assert.deepEqual(replayScenarioPassedTargetIds(records),['th07-normal']);
});

test('REPLAY-01 is a numbered partial result only when the same fixture imports and reaches native playback terminal',()=>{
  const record=successfulReplayRecord(),check=replayOriginalImportCheck(record);
  assert.equal(check.id,'REPLAY-01');assert.equal(check.status,'partial');assert.equal(check.complete,false);
  assert.equal(check.observation.game,'th06');assert.deepEqual(check.observation.adaptationIds,['REPLAY-01']);
  assert.equal(check.observation.observation.sameReplayBytes,true);
  assert.equal(check.observation.observation.fullInputConsumption.status,'unproven');

  for(const mutate of [
    item=>{item.checks=item.checks.filter(check=>check.id!=='import-current-replay-through-shell');},
    item=>{item.checks=item.checks.filter(check=>check.id!=='native-replay-plays-and-advances');},
    item=>{item.checks=item.checks.filter(check=>check.id!=='native-terminal-signal-observed');},
    item=>{item.checks[0].value.sha256='b'.repeat(64);},
    item=>{item.checks[1].value.frame=119;},
    item=>{delete item.finalErrorAudit;},
    item=>{item.playback.fullInputConsumption={status:'passed'};},
  ]){
    const incomplete=successfulReplayRecord();mutate(incomplete);
    assert.equal(replayOriginalImportCheck(incomplete).status,'incomplete');
  }
});

test('a failed final Replay audit makes the numbered import observation fail',()=>{
  const record=successfulReplayRecord();
  gateReplayRecordOnFinalErrors(record,{pageErrors:[],consoleErrors:[],httpErrors:[],
    protocolErrors:[{event:'abort'}],responseErrors:[],teardownErrors:[]});
  const check=replayOriginalImportCheck(record);
  assert.equal(record.finalErrorAudit.status,'failed');assert.equal(record.playback.scenarioStatus,'failed');
  assert.equal(check.status,'failed');assert.equal(check.complete,false);
});

test('th06 and th07 retain their native cursor/total self-consistency check without claiming full input consumption',()=>{
  for(const game of ['th06','th07']){
    const result=assessReplayTerminalSignal({game,state:{frame:4856,total:4856,ended:true}});
    assert.equal(result.status,'observed');assert.equal(result.nativeFormatConsistency.status,'passed');
    assert.deepEqual(result.terminalSignalObserved,{status:'passed',value:true,field:'ended'});
    assert.equal(result.fullInputConsumption.status,'unproven');
    assert.equal(assessReplayTerminalSignal({game,state:{frame:4855,total:4856,ended:true}}).status,'failed');
    assert.equal(assessReplayTerminalSignal({game,state:{frame:4856,total:0,ended:true}}).status,'failed');
  }
});

test('th08 reports the native terminal signal without imposing a declared total and keeps the candidate counter separate',()=>{
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

test('th08 normal reports capacity and native terminal evidence without promoting an unmapped endpoint',()=>{
  const expected={stage:5,sha256:'6de76d9f5d0ef8d58887bc74087fd566c246c676b6429850ea4702776f2533d8',
    capacity:{maxSuccessfulFrames:11084,inputCursorAtCapacity:22168,timingCursorAtCapacity:370,
      inputFrameCapacity:11084,timingFrameCapacity:11100},
    blocks:{inputBytes:22168,timingBytes:371},stride:2,debugRecords:false};
  const actual={available:true,loaded:true,stage:5,frame:11047,reportedFrame:11047,inputCursor:22094,inputBytes:22168,
    timingCursor:369,timingBytes:371,stride:2,ended:true,inGame:false};
  const withinCapacity={status:'incomplete',expected,actual,matches:{withinStageCapacity:true,atFileDerivedCapacity:false},
    reason:'The accepted original capture has no mapped natural ReplayStream cursor for this fixture.'};
  const terminal=assessReplayTerminalSignal({game:'th08',variant:'normal',state:{frame:11047,total:0,ended:true},nativeInputConsumption:withinCapacity});
  assert.equal(terminal.status,'incomplete');assert.equal(terminal.fullInputConsumption.status,'incomplete');
  assert.equal(terminal.terminalSignalObserved.status,'passed');
  assert.equal(assessReplayTerminalSignal({game:'th08',variant:'normal',state:{frame:11047,total:0,ended:true}}).status,'incomplete');
  assert.equal(assessReplayTerminalSignal({game:'th08',variant:'normal',state:{frame:11047,total:0,ended:true},
    nativeInputConsumption:{status:'failed',reason:'frame/input cursor relation'}}).status,'failed');

  const capacityOnlyClaim={status:'passed',expected,actual:{...actual,frame:11084,reportedFrame:11084,inputCursor:22168,timingCursor:370},
    naturalTerminalReference:null};
  const noReference=assessReplayTerminalSignal({game:'th08',variant:'normal',state:{frame:11084,total:0,ended:true},
    nativeInputConsumption:capacityOnlyClaim});
  assert.equal(noReference.status,'incomplete');
  assert.equal(noReference.fullInputConsumption.status,'incomplete');

  const record={id:'th08-normal',game:'th08',variant:'normal',status:'passed',finalErrorAudit:{status:'passed'},checks:[
    {id:'import-current-replay-through-shell',status:'passed',value:{path:'replay/th8_udabcd.rpy',bytes:3539,sha256:expected.sha256}},
    {id:'native-replay-plays-and-advances',status:'passed',value:{startFrame:0,frame:120,naturalFrames:120,observed:[{frame:120}]}},
    {id:'native-terminal-signal-observed',status:'passed'},
  ],playback:{status:'observed',scenarioStatus:'passed',fixture:{path:'replay/th8_udabcd.rpy',bytes:3539,sha256:expected.sha256},
    terminalSignalObserved:{status:'passed',value:true},fullInputConsumption:capacityOnlyClaim}};
  const check=replayOriginalImportCheck(record,{game:'th08',variant:'normal'});
  assert.equal(check.status,'incomplete');assert.equal(check.complete,false);
  assert.match(check.reason,/independently sourced natural endpoint/);
  record.playback.fullInputConsumption={...capacityOnlyClaim,status:'incomplete',reason:'no natural endpoint reference'};
  record.playback.status='incomplete';record.playback.scenarioStatus='incomplete';record.status='incomplete';
  const incomplete=replayOriginalImportCheck(record,{game:'th08',variant:'normal'});
  assert.equal(incomplete.status,'incomplete');assert.equal(incomplete.reason,'no natural endpoint reference');
});

test('th09 reports the native terminal signal and leaves an unbound persisted comparison as a candidate count',()=>{
  const reference={kind:'original-process-and-cpp-session-replay-comparison',counter:{name:'comparedTicks',value:7996},
    replaySha256:'b'.repeat(64),persistedReplaySha256:null,selectedReplaySha256:'b'.repeat(64),
    counterBoundToSelectedReplay:false,comparableToNativeCursor:false,
    countDefinition:'Persisted result does not include the selected Replay hash.'};
  const result=assessReplayTerminalSignal({game:'th09',state:{frame:8004,total:0,ended:true},candidateCounterReference:reference});
  assert.equal(result.status,'observed');assert.equal(result.candidateCounterObservation.numericDelta,8);
  assert.equal(result.candidateCounterObservation.semanticComparison,'unproven');
  assert.equal(result.candidateCounterObservation.counterBoundToSelectedReplay,false);
});

test('th11 preserves cursor, total and metadata while leaving full input consumption unproven',()=>{
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

test('th10 and th15 require rate-window progress and report terminal signals without full input proof',()=>{
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
