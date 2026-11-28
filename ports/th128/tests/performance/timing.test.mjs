// Production timing regression; no native World or device execution.
// Scope: DISPLAY-01/02/03/04/05/08 presentation-schedule behaviour. No World, no native core.
import test from 'node:test';
import assert from 'node:assert/strict';
import { FrameStepper, WorldPresentationInterpolator, renderInterpolationPhase } from '../../site/runtime/timing.mjs';

const STEP = 1000 / 60;
const T0 = 1_000_000;

function schedule(hz, seconds) {
  const frames = Math.round(hz * seconds);
  return Array.from({ length: frames + 1 }, (_, i) => T0 + (i * 1000) / hz);
}

function drive(hz, seconds, world) {
  const clock = new FrameStepper();
  const stamps = schedule(hz, seconds);
  const perFrame = [];
  for (const now of stamps) {
    const { steps, stalled } = clock.advance(now, true);
    assert.equal(stalled, false, 'steady schedule must never stall');
    assert.ok(steps >= 0 && steps <= 4, 'step count must stay in the bounded range');
    perFrame.push(steps);
    for (let i = 0; i < steps; i++) world.rngCalls += 1;
    const phase = renderInterpolationPhase(clock);
    assert.ok(phase >= 0 && phase <= 1, 'interpolation phase must stay in [0,1]');
  }
  return { clock, perFrame, steps: world.rngCalls };
}

test('DISPLAY-01 fixed logical rate is identical across 60/120/144/240 Hz schedules', () => {
  const results = {};
  for (const hz of [60, 120, 144, 240]) {
    const world = { rngCalls: 0 };
    results[hz] = drive(hz, 60, world);
  }
  const totals = Object.fromEntries(Object.entries(results).map(([hz, r]) => [hz, r.steps]));
  const values = Object.values(totals);
  assert.ok(Math.min(...values) >= 3597 && Math.max(...values) <= 3603, `one logic minute must stay on 3600 steps (${JSON.stringify(totals)})`);
  assert.ok(Math.max(...values) - Math.min(...values) <= 2, `refresh rate may not change the logical total (${JSON.stringify(totals)})`);
  for (const hz of [120, 144, 240]) {
    const perFrame = results[hz].perFrame;
    assert.ok(perFrame.every((steps) => steps <= 1), `${hz} Hz may never run more than one logic step inside a display frame`);
    assert.ok(perFrame.filter((steps) => steps === 0).length > 0, `${hz} Hz must have presentation-only frames`);
  }
  assert.equal(results[60].perFrame[0], 0, 'the first display frame only primes the clock');
  assert.ok(results[60].perFrame.slice(1).every((steps) => steps === 1), '60 Hz keeps one logic step per frame after priming');
});

test('DISPLAY-02/03 presentation-only frames never advance logic and stay repeatable', () => {
  const clock = new FrameStepper();
  const world = { rngCalls: 0 };
  let presentationOnly = 0;
  for (const now of schedule(120, 5)) {
    const { steps } = clock.advance(now, true);
    const before = world.rngCalls;
    for (let i = 0; i < steps; i++) world.rngCalls += 1;
    if (steps === 0) {
      presentationOnly += 1;
      assert.equal(world.rngCalls, before, 'extra display frames must not consume logic');
    }
    const phase = renderInterpolationPhase(clock);
    assert.equal(renderInterpolationPhase(clock), phase, 'phase sampling is a pure read');
  }
  assert.ok(presentationOnly > 100, '120 Hz over five seconds must include many extra frames');
  assert.ok(world.rngCalls >= 295 && world.rngCalls <= 301, `five logical seconds stay near 300 steps (${world.rngCalls})`);
});

test('DISPLAY-08 the 60 Hz limiter changes presentation only, not logic', () => {
  const clock = new FrameStepper();
  const world = { rngCalls: 0 };
  let presented = 0;
  for (const now of schedule(144, 10)) {
    const { steps } = clock.advance(now, true);
    for (let i = 0; i < steps; i++) world.rngCalls += 1;
    if (steps > 0) presented += 1; // play.mjs presents when a logic step exists while limited
  }
  assert.ok(world.rngCalls >= 597 && world.rngCalls <= 603, `ten logical seconds stay near 600 steps (${world.rngCalls})`);
  assert.ok(presented >= 597 && presented <= 603, 'limiting to the logic rate presents once per logic step');
});

test('stall and pause boundaries keep the accumulator in range', () => {
  const clock = new FrameStepper();
  assert.deepEqual(clock.advance(T0, true), { steps: 0, stalled: false });
  assert.deepEqual(clock.advance(T0 + 250, true), { steps: 4, stalled: false });
  assert.equal(renderInterpolationPhase(clock), 1, 'a saturated frame renders at the new logic state');
  assert.deepEqual(clock.advance(T0 + 250 + 250.5, true), { steps: 0, stalled: true });
  assert.equal(clock.accumulator, 0, 'a stall discards the backlog instead of replaying it');
  assert.equal(renderInterpolationPhase(clock), 0);
  assert.deepEqual(clock.advance(T0 + 400, true), { steps: 0, stalled: true }, 'backwards time stalls');
  assert.throws(() => clock.advance(Number.NaN, true), /Invalid frame timestamp/);

  const paused = new FrameStepper();
  assert.deepEqual(paused.advance(T0, true), { steps: 0, stalled: false });
  assert.deepEqual(paused.advance(T0 + 5000, false), { steps: 0, stalled: false }, 'an inactive render clock resets instead of stalling');
  assert.equal(paused.accumulator, 0);
  assert.deepEqual(paused.advance(T0 + 5008, true), { steps: 0, stalled: false }, 'resume starts a fresh accumulator');
  assert.ok(renderInterpolationPhase(paused) > 0.4 && renderInterpolationPhase(paused) < 0.6);
});

test('interpolation phase rejects foreign clocks', () => {
  assert.throws(() => renderInterpolationPhase({ accumulator: 1, step: STEP }), /Invalid display interpolation clock/);
  const broken = new FrameStepper();
  broken.step = 0;
  assert.throws(() => renderInterpolationPhase(broken), /Invalid display interpolation clock/);
});

function record(id, x, extra = {}) {
  return { id, drawPass: 'player', bank: 0, sprite: 1, x, y: 0, z: 0, ...extra };
}

test('DISPLAY-04 continuous fields interpolate while discrete fields stay on the current frame', () => {
  const history = new WorldPresentationInterpolator();
  assert.equal(history.capture({ generation: 1, stage: 1, frame: 10, records: [record(1, 0, { sprite: 1, width: 8, rotation: 0 })] }), true);
  assert.equal(history.capture({ generation: 1, stage: 1, frame: 11, records: [record(1, 100, { sprite: 1, width: 16, rotation: Math.PI / 2 })] }), true);
  const sample = history.sample(0.5);
  assert.equal(sample.frame, 11);
  assert.equal(sample.records[0].x, 50);
  assert.equal(sample.records[0].width, 16, 'discrete size selection follows the current logic frame');
  assert.ok(Math.abs(sample.records[0].rotation - Math.PI / 4) < 1e-9);
  assert.equal(sample.records[0].presentationInterpolated, true);
  assert.equal(history.sample(1).records[0].x, 100, 'alpha 1 equals the current frame');
  assert.equal(history.sample(0).records[0].x, 0, 'alpha 0 equals the previous frame');
});

test('DISPLAY-03 sampling never mutates captured snapshots and is repeatable', () => {
  const history = new WorldPresentationInterpolator();
  const first = { generation: 2, stage: 3, frame: 0, records: [record(7, 10, { geometry: { kind: 'circle-fan', owner: 'boss', column: 2, vertices: [{ x: 0, y: 0 }] } })] };
  const second = { generation: 2, stage: 3, frame: 1, records: [record(7, 30, { geometry: { kind: 'circle-fan', owner: 'boss', column: 2, vertices: [{ x: 20, y: 0 }] } })] };
  const firstCopy = structuredClone(first);
  const secondCopy = structuredClone(second);
  assert.equal(history.capture(first), true);
  assert.equal(history.capture(second), true);
  assert.equal(history.capture(second), false, 'the same logic frame is captured once');
  const a = history.sample(0.25);
  const b = history.sample(0.25);
  assert.deepEqual(a, b, 'repeated draws for one phase produce the same picture');
  a.records[0].x = -999;
  assert.equal(history.sample(0.25).records[0].x, 15, 'draw-time mutation cannot leak into the history');
  assert.deepEqual(first, firstCopy, 'the captured input snapshot is not modified');
  assert.deepEqual(second, secondCopy);
  assert.equal(history.sample(0.25).records[0].geometry.vertices[0].x, 5);
});

test('DISPLAY-05 births, deaths, teleports and frame gaps never smear', () => {
  const history = new WorldPresentationInterpolator();
  const born = new WorldPresentationInterpolator();
  assert.equal(born.capture({ generation: 1, stage: 1, frame: 0, records: [] }), true);
  assert.equal(born.capture({ generation: 1, stage: 1, frame: 1, records: [record(9, 500)] }), true);
  assert.equal(born.sample(0.5).records[0].x, 500, 'a newly born record uses its current state');

  assert.equal(history.capture({ generation: 1, stage: 2, frame: 0, records: [record(1, 0), record(2, 0)] }), true);
  assert.equal(history.capture({ generation: 1, stage: 2, frame: 1, records: [record(1, 300), record(3, 40)] }), true);
  const sample = history.sample(0.5);
  assert.equal(sample.records[0].x, 300, 'a teleport jump is not interpolated');
  assert.equal(sample.records.length, 2, 'a destroyed record disappears immediately');
  assert.equal(sample.records[1].x, 40);

  const gap = new WorldPresentationInterpolator();
  assert.equal(gap.capture({ generation: 1, stage: 1, frame: 4, records: [record(1, 0)] }), true);
  assert.equal(gap.capture({ generation: 1, stage: 1, frame: 7, records: [record(1, 10)] }), true);
  assert.equal(gap.sample(0.5).records[0].x, 10, 'non-consecutive logic frames are presented as-is');

  const restage = new WorldPresentationInterpolator();
  assert.equal(restage.capture({ generation: 1, stage: 1, frame: 9, records: [record(1, 0)] }), true);
  assert.equal(restage.capture({ generation: 2, stage: 1, frame: 10, records: [record(1, 10)] }), true);
  assert.equal(restage.sample(0.5).records[0].x, 10, 'a new owner generation resets interpolation');
});

test('interpolator validation matches the native presentation contract', () => {
  const history = new WorldPresentationInterpolator();
  assert.throws(() => history.capture(null), /Invalid display presentation snapshot/);
  assert.throws(() => history.capture({ generation: 0, stage: 1, frame: 0, records: [] }), /Invalid display presentation snapshot/);
  assert.throws(() => history.capture({ generation: 1, stage: 17, frame: 0, records: [] }), /Invalid display presentation snapshot/);
  assert.throws(() => history.capture({ generation: 1, stage: 1, frame: -1, records: [] }), /Invalid display presentation snapshot/);
  assert.throws(() => history.capture({ generation: 1, stage: 1, frame: 0, records: [{ id: 1, x: Number.NaN }] }), /Invalid display presentation record/);
  assert.throws(() => history.capture({ generation: 1, stage: 1, frame: 0, records: [{ id: 1, geometry: { vertices: Array.from({ length: 65537 }, () => ({ x: 0, y: 0 })) } }] }), /Invalid display geometry extent/);
  history.capture({ generation: 1, stage: 1, frame: 0, records: [] });
  assert.throws(() => history.sample(-0.1), /Invalid display interpolation phase/);
  assert.throws(() => history.sample(1.1), /Invalid display interpolation phase/);
  assert.equal(new WorldPresentationInterpolator().sample(0.5), null, 'no capture yet yields no sample');
});

test('DISPLAY-04 background meshes interpolate only across a stable camera phase', () => {
  const make = (x, fog = [0, 0, 0, 0, 1, 2]) => [{
    presentationId: 'stage-surface', bank: 0, entry: 1, blend: 0, layer: 0, fog,
    vertices: Array.from({ length: 66 }, (_, i) => (i % 11 === 0 ? x : i % 11 === 3 ? 255 : i % 11 === 4 ? 0.25 : 0)),
  }];
  const history = new WorldPresentationInterpolator();
  assert.equal(history.capture({ generation: 1, stage: 1, frame: 0, records: [], background: make(0) }), true);
  assert.equal(history.capture({ generation: 1, stage: 1, frame: 1, records: [], background: make(100) }), true);
  const sample = history.sample(0.5);
  assert.equal(sample.background[0].vertices[0], 50);
  assert.equal(sample.background[0].vertices[3], 255, 'vertex alpha stays at the current frame');
  assert.equal(sample.background[0].vertices[4], 0.25, 'UV fields are not interpolated');

  const split = new WorldPresentationInterpolator();
  assert.equal(split.capture({ generation: 1, stage: 1, frame: 0, records: [], background: make(0, [0, 0, 0, 0, 1, 2]) }), true);
  assert.equal(split.capture({ generation: 1, stage: 1, frame: 1, records: [], background: make(100, [9, 0, 0, 0, 1, 2]) }), true);
  assert.equal(split.sample(0.5).background[0].vertices[0], 100, 'a changed fog phase snaps instead of blending');
});
