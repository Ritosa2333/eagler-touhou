import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core, oracle, memory, string, root, enableNativeMath} from './helpers.mjs';
import {resourceLoader, nativePool, prepareBank} from './anm-oracle.mjs';
import {nativeGraphics} from './rendering-oracle.mjs';
import {assertScreenVertexGridOrEqual} from '../../../../../tools/testing/display-grid-evidence.mjs';

const gridVertices = [
  [127.5, 10.5, 0, 1, 0xffffffff, 0.03125, 0.96875],
  [135.5, 10.5, 0, 1, 0xffffffff, 0.0625, 0.96875],
  [127.5, 18.5, 0, 1, 0xffffffff, 0.03125, 1],
  [135.5, 10.5, 0, 1, 0xffffffff, 0.0625, 0.96875],
  [127.5, 18.5, 0, 1, 0xffffffff, 0.03125, 1],
  [135.5, 18.5, 0, 1, 0xffffffff, 0.0625, 1],
  [135.5, 10.5, 0, 1, 0xffffffff, 0.21875, 0.96875],
  [143.5, 10.5, 0, 1, 0xffffffff, 0.25, 0.96875],
  [135.5, 18.5, 0, 1, 0xffffffff, 0.21875, 1],
  [143.5, 10.5, 0, 1, 0xffffffff, 0.25, 0.96875],
  [135.5, 18.5, 0, 1, 0xffffffff, 0.21875, 1],
  [143.5, 18.5, 0, 1, 0xffffffff, 0.25, 1],
];
const quarterVertices = [
  [127.75, 10.5, 0, 1, 0xffffffff, 0.03125, 0.96875],
  [135.75, 10.5, 0, 1, 0xffffffff, 0.0625, 0.96875],
  [127.75, 18.5, 0, 1, 0xffffffff, 0.03125, 1],
  [135.75, 10.5, 0, 1, 0xffffffff, 0.0625, 0.96875],
  [127.75, 18.5, 0, 1, 0xffffffff, 0.03125, 1],
  [135.75, 18.5, 0, 1, 0xffffffff, 0.0625, 1],
  [135.75, 10.5, 0, 1, 0xffffffff, 0.21875, 0.96875],
  [143.75, 10.5, 0, 1, 0xffffffff, 0.25, 0.96875],
  [135.75, 18.5, 0, 1, 0xffffffff, 0.21875, 1],
  [143.75, 10.5, 0, 1, 0xffffffff, 0.25, 0.96875],
  [135.75, 18.5, 0, 1, 0xffffffff, 0.21875, 1],
  [143.75, 18.5, 0, 1, 0xffffffff, 0.25, 1],
];
const vertexBytes = vertices => {
  const bytes = Buffer.alloc(vertices.length * 28);
  vertices.forEach((vertex, index) => {
    const base = index * 28;
    bytes.writeFloatLE(vertex[0], base);
    bytes.writeFloatLE(vertex[1], base + 4);
    bytes.writeFloatLE(vertex[2], base + 8);
    bytes.writeFloatLE(vertex[3], base + 12);
    bytes.writeUInt32LE(vertex[4] >>> 0, base + 16);
    bytes.writeFloatLE(vertex[5], base + 20);
    bytes.writeFloatLE(vertex[6], base + 24);
  });
  return bytes;
};
const expectedGrid = vertexBytes(gridVertices);
const expectedQuarter = vertexBytes(quarterVertices);
const expectedNativeStates = [[15, 1], [19, 5], [20, 6], [171, 1]];
const expectedNativeSamplers = [[1, 1], [2, 1], [5, 2], [6, 2]];
const expectedCandidateStates = [1, 4, 5, 0, 1, 0, 0, 1, 0, 0xffffffff, 1, 2, 3, 4, 28];

async function renderPopupAt(x, scenario) {
  const c = await core();
  const m = await oracle();
  const manager = c.anm_manager_create();
  const popup = c.popup_create(manager);
  const renderer = c.anm_renderer_create();
  const input = c.allocate(12);
  const player = c.allocate(12);
  const native = m.allocate(0x1040);
  const position = m.allocate(12);
  const nativePlayer = m.allocate(0x1000);
  const text = m.allocate(0x19300);
  const multiplier = m.allocate(4);
  const wrapper = m.allocate(32);
  const anm = readFileSync(resolve(root, 'reference/assets/ascii.anm'));
  const anmBytes = c.allocate(anm.length);
  const texts = [];
  enableNativeMath(m);
  try {
    memory(c, anmBytes, anm.length).set(anm);
    assert.equal(c.anm_manager_load(manager, 5, anmBytes, anm.length), 1);
    assert.equal(c.anm_manager_fallback(manager, 5), 1);
    assert.equal(c.popup_init(popup, 5), 1);
    const nativeManager = nativePool(m);
    const loader = resourceLoader(m);
    const bank = prepareBank(m, loader, 'ascii.anm', anm, nativeManager, 5);
    const graphics = nativeGraphics(m, nativeManager);
    const environment = c.anm_manager_environment(manager);
    const view = new DataView(c.memory.buffer);
    for (let index = 0; index < 4; index++) {
      const offset = index % 2 ? 16 : 320;
      view.setInt32(c.anm_environment_offsets(environment) + index * 4, offset, true);
      m.i32(0x51bc18 + index * 4, offset);
    }
    assert.equal(view.getFloat32(c.anm_environment_raster_scale(environment), true), 1,
      `${scenario}: default raster_scale`);
    m.view(native, 0x1040).fill(0);
    m.u32(0x4e9bd0, native);
    m.u32(0x4e9a58, text);
    m.u32(0x4e9bb8, nativePlayer);
    m.u32(0x4e79cc, 4);
    m.u32(0x4ca620, 0x4e73e8);
    m.f32(0x4e73e8, 1);
    m.call(0x40db10, {ecx: native + 0x18});
    m.call(0x40d680, {ecx: native + 0x18});
    m.u32(native + 0x18 + 0x28, 5);
    m.call(0x477160, {ecx: bank, args: [native + 0x18, 259]});
    m.u32(native + 0x34, (m.u32(native + 0x34) & 0xffbbffff) | 0x380000);
    const code = Buffer.alloc(13);
    code.set([0xf3, 0x0f, 0x10, 0x1d]);
    code.writeUInt32LE(multiplier, 4);
    code[8] = 0xe9;
    code.writeInt32LE(0x45f050 - wrapper - 13, 9);
    m.write(wrapper, code);
    m.replace(0x40c570, 'popup motion text formatting', () => {
      const stack = m.reg('ESP');
      const positionBytes = Buffer.from(m.bytes(m.u32(stack + 8), 12));
      const format = m.string(m.u32(stack + 12));
      let value;
      if (format === 'BONUS %.1f') {
        const number = m.bytes(stack + 16, 8);
        value = 'BONUS ' + new DataView(number.buffer).getFloat64(0, true).toFixed(1);
      } else if (format === '%d') value = String(m.i32(stack + 16));
      else value = format;
      texts.push({position: positionBytes, color: m.u32(text + 0x19224), value});
      return 0;
    });
    const putPosition = (px, py, pz = 0) => [px, py, pz].forEach((value, index) => {
      m.f32(position + index * 4, value);
      view.setFloat32(input + index * 4, value, true);
    });
    const compareEntries = label => assert.deepEqual(
      Buffer.from(memory(c, c.popup_entries(popup), 18 * 72)),
      Buffer.from(m.bytes(native + 0x620, 18 * 72)), label);
    let nativeDraws;
    let candidateDraws;
    for (let step = 0; step <= 2; step++) {
      if (step === 2) {
        putPosition(x, 0, 0);
        m.call(0x45ef60, {args: [position, 17, 0xffffffff]});
        c.popup_number(popup, input, 17, 0xffffffff);
        compareEntries(`${scenario}: popup creation`);
      }
      if (step % 13 === 0) {
        putPosition((step % 11 - 5) * 53, step % 3 === 0 ? -16 : step % 99, 0);
        const value = step % 3 === 0 ? -1 : Math.imul(step, 512);
        const color = (0xc07711aa + step * 31) >>> 0;
        const ratio = Math.fround(step * 0.135);
        m.f32(multiplier, ratio);
        m.call(wrapper, {ecx: native, args: [position, value, color]});
        c.popup_bonus(popup, input, value, color, ratio);
        compareEntries(`${scenario}: bonus update`);
      }
      m.f32(0x4e73e8, 1);
      m.call(0x45e9b0, {ecx: native});
      c.popup_update(popup, 1);
      compareEntries(`${scenario}: update ${step}`);
      const nativeX = -500, nativeY = -500;
      m.f32(nativePlayer + 0x618, nativeX);
      m.f32(nativePlayer + 0x61c, nativeY);
      [nativeX, nativeY, 0].forEach((value, index) => view.setFloat32(player + index * 4, value, true));
      graphics.reset();
      c.anm_renderer_reset(renderer);
      m.call(0x45eb10, {ecx: native, limit: 100000000});
      m.call(0x47e3f0, {ecx: nativeManager});
      assert.equal(c.popup_draw_renderer(popup, renderer, player), 1,
        `${scenario}: ${string(c, c.anm_renderer_error(renderer))}`);
      if (step === 2) {
        nativeDraws = graphics.draws();
        candidateDraws = Array.from({length: c.anm_renderer_calls(renderer)}, (_, index) => ({
          bytes: Buffer.from(memory(c, c.anm_renderer_history(renderer, index), c.anm_renderer_history_size(renderer, index))),
          states: Array.from(new Uint32Array(c.memory.buffer, c.anm_renderer_history_state(renderer, index), 15)),
        }));
      }
    }
    assert.equal(nativeDraws.length, 1, `${scenario}: exactly one original glyph batch`);
    assert.equal(candidateDraws.length, 1, `${scenario}: exactly one candidate glyph batch`);
    const original = nativeDraws[0];
    const candidate = candidateDraws[0];
    assert.equal(original.topology, 4, `${scenario}: native triangle-list topology`);
    assert.equal(original.count, 4, `${scenario}: native four-glyph primitive count`);
    assert.equal(original.stride, 28, `${scenario}: native vertex stride`);
    assert.equal(original.bytes.length, expectedGrid.length, `${scenario}: native vertex count`);
    assert.deepEqual([...original.states].sort((a, b) => a[0] - b[0]), expectedNativeStates,
      `${scenario}: native material state`);
    assert.deepEqual([...original.samplers].sort((a, b) => a[0] - b[0]), expectedNativeSamplers,
      `${scenario}: native sampler state`);
    assert.deepEqual(candidate.states, expectedCandidateStates, `${scenario}: candidate material state`);
    assert.equal(candidate.bytes.length, expectedGrid.length, `${scenario}: candidate vertex count`);
    assert.deepEqual(original.bytes, expectedGrid, `${scenario}: literal original pixel-grid vertices`);
    if (scenario === 'integer grid') {
      assert.deepEqual(candidate.bytes, expectedGrid, `${scenario}: complete candidate bytes equal original`);
    } else {
      assert.deepEqual(candidate.bytes, expectedQuarter,
        `${scenario}: literal fractional candidate vertices retain +0.25 X`);
      assertScreenVertexGridOrEqual(candidate.bytes, original.bytes, {label: scenario});
    }
  } finally {
    c.popup_delete(popup);
    c.anm_renderer_delete(renderer);
    c.anm_manager_delete(manager);
    for (const pointer of [input, player, anmBytes]) c.release(pointer);
    m.close();
  }
}

test('TH15 MP popup original integer-grid control keeps complete vertex bytes equal', async () => {
  await renderPopupAt(-180, 'integer grid');
});

test('TH15 MP popup fractional control preserves literal quarter-pixel presentation motion', async () => {
  await renderPopupAt(-179.75, 'fractional quarter pixel');
});
