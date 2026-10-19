import test from 'node:test';
import assert from 'node:assert/strict';
import {assertGamepadActionEvidence,assertGamepadDisconnectEvidence,assertGamepadNativeObserver,gamepadProbeFailureEvidence,gamepadTargetFor,nativeGamepadReadiness,waitForGamepadCondition} from './gamepad-probe.mjs';

function observation({game='th08',seat=0,includeSeat=true,input=0,x=1,phase=game==='th09'?'match':'stage',mask=0x107,abi=true}={}){
  return {game,phase:{name:phase},input:{value:input},position:{x,y:1},...(includeSeat?{localSeat:seat}:{}),
    capabilities:{phase:true,input:true,position:true},...(abi?{nativeObservation:{version:1,validMask:mask}}:{})};
}

test('native gamepad evidence requires the real ABI, gameplay fields, and the expected local seat',()=>{
  const target=gamepadTargetFor('th08','multiplayer',1);
  assert.equal(target.mappingSeat,0,'TH08 uses the shell default gamepad slot while reporting its room seat');
  assert.equal(assertGamepadNativeObserver(observation({seat:1}),target).localSeat,1);
  assert.equal(assertGamepadNativeObserver(observation({seat:1,abi:false}),target).localSeat,1,
    'TH08 may use its seat-specific existing multiplayer status getter');
  assert.throws(()=>assertGamepadNativeObserver(observation({seat:1,abi:false,includeSeat:false}),target),/expected 1/);
  assert.throws(()=>assertGamepadNativeObserver(observation({game:'th11',seat:1,mask:0x103}),gamepadTargetFor('th11','multiplayer',1)),/native phase, consumed input, position, and local seat are required/);
  assert.throws(()=>assertGamepadNativeObserver(observation({seat:0}),target),/expected 1/);
  assert.throws(()=>assertGamepadNativeObserver(observation({game:'th11',seat:1,abi:false}),gamepadTargetFor('th11','multiplayer',1)),/no native observer/);
  assert.throws(()=>gamepadTargetFor('th09','multiplayer',2),/invalid local seat/);
});

test('initial observation waits through native field readiness and rejects unknown ABI or another seat',()=>{
  const target=gamepadTargetFor('th08','normal',0);
  const enteringGameplay=observation({game:'th08',seat:0,abi:false,phase:'title',input:0});
  enteringGameplay.capabilities.input=false;enteringGameplay.capabilities.position=false;
  let readiness=nativeGamepadReadiness(enteringGameplay,target,{mappingSamples:1});
  assert.equal(readiness.ready,false);
  assert.ok(readiness.reasons.includes('native phase is title, waiting for stage'));
  assert.ok(readiness.reasons.includes('native player position is not ready'));
  const gameplay=observation({game:'th08',seat:0,abi:false,phase:'stage',input:0});
  readiness=nativeGamepadReadiness(gameplay,target,{mappingSamples:3});
  assert.equal(readiness.ready,true);
  const unknown=nativeGamepadReadiness(observation({game:'th11',seat:0,abi:false}),gamepadTargetFor('th11','normal',0),{mappingSamples:3});
  assert.equal(unknown.ready,false);
  assert.ok(unknown.reasons.includes('native observation ABI unavailable'));
  assert.throws(()=>nativeGamepadReadiness(observation({game:'th08',seat:1,abi:false}),target,{mappingSamples:3}),/expected 0/);
});

test('button evidence binds the mapped getter call to native input change and release',()=>{
  const target=gamepadTargetFor('th11','multiplayer',2);
  const before=observation({game:'th11',seat:2,input:0});
  const held=observation({game:'th11',seat:2,input:1});
  const released=observation({game:'th11',seat:2,input:0});
  const evidence={target,action:'shoot',bits:1,heldSamples:[{seat:0,bits:1}],releasedSamples:[{seat:0,bits:0}],before,held,released};
  assert.equal(assertGamepadActionEvidence(evidence).localSeat,2);
  assert.throws(()=>assertGamepadActionEvidence({...evidence,heldSamples:[{seat:1,bits:1}]}),/expected native gamepad getter for mapping seat 0/);
  assert.throws(()=>assertGamepadActionEvidence({...evidence,held:observation({game:'th11',seat:2,input:0})}),/did not change native consumed input/);
  assert.throws(()=>assertGamepadActionEvidence({...evidence,released:observation({game:'th11',seat:2,input:1})}),/did not restore neutral native input/);
});

test('rightward action needs positive native movement and disconnect needs neutral native input',()=>{
  const target=gamepadTargetFor('th09','multiplayer',1);
  const before=observation({game:'th09',seat:1,input:0,x:4});
  const held=observation({game:'th09',seat:1,input:128,x:5});
  const released=observation({game:'th09',seat:1,input:0,x:5});
  const disconnectHeld=observation({game:'th09',seat:1,input:4,x:5});
  const evidence={target,action:'right',bits:128,heldSamples:[{seat:1,bits:128}],releasedSamples:[{seat:1,bits:0}],before,held,released};
  assert.equal(assertGamepadActionEvidence(evidence).mappingSeat,1);
  assert.throws(()=>assertGamepadActionEvidence({...evidence,held:observation({game:'th09',seat:1,input:128,x:4})}),/did not move in positive x/);
  assert.deepEqual(assertGamepadDisconnectEvidence({target,heldSamples:[{seat:1,bits:4}],heldNative:disconnectHeld,heldBits:4,
    samples:[{seat:1,bits:-1}],native:released,neutralInput:0}),
    {mappingSeat:1,localSeat:1,heldBits:4,nativeInputWhileHeld:4,nativeInputAfterDisconnect:0});
  assert.throws(()=>assertGamepadDisconnectEvidence({target,heldSamples:[{seat:0,bits:4}],heldNative:disconnectHeld,heldBits:4,
    samples:[{seat:1,bits:0}],native:released,neutralInput:0}),/disconnect was not preceded by a held native gamepad input/);
  assert.throws(()=>assertGamepadDisconnectEvidence({target,heldSamples:[{seat:1,bits:4}],heldNative:released,heldBits:4,
    samples:[{seat:1,bits:0}],native:released,neutralInput:0}),/disconnect fixture had no held native input/);
  assert.throws(()=>assertGamepadDisconnectEvidence({target,heldSamples:[{seat:1,bits:4}],heldNative:disconnectHeld,heldBits:4,
    samples:[{seat:1,bits:0}],native:held,neutralInput:0}),/disconnect left native consumed input active/);
});

test('invalid native fields fail immediately with the full action snapshot and recent samples',async()=>{
  const target=gamepadTargetFor('th15','multiplayer',0),native=observation({game:'th15',seat:0,input:0,x:192,mask:261});
  const recentSamples=Array.from({length:10},(_,index)=>({seat:0,bits:index===9?128:0,at:index*50}));
  let reads=0,waits=0,failure,clock=1000;
  await assert.rejects(()=>waitForGamepadCondition({game:'th15',target,label:'shoot native input effect',action:'shoot',phase:'held',
    timeoutMs:8000,readSamples:async()=>{reads++;clock+=7;return recentSamples;},
    readNative:async()=>{clock+=3;return native;},pause:async milliseconds=>{waits++;clock+=milliseconds;},now:()=>clock}),error=>{
    failure=error;
    assert.match(error.message,/native phase, consumed input, position, and local seat are required/);
    assert.deepEqual(error.diagnostic,{game:'th15',label:'shoot native input effect',action:'shoot',phase:'held',timeoutMs:8000,
      attempts:1,elapsedMs:10,lastNativeObservation:native,readiness:null,recentSamples:recentSamples.slice(-8)});
    return true;
  });
  assert.equal(reads,1,'An invalid ABI remains an immediate assertion failure');
  assert.equal(waits,0,'Invalid observations are not retried or hidden');
  const recorded=gamepadProbeFailureEvidence(failure,target);
  assert.equal(recorded.passed,false);
  assert.equal(recorded.target,target);
  assert.deepEqual(recorded.diagnostic,failure.diagnostic);
  assert.match(recorded.error,/native phase, consumed input, position, and local seat are required/);
});

test('native gamepad timeouts retain their last snapshot, label, phase, and recent samples',async()=>{
  const target=gamepadTargetFor('th15','multiplayer',0),native=observation({game:'th15',seat:0,input:0,x:192});
  const samples=Array.from({length:10},(_,index)=>({seat:0,bits:0,at:index*50}));
  let clock=0,failure;
  await assert.rejects(()=>waitForGamepadCondition({game:'th15',target,label:'right native input effect',action:'right',phase:'held',timeoutMs:100,
    readSamples:async()=>samples,readNative:async()=>native,predicate:()=>false,pause:async milliseconds=>{clock+=milliseconds;},now:()=>clock}),error=>{
    failure=error;
    assert.match(error.message,/timed out waiting for right native input effect/);
    assert.deepEqual(error.diagnostic,{game:'th15',label:'right native input effect',action:'right',phase:'held',timeoutMs:100,
      attempts:2,elapsedMs:100,lastNativeObservation:native,readiness:null,recentSamples:samples.slice(-8)});
    return true;
  });
  assert.deepEqual(gamepadProbeFailureEvidence(failure,target).diagnostic,failure.diagnostic);
});
