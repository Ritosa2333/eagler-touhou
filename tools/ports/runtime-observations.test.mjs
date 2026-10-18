import test from 'node:test';
import assert from 'node:assert/strict';
import {assertRuntimeObservation, readRuntimeObservation,runtimeScenarioResult,REQUIRED_RUNTIME_CHECKS} from './runtime-observations.mjs';

test('observed actions and missing required behaviors cannot produce an overall passing Runtime',()=>{
  const checks=REQUIRED_RUNTIME_CHECKS.map(id=>({id,status:'passed'}));
  assert.equal(runtimeScenarioResult({checks}).passed,true);
  const partial=runtimeScenarioResult({checks:checks.filter(check=>check.id!=='native-pause-and-restart')});
  assert.equal(partial.status,'incomplete');assert.equal(partial.passed,false);
  assert.deepEqual(partial.unverifiedChecks,['native-pause-and-restart']);
  assert.equal(runtimeScenarioResult({checks:[]}).passed,false);
  for(const id of ['hosted-keyboard-native-effect','native-gamepad-effect']) {
    const sampled=runtimeScenarioResult({checks:checks.map(check=>check.id===id?{...check,status:'observed'}:check)});
    assert.equal(sampled.status,'incomplete');assert.deepEqual(sampled.unverifiedChecks,[id]);
  }
  assert.equal(runtimeScenarioResult({checks:[...checks,{id:'additional',status:'failed'}]}).status,'failed');
});

function withWindow(value, operation) {
  const previous = globalThis.window;
  globalThis.window = value;
  try { return operation(); }
  finally {
    if (previous === undefined) delete globalThis.window;
    else globalThis.window = previous;
  }
}

function wasmFixture() {
  const buffer = new ArrayBuffer(4096), view = new DataView(buffer);
  return {
    buffer, view, memory: {buffer},
    int32(pointer, values) { values.forEach((value, index) => view.setInt32(pointer + index * 4, value, true)); },
    uint32(pointer, values) { values.forEach((value, index) => view.setUint32(pointer + index * 4, value, true)); },
    float32(pointer, values) { values.forEach((value, index) => view.setFloat32(pointer + index * 4, value, true)); },
  };
}

test('TH08 production exports expose native control/enemy frames, mapped buttons, and position for normal and MP', () => {
  for (const variant of ['normal', 'multiplayer']) {
    const wasm = wasmFixture(), app = 8;
    wasm.uint32(128, [0, 1, 0, 17, 0, 0, 0, 0, 0, 0, 0]);
    wasm.float32(128 + 7 * 4, [40, 80]);
    wasm.int32(512, [0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0]);
    wasm.uint32(768, [0, 0, 0, 0, 0, 22]);
    const core = {memory: wasm.memory, status: (_app, field) => [2, 1, 0, 1, 0, 0, 17, 0, 0, 0, 0][field],
      trace: () => 128, diagnostics: () => 512, sdl_stats: () => 768};
    const observed = withWindow({__th08Runtime: {core, app, status: () => []}}, () => readRuntimeObservation({game: 'th08', variant}));
    assert.equal(observed.frames.enemy, 17);
    assert.equal(observed.frames.control, 17);
    assert.equal(observed.render.presentationFrame, 22);
    assert.deepEqual(observed.position, {x: 40, y: 80});
    assert.equal(observed.input.value, 1);
    assert.equal(observed.pause.active, false);
    assert.throws(() => assertRuntimeObservation(observed, 'phase', {name: 'stage'}), /no read-only observer/);
    assertRuntimeObservation(observed, 'input-held', {mask: 1});
    assertRuntimeObservation({...observed, position: {x: 41, y: 80}}, 'position-moved',
      {before: {x: 40, y: 80}, axis: 'x', direction: 'positive'});
  }
});

test('TH09 live session phase/frame, pause, player position, and presentation are distinct observations', () => {
  const wasm = wasmFixture();
  wasm.int32(512, [70, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0]);
  wasm.float32(640, [12, 34, 1, -1, 0, 0, 0, 0]);
  wasm.uint32(768, [0, 0, 0, 0, 0, 0, 0, 0, 0, 31]);
  const core = {memory: wasm.memory, _th09_probe_status: () => 512, _th09_touch_probe: () => 640, _th09_game_metrics: () => 768};
  const runtime = {core, status: () => ({title: [70], session: [1, 24, 0, 0, 0, 1, 0, 1], touch: [0, 0, 0, 0]})};
  const observed = withWindow({__th09Runtime: runtime}, () => readRuntimeObservation('th09'));
  assert.deepEqual(observed.phase, {value: 1, name: 'match'});
  assert.equal(observed.frames.session, 24);
  assert.equal(observed.frames.runtime, 70);
  assert.deepEqual(observed.position, {x: 12, y: 34});
  assert.equal(observed.pause.active, true);
  assert.equal(observed.render.presentationFrame, 31);
  assert.throws(() => assertRuntimeObservation(observed, 'input-held'), /no read-only observer/);
});

test('TH10 production status and world actor expose native input, player movement, and presentation for both variants', () => {
  for (const variant of ['normal', 'multiplayer']) {
    const wasm = wasmFixture(), actor = 1024;
    wasm.view.setFloat32(actor + 0x3c0, 240, true);
    wasm.view.setFloat32(actor + 0x3c4, 300, true);
    wasm.uint32(2048, [0, 0, 0, 0, 0, 91]);
    const core = {memory: wasm.memory, _application_world: () => 8, _world_actor: () => actor,
      _sdl_stats: () => 2048};
    const runtime = {core, app: 4, status: () => [1, 1, 0, 3, 72, 0, 0, 0, 0, 4]};
    const observed = withWindow({__th10Runtime: runtime}, () => readRuntimeObservation({game: 'th10', variant}));
    assert.equal(observed.input.value, 4);
    assert.deepEqual(observed.position, {x: 240, y: 300});
    assert.equal(observed.render.presentationFrame, 91);
    assert.throws(() => assertRuntimeObservation(observed, 'phase', {name: 'stage'}), /no read-only observer/);
    assertRuntimeObservation(observed, 'input-held', {mask: 4});
    assert.throws(() => assertRuntimeObservation(observed, 'restart-frame-reset', {before: 80}), /restart frame is not observable/);
  }
});

test('TH11 reads raw window.core exports from the actual shell, while MP restart remains explicitly unsupported', () => {
  const core = {th11_phase: () => 2, th11_frame: () => 85};
  const module = {HEAPU8: new Uint8Array(4096)};
  const observed = withWindow({Module: module, core}, () => readRuntimeObservation({game: 'th11', variant: 'normal'}));
  assert.deepEqual(observed.phase, {value: 2, name: 'paused'});
  assert.equal(observed.frames.session, 85);
  assertRuntimeObservation(observed, 'pause-active');
  assertRuntimeObservation({...observed, phase: {value: 1, name: 'stage'}}, 'phase', {name: 'stage'});
  const multiplayer = withWindow({Module: module, core}, () => readRuntimeObservation({game: 'th11', variant: 'multiplayer'}));
  assert.equal(multiplayer.capabilities.restartFrame, null);
  assert.throws(() => assertRuntimeObservation(multiplayer, 'restart-frame-reset', {before: 85}), /restart frame is not observable/);
  assert.throws(() => assertRuntimeObservation(observed, 'position-moved', {before: {x: 1, y: 2}, axis: 'x'}), /no read-only observer/);
  const moduleOnly = {_th11_phase: () => 1, _th11_frame: () => 86};
  const emscripten = withWindow({Module: moduleOnly}, () => readRuntimeObservation({game: 'th11', variant: 'normal'}));
  assert.deepEqual(emscripten.phase, {value: 1, name: 'stage'});
  assert.equal(emscripten.frames.session, 86);
});

test('TH15 phase is semantic but its application frame does not advertise restart or pause freeze', () => {
  const module = {_th15_phase: () => 2, _th15_frame: () => 301};
  const observed = withWindow({Module: module}, () => readRuntimeObservation({game: 'th15', variant: 'normal'}));
  assert.deepEqual(observed.phase, {value: 2, name: 'paused'});
  assert.equal(observed.frames.application, 301);
  assertRuntimeObservation(observed, 'pause-active');
  assert.throws(() => assertRuntimeObservation(observed, 'restart-frame-reset', {before: 300}), /restart frame is not observable/);
  assert.throws(() => assertRuntimeObservation(observed, 'presentation-advanced', {after: 300}), /no read-only observer/);
});

test('TH20 reports native input and presentation without calling its pulse-consuming touch probe', () => {
  const wasm = wasmFixture();
  wasm.uint32(768, [0, 0, 0, 0, 0, 44]);
  let touched = false;
  const core = {memory: wasm.memory, sdl_stats: () => 768, sdl_touch_probe: () => { touched = true; throw Error('must not be called'); }};
  const runtime = {core, status: () => [4, 0, 0, 1, 0, 0, 0, 2, 0, 16]};
  const observed = withWindow({__th20Runtime: runtime}, () => readRuntimeObservation('th20'));
  assert.equal(touched, false);
  assert.equal(observed.status.scene, 4);
  assert.equal(observed.input.value, 16);
  assert.equal(observed.render.presentationFrame, 44);
  assertRuntimeObservation(observed, 'input-held', {mask: 16});
  assert.throws(() => assertRuntimeObservation(observed, 'pause-active'), /no read-only observer/);
  assert.throws(() => assertRuntimeObservation(observed, 'position-moved', {before: {x: 0, y: 0}, axis: 'x'}), /no read-only observer/);
});

test('TH06/07 replay readiness and rendering cannot be mistaken for live gameplay or keyboard behavior', () => {
  const wasm = wasmFixture();
  wasm.uint32(256, [9, 120, 0, 2, 1]);
  const module = {_eagler_replay_ready: () => 1, _eagler_replay_state: () => 256, HEAPU8: new Uint8Array(wasm.buffer)};
  const th06 = withWindow({Module: module}, () => readRuntimeObservation('th06'));
  const th07 = withWindow({Module: module, __eaglerDebugRenderCallbacks: 24}, () => readRuntimeObservation('th07'));
  assert.equal(th06.replay.ready, true);
  assert.deepEqual(th06.replay.state, {frame: 9, total: 120, ended: false, stage: 2, startStage: 1});
  assert.equal(th07.render.renderCallbacks, 24);
  for (const observed of [th06, th07]) {
    assert.throws(() => assertRuntimeObservation(observed, 'phase', {name: 'stage'}), /no read-only observer/);
    assert.throws(() => assertRuntimeObservation(observed, 'input-held'), /no read-only observer/);
    assert.throws(() => assertRuntimeObservation(observed, 'restart-frame-reset', {before: 10}), /restart frame is not observable/);
  }
});

test('negative controls reject missing progress, unchanged position, and unobservable actions', () => {
  const base = {game: 'fixture', capabilities: {phase: true, input: true, position: true, pause: false,
    restartFrame: null, presentationFrame: false}, phase: {name: 'stage'}, input: {value: 0}, frames: {session: 5}, position: {x: 4, y: 8}};
  assert.throws(() => assertRuntimeObservation(base, 'input-held'), /no native input is held/);
  assert.throws(() => assertRuntimeObservation({...base, input:{}}, 'input-held'), /native input state is unavailable/);
  assertRuntimeObservation({...base, input:{value:4}}, 'input-released', {mask:1});
  assert.throws(() => assertRuntimeObservation({...base, input:{value:5}}, 'input-released', {mask:1}), /mask remains held/);
  assert.throws(() => assertRuntimeObservation(base, 'frame-advanced', {key: 'session', before: 5}), /did not advance/);
  assert.throws(() => assertRuntimeObservation(base, 'position-moved', {before: {x: 4, y: 8}, axis: 'x'}), /position did not change/);
  assert.throws(() => assertRuntimeObservation(base, 'pause-active'), /no read-only observer/);
});

test('the browser-side reader has no imported closure dependencies', () => {
  const serialized = new Function(`return (${readRuntimeObservation.toString()})`)();
  const core = {th11_phase: () => 1, th11_frame: () => 12};
  const observed = withWindow({Module: {HEAPU8: new Uint8Array(256)}, core}, () => serialized({game: 'th11', variant: 'normal'}));
  assert.deepEqual(observed.phase, {value: 1, name: 'stage'});
  assert.equal(observed.frames.session, 12);
});
