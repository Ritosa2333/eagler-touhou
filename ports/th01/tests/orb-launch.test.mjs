import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';

const binary = await readFile(new URL('../web/game.wasm', import.meta.url));
async function launch(players, count) {
  const core = (await WebAssembly.instantiate(binary, {})).instance.exports;
  core.th_init(players, count, 0x12345678, 0);
  core.th_set_netplay_rules(players > 1 ? 1 : 0);
  core.th_step(4, 0, 0);
  return core;
}
function positions(core, count) {
  return Array.from({length: count}, (_, i) => [core.th_get(20, i), core.th_get(21, i)]);
}

test('single-orb initial motion follows the original leftward and upward launch', async () => {
  const core = await launch(1, 1);
  const first = positions(core, 1)[0];
  core.th_step(0, 0, 0);
  const second = positions(core, 1)[0];
  assert.deepEqual([second[0] - first[0], second[1] - first[1]], [-4, -8]);
});

test('all configured orb counts produce identical launch trajectories on both peers', async () => {
  for (const count of [1, 2, 3, 4, 6, 8, 9]) {
    const a = await launch(2, count), b = await launch(2, count);
    for (let frame = 0; frame < 120; frame++) {
      assert.deepEqual(positions(a, count), positions(b, count), `${count} orbs, frame ${frame}`);
      assert.equal(a.th_net_checksum(), b.th_net_checksum(), `${count} orbs, frame ${frame}`);
      a.th_step(0, 0, 0); b.th_step(0, 0, 0);
    }
  }
});

test('launch replays identically after restoring a rollback snapshot', async () => {
  for (const count of [1, 2, 3, 4, 6, 8, 9]) {
    const core = (await WebAssembly.instantiate(binary, {})).instance.exports;
    core.th_init(2, count, 0x12345678, 0);
    core.th_set_netplay_rules(1);
    const heap = new Uint8Array(core.memory.buffer);
    const snapshot = heap.slice();
    const run = () => {
      const trace = [];
      for (let frame = 0; frame < 30; frame++) {
        core.th_step(frame === 0 ? 4 : 0, 0, 0);
        trace.push([positions(core, count), core.th_net_checksum()]);
      }
      return trace;
    };
    const expected = run();
    heap.set(snapshot);
    assert.deepEqual(run(), expected, `${count} orbs`);
  }
});

test('multiple orbs begin at distinct positions and move upward', async () => {
  for (const count of [2, 3, 4, 6, 8, 9]) {
    const core = await launch(2, count);
    const first = positions(core, count);
    core.th_step(0, 0, 0);
    const second = positions(core, count);
    const vectors = first.map(([x, y], i) => [second[i][0] - x, second[i][1] - y]);
    assert(vectors.every(([dx, dy]) => dx < 0 && dy < 0), `${count} orbs move up and left`);
    assert.equal(new Set(first.map(v => v.join(','))).size, count, `${count} distinct positions`);
  }
});

test('floor contact resets the combo for one orb and preserves it for multiple orbs', async () => {
  // Addresses belong to the audited native core; forcing contact tests the real bounce path.
  for (const count of [1, 2, 3, 4, 6, 8, 9]) {
    const core = await launch(2, count);
    const memory = new DataView(core.memory.buffer), orb = 4171008, combo = 2538596;
    memory.setInt32(combo, 17, true);
    memory.setInt32(orb + 8, 367, true);
    memory.setInt32(orb + 52, 367 * 256, true);
    memory.setFloat64(orb + 24, 10, true);
    memory.setInt32(orb + 32, 0, true);
    memory.setInt32(orb + 60, 10 * 256, true);
    core.th_step(0, 0, 0);
    assert.equal(memory.getInt32(combo, true), count === 1 ? 0 : 17, `${count} orbs`);
    core.th_stage_command(14, 0, 2, 0, 0, 0);
    assert.equal(memory.getInt32(combo, true), 0, `${count} orbs reset at stage change`);
  }
});

test('six-orb launch starts in two aligned rows of three', async () => {
  const core = (await WebAssembly.instantiate(binary, {})).instance.exports;
  core.th_init(1, 6, 0x12345678, 0);
  const [a, b, c, d, e, f] = positions(core, 6);
  assert.deepEqual([a[0], b[0], c[0]], [d[0], e[0], f[0]]);
  assert.equal(a[1], b[1]); assert.equal(b[1], c[1]);
  assert.equal(d[1], e[1]); assert.equal(e[1], f[1]);
  assert.equal(d[1] - a[1], 32);
  assert.equal(a[0] - b[0], 32); assert.equal(b[0] - c[0], 32);
});

test('four-orb launch starts in a two by two grid', async () => {
  const core = (await WebAssembly.instantiate(binary, {})).instance.exports;
  core.th_init(1, 4, 0x12345678, 0);
  const [a, b, c, d] = positions(core, 4);
  assert.deepEqual([a[0], b[0]], [c[0], d[0]]);
  assert.equal(a[1], b[1]); assert.equal(c[1], d[1]);
  assert.equal(a[0] - b[0], 32); assert.equal(c[1] - a[1], 32);
});
