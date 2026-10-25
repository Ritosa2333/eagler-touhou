import assert from 'node:assert/strict';
import test from 'node:test';
import { runTouchMotionSceneGroup as runTouchMotionSceneGroupCore } from './touch-motion-scenes.mjs';
import { TOUCH_TRACE_SCHEMA } from './touch-motion-contract.mjs';

const QUEUED_MOVEMENT_POLICY=Object.freeze({
  semantics:'queued-for-next-movement',observationSource:'fixture.native-buttons',
  observationPhase:'after-movement-update',directionMasks:Object.freeze({ArrowRight:128,ArrowLeft:64}),
  focusMask:4,maxMovementTicks:1,
  keyboardTouchOwnership:Object.freeze({semantics:'independent-input-owners',source:Object.freeze(['fixture.independent-owner-policy'])}),
  touchFocusTransitions:Object.freeze({semantics:'action-queue-then-next-movement',maxMovementTicks:1}),
});
const TH08_TAKEOVER_POLICY=Object.freeze({...QUEUED_MOVEMENT_POLICY,
  keyboardTouchOwnership:Object.freeze({semantics:'direction-key-clears-touch-motion',source:Object.freeze([
    'ports/th08/web/cpp/sdl/GameHost.cpp | poll | touch.sample arrows',
    'ports/th08/portable/input/TouchController.hpp | TouchController::sample | arrows => clear_motion',
  ])}),
});
const queuedAfter=(row,value)=>({...row,nextMovementInputAfter:value});
const runTouchMotionSceneGroup=(adapter,options={})=>runTouchMotionSceneGroupCore(adapter,{
  inputBoundary:options.queuedMovementPolicy?'queued-for-next-movement':'synchronous-consumption',
  inputBoundarySource:'fixture | synchronous input consumption',...options});
const runQueuedScene=(adapter,options={})=>runTouchMotionSceneGroup(adapter,{...options,queuedMovementPolicy:QUEUED_MOVEMENT_POLICY});
const runTh08TakeoverScene=(adapter,options={})=>runTouchMotionSceneGroup(adapter,{...options,queuedMovementPolicy:TH08_TAKEOVER_POLICY});

function scriptedAdapter(rows) {
  const log = [];
  let nextSequence = 0;
  const heldKeys = new Set();
  const samples = [];
  let nextMovementInput=0;
  const inputBoundary=()=>({available:true,semantics:'queued-for-next-movement',
    observationSource:'fixture.native-buttons',observationPhase:'after-movement-update',
    value:nextMovementInput,traceTotal:samples.length});
  const adapter = {
    log,
    samples,
    async readTrace() {
      return { schema: TOUCH_TRACE_SCHEMA, abiVersion: 2, localSeat: 0, executionKind: 0,
        total: samples.length, samples: samples.slice(),nextMovementInputBoundary:inputBoundary(),stableTraceBoundary:true };
    },
    async controls(fields) { log.push({ type: 'controls', fields: { ...fields } }); },
    async pointer(type, id, x, y) { log.push({ type: 'pointer', action: type, id, x, y }); },
    async key(code, down) {
      const changed = heldKeys.has(code) !== down;
      if (down) heldKeys.add(code); else heldKeys.delete(code);
      log.push({ type: 'key', code, down });
      return { acknowledged: true, changed, nativeTraceTotal: samples.length, stableTraceBoundary: true,
        nextMovementInputBoundary:inputBoundary() };
    },
    async wait(ms) {
      assert.equal(ms, 8, 'scene scheduler polls in bounded 8 ms steps');
      log.push({ type: 'wait', ms });
      const row = rows.shift();
      assert.ok(row, 'fixed fixture ran out before the intended assertion');
      nextSequence++;
      const sample={...row};
      if(Object.hasOwn(sample,'nextMovementInputAfter')){nextMovementInput=sample.nextMovementInputAfter;delete sample.nextMovementInputAfter;}
      samples.push({ sequence: nextSequence, frame: nextSequence, ...sample,
        abiVersion: 2, localSeat: 0, executionKind: 0 });
    },
  };
  return adapter;
}

function exposeAbi2(adapter, mapSample = sample => sample) {
  const read = adapter.readTrace.bind(adapter);
  adapter.readTrace = async () => {
    const trace = await read();
    return {
      ...trace,
      abiVersion: 2,
      localSeat: 0,
      executionKind: 0,
      samples: trace.samples.map(sample => ({
        ...mapSample(sample),
        abiVersion: 2,
        localSeat: 0,
        executionKind: 0,
      })),
    };
  };
  return adapter;
}

function focusTraceRows(rows,ranges){
  return rows.map((row,index)=>ranges.some(([start,end])=>index>=start&&index<=end)?{...row,flags:row.flags|4}:row);
}

// These are fixed native observations, not rows synthesized from the scene oracle.
const neutral = (x, y, { mode = 0, targetX = x, targetY = y, speed = 4, sensitivity = 1 } = {}) => ({
  mode, flags: 1, beforeX: x, beforeY: y, targetX, targetY, afterX: x, afterY: y,
  speed, scaleX: 1, scaleY: 1, sensitivity,
});

// Runtime mode-0 observations keep the touch target at the starting position;
// keyboard movement appears only in after-before (ports/th08/web/cpp/platform/BrowserRuntime.cpp:73-74,244).
const keyboardMovement = (x,y,afterX,{speed=4,flags=1,nextInput=0}={})=>queuedAfter({
  ...neutral(x,y,{speed}),flags,afterX,
},nextInput);

// Fixed native keyboard observations establish 4-unit fast and 2-unit focused
// movement independently of the touch oracle used by the scene.
const fixedKeyboardCalibration = focusTraceRows([
  neutral(320, 240), neutral(320, 240), neutral(320, 240), neutral(320, 240),
  neutral(320, 240, { sensitivity: 1 }),
  keyboardMovement(320,240,324,{speed:4}),
  keyboardMovement(324,240,328,{speed:4}),
  keyboardMovement(328,240,332,{speed:4,nextInput:128}),
  neutral(332, 240), neutral(332, 240), neutral(332, 240),
  neutral(332, 240),
  keyboardMovement(332,240,330,{speed:2,flags:5,nextInput:68}),
  keyboardMovement(330,240,328,{speed:2,flags:5,nextInput:68}),
  keyboardMovement(328,240,326,{speed:2,flags:5,nextInput:68}),
  neutral(326, 240), neutral(326, 240), neutral(326, 240),
],[[12,14]]);

function keyboardReleaseRows(){
  const rows=fixedKeyboardCalibration.slice();
  rows[5]=queuedAfter({...rows[5]},128);
  rows[6]=queuedAfter({...rows[6]},128);
  rows[7]=queuedAfter({...rows[7]},128);
  rows[8]=keyboardMovement(332,240,336,{speed:4});
  rows[9]=queuedAfter(neutral(336,240,{speed:4}),0);
  rows[10]=queuedAfter(neutral(336,240,{speed:4}),0);
  rows[11]=queuedAfter(neutral(336,240,{speed:4}),0);
  rows.splice(12,0,queuedAfter(neutral(336,240,{speed:4}),0));
  rows[13]=keyboardMovement(336,240,334,{speed:2,flags:5,nextInput:68});
  rows[14]=keyboardMovement(334,240,332,{speed:2,flags:5,nextInput:68});
  rows[15]=keyboardMovement(332,240,330,{speed:2,flags:5,nextInput:68});
  rows[16]=keyboardMovement(330,240,328,{speed:2,flags:5});
  rows[17]=queuedAfter(neutral(328,240,{speed:4}),0);
  rows[18]=queuedAfter(neutral(328,240,{speed:4}),0);
  rows[19]=queuedAfter(neutral(328,240,{speed:4}),0);
  rows.push(queuedAfter(neutral(328,240,{speed:4}),0));
  return rows;
}

function twoFingerQueuedRows(){
  const rows=keyboardReleaseRows();
  const fast=(x,y,options={},next=0)=>queuedAfter(neutral(x,y,options),next);
  const touchFast=(x,y,options={},next=0)=>queuedAfter(neutral(x,y,{mode:1,...options}),next);
  const touchSlow=(x,y,options={},next=4)=>queuedAfter({...neutral(x,y,{mode:1,...options}),flags:5},next);
  rows.push(
    fast(328,240,{sensitivity:1}),
    touchFast(328,240),
    queuedAfter({...neutral(328,240,{mode:1,targetY:244}),afterY:244},0),
    touchFast(328,244,{targetY:244}), touchFast(328,244,{targetY:244}),
    touchFast(328,244,{targetY:244},4),
    touchSlow(328,244,{targetY:244,speed:2},4), touchSlow(328,244,{targetY:244,speed:2},4),
    touchSlow(328,244,{targetY:244,speed:2},4), touchSlow(328,244,{targetY:244,speed:2},4),
    queuedAfter({...neutral(328,244,{mode:1,targetY:245,speed:2}),flags:5,afterY:245},4),
    touchSlow(328,245,{targetY:245,speed:2},4), touchSlow(328,245,{targetY:245,speed:2},4),
    touchSlow(328,245,{targetY:245,speed:2},0),
    touchFast(328,245,{targetY:245}), touchFast(328,245,{targetY:245}),
    queuedAfter({...neutral(328,245,{mode:1,targetY:240}),afterY:241},0),
    queuedAfter({...neutral(328,241,{mode:1,targetY:240}),afterY:240},0),
    touchFast(328,240,{targetY:240}),
    fast(328,240), fast(328,240), fast(328,240),
    fast(328,240,{sensitivity:1}),
    touchFast(328,240),
    { ...neutral(328,240,{mode:1,targetX:334,targetY:248}),afterX:330.4,afterY:243.2 },
    { ...neutral(330.4,243.2,{mode:1,targetX:334,targetY:248}),afterX:332.8,afterY:246.4 },
    { ...neutral(332.8,246.4,{mode:1,targetX:334,targetY:248}),afterX:334,afterY:248 },
    touchFast(334,248,{targetX:334,targetY:248},64),
    queuedAfter({...neutral(334,248,{speed:4}),afterX:330},64),
    queuedAfter({...neutral(330,248,{speed:4}),afterX:326},64),
    queuedAfter({...neutral(326,248,{speed:4}),afterX:322},64),
    queuedAfter({...neutral(322,248,{speed:4}),afterX:318},64),
    fast(318,248,{speed:4},0), fast(318,248,{speed:4},0), fast(318,248,{speed:4},0),
    queuedAfter({...neutral(318,248,{speed:2}),flags:5,sensitivity:1},4), // focused cancel controls
    queuedAfter({...neutral(318,248,{mode:1,speed:2}),flags:5,sensitivity:1},4), // pointer down
    queuedAfter({...neutral(318,248,{mode:1,targetX:321,targetY:252,speed:2}),flags:5,
      afterX:319.2,afterY:249.6},4),
    queuedAfter({...neutral(319.2,249.6,{mode:1,targetX:321,targetY:252,speed:2}),flags:5,
      afterX:320.4,afterY:251.2},4),
    queuedAfter({...neutral(320.4,251.2,{mode:1,targetX:321,targetY:252,speed:2}),flags:5,
      afterX:321,afterY:252},4),
    queuedAfter({...neutral(321,252,{speed:2}),flags:5,sensitivity:1},0),
    fast(321,252,{speed:4},0), fast(321,252,{speed:4},0),
    fast(321,252,{speed:4,sensitivity:1},0), // restore the touch control after cancel
  );
  return {rows,baseCount:keyboardReleaseRows().length};
}

function th08TakeoverRows(){
  const fixture=twoFingerQueuedRows();
  const base=fixture.baseCount;
  fixture.rows[base+27]=queuedAfter(neutral(334,248,{speed:4}),64);
  fixture.rows[base+28]=keyboardMovement(334,248,330,{speed:4,nextInput:64});
  fixture.rows[base+29]=keyboardMovement(330,248,326,{speed:4,nextInput:64});
  fixture.rows[base+30]=keyboardMovement(326,248,322,{speed:4,nextInput:64});
  fixture.rows[base+31]=keyboardMovement(322,248,318,{speed:4});
  return fixture;
}

function keyboardReleaseAdapter(rows,{stopAtControls=4}={}){
  const adapter=scriptedAdapter(rows),held=new Set(),nativeKey=adapter.key.bind(adapter),nativeControls=adapter.controls.bind(adapter);
  let controlCount=0;
  adapter.key=async(code,down)=>{
    const result=await nativeKey(code,down);
    const changed=held.has(code)!==down;
    if(down)held.add(code);else held.delete(code);
    return{...result,acknowledged:true,changed,nativeTraceTotal:adapter.samples.length,stableTraceBoundary:true,
      nextMovementInputBoundary:result.nextMovementInputBoundary};
  };
  adapter.controls=async(fields)=>{
    controlCount++;
    if(controlCount===stopAtControls)throw Error('fixture stopped after keyboard calibration');
    return nativeControls(fields);
  };
  return adapter;
}

test('keyboard release permits the one queued movement tick, then requires three neutral ticks', async () => {
  const adapter = keyboardReleaseAdapter(keyboardReleaseRows());
  await assert.rejects(runQueuedScene(adapter, { mode: 1 }), error => {
    const evidence = error.touchMotionEvidence;
    assert.equal(error.message, 'fixture stopped after keyboard calibration');
    assert.equal(evidence.calibration.fast.neutralRows.length, 3);
    assert.equal(evidence.calibration.slow.neutralRows.length, 3);
    assert.deepEqual(evidence.calibration.fast.releaseAcknowledgements.map(item => item.nativeTraceTotal), [8]);
    assert.deepEqual(evidence.calibration.slow.releaseAcknowledgements.map(item => item.nativeTraceTotal), [16, 16]);
    assert.equal(evidence.calibration.slow.releaseBoundarySequence, 16);
    assert.deepEqual(evidence.keyboardDrainRows.map(row => [row.label, row.classification, row.sample.sequence]), [
      ['fast keyboard release', 'single-next-movement-tick-from-published-input', 9],
      ['slow keyboard release', 'single-next-movement-tick-from-published-input', 17],
    ]);
    assert.deepEqual(evidence.keyboardDrainRows[0].movement, { x: 4, y: 0 });
    assert.deepEqual(evidence.keyboardDrainRows[1].movement, { x: -2, y: 0 });
    assert.equal(evidence.keyboardDrainRows[0].queuedInput.value, 128,
      'the recorded boundary identifies the input published for the next movement callback');
    assert.equal(evidence.keyboardDrainRows[1].queuedInput.value, 68,
      'the recorded boundary identifies the focused direction published for the next movement callback');
    assert.equal(evidence.keyboardDrainRows[1].sample.flags, 5,
      'the single queued focused movement retains the actual native focus observation');
    return true;
  });
});

test('keyboard release fails when the one queued movement does not match published direction and speed', async () => {
  const rows = keyboardReleaseRows();
  rows[16] = { ...neutral(330, 240, { speed: 2 }), flags: 5, afterX: 326 };
  const adapter = keyboardReleaseAdapter(rows);
  await assert.rejects(runQueuedScene(adapter, { mode: 1 }), error => {
    assert.match(error.message, /queued keyboard movement/);
    assert.equal(error.touchMotionEvidence.calibration.slow.releaseBoundarySequence, 16);
    assert.equal(error.touchMotionEvidence.nativeRows.at(-1).sequence, 17,
      'the rejected queued movement remains in the trace evidence');
    return true;
  });
});

test('queued direction evidence requires its exact calibrated movement, including nonzero displacement', async () => {
  const rows = keyboardReleaseRows();
  rows[8] = queuedAfter({ ...neutral(332, 240, { speed: 4 }), afterX: 332 }, 0);
  const adapter = keyboardReleaseAdapter(rows);
  await assert.rejects(runQueuedScene(adapter, { mode: 1 }), error => {
    assert.match(error.message, /queued keyboard movement/);
    assert.equal(error.touchMotionEvidence.keyboardDrainRows.length, 0,
      'the zero-displacement queued tick is rejected before it can be counted as drained');
    return true;
  });
});

test('keyboard release fails when the published direction or focus remains queued after the allowed movement', async () => {
  const rows = keyboardReleaseRows();
  rows[16] = queuedAfter({ ...neutral(330, 240, { speed: 2 }), flags: 5, afterX: 328 }, 68);
  const adapter = keyboardReleaseAdapter(rows);
  await assert.rejects(runQueuedScene(adapter, { mode: 1 }), error => {
    assert.match(error.message, /released direction or focus remained/);
    assert.equal(error.touchMotionEvidence.calibration.slow.releaseBoundarySequence, 16);
    return true;
  });
});

test('keyboard release fails if a second movement follows the one row published at the boundary', async () => {
  const rows = keyboardReleaseRows();
  rows[17] = { ...neutral(328, 240, { speed: 2 }), afterX: 326 };
  const adapter = keyboardReleaseAdapter(rows);
  await assert.rejects(runQueuedScene(adapter, { mode: 1 }), error => {
    assert.match(error.message, /position drift X/);
    assert.equal(error.touchMotionEvidence.calibration.slow.releaseBoundarySequence, 16);
    return true;
  });
});

test('queued-movement keyboard release is incomplete without input-boundary evidence', async () => {
  const adapter = keyboardReleaseAdapter(keyboardReleaseRows());
  const read = adapter.readTrace.bind(adapter);
  adapter.readTrace = async () => ({ ...await read(), nextMovementInputBoundary: null });
  const result = await runQueuedScene(adapter, { mode: 1 });
  assert.equal(result.checkStatus, 'incomplete');
  assert.match(result.reason, /declared input observation before keyboard calibration/);
  assert.equal(adapter.log.filter(entry => entry.type === 'key').length, 0,
    'missing native queue evidence stops before keyboard input is sent');
});

test('queued-movement keyboard release rejects an input observation with an undeclared phase', async () => {
  const adapter = keyboardReleaseAdapter(keyboardReleaseRows());
  const read = adapter.readTrace.bind(adapter);
  adapter.readTrace = async () => {
    const trace = await read();
    return { ...trace, nextMovementInputBoundary: {
      ...trace.nextMovementInputBoundary, observationPhase: 'unknown',
    } };
  };
  const result = await runQueuedScene(adapter, { mode: 1 });
  assert.equal(result.checkStatus, 'incomplete');
  assert.match(result.reason, /declared input observation before keyboard calibration/);
  assert.equal(adapter.log.filter(entry => entry.type === 'key').length, 0);
});

test('scene options placed on the IO adapter fail before any input is emitted', async () => {
  for (const field of ['mode', 'inputSpace', 'precision', 'inputCommand', 'inputBoundary', 'inputBoundarySource', 'queuedMovementPolicy', 'focusMode', 'scenarioSet']) {
    const adapter = scriptedAdapter(fixedKeyboardCalibration.slice());
    adapter[field] = field === 'queuedMovementPolicy' ? QUEUED_MOVEMENT_POLICY : {};
    await assert.rejects(runTouchMotionSceneGroup(adapter, { mode: 1 }), error => {
      assert.ok(error instanceof TypeError);
      assert.match(error.message, new RegExp(`second argument.*IO adapter: ${field}`));
      return true;
    });
    assert.equal(adapter.log.length, 0, `${field} must be rejected before the adapter sends input`);
  }
});

test('an undeclared movement input boundary is incomplete before the scene sends input',async()=>{
  const adapter=scriptedAdapter(fixedKeyboardCalibration.slice());
  const result=await runTouchMotionSceneGroupCore(adapter,{mode:1});
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/input-consumption boundary is unknown/);
  assert.equal(adapter.log.length,0);
});

test('queued-movement keyboard policy must retain the fixed one-tick limit', async () => {
  const adapter=keyboardReleaseAdapter(keyboardReleaseRows());
  const result=await runTouchMotionSceneGroup(adapter,{mode:1,
    queuedMovementPolicy:{...QUEUED_MOVEMENT_POLICY,maxMovementTicks:2}});
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/policy is missing or invalid/);
  assert.equal(adapter.log.length,0,'invalid queued-movement policy is rejected before live input');
});

test('queued-movement policy cannot omit the th08 normal focus-transition phase',async()=>{
  const adapter=keyboardReleaseAdapter(keyboardReleaseRows());
  const policy={...QUEUED_MOVEMENT_POLICY};
  delete policy.touchFocusTransitions;
  const result=await runTouchMotionSceneGroup(adapter,{mode:1,queuedMovementPolicy:policy});
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/policy is missing or invalid/);
  assert.equal(adapter.log.length,0,'missing focus-phase policy cannot fall back to immediate-focus checks');
});

test('queued-movement policy cannot omit source-attested keyboard/touch ownership semantics',async()=>{
  const adapter=keyboardReleaseAdapter(keyboardReleaseRows());
  const policy={...QUEUED_MOVEMENT_POLICY};
  delete policy.keyboardTouchOwnership;
  const result=await runTouchMotionSceneGroup(adapter,{mode:1,queuedMovementPolicy:policy});
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/policy is missing or invalid/);
  assert.equal(adapter.log.length,0,'missing ownership semantics cannot fall back to mixed-owner assumptions');
});

test('strict keyboard calibration requires a bridge acknowledgement for each key transition', async () => {
  const adapter = scriptedAdapter(fixedKeyboardCalibration.slice());
  adapter.key = async (code, down) => {
    adapter.log.push({ type: 'key', code, down });
    return { acknowledged: !down, changed: false, stableTraceBoundary: true };
  };
  await assert.rejects(runTouchMotionSceneGroup(adapter, { mode: 1 }), error => {
    assert.match(error.message, /did not acknowledge down for ArrowRight/);
    return true;
  });
});

test('strict keyboard calibration reports incomplete without a stable trace boundary and clears the accepted key', async () => {
  const adapter = scriptedAdapter(fixedKeyboardCalibration.slice());
  const key = adapter.key.bind(adapter);
  adapter.key = async (code, down) => ({ ...await key(code, down), stableTraceBoundary: false });
  const result = await runTouchMotionSceneGroup(adapter, { mode: 1 });
  assert.equal(result.checkStatus, 'incomplete');
  assert.match(result.reason, /stopped native clock and an acknowledged trace boundary/);
  assert.ok(adapter.log.some(entry => entry.type === 'key' && entry.code === 'ArrowRight' && entry.down));
  assert.ok(adapter.log.some(entry => entry.type === 'key' && entry.code === 'ArrowRight' && !entry.down),
    'a key whose transition cannot be ordered with the native trace is immediately released');
});

const fixedFocusedSensitivityRows = focusTraceRows([
  ...fixedKeyboardCalibration,
  neutral(326, 240, { speed: 2, sensitivity: 1 }), // focused 100% consumed
  neutral(326, 240, { mode: 1, speed: 2 }), // 100% pointer-down
  { ...neutral(326, 240, { mode: 1, targetX: 326.15, targetY: 240.2, speed: 2 }),
    afterX: 326.15, afterY: 240.2 },
  neutral(326.15, 240.2, { mode: 1, targetX: 326.15, targetY: 240.2, speed: 2 }),
  neutral(326.15, 240.2, { mode: 1, targetX: 326.15, targetY: 240.2, speed: 2 }),
  neutral(326.15, 240.2), neutral(326.15, 240.2), neutral(326.15, 240.2),
  neutral(326.15, 240.2, { speed: 2, sensitivity: 2 }), // focused 200% consumed
  neutral(326.15, 240.2, { mode: 1, speed: 2, sensitivity: 2 }), // 200% pointer-down
  { ...neutral(326.15, 240.2, { mode: 1, targetX: 326.45, targetY: 240.6, speed: 2, sensitivity: 2 }),
    afterX: 326.45, afterY: 240.6 },
  neutral(326.45, 240.6, { mode: 1, targetX: 326.45, targetY: 240.6, speed: 2, sensitivity: 2 }),
  neutral(326.45, 240.6, { mode: 1, targetX: 326.45, targetY: 240.6, speed: 2, sensitivity: 2 }),
  neutral(326.45, 240.6), neutral(326.45, 240.6), neutral(326.45, 240.6),
  neutral(326.45, 240.6, { speed: 2, sensitivity: 3 }), // focused 300% consumed
  neutral(326.45, 240.6, { mode: 1, speed: 2, sensitivity: 3 }), // 300% pointer-down
  { ...neutral(326.45, 240.6, { mode: 1, targetX: 326.9, targetY: 241.2, speed: 2, sensitivity: 3 }),
    afterX: 326.9, afterY: 241.2 },
  neutral(326.9, 241.2, { mode: 1, targetX: 326.9, targetY: 241.2, speed: 2, sensitivity: 3 }),
  neutral(326.9, 241.2, { mode: 1, targetX: 326.9, targetY: 241.2, speed: 2, sensitivity: 3 }),
  neutral(326.9, 241.2), neutral(326.9, 241.2), neutral(326.9, 241.2),
  neutral(326.9, 241.2, { speed: 4, sensitivity: 1 }), // focused input released
  neutral(326.9, 241.2), neutral(326.9, 241.2), neutral(326.9, 241.2),
  neutral(326.9, 241.2, { mode: 1, speed: 4 }), // restored-fast pointer-down
  { ...neutral(326.9, 241.2, { mode: 1, targetX: 332.9, targetY: 249.2, speed: 4 }),
    afterX: 329.3, afterY: 244.4 },
  { ...neutral(329.3, 244.4, { mode: 1, targetX: 332.9, targetY: 249.2, speed: 4 }),
    afterX: 331.7, afterY: 247.6 },
  { ...neutral(331.7, 247.6, { mode: 1, targetX: 332.9, targetY: 249.2, speed: 4 }),
    afterX: 332.9, afterY: 249.2 },
  neutral(332.9, 249.2), neutral(332.9, 249.2), neutral(332.9, 249.2),
  neutral(332.9, 249.2, { mode: 1, speed: 4 }), // restored-fast reverse pointer-down
  { ...neutral(332.9, 249.2, { mode: 1, targetX: 326.9, targetY: 241.2, speed: 4 }),
    afterX: 330.5, afterY: 246 },
  { ...neutral(330.5, 246, { mode: 1, targetX: 326.9, targetY: 241.2, speed: 4 }),
    afterX: 328.1, afterY: 242.8 },
  { ...neutral(328.1, 242.8, { mode: 1, targetX: 326.9, targetY: 241.2, speed: 4 }),
    afterX: 326.9, afterY: 241.2 },
  neutral(326.9, 241.2), neutral(326.9, 241.2), neutral(326.9, 241.2),
],[[18,41]]);

const fixedInputOwnershipRows = [
  ...fixedKeyboardCalibration,
  neutral(326, 240, { sensitivity: 1 }), // owners start with calibrated fast speed
  neutral(326, 240, { mode: 1 }), // primary finger down
  { ...neutral(326, 240, { mode: 1, targetY: 244 }), afterY: 244 },
  neutral(326, 244, { mode: 1, targetY: 244 }), neutral(326, 244, { mode: 1, targetY: 244 }),
  neutral(326, 244, { mode: 1, targetY: 244 }), neutral(326, 244, { mode: 1, targetY: 244 }),
  neutral(326, 244, { mode: 1, targetY: 244 }), neutral(326, 244, { mode: 1, targetY: 244 }),
  { ...neutral(326, 244, { mode: 1, targetY: 246 }), afterY: 246 },
  neutral(326, 246, { mode: 1, targetY: 246 }), neutral(326, 246, { mode: 1, targetY: 246 }),
  neutral(326, 246, { mode: 1, targetY: 246 }), neutral(326, 246, { mode: 1, targetY: 246 }),
  { ...neutral(326, 246, { mode: 1, targetY: 240 }), afterY: 242 },
  { ...neutral(326, 242, { mode: 1, targetY: 240 }), afterY: 240 },
  neutral(326, 240, { mode: 1, targetY: 240 }),
  neutral(326, 240), neutral(326, 240), neutral(326, 240),
  neutral(326, 240, { sensitivity: 1 }), // keyboard + touch ownership controls
  neutral(326, 240, { mode: 1 }), // keyboard remains held as touch captures
  { ...neutral(326, 240, { mode: 1, targetX: 332, targetY: 248 }), afterX: 328.4, afterY: 243.2 },
  { ...neutral(328.4, 243.2, { mode: 1, targetX: 332, targetY: 248 }), afterX: 330.8, afterY: 246.4 },
  { ...neutral(330.8, 246.4, { mode: 1, targetX: 332, targetY: 248 }), afterX: 332, afterY: 248 },
  neutral(332, 248, { mode: 1, targetX: 332, targetY: 248 }), // both owners are active before touch cancel
  { ...neutral(332, 248), afterX: 328 },
  { ...neutral(328, 248), afterX: 324 },
  { ...neutral(324, 248), afterX: 320 },
  neutral(320, 248), neutral(320, 248), neutral(320, 248),
];

const fixedTwoFingerMismatchRows = focusTraceRows([
  ...fixedKeyboardCalibration,
  neutral(326, 240, { sensitivity: 1 }),
  neutral(326, 240, { mode: 1 }),
  { ...neutral(326, 240, { mode: 1, targetY: 244 }), afterY: 244 },
  neutral(326, 244, { mode: 1, targetY: 244 }), neutral(326, 244, { mode: 1, targetY: 244 }),
  neutral(326, 244, { mode: 1, targetY: 244 }), neutral(326, 244, { mode: 1, targetY: 244 }),
  neutral(326, 244, { mode: 1, targetY: 244 }), neutral(326, 244, { mode: 1, targetY: 244 }),
  neutral(326, 244, { mode: 1, targetY: 244, speed: 2 }),
  neutral(326, 244, { mode: 1, targetY: 244, speed: 2 }),
  neutral(326, 244, { mode: 1, targetY: 244, speed: 2 }),
  neutral(326, 244, { mode: 1, targetY: 244, speed: 2 }),
  neutral(326, 244, { mode: 1, targetY: 244, speed: 2 }),
  { ...neutral(326, 244, { mode: 1, targetY: 246, speed: 2 }), afterY: 245 }, // wrong: two-finger target should be 245
],[[23,32]]);

const fixedModeTwoSuccessAndMismatch = focusTraceRows([
  neutral(320, 240), neutral(320, 240), neutral(320, 240), neutral(320, 240), // initial observation and stable window
  neutral(320, 240, { sensitivity: 1 }), // fast control consumed
  { ...neutral(320, 240, { speed: 4 }), afterX: 324 },
  { ...neutral(324, 240, { speed: 4 }), afterX: 328 },
  { ...neutral(328, 240, { speed: 4 }), afterX: 332 },
  neutral(332, 240), neutral(332, 240), neutral(332, 240), // fast key released
  neutral(332, 240), // slow control consumed
  { ...neutral(332, 240, { speed: 2 }), afterX: 330 },
  { ...neutral(330, 240, { speed: 2 }), afterX: 328 },
  { ...neutral(328, 240, { speed: 2 }), afterX: 326 },
  neutral(326, 240), neutral(326, 240), neutral(326, 240), // focused key released
  neutral(326, 240), // 100% control consumed
  neutral(326, 240, { mode: 2 }), // pointer-down consumed without movement
  { ...neutral(326, 240, { mode: 2, targetX: 326.3, targetY: 240.4, sensitivity: 1 }),
    afterX: 326.3, afterY: 240.4 },
  neutral(326.3, 240.4, { mode: 2, targetX: 326.3, targetY: 240.4, sensitivity: 1 }),
  neutral(326.3, 240.4, { mode: 2, targetX: 326.3, targetY: 240.4, sensitivity: 1 }),
  neutral(326.3, 240.4), neutral(326.3, 240.4), neutral(326.3, 240.4), // ordinary up
  neutral(326.3, 240.4, { mode: 2 }), // next pointer-down
  neutral(326.3, 240.4, { mode: 2, targetX: 999, targetY: 999, sensitivity: 1 }), // deliberately wrong target
],[[12,14]]);

const fixedModeOneLimitAndCancel = focusTraceRows([
  neutral(320, 240), neutral(320, 240), neutral(320, 240), neutral(320, 240),
  neutral(320, 240, { sensitivity: 1 }),
  { ...neutral(320, 240, { speed: 4 }), afterX: 324 },
  { ...neutral(324, 240, { speed: 4 }), afterX: 328 },
  { ...neutral(328, 240, { speed: 4 }), afterX: 332 },
  neutral(332, 240), neutral(332, 240), neutral(332, 240),
  neutral(332, 240),
  { ...neutral(332, 240, { speed: 2 }), afterX: 330 },
  { ...neutral(330, 240, { speed: 2 }), afterX: 328 },
  { ...neutral(328, 240, { speed: 2 }), afterX: 326 },
  neutral(326, 240), neutral(326, 240), neutral(326, 240),
  neutral(326, 240), // sensitivity 100% consumed
  neutral(326, 240, { mode: 1 }), // micro pointer-down
  { ...neutral(326, 240, { mode: 1, targetX: 326.3, targetY: 240.4, sensitivity: 1 }),
    afterX: 326.3, afterY: 240.4 },
  neutral(326.3, 240.4, { mode: 1, targetX: 326.3, targetY: 240.4, sensitivity: 1 }),
  neutral(326.3, 240.4, { mode: 1, targetX: 326.3, targetY: 240.4, sensitivity: 1 }),
  neutral(326.3, 240.4), neutral(326.3, 240.4), neutral(326.3, 240.4), // up
  neutral(326.3, 240.4, { mode: 1 }), // diagonal pointer-down
  { ...neutral(326.3, 240.4, { mode: 1, targetX: 332.3, targetY: 248.4, sensitivity: 1 }),
    afterX: 328.7, afterY: 243.6 },
  { ...neutral(328.7, 243.6, { mode: 1, targetX: 332.3, targetY: 248.4, sensitivity: 1 }),
    afterX: 331.1, afterY: 246.8 },
  { ...neutral(331.1, 246.8, { mode: 1, targetX: 332.3, targetY: 248.4, sensitivity: 1 }),
    afterX: 332.3, afterY: 248.4 },
  neutral(332.3, 248.4), neutral(332.3, 248.4), neutral(332.3, 248.4), // up
  neutral(332.3, 248.4, { mode: 1 }), // reverse pointer-down
  { ...neutral(332.3, 248.4, { mode: 1, targetX: 326.3, targetY: 240.4, sensitivity: 1 }),
    afterX: 329.9, afterY: 245.2 },
  { ...neutral(329.9, 245.2, { mode: 1, targetX: 326.3, targetY: 240.4, sensitivity: 1 }),
    afterX: 327.5, afterY: 242.0 },
  { ...neutral(327.5, 242.0, { mode: 1, targetX: 326.3, targetY: 240.4, sensitivity: 1 }),
    afterX: 326.3, afterY: 240.4 },
  neutral(326.3, 240.4), neutral(326.3, 240.4), neutral(326.3, 240.4), // cancel
  neutral(326.3, 240.4, { sensitivity: 2 }), // sensitivity 200% consumed
  neutral(326.3, 240.4, { mode: 1 }), // 200% micro pointer-down
  neutral(326.3, 240.4, { mode: 1, targetX: 999, targetY: 999, sensitivity: 2 }), // deliberately wrong
],[[12,14]]);

const fixedJoystickFocusAndRestore = focusTraceRows([
  neutral(320, 240), neutral(320, 240), neutral(320, 240), neutral(320, 240),
  neutral(320, 240, { sensitivity: 1 }),
  { ...neutral(320, 240, { speed: 4 }), afterX: 324 },
  { ...neutral(324, 240, { speed: 4 }), afterX: 328 },
  { ...neutral(328, 240, { speed: 4 }), afterX: 332 },
  neutral(332, 240), neutral(332, 240), neutral(332, 240),
  neutral(332, 240),
  { ...neutral(332, 240, { speed: 2 }), afterX: 330 },
  { ...neutral(330, 240, { speed: 2 }), afterX: 328 },
  { ...neutral(328, 240, { speed: 2 }), afterX: 326 },
  neutral(326, 240), neutral(326, 240), neutral(326, 240),
  neutral(326, 240), // free joystick baseline controls consumed
  neutral(326, 240, { speed: 2 }), // focus is consumed before nonzero joystick axes
  { ...neutral(326, 240, { mode: 1, targetX: 326.46028016, targetY: 240.1953795, speed: 2 }),
    afterX: 326.46028016, afterY: 240.1953795 },
  { ...neutral(326.46028016, 240.1953795, { mode: 1, targetX: 326.92056032, targetY: 240.390759, speed: 2 }),
    afterX: 326.92056032, afterY: 240.390759 },
  { ...neutral(326.92056032, 240.390759, { mode: 1, targetX: 327.38084048, targetY: 240.58613849, speed: 2 }),
    afterX: 327.38084048, afterY: 240.58613849 },
  neutral(327.38084048, 240.58613849), neutral(327.38084048, 240.58613849), neutral(327.38084048, 240.58613849),
  neutral(327.38084048, 240.58613849, { speed: 2 }), // reverse keeps the focused scale prepared
  { ...neutral(327.38084048, 240.58613849, { mode: 1, targetX: 326.92056032, targetY: 240.390759, speed: 2 }),
    afterX: 326.92056032, afterY: 240.390759 },
  { ...neutral(326.92056032, 240.390759, { mode: 1, targetX: 326.46028016, targetY: 240.1953795, speed: 2 }),
    afterX: 326.46028016, afterY: 240.1953795 },
  { ...neutral(326.46028016, 240.1953795, { mode: 1, targetX: 326, targetY: 240, speed: 2 }),
    afterX: 326, afterY: 240 },
  neutral(326, 240), neutral(326, 240), neutral(326, 240),
  neutral(326, 240, { speed: 4 }), // focus release settles before the fast joystick input
  neutral(326, 240, { mode: 1, targetX: 999, targetY: 999, speed: 4 }), // focus-restored forward deliberately mismatched
],[[12,14],[19,32]]);

test('mode 2 scene schedules calibrated input and rejects a target inconsistent with the command', async () => {
  const adapter = scriptedAdapter(fixedModeTwoSuccessAndMismatch.slice());
  await assert.rejects(runTouchMotionSceneGroup(adapter, {
    mode: 2,
    inputSpace: { width: 640, height: 480 },
  }), error => {
    const evidence = error.touchMotionEvidence;
    assert.ok(evidence, 'assertion failure preserves the scenario evidence');
    assert.ok(evidence.scenarios[0], JSON.stringify(evidence.failure));
    assert.equal(evidence.scenarios[0].label, 'sensitivity-100-micro-up');
    assert.equal(evidence.scenarios[0].status, 'passed');
    assert.equal(evidence.scenarios[0].firstMovementTargetArrival, 0,
      'unlimited movement reaches the independently calculated micro target on tick one');
    assert.deepEqual(evidence.scenarios[0].expectedTarget, { x: 326.3, y: 240.4 });
    assert.equal(evidence.scenarios[1].label, 'sensitivity-100-diagonal-up');
    assert.deepEqual(evidence.scenarios[1].requestedDelta, { x: 6, y: 8 });
    assert.deepEqual(evidence.scenarios[1].expectedTarget, { x: 332.3, y: 248.4 },
      'expected target comes from pointer command and sensitivity, not trace.target');
    assert.match(error.message, /unexpected point before consuming the command/);
    return true;
  });

  const pointers = adapter.log.filter(entry => entry.type === 'pointer').map(entry => entry.action);
  assert.deepEqual(pointers, ['cancel', 'down', 'move', 'up', 'down', 'move']);
  assert.ok(adapter.log.some(entry => entry.type === 'key' && entry.code === 'ArrowRight' && entry.down));
  assert.ok(adapter.log.some(entry => entry.type === 'key' && entry.code === 'ShiftLeft' && entry.down));
  assert.ok(adapter.log.filter(entry => entry.type === 'wait').every(entry => entry.ms === 8));
  assert.equal(adapter.log.some(entry => entry.type === 'launch' || entry.type === 'restart'), false,
    'the scene group reuses the current Runtime without launch/restart operations');
});

test('mode 1 checks fractional follow-through, vector limiting, reverse motion and cancel', async () => {
  const adapter = scriptedAdapter(fixedModeOneLimitAndCancel.slice());
  await assert.rejects(runTouchMotionSceneGroup(adapter, {
    mode: 1,
    inputSpace: { width: 640, height: 480 },
  }), error => {
    const evidence = error.touchMotionEvidence;
    assert.ok(evidence?.scenarios[0], 'assertion failure preserves the scenario evidence');
    assert.deepEqual(evidence.scenarios.slice(0, 3).map(scene => scene.status),
      ['passed', 'passed', 'passed']);
    assert.equal(evidence.scenarios[0].oracle.saturated, 0,
      'a sub-speed micro target is followed without losing its fractional displacement');
    assert.equal(evidence.scenarios[1].oracle.saturated, 2,
      'a 6-8 diagonal is capped on its first two ticks and arrives on the third');
    assert.equal(evidence.scenarios[2].release, 'cancel');
    assert.ok(Math.abs(evidence.scenarios[3].expectedTarget.x - 326.9) < 1e-12 &&
      Math.abs(evidence.scenarios[3].expectedTarget.y - 241.2) < 1e-12,
    '200% target is computed from the command rather than the reported target');
    assert.match(error.message, /unexpected point before consuming the command/);
    return true;
  });
  const actions = adapter.log.filter(entry => entry.type === 'pointer').map(entry => entry.action);
  assert.deepEqual(actions, ['cancel', 'down', 'move', 'up', 'down', 'move', 'up', 'down', 'move', 'cancel',
    'down', 'move']);
});

test('a target-consumed zero-displacement touch tick cannot be hidden as idle prefix', async () => {
  const rows = fixedModeTwoSuccessAndMismatch.slice();
  rows[20] = neutral(326, 240, { mode: 2, targetX: 326.3, targetY: 240.4, sensitivity: 1 });
  rows[21] = { ...neutral(326, 240, { mode: 2, targetX: 326.3, targetY: 240.4, sensitivity: 1 }),
    afterX: 326.3, afterY: 240.4 };
  rows[22] = neutral(326.3, 240.4, { mode: 2, targetX: 326.3, targetY: 240.4, sensitivity: 1 });
  const adapter = scriptedAdapter(rows);
  await assert.rejects(runTouchMotionSceneGroup(adapter, { mode: 2 }), error => {
    const scenario = error.touchMotionEvidence?.scenarios[0];
    assert.ok(scenario, 'the failure keeps the consumed-target scenario evidence');
    assert.equal(scenario.movementRows.length, 3);
    assert.equal(scenario.firstTargetConsumedSequence, scenario.movementRows[0].sequence);
    assert.equal(scenario.firstMovementSequence, scenario.movementRows[1].sequence);
    assert.equal(scenario.movementRows[0].targetX, 326.3);
    assert.equal(scenario.movementRows[0].afterX, 326,
      'first consumed target row is retained even though its position did not yet move');
    assert.match(error.message, /Per-tick touch position X/);
    return true;
  });
});

test('joystick-free follows int16 command targets per tick and restores fast movement after focus', async () => {
  const adapter = scriptedAdapter(fixedJoystickFocusAndRestore.slice());
  await assert.rejects(runTouchMotionSceneGroup(adapter, {
    mode: 'joystick-free',
    inputSpace: { width: 640, height: 480 },
  }), error => {
    const evidence = error.touchMotionEvidence;
    assert.ok(evidence?.scenarios[0], 'joystick assertion preserves all scenario evidence');
    assert.deepEqual(evidence.scenarios.slice(0, 2).map(scene => scene.status), ['passed', 'passed']);
    assert.equal(evidence.scenarios[0].traceMode, 1,
      'free joystick uses native limited movement mode');
    assert.deepEqual(evidence.scenarios[0].input, { joystickX: 7541, joystickY: 3201 });
    assert.equal(evidence.scenarios[0].focused, true);
    assert.equal(evidence.scenarios[0].movementRows.length, 3);
    assert.equal(evidence.scenarios[0].speed, 2,
      'focused joystick uses the independently calibrated keyboard slow speed');
    assert.equal(evidence.scenarios[0].focusReadySpeed, 2,
      'focus is consumed and calibrated before the joystick axes are sent');
    assert.equal(evidence.scenarios[0].expectedTargets[0].x, 326.460280159917);
    assert.equal(evidence.scenarios[1].input.joystickX, -7541,
      'the reverse vector is paired to recenter the player');
    assert.deepEqual(evidence.scenarios[2].input, { joystickX: 6542, joystickY: 4930 });
    assert.equal(evidence.scenarios[2].focused, false);
    assert.equal(evidence.scenarios[2].speed, 4,
      'the first nonfocused joystick action restores calibrated fast movement');
    assert.equal(evidence.scenarios[2].focusReadySpeed, 4,
      'fast movement is confirmed while axes remain neutral before the action');
    assert.ok(Math.abs(evidence.scenarios[2].expectedTargets[0].x - 326.798608355968) < 1e-9);
    assert.match(error.message, /command-derived joystick target X/);
    return true;
  });
  const controls = adapter.log.filter(entry => entry.type === 'controls').map(entry => entry.fields);
  assert.ok(controls.some(fields => fields.joystickX === 7541 && fields.joystickY === 3201 && fields.focusEnabled));
  assert.ok(controls.some(fields => fields.joystickX === -7541 && fields.joystickY === -3201 && fields.focusEnabled));
  assert.ok(controls.some(fields => fields.joystickX === 6542 && fields.joystickY === 4930 && !fields.focusEnabled));
  for (const [x, y, focus] of [[7541, 3201, true], [-7541, -3201, true], [6542, 4930, false]]) {
    const index = controls.findIndex(fields => fields.joystickX === x && fields.joystickY === y &&
      fields.focusEnabled === focus);
    assert.ok(index > 0, 'each vector is preceded by a native focus preparation command');
    assert.equal(controls[index - 1].joystickX, 0);
    assert.equal(controls[index - 1].joystickY, 0);
    assert.equal(controls[index - 1].focusEnabled, focus);
  }
});

test('quantized joystick command targets advance from each independently predicted native position', async () => {
  const rows = fixedJoystickFocusAndRestore.slice();
  rows[20] = { ...neutral(326, 240, { mode: 1, targetX: 326.460280159917,
    targetY: 240.19537949766533, speed: 2 }), afterX: 326.4609375, afterY: 240.1953125 };
  rows[21] = { ...neutral(326.4609375, 240.1953125, { mode: 1, targetX: 326.921217659917,
    targetY: 240.39069199766533, speed: 2 }), afterX: 326.921875, afterY: 240.390625 };
  rows[22] = { ...neutral(326.921875, 240.390625, { mode: 1, targetX: 327.382155159917,
    targetY: 240.58600449766533, speed: 2 }), afterX: 327.3828125, afterY: 240.5859375 };
  rows[23] = neutral(327.3828125, 240.5859375);
  rows[24] = neutral(327.3828125, 240.5859375);
  rows[25] = neutral(327.3828125, 240.5859375);
  rows[26] = neutral(327.3828125, 240.5859375, { speed: 2 });
  rows[27] = { ...neutral(327.3828125, 240.5859375, { mode: 1, targetX: 326.922532340083,
    targetY: 240.39055800233467, speed: 2 }), afterX: 326.921875, afterY: 240.390625 };
  rows[28] = { ...neutral(326.921875, 240.390625, { mode: 1, targetX: 326.461594840083,
    targetY: 240.19524550233467, speed: 2 }), afterX: 326.4609375, afterY: 240.1953125 };
  rows[29] = { ...neutral(326.4609375, 240.1953125, { mode: 1, targetX: 326.000657340083,
    targetY: 239.99993300233467, speed: 2 }), afterX: 326, afterY: 240 };
  rows[30] = neutral(326, 240);
  rows[31] = neutral(326, 240);
  rows[32] = neutral(326, 240);
  focusTraceRows(rows, [[12, 14], [19, 32]]).forEach((row, index) => { rows[index] = row; });
  const adapter = scriptedAdapter(rows);
  const precision = { unitsPerPixel: 128, rounding: 'nearest', scalePlacement: 'before-limit' };
  await assert.rejects(runTouchMotionSceneGroup(adapter, {
    mode: 'joystick-free',
    precision,
    inputSpace: { width: 640, height: 480 },
  }), error => {
    const scenes = error.touchMotionEvidence?.scenarios;
    assert.ok(scenes?.[0], 'quantized failure retains fixed-observation evidence');
    assert.deepEqual(scenes.slice(0, 2).map(scene => scene.status), ['passed', 'passed']);
    assert.deepEqual(scenes[0].expectedTargets, [
      { x: 326.460280159917, y: 240.19537949766533 },
      { x: 326.921217659917, y: 240.39069199766533 },
      { x: 327.382155159917, y: 240.58600449766533 },
    ]);
    assert.deepEqual(scenes[0].movementRows.map(row => ({ x: row.afterX, y: row.afterY })), [
      { x: 326.4609375, y: 240.1953125 },
      { x: 326.921875, y: 240.390625 },
      { x: 327.3828125, y: 240.5859375 },
    ]);
    assert.deepEqual(error.touchMotionEvidence.precision, precision);
    assert.match(error.message, /command-derived joystick target/);
    return true;
  });
});

test('focused direct touch repeats command-derived micro sensitivity at 100, 200 and 300 percent', async () => {
  const adapter = scriptedAdapter(fixedFocusedSensitivityRows.slice());
  const result = await runTouchMotionSceneGroup(adapter, {
    mode: 1,
    scenarioSet: 'focus-sensitivity',
    inputSpace: { width: 640, height: 480 },
  });
  assert.equal(result.checkStatus, 'passed', JSON.stringify(result.evidence?.failure));
  const focused = result.evidence.scenarios.filter(scene => scene.label.startsWith('focused-sensitivity-') &&
    scene.label.endsWith('-micro-up'));
  assert.deepEqual(focused.map(scene => scene.sensitivityPercent), [100, 200, 300]);
  assert.deepEqual(focused.map(scene => scene.expectedTarget), [
    { x: 326.15, y: 240.2 },
    { x: 326.45, y: 240.6 },
    { x: 326.9, y: 241.2 },
  ], 'each target includes the independently calibrated focused movement ratio');
  assert.ok(focused.every(scene => scene.focusFactor === 0.5 && scene.oracle.arrived));
  assert.ok(focused.every(scene => scene.movementRows.every(row => row.speed === 2 && row.sensitivity ===
    scene.sensitivityPercent / 100)), 'focused speed remains independently calibrated at each sensitivity');
  assert.equal(result.evidence.scenarios.at(-1).label, 'focus-restored-fast-diagonal-reverse-cancel');
  assert.equal(result.evidence.scenarios.at(-1).status, 'passed');
  assert.equal(adapter.log.some(entry => entry.type === 'launch' || entry.type === 'restart'), false);
});

test('ABI v2 accepts focused native rows and verifies the real focus bit across the scene', async () => {
  const rows = fixedFocusedSensitivityRows.map((row, index) => ({
    ...row,
    flags: (index >= 12 && index <= 14) || (index >= 18 && index <= 41) ? 5 : 1,
  }));
  const adapter = exposeAbi2(scriptedAdapter(rows));
  const result = await runTouchMotionSceneGroup(adapter, {
    mode: 1,
    scenarioSet: 'focus-sensitivity',
    inputSpace: { width: 640, height: 480 },
  });
  assert.equal(result.checkStatus, 'passed', JSON.stringify(result.evidence?.failure));
  assert.equal(result.evidence.trace.abiVersion, 2);
  assert.ok(result.evidence.scenarios.filter(scene => scene.focused)
    .flatMap(scene => scene.movementRows).every(row => row.abiVersion === 2 && row.flags === 5));
  assert.ok(result.evidence.calibration.slow.rows.every(row => row.flags === 5),
    'focused keyboard calibration confirms native focus independently of touch movement');
});

test('ABI v2 rejects a focused scenario whose native focus bit is missing', async () => {
  const rows = fixedFocusedSensitivityRows.map((row, index) => ({
    ...row,
    flags: (index >= 12 && index <= 14) || (index >= 18 && index <= 41) ? 5 : 1,
  }));
  const firstFocusedMove = rows.findIndex(row => row.mode === 1 && row.speed === 2 &&
    (row.afterX !== row.beforeX || row.afterY !== row.beforeY));
  assert.ok(firstFocusedMove >= 0);
  rows[firstFocusedMove].flags = 1;
  const adapter = exposeAbi2(scriptedAdapter(rows));
  await assert.rejects(runTouchMotionSceneGroup(adapter, {
    mode: 1,
    scenarioSet: 'focus-sensitivity',
  }), error => {
    assert.match(error.message, /native focus state differs/i);
    assert.ok(error.touchMotionEvidence?.scenarios[0]);
    return true;
  });
});

test('ABI v2 rejects unknown eligibility flags before sending game input', async () => {
  const adapter = exposeAbi2(scriptedAdapter([]), sample => ({ ...sample, flags: 7 }));
  adapter.samples.push({ sequence: 1, frame: 1, ...neutral(320, 240, { mode: 0 }), flags: 7 });
  const result = await runTouchMotionSceneGroup(adapter, { mode: 1 });
  assert.equal(result.checkStatus, 'incomplete');
  assert.match(result.reason, /Native gameplay eligibility changed at sequence 1/);
  assert.equal(result.evidence.failure.latestTraceRows.at(-1).flags, 7,
    'the disqualifying native row is retained with the evidence');
  assert.equal(adapter.log.length, 0,
    'invalid trace flags are rejected before neutralization commands touch the Runtime');
});

test('strict scenes report legacy ABI v1 before sending game input', async () => {
  const adapter = scriptedAdapter([neutral(320, 240)]);
  adapter.samples.push({ sequence: 1, frame: 1, ...neutral(320, 240, { mode: 0 }), flags: 5 });
  const read=adapter.readTrace.bind(adapter);
  adapter.readTrace=async()=>{
    const trace=await read();
    return{schema:trace.schema,total:trace.total,samples:trace.samples.map(({abiVersion,localSeat,executionKind,...sample})=>sample)};
  };
  const result = await runTouchMotionSceneGroup(adapter, { mode: 1 });
  assert.equal(result.checkStatus, 'incomplete');
  assert.equal(result.reason, 'Strict touch motion requires native ABI v2; package exposes ABI v1');
  assert.equal(result.evidence.trace.abiVersion, 1);
  assert.equal(result.evidence.failure.latestTraceRows.at(-1).flags, 5);
  assert.equal(adapter.log.length, 0);
});

test('multipointer ownership and keyboard owner survive a per-pointer cancel in one Runtime', async () => {
  const adapter = scriptedAdapter(fixedInputOwnershipRows.slice());
  const result = await runTouchMotionSceneGroup(adapter, {
    mode: 1,
    scenarioSet: 'input-ownership',
    focusMode: 'hold-button',
    inputSpace: { width: 640, height: 480 },
  });
  assert.equal(result.checkStatus, 'passed', JSON.stringify(result.evidence?.failure));
  const [multipointer, keyboard] = result.evidence.scenarios;
  assert.equal(multipointer.status, 'passed');
  assert.equal(multipointer.focusFactor, 1);
  assert.equal(multipointer.secondaryDownRows.length, 2);
  assert.equal(multipointer.secondaryMoveRows.length, 2);
  assert.equal(multipointer.secondaryUpRows.length, 2);
  assert.ok([...multipointer.secondaryDownRows, ...multipointer.secondaryMoveRows,
    ...multipointer.secondaryUpRows].every(row => row.targetX === 326 && row.afterX === 326));
  assert.deepEqual(multipointer.primaryWithSecondExpectedTarget, { x: 326, y: 246 });
  assert.equal(multipointer.observations.at(-1).oracle.arrived, true);
  assert.equal(keyboard.status, 'passed');
  assert.equal(keyboard.touchDrag.release, 'cancel');
  assert.equal(keyboard.mixedOwnerRows.length, 1);
  assert.equal(keyboard.mixedOwnerRows[0].mode, 1,
    'both physical keyboard and touch owners are live before touch cancellation');
  assert.equal(keyboard.keyboardRows.length, 3);
  assert.ok(keyboard.keyboardRows.every(row => row.mode === 0 && row.afterX < row.beforeX),
    'post-cancel movement is caused by the still-held independently calibrated keyboard owner');
  const keyDownIndex = adapter.log.findIndex(entry => entry.type === 'key' && entry.down &&
    entry.code === keyboard.direction);
  const cancelIndex = adapter.log.findIndex((entry, index) => index > keyDownIndex && entry.type === 'pointer' &&
    entry.action === 'cancel');
  assert.ok(keyDownIndex >= 0 && cancelIndex > keyDownIndex,
    'keyboard is held before the touch pointer is cancelled');
  assert.equal(adapter.log.some(entry => entry.type === 'launch' || entry.type === 'restart'), false,
    'both ownership scenarios reuse the current Runtime');
});

test('th08 normal key takeover is checked at both queued boundaries including cancel first movement',async()=>{
  const fixture=th08TakeoverRows();
  const adapter=scriptedAdapter(fixture.rows.slice());
  const result=await runTh08TakeoverScene(adapter,{mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',
    inputSpace:{width:640,height:480}});
  assert.equal(result.checkStatus,'passed',JSON.stringify(result.evidence?.failure));
  const [,keyboard]=result.evidence.scenarios;
  assert.equal(keyboard.direction,'ArrowLeft');
  assert.equal(keyboard.directionTakeover.firstMovement.mode,0,
    'the first movement consumes the pre-keydown neutral queue after arrows clear touch motion');
  assert.equal(keyboard.directionTakeover.firstMovement.targetX,keyboard.directionTakeover.firstMovement.beforeX,
    'the trace target remains the neutral touch target while the keyboard action is queued');
  assert.deepEqual({x:keyboard.directionTakeover.firstMovement.beforeX,y:keyboard.directionTakeover.firstMovement.beforeY},
    {x:keyboard.directionTakeover.firstMovement.afterX,y:keyboard.directionTakeover.firstMovement.afterY},
    'the takeover tick has no movement');
  assert.equal(keyboard.directionTakeover.afterFirstMovementInput.value&192,64,
    'the completed takeover tick publishes only the requested ArrowLeft input');
  assert.equal(keyboard.touchDrag.cancelFocusQueueBefore.value&192,64,
    'the ArrowLeft queue is present immediately before touch cancellation');
  assert.equal(keyboard.cancelFirstKeyboardMovement.sequence,keyboard.directionTakeover.firstMovement.sequence+1,
    'the cancel receipt row is retained as the first keyboard movement row');
  assert.equal(keyboard.cancelFirstKeyboardMovement.beforeX-keyboard.cancelFirstKeyboardMovement.afterX,4,
    'the first post-cancel row applies the independently calibrated keyboard step');
  assert.equal(keyboard.cancelFirstKeyboardMovement.targetX,keyboard.cancelFirstKeyboardMovement.beforeX,
    'mode-0 trace target remains at the start while after records keyboard movement');
  assert.equal(keyboard.keyboardRows.length,3);
  assert.ok(keyboard.keyboardRows.every(row=>row.mode===0&&row.speed===4&&row.beforeX-row.afterX===4),
    'the cancel receipt and following rows all prove the queued keyboard direction');
  assert.equal(result.evidence.keyboardDrainRows.at(-1).classification,
    'one movement queued at direction-key release acknowledgement');
  assert.equal(result.evidence.keyboardDrainRows.at(-1).movement.x,-4,
    'keyup drains exactly the direction present in the acknowledged native queue');
  assert.equal(result.evidence.keyboardDrainRows.at(-1).nextMovementInput.value&196,0,
    'the post-movement queue clears direction and focus');
  assert.equal(keyboard.neutralRows.length,3,'three stable neutral rows follow the one queued release movement');
});

test('th08 normal takeover fails when a later cancel movement loses direction or restores focus',async()=>{
  for(const [nextInput,expected] of [[0,/held keyboard direction disappeared/],[68,/focus returned/]]){
    const fixture=th08TakeoverRows();
    fixture.rows[fixture.baseCount+29]=keyboardMovement(330,248,326,{speed:4,nextInput});
    await assert.rejects(runTh08TakeoverScene(scriptedAdapter(fixture.rows.slice()),{
      mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',inputSpace:{width:640,height:480}}),error=>{
      assert.equal(error.name,'AssertionError','a contradictory observed queue is a failed behavior assertion');
      assert.match(error.message,expected);
      return true;
    });
  }
});

test('th08 normal takeover needs valid boundary evidence and fails on a contradictory observed queue',async()=>{
  const missing=th08TakeoverRows();
  const adapter=scriptedAdapter(missing.rows.slice());
  const read=adapter.readTrace.bind(adapter);
  adapter.readTrace=async()=>{
    const trace=await read();
    return trace.total===missing.baseCount+29?{...trace,nextMovementInputBoundary:null}:trace;
  };
  const incomplete=await runTh08TakeoverScene(adapter,{mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',
    inputSpace:{width:640,height:480}});
  assert.equal(incomplete.checkStatus,'incomplete');
  assert.match(incomplete.reason,/stable native input observation/);

  const contradictory=th08TakeoverRows();
  contradictory.rows[contradictory.baseCount+29]=keyboardMovement(330,248,326,{speed:4,nextInput:68});
  await assert.rejects(runTh08TakeoverScene(scriptedAdapter(contradictory.rows.slice()),{
    mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',inputSpace:{width:640,height:480}}),error=>{
    assert.equal(error.name,'AssertionError');
    assert.match(error.message,/focus returned while the keyboard owner remained held/);
    return true;
  });
});

test('th08 normal takeover rejects missing or contradictory requested direction publication',async()=>{
  for(const queuedValue of [0,64|128]){
    const fixture=th08TakeoverRows();
    fixture.rows[fixture.baseCount+27]=queuedAfter(neutral(334,248,{speed:4}),queuedValue);
    await assert.rejects(runTh08TakeoverScene(scriptedAdapter(fixture.rows.slice()),{
      mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',inputSpace:{width:640,height:480}}),error=>{
      assert.match(error.message,/direction key was not published/);
      return true;
    });
  }
});

test('th08 normal takeover rejects cancel clearing the held keyboard direction',async()=>{
  const fixture=th08TakeoverRows();
  fixture.rows[fixture.baseCount+28]=queuedAfter(fixture.rows[fixture.baseCount+28],0);
  await assert.rejects(runTh08TakeoverScene(scriptedAdapter(fixture.rows.slice()),{
    mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',inputSpace:{width:640,height:480}}),error=>{
    assert.match(error.message,/queued direction masks changed across a touch-focus transition/);
    const keyboard=error.touchMotionEvidence.scenarios[1];
    assert.equal(keyboard.touchDrag.cancelFocusQueueTransition.firstMovement.sequence,fixture.baseCount+29);
    return true;
  });
});

test('th08 normal takeover rejects a stationary, reversed, or drifting cancel first movement',async()=>{
  const mutations=[
    row=>({...row,targetX:row.beforeX,afterX:row.beforeX}),
    row=>({...row,targetX:row.beforeX-3.5,afterX:row.beforeX-3.5}),
    row=>({...row,targetX:row.beforeX+4,afterX:row.beforeX+4}),
  ];
  for(const mutate of mutations){
    const fixture=th08TakeoverRows();
    fixture.rows[fixture.baseCount+28]=mutate(fixture.rows[fixture.baseCount+28]);
    await assert.rejects(runTh08TakeoverScene(scriptedAdapter(fixture.rows.slice()),{
      mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',inputSpace:{width:640,height:480}}),error=>{
      assert.match(error.message,/keyboard-owned X movement/);
      return true;
    });
  }
  const drift=th08TakeoverRows();
  drift.rows[drift.baseCount+32]={...drift.rows[drift.baseCount+32],afterX:314};
  await assert.rejects(runTh08TakeoverScene(scriptedAdapter(drift.rows.slice()),{
    mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',inputSpace:{width:640,height:480}}),error=>{
    assert.match(error.message,/position drift X/);
    return true;
  });
});

test('th08 normal takeover requires the observed queue after keydown and clears it after keyup movement',async()=>{
  const missing=th08TakeoverRows();
  const missingAdapter=scriptedAdapter(missing.rows.slice());
  const read=missingAdapter.readTrace.bind(missingAdapter);
  missingAdapter.readTrace=async()=>{
    const trace=await read();
    return trace.total===missing.baseCount+28?{...trace,nextMovementInputBoundary:null}:trace;
  };
  const incomplete=await runTh08TakeoverScene(missingAdapter,{mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',
    inputSpace:{width:640,height:480}});
  assert.equal(incomplete.checkStatus,'incomplete');
  assert.match(incomplete.reason,/stable native input observation/);

  const beforeCancel=th08TakeoverRows();
  const beforeCancelAdapter=scriptedAdapter(beforeCancel.rows.slice());
  const readBeforeCancel=beforeCancelAdapter.readTrace.bind(beforeCancelAdapter);
  let sawTakeoverBoundary=false;
  beforeCancelAdapter.readTrace=async()=>{
    const trace=await readBeforeCancel();
    if(trace.total===beforeCancel.baseCount+28){
      if(!sawTakeoverBoundary){sawTakeoverBoundary=true;return trace;}
      return{...trace,nextMovementInputBoundary:null};
    }
    return trace;
  };
  const missingBeforeCancel=await runTh08TakeoverScene(beforeCancelAdapter,{mode:1,scenarioSet:'input-ownership',
    focusMode:'two-finger',inputSpace:{width:640,height:480}});
  assert.equal(missingBeforeCancel.checkStatus,'incomplete');
  assert.match(missingBeforeCancel.reason,/stable native input observation/);
  const keyDown=beforeCancelAdapter.log.findIndex(entry=>entry.type==='key'&&entry.down&&entry.code==='ArrowLeft');
  assert.ok(keyDown>=0);
  assert.equal(beforeCancelAdapter.log.some((entry,index)=>index>keyDown&&entry.type==='pointer'&&entry.action==='cancel'),false,
    'missing pre-cancel direction evidence stops before releasing the touch pointer');

  const uncleared=th08TakeoverRows();
  uncleared.rows[uncleared.baseCount+31]=queuedAfter(uncleared.rows[uncleared.baseCount+31],64);
  await assert.rejects(runTh08TakeoverScene(scriptedAdapter(uncleared.rows.slice()),{
    mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',inputSpace:{width:640,height:480}}),error=>{
    assert.match(error.message,/released direction or focus remained/);
    return true;
  });
});

test('two-finger focus uses the independent keyboard ratio and rejects an unmapped target', async () => {
  const adapter = scriptedAdapter(fixedTwoFingerMismatchRows.slice());
  await assert.rejects(runTouchMotionSceneGroup(adapter, {
    mode: 1,
    scenarioSet: 'input-ownership',
    focusMode: 'two-finger',
    inputSpace: { width: 640, height: 480 },
  }), error => {
    const multipointer = error.touchMotionEvidence?.scenarios[0];
    assert.ok(multipointer, 'the trace failure retains two-finger evidence');
    assert.equal(multipointer.focusFactor, 0.5,
      'two-finger target scale comes from the independently calibrated keyboard speed ratio');
    assert.deepEqual(multipointer.primaryWithSecondExpectedTarget, { x: 326, y: 245 });
    assert.match(error.message, /native target changed before consuming the pointer command/);
    return true;
  });
});

test('two-finger focus follows the published pre-action and post-first-tick input masks', async () => {
  const fixture=twoFingerQueuedRows();
  const adapter=scriptedAdapter(fixture.rows.slice());
  const result=await runQueuedScene(adapter,{mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',
    inputSpace:{width:640,height:480}});
  assert.equal(result.checkStatus,'passed',JSON.stringify(result.evidence?.failure));
  const [multipointer,keyboard,keyboardTouchDrag,focusedCancel]=result.evidence.scenarios;
  assert.equal(multipointer.secondaryDownInputBefore.value & 4,0);
  assert.equal(multipointer.secondaryDownFirstTick.sample.flags & 4,0,
    'the first row consumes the focus state published before the second finger joined');
  assert.equal(multipointer.secondaryDownFirstTick.sample.speed,4);
  assert.equal(multipointer.secondaryDownFirstTick.nextMovementInput.value & 4,4,
    'the first row publishes focus for the following player movement');
  assert.ok(multipointer.secondaryDownRows.slice(1).every(row=>(row.flags&4)!==0&&row.speed===2),
    'the next player movements use the newly published focus state and calibrated slow speed');
  assert.equal(multipointer.secondaryUpFirstTick.sample.flags & 4,4);
  assert.equal(multipointer.secondaryUpFirstTick.nextMovementInput.value & 4,0,
    'lifting the focus finger publishes the cleared state for the following movement');
  assert.ok(multipointer.secondaryUpRows.slice(1).every(row=>(row.flags&4)===0&&row.speed===4));
  assert.equal(keyboard.keyboardRows.length,3,
    'canceling the touch owner preserves and verifies the separately queued keyboard direction');
  assert.equal(keyboardTouchDrag.label,keyboard.label);
  assert.equal(focusedCancel.cancelFocusQueueTransition.preActionInput.value&4,4,
    'focused cancellation begins with focus in the native queue');
  assert.equal(focusedCancel.cancelFocusQueueTransition.firstMovementConsumedFocus,true);
  assert.equal(focusedCancel.cancelFocusQueueTransition.firstMovement.flags&4,4,
    'the first post-cancel row reports the focus state actually queued before cancel');
  assert.equal(focusedCancel.cancelFocusQueueTransition.afterFirstMovementInput.value&4,0,
    'the completed cancel row publishes focus off for the next movement');
  assert.equal(focusedCancel.releaseRows.length,3);
  assert.ok(focusedCancel.releaseRows.every(row=>row.beforeX===row.afterX&&row.beforeY===row.afterY),
    'focus-state latency never permits touch position drift after cancel');
});

test('two-finger focus fails when the queue never updates after the first stale-focus tick', async () => {
  const fixture=twoFingerQueuedRows();
  fixture.rows[fixture.baseCount+5]=queuedAfter(fixture.rows[fixture.baseCount+5],0);
  const adapter=scriptedAdapter(fixture.rows.slice());
  await assert.rejects(runQueuedScene(adapter,{mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',
    inputSpace:{width:640,height:480}}),error=>{
    assert.match(error.message,/next native movement queue has the wrong focus mask/);
    assert.equal(error.touchMotionEvidence.scenarios[0].secondaryDownFirstTick.sample.sequence,
      fixture.baseCount+6);
    return true;
  });
});

test('two-finger focus fails if the following movement still reports the old focus state', async () => {
  const fixture=twoFingerQueuedRows();
  fixture.rows[fixture.baseCount+6]={...fixture.rows[fixture.baseCount+6],flags:1,speed:4};
  const adapter=scriptedAdapter(fixture.rows.slice());
  await assert.rejects(runQueuedScene(adapter,{mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',
    inputSpace:{width:640,height:480}}),error=>{
    assert.match(error.message,/native focus state differs/);
    assert.equal(error.touchMotionEvidence.scenarios[0].secondaryDownFirstTick.nextMovementInput.value&4,4);
    return true;
  });
});

test('focused touch cancel fails when its queued focus state never clears', async () => {
  const fixture=twoFingerQueuedRows();
  fixture.rows[fixture.baseCount+40]=queuedAfter(fixture.rows[fixture.baseCount+40],4);
  const adapter=scriptedAdapter(fixture.rows.slice());
  await assert.rejects(runQueuedScene(adapter,{mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',
    inputSpace:{width:640,height:480}}),error=>{
    const cancel=error.touchMotionEvidence.scenarios[3].cancelFocusQueueTransition;
    assert.equal(cancel.preActionInput.value&4,4);
    assert.equal(cancel.firstMovement.flags&4,4);
    assert.match(error.message,/next native movement queue has the wrong focus mask/);
    return true;
  });
});

test('focused touch cancel fails when another movement still consumes stale focus', async () => {
  const fixture=twoFingerQueuedRows();
  fixture.rows[fixture.baseCount+41]={...fixture.rows[fixture.baseCount+41],flags:5,speed:2};
  const adapter=scriptedAdapter(fixture.rows.slice());
  await assert.rejects(runQueuedScene(adapter,{mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',
    inputSpace:{width:640,height:480}}),error=>{
    const cancel=error.touchMotionEvidence.scenarios[3].cancelFocusQueueTransition;
    assert.equal(cancel.afterFirstMovementInput.value&4,0);
    assert.match(error.message,/native focus state differs/);
    return true;
  });
});

test('touch release drift and contradictory queued directions remain hard failures', async () => {
  const drift=twoFingerQueuedRows();
  drift.rows[drift.baseCount+19]={...drift.rows[drift.baseCount+19],afterY:240.1};
  await assert.rejects(runQueuedScene(scriptedAdapter(drift.rows.slice()),{mode:1,scenarioSet:'input-ownership',
    focusMode:'two-finger',inputSpace:{width:640,height:480}}),error=>{
    assert.match(error.message,/position drift/i);
    return true;
  });

  const mismatch=twoFingerQueuedRows();
  mismatch.rows[mismatch.baseCount+5]=queuedAfter(mismatch.rows[mismatch.baseCount+5],4|64);
  await assert.rejects(runQueuedScene(scriptedAdapter(mismatch.rows.slice()),{mode:1,scenarioSet:'input-ownership',
    focusMode:'two-finger',inputSpace:{width:640,height:480}}),error=>{
    assert.match(error.message,/queued direction masks changed/);
    return true;
  });

  const cancelDrift=twoFingerQueuedRows();
  cancelDrift.rows[cancelDrift.baseCount+40]={...cancelDrift.rows[cancelDrift.baseCount+40],afterX:321.1};
  await assert.rejects(runQueuedScene(scriptedAdapter(cancelDrift.rows.slice()),{mode:1,scenarioSet:'input-ownership',
    focusMode:'two-finger',inputSpace:{width:640,height:480}}),error=>{
    assert.match(error.message,/position drift \(afterX\)/);
    return true;
  });
});

test('touch-focus transition is incomplete when its action-boundary observation disappears', async () => {
  const fixture=twoFingerQueuedRows();
  const adapter=scriptedAdapter(fixture.rows.slice());
  const pointer=adapter.pointer.bind(adapter),readTrace=adapter.readTrace.bind(adapter);
  let downCount=0,hideBoundary=false;
  adapter.pointer=async(type,...args)=>{
    await pointer(type,...args);
    if(type==='down'&&++downCount===2)hideBoundary=true;
  };
  adapter.readTrace=async()=>{
    const trace=await readTrace();
    return hideBoundary?{...trace,nextMovementInputBoundary:null}:trace;
  };
  const result=await runQueuedScene(adapter,{mode:1,scenarioSet:'input-ownership',focusMode:'two-finger',
    inputSpace:{width:640,height:480}});
  assert.equal(result.checkStatus,'incomplete');
  assert.match(result.reason,/stable native input observation/);
  assert.equal(result.evidence.scenarios[0].secondaryDownInputBefore.value&4,0);
});

test('ABI v2 rejects replay or resimulation touch rows', async () => {
  const adapter = exposeAbi2(scriptedAdapter([neutral(320, 240)]));
  const read = adapter.readTrace.bind(adapter);
  adapter.readTrace = async () => {
    const trace = await read();
    return { ...trace, samples: trace.samples.map(sample => ({ ...sample, executionKind: 1 })) };
  };
  await assert.rejects(runTouchMotionSceneGroup(adapter, { mode: 1 }), error => {
    assert.match(error.message, /Touch trace row executionKind must be forward \(0\); replay\/resimulation rows are excluded/);
    assert.ok(error.touchMotionEvidence);
    return true;
  });
});

test('ABI v2 rejects switching the local-seat trace stream mid-scene', async () => {
  const adapter = scriptedAdapter([neutral(320, 240)]);
  const read = adapter.readTrace.bind(adapter);
  let reads = 0;
  adapter.readTrace = async () => {
    const trace = await read();
    const localSeat = reads++ === 0 ? 0 : 1;
    return {
      ...trace,
      abiVersion: 2,
      localSeat,
      executionKind: 0,
      samples: trace.samples.map(sample => ({ ...sample, abiVersion: 2, localSeat, executionKind: 0 })),
    };
  };
  await assert.rejects(runTouchMotionSceneGroup(adapter, { mode: 1 }), error => {
    assert.match(error.message, /Touch trace changed local seat during the scene/);
    assert.ok(error.touchMotionEvidence);
    return true;
  });
});

test('missing native trace is incomplete and does not issue gameplay commands', async () => {
  let calls = 0;
  const result = await runTouchMotionSceneGroup({
    async readTrace() { return { available: false, reason: 'fixture has no export' }; },
    async controls() { calls++; },
    async pointer() { calls++; },
    async key() { calls++; },
    async wait() { calls++; },
  }, { mode: 1 });
  assert.equal(result.checkStatus, 'incomplete');
  assert.match(result.reason, /no export/);
  assert.equal(calls, 0);
});

test('missing adapter interface is rejected with evidence before dispatch', async () => {
  let reads = 0;
  await assert.rejects(runTouchMotionSceneGroup({
    async readTrace() {
      reads++;
      return { schema: TOUCH_TRACE_SCHEMA, total: 0, samples: [] };
    },
  }, { mode: 1 }), error => {
    assert.match(error.message, /requires controls\(\)/);
    assert.ok(error.touchMotionEvidence);
    return true;
  });
  assert.equal(reads, 0, 'adapter shape is validated before any trace or game interaction');
});

test('Pointer command recording preserves missing-interface validation before any IO',async()=>{
  let calls=0;
  await assert.rejects(runTouchMotionSceneGroup({readTrace:async()=>{calls++;},controls:async()=>{calls++;},
    key:async()=>{calls++;},wait:async()=>{calls++;}}, {mode:1}),/requires pointer\(\)/);
  assert.equal(calls,0);
});
