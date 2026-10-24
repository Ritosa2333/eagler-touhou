import test from 'node:test';
import assert from 'node:assert/strict';
import {Th08RuntimeDriver} from '../../../../ports/th08/portable/presentation-lab/adapter.mjs';
import {createReplayLedgerInspector} from '../../../../ports/th08/portable/presentation-lab/replay-capture.mjs';
import {inspectTh08CaptureJournal,assessTh08ReplayCapture} from './th08-replay.mjs';

const metadata={schema:'th08/replay-capture-journal/1',applicationTicks:3,capturedRows:3};
function samples(){return [1,2,3].map(frame=>({applicationTick:frame,advanced:true,scene:2,stage:0,
  traceFrame:frame,flags:0xc,captured:true,rowIndex:frame-1,reason:null,
  replay:{loaded:1,frame,inputCursor:frame*2,inputBytes:100,timingCursor:1,timingBytes:10,stride:2}}));}
const row=s=>({applicationTick:s.applicationTick,frame:s.traceFrame,flags:s.flags,stage:s.stage,replay:{...s.replay}});

test('capture journal binds every trace row to a successful native consumption boundary',async()=>{
  const cursorLedger=samples();
  const capture={journal:metadata,applicationTicks:3,cursorLedger};
  assert.equal((await inspectTh08CaptureJournal(capture,{rows:cursorLedger.map(row)})).status,'passed');
  for(const mutate of [
    s=>s.splice(1,1),s=>s[1].applicationTick=1,s=>s[1].replay.inputCursor=8,
    s=>s[1].replay.frame=0,s=>s[1].replay.stride=6,s=>s[1].replay.timingCursor=2,
    s=>s[1].captured=false,s=>s[1].rowIndex=0,s=>s[1].advanced=false,
  ]){
    const changed=samples();mutate(changed);
    assert.equal((await inspectTh08CaptureJournal({...capture,cursorLedger:changed},{rows:cursorLedger.map(row)})).status,'incomplete');
  }
  const inspector=createReplayLedgerInspector();
  assert.equal(inspector.finish(metadata).status,'incomplete');
});

test('capture eligibility refuses a different DATA or the Runtime-imported Replay identity',()=>{
  const {game,adapterVersion,nativeAbi}=Th08RuntimeDriver.prototype.describe();
  const hash='a'.repeat(64),build={game,adapterVersion,nativeAbi,runtimeEpoch:1,schema:'th08/presentation-lab-build/1',commit:'b'.repeat(40),instrumented:true,
    wasm:'c'.repeat(64),loader:'d'.repeat(64),sourceDigest:'e'.repeat(64),toolDigest:'f'.repeat(64),toolSources:{'adapter.mjs':hash},
    inputs:{data:{bytes:100,sha256:hash},replay:{bytes:123,sha256:hash}}};
  const capture={schema:'th08/current-replay-capture/v3',complete:true,stopReason:'returned-to-title',terminalScene:1,
    replay:{bytes:123,sha256:hash},errors:[],build};
  const expected={lane:'quick',id:'demo0',expectedSha256:hash,expectedBytes:123,expectedResourceSha256:hash};
  assert.equal(assessTh08ReplayCapture(capture,expected),null);
  for(const key of ['data','replay']){
    const changed=structuredClone(capture);changed.build.inputs[key].sha256='9'.repeat(64);
    assert.equal(assessTh08ReplayCapture(changed,expected).status,'incompatible');
  }
  const errored={...capture,complete:false,errors:['Runtime stopped after captured tick']};
  assert.equal(assessTh08ReplayCapture(errored,{...expected,allowPrefix:true}),null);
  assert.equal(assessTh08ReplayCapture(errored,expected).status,'incomplete');
});

test('filtered application steps remain in the ledger and completion binds the actual last scene',async()=>{
  let captured=0;
  const cursorLedger=Array.from({length:32},(_,index)=>{
    const active=index>=26&&index<31,frame=active?++captured:0;
    return{applicationTick:index+1,advanced:true,scene:active?2:1,stage:0,traceFrame:frame,flags:active?12:0,
      captured:active,rowIndex:active?frame-1:null,reason:active?null:'owner-transition',
      replay:{loaded:active?1:0,frame,inputCursor:frame*2,inputBytes:100,timingCursor:active?1:0,timingBytes:10,stride:2}};
  });
  const rows=cursorLedger.filter(sample=>sample.captured).map(row);
  const capture={applicationTicks:32,cursorLedger,complete:true,stopReason:'returned-to-title',terminalScene:1,
    journal:{schema:'th08/replay-capture-journal/1',applicationTicks:32,capturedRows:5}};
  assert.equal((await inspectTh08CaptureJournal(capture,{rows})).status,'passed');
  for(const mutate of [c=>c.cursorLedger.splice(9,1),c=>c.cursorLedger.at(-1).scene=2,
    c=>{c.cursorLedger.pop();c.applicationTicks--;c.journal.applicationTicks--;}]){
    const changed=structuredClone(capture);mutate(changed);
    assert.equal((await inspectTh08CaptureJournal(changed,{rows})).status,'incomplete');
  }
});

test('native dialogue and fast playback batches retain matching Replay and trace progression',async()=>{
  const cursorLedger=samples();
  for(let index=1;index<3;index++){
    const frame=index===1?4:9;
    Object.assign(cursorLedger[index],{traceFrame:frame});
    Object.assign(cursorLedger[index].replay,{frame,inputCursor:frame*2});
  }
  const capture={journal:metadata,applicationTicks:3,cursorLedger};
  assert.equal((await inspectTh08CaptureJournal(capture,{rows:cursorLedger.map(row)})).status,'passed');
  for(const mutate of [s=>s[1].traceFrame++,s=>{s[1].traceFrame=8;s[1].replay.frame=8;s[1].replay.inputCursor=16;}]){
    const changed=structuredClone(cursorLedger);mutate(changed);
    assert.equal((await inspectTh08CaptureJournal({...capture,cursorLedger:changed},{rows:changed.map(row)})).status,'incomplete');
  }
});
