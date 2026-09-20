import test from 'node:test';
import assert from 'node:assert/strict';
import {PresentationLabControllerCore} from './controller-core.mjs';

function fixture() {
  const memory = new WebAssembly.Memory({initial: 1});
  let ticks = 0, draws = 0, stopped = 0, fault = false;
  const core = {
    memory, allocate: () => 16, deallocate: () => {}, sdl_key: () => {}, sdl_keys_clear: () => {},
    sdl_loop_start: () => {}, sdl_loop_stop: () => { stopped++; },
    sdl_loop_tick: () => { ticks++; return 0; },
  };
  const adapter = {
    scanSchema: 'test/scan/1', sessionSchema: 'test/session/1',
    validate: () => {}, enable: () => {}, records: (_core, reference) => ({tick: ticks, reference, records: []}),
    state: () => [ticks], timing: () => ({ticks}), trace: () => [ticks], worldFrozen: () => false,
    setFault: (_core, value) => { fault = value; },
    draw: (_core, alpha) => { assert.equal(ticks, 0); assert.ok(alpha >= 0 && alpha <= 1); draws++; return 0; },
    gate: () => true, picture: (_runtime, _frame, alpha) => ({alpha}), scene: () => ({ticks}),
    analyze: input => ({schema: 'test/audit/1', tick: input.current.tick, objects: [], purity: true}),
    compact: report => report, mergeIssues: () => [],
  };
  const controller = new PresentationLabControllerCore({core, app: 1}, {wasm: 'fixture'}, adapter);
  return {controller, counts: () => ({ticks, draws, stopped, fault})};
}

test('alpha preview is draw-only and restores the negative control', () => {
  const {controller, counts} = fixture();
  controller.preview(.5, {negativeControl: true});
  assert.deepEqual(counts(), {ticks: 0, draws: 1, stopped: 1, fault: false});
});

test('fast stepping executes every fixed tick', () => {
  const {controller, counts} = fixture();
  controller.step(60);
  assert.equal(counts().ticks, 60);
  assert.throws(() => controller.step(0), /1\.\.600/);
});

test('sweep samples stable alpha endpoints without advancing simulation', () => {
  const {controller, counts} = fixture();
  const report = controller.sweep({images: true});
  assert.equal(counts().ticks, 0);
  assert.equal(counts().draws, 8);
  assert.equal(report.images.length, 5);
});
