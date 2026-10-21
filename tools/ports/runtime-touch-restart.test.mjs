import assert from 'node:assert/strict';
import test from 'node:test';
import {assessRuntimeTouchRestart,captureRuntimeRestartBoundary,compareRuntimeRestartIdentity,createLauncherRestartPress} from './runtime-touch-restart.mjs';

const identity=(overrides={})=>({hostPageId:'page-1',hostTimeOrigin:1000,frameDocumentId:'doc-1',
  frameUrl:'https://fixture.test/runtime/th08?runtimeEpoch=1',epoch:1,runtimeId:'runtime-1',
  moduleId:'module-1',coreId:'core-1',firstFrameId:'first-frame-1',firstFrameCount:1,...overrides});
const observation=(enemy,pause=false,overrides={})=>({game:'th08',
  capabilities:{pause:true,restartFrame:'enemy'},frames:{enemy},pause:{active:pause},...overrides});
const sequence=()=>({before:observation(120),paused:observation(120,true),
  pauseStable:observation(120,true),resetBoundary:{frameKey:'enemy',frame:1,ready:true,pauseActive:false,
    operationStarted:true,index:1,elapsedMs:75},restarted:observation(121),advanced:observation(122),
  beforeIdentity:identity(),afterIdentity:identity(),expectedEpoch:1,
  gameplayReady:{restarted:true,advanced:true}});

test('restart evidence accepts the same Runtime after a native frame reset and advance',()=>{
  const evidence=sequence(),result=assessRuntimeTouchRestart(evidence);
  assert.equal(result.checkStatus,'passed');
  assert.deepEqual(result.identity,{status:'same',missing:[],differences:[]});
  assert.equal(result.restartFrame,'enemy');
  assert.deepEqual([result.before,result.paused,result.resetBoundary.frame,result.restarted,result.advanced],[120,120,1,121,122]);
});

test('restart evidence fails if a page reload or different Runtime produces a fresh gameplay frame',()=>{
  const evidence=sequence();
  evidence.afterIdentity=identity({frameDocumentId:'doc-2',runtimeId:'runtime-2',moduleId:'module-2',
    coreId:'core-2',firstFrameId:'first-frame-2',hostTimeOrigin:2000});
  assert.throws(()=>assessRuntimeTouchRestart(evidence),/Restart changed the host/);
});

test('restart identity stays incomplete when mounted Runtime or original frame receipt is absent',()=>{
  const incomplete=compareRuntimeRestartIdentity(identity({moduleId:null}),identity());
  assert.equal(incomplete.status,'incomplete');
  assert.ok(incomplete.missing.includes('before.moduleId'));
  const noFrame=compareRuntimeRestartIdentity(identity({firstFrameCount:0}),identity());
  assert.equal(noFrame.status,'incomplete');
});

test('restart input substitutes a Launcher action only for the flow initial R',async()=>{
  const calls=[],input=createLauncherRestartPress({press:async code=>calls.push(code),
    requestRestart:async()=>calls.push('production-restart-button')});
  await input.press('KeyR');await input.press('ArrowUp');await input.press('KeyR');await input.press('KeyZ');
  assert.equal(input.wasRequested(),true);
  assert.deepEqual(calls,['production-restart-button','ArrowUp','KeyR','KeyZ']);
  assert.deepEqual(input.forwardedCodes(),['ArrowUp','KeyR','KeyZ']);
});

test('restart input does not fall back to a keyboard when the flow starts with another action',async()=>{
  const calls=[],input=createLauncherRestartPress({press:async code=>calls.push(code),
    requestRestart:async()=>calls.push('production-restart-button')});
  await assert.rejects(input.press('KeyZ'),error=>error.status==='incomplete'&&
    error.evidence.gameplayInputSent===false);
  assert.equal(input.wasRequested(),false);assert.deepEqual(calls,[]);
});

test('restart evidence is incomplete when native gameplay readiness or frame evidence is unavailable',()=>{
  const notReady=sequence();notReady.gameplayReady.advanced=false;
  assert.equal(assessRuntimeTouchRestart(notReady).checkStatus,'incomplete');
  const noRestartFrame=sequence();noRestartFrame.before.capabilities.restartFrame=null;
  assert.equal(assessRuntimeTouchRestart(noRestartFrame).checkStatus,'incomplete');
});

test('a reset sample is compared with paused frame even when the first ready snapshot has advanced beyond it',()=>{
  const evidence=sequence();evidence.paused=observation(8,true);evidence.pauseStable=observation(8,true);
  evidence.resetBoundary={...evidence.resetBoundary,frame:0};
  evidence.restarted=observation(14);evidence.advanced=observation(15);
  assert.equal(assessRuntimeTouchRestart(evidence).checkStatus,'passed');
});

test('a no-op restart or missing concurrent reset sample cannot pass a complete report shape',()=>{
  const evidence=sequence();evidence.resetBoundary={...evidence.resetBoundary,frame:120};
  assert.equal(assessRuntimeTouchRestart(evidence).checkStatus,'incomplete');
  const missing=sequence();delete missing.resetBoundary;
  assert.equal(assessRuntimeTouchRestart(missing).checkStatus,'incomplete');
  const beforeInput=sequence();beforeInput.resetBoundary={...beforeInput.resetBoundary,operationStarted:false};
  assert.equal(assessRuntimeTouchRestart(beforeInput).checkStatus,'incomplete');
  const stillPaused=sequence();stillPaused.restarted=observation(1,true);
  assert.throws(()=>assessRuntimeTouchRestart(stillPaused),/pause state remains active/);
  const noAdvance=sequence();noAdvance.advanced=observation(121);
  assert.throws(()=>assessRuntimeTouchRestart(noAdvance),/did not advance/);
});

test('restart boundary sampling starts before the operation and captures through a 90+100 ms input lifecycle',async()=>{
  let frame=240,pause=true,operationFinishedAt=null,operationStartedAtReads=0;
  const observe=async()=>{operationStartedAtReads++;return observation(frame,pause);};
  const sleep=ms=>new Promise(resolve=>setTimeout(resolve,ms));
  const startedAt=performance.now();
  const result=await captureRuntimeRestartBoundary({observe,frameKey:'enemy',pausedFrame:240,
    isGameplayReady:state=>state.frames.enemy>=0,timeoutMs:1000,pollIntervalMs:5,
    wait:ms=>sleep(ms),now:()=>performance.now(),
    operate:async()=>{
      assert.equal(operationStartedAtReads,1,'the sampler must take its first native observation before input starts');
      await sleep(90);frame=0;pause=false;
      await sleep(100);operationFinishedAt=performance.now();return{keyReleased:true};
    }});
  assert.equal(result.status,'observed');
  assert.equal(result.resetBoundary.frame,0);
  assert.equal(result.resetBoundary.operationStarted,true);
  assert.equal(result.resetBoundary.pauseActive,false);
  assert.ok(result.resetBoundary.elapsedMs<operationFinishedAt-startedAt,
    'the reset sample must be captured while the operation is still active');
  assert.equal(result.operationResult.keyReleased,true);
  assert.ok(result.sampleCount<=result.sampleLimit&&result.sampleLimit<=512);
});

test('a no-op operation yields bounded incomplete evidence rather than a reset pass',async()=>{
  const result=await captureRuntimeRestartBoundary({observe:async()=>observation(300,true),
    operate:async()=>({keyReleased:true}),frameKey:'enemy',pausedFrame:300,
    isGameplayReady:()=>true,timeoutMs:8,pollIntervalMs:1,wait:async()=>{}});
  assert.equal(result.status,'incomplete');
  assert.equal(result.resetBoundary,null);
  assert.ok(result.samples.length<=32);
  assert.equal(result.samplesOmitted,result.sampleCount-result.samples.length);
});

test('restart input is not sent when the pre-action sample has left the paused frame boundary',async()=>{
  let operated=false;
  const result=await captureRuntimeRestartBoundary({observe:async()=>observation(4,false),
    operate:async()=>{operated=true;},frameKey:'enemy',pausedFrame:40,
    isGameplayReady:()=>true,timeoutMs:100,pollIntervalMs:1});
  assert.equal(result.status,'incomplete');
  assert.equal(result.operationStarted,false);
  assert.equal(operated,false);
});

test('observation failure stops sampling and still waits for the operation to finish',async()=>{
  let reads=0,operationFinished=false;
  const result=await captureRuntimeRestartBoundary({observe:async()=>{
    reads++;if(reads===1)return observation(50,true);throw Error('native observation failed');
  },operate:async()=>{await new Promise(resolve=>setTimeout(resolve,10));operationFinished=true;},
  frameKey:'enemy',pausedFrame:50,isGameplayReady:()=>true,timeoutMs:100,pollIntervalMs:1,
  wait:ms=>new Promise(resolve=>setTimeout(resolve,ms))});
  assert.equal(result.status,'incomplete');
  assert.match(result.error,/native observation failed/);
  assert.equal(operationFinished,true);
});

test('operation failure stops the bounded observation loop and is preserved',async()=>{
  let observations=0;
  await assert.rejects(captureRuntimeRestartBoundary({observe:async()=>{observations++;return observation(50,true);},
    operate:async()=>{throw Error('restart input failed');},frameKey:'enemy',pausedFrame:50,
    isGameplayReady:()=>true,timeoutMs:100,pollIntervalMs:1,
    wait:ms=>new Promise(resolve=>setTimeout(resolve,ms))}),/restart input failed/);
  assert.ok(observations<=2,'failed operations must stop the sampler promptly');
});
