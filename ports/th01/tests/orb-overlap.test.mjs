import {readFile} from 'node:fs/promises';
import assert from 'node:assert/strict';
import {test} from 'node:test';

const module = await WebAssembly.compile(await readFile(new URL('../web/game.wasm', import.meta.url)));
const distance = (core, a, b) => Math.hypot(core.th_get(20, a) - core.th_get(20, b), core.th_get(21, a) - core.th_get(21, b));

function start(count, stage, route) {
  const core = new WebAssembly.Instance(module).exports;
  core.th_init(2, count, 123, 0);
  core.th_set_netplay_rules(1);
  core.th_stage_command(14, 0, stage, route, 0, 0);
  core.th_debug_invincible(1);
  return core;
}

function step(core, frame) {
  const fire = frame % 8 === 0 ? 4 : 0;
  const left = frame % 100 < 50;
  core.th_step((left ? 1 : 2) | fire, (left ? 2 : 1) | fire, 0);
}

test('eight orbs spread after a stacked shot even on Hell stage 19', () => {
  const core = start(8, 19, 1);
  assert.equal(distance(core, 6, 7), 0, 'the launch still starts at one point');
  for (let frame = 0; frame <= 95; frame++) step(core, frame);
  assert.equal(core.th_get(4, 0), 8);
  assert.ok(distance(core, 6, 7) >= 12, 'a pair caught in nearly identical bounces must separate');
});

test('Makai stage 8 orbs separate after teleporting and replay identically across rollback', () => {
  const core = start(9, 8, 0);
  const heap = new Uint8Array(core.memory.buffer);
  const memory = new DataView(core.memory.buffer);
  const hidden = slot => memory.getInt32(4171008 + slot * 96 + 16, true) !== 0;
  const snapshot = heap.slice();
  const run = () => {
    let entered = false;
    let released = false;
    let mergedFrames = 0;
    const checksums = [];
    for (let frame = 0; frame <= 75; frame++) {
      step(core, frame);
      entered ||= hidden(2) && hidden(8);
      if (entered && !hidden(2) && !hidden(8)) {
        released = true;
        if (distance(core, 2, 8) < 12) mergedFrames++;
      }
      checksums.push(core.th_net_checksum());
    }
    assert.equal(entered, true, 'the test must exercise the same portal with both orbs');
    assert.equal(released, true, 'both orbs must come out of the portal');
    assert.ok(mergedFrames <= 12, `orbs visually merged for ${mergedFrames} frames after teleporting`);
    assert.equal(core.th_get(4, 0), 9);
    return checksums;
  };
  const expected = run();
  heap.set(snapshot);
  assert.deepEqual(run(), expected);
});
