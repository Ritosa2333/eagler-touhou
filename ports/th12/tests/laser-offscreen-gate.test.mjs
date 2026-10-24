import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root = path.resolve(import.meta.dirname, '..'), workspace = path.dirname(root);
const folder = path.join(root, 'artifacts/laser-offscreen-gate-tests');
fs.mkdirSync(folder, {recursive: true});
const python = emscripten(root).python;
const compiler = emscripten(root).compiler;
const names = ['offscreen_callback_begin', 'offscreen_callback_tick', 'offscreen_callback_snapshot',
 'offscreen_missing', 'offscreen_lifecycle_begin', 'offscreen_lifecycle_tick', 'offscreen_lifecycle_snapshot'];
const built = spawnSync(python, [compiler, 'tests/fixtures/laser-offscreen-gate.cpp', 'src/game/Laser.cpp',
 '-std=c++20', '-O2', '-o', path.join(folder, 'core.mjs'), '-sMODULARIZE=1', '-sEXPORT_ES6=1', '-sENVIRONMENT=node',
 '-sEXPORTED_FUNCTIONS=' + JSON.stringify(names.map(n => '_' + n)), '-sEXPORTED_RUNTIME_METHODS=["HEAPF32"]'],
 {cwd: root, encoding: 'utf8', env: {...process.env, EM_CACHE: path.join(root, 'artifacts/emscripten-cache'),
 EMSDK_PYTHON: python, PATH: path.dirname(python) + path.delimiter + process.env.PATH}});
assert.equal(built.status, 0, built.stdout + '\n' + built.stderr);
const create = (await import(pathToFileURL(path.join(folder, 'core.mjs')))).default;
const original = JSON.parse(fs.readFileSync(new URL('./fixtures/laser-offscreen-gate-original.json', import.meta.url)));
const snapshot = (core, name, count) => [...core.HEAPF32.subarray(core[name]() / 4, core[name]() / 4 + count)];
const close = (a, b, label) => assert.ok(Math.abs(a - b) <= .00004, `${label}: ${a} vs native ${b}`);
function compareMotion(actual, expected, label, includePrevious = true) {
 assert.deepEqual(actual.slice(0, 2), [expected.active, expected.cursor], label + ' active/cursor');
 // An absent branch leaves native synthetic zero-filled memory untouched;
 // only initialized native timer fields are meaningful in these cases.
 if (expected.offscreenGate.some(v => v !== 0)) {
  assert.deepEqual(actual.slice(2, 5), expected.offscreenGate, label + ' gate timer');
  assert.equal(actual[5], expected.storedFlags ?? 0, label + ' stored gate flags');
 }
 if (expected.polarTimer.some(v => v !== 0) || (expected.active & 8)) {
  const indices = includePrevious ? [0, 1, 2] : [1, 2];
  for (const i of indices) assert.equal(actual[6 + i], expected.polarTimer[i], label + ' polar timer ' + i);
 }
 [expected.angle, expected.speed, ...expected.velocity].forEach((v, i) => close(actual[9 + i], v, label + ' motion ' + i));
}
test('actual Curve400 lifetime expires after decrement and repeats dispatch in the same tick', async () => {
 assert.equal(original.originalExeSha256, '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 let snapshots = 0;
 for (const row of original.callbackCases) {
  const core = await create();
  core._offscreen_callback_begin(row.kind, row.duration, row.rate, row.flags, Number(row.concurrent));
  for (let i = 1; i < row.frames.length; i++) {
   core._offscreen_callback_tick();
   compareMotion(snapshot(core, '_offscreen_callback_snapshot', 14), row.frames[i], JSON.stringify(row) .slice(0, 85) + ' tick ' + (i - 1), row.kind === 2);
   assert.equal(core._offscreen_missing(), 0, 'Curve400 is implemented'); snapshots++;
  }
 }
 assert.equal(snapshots, 276);
});
test('actual42-node curves survive leaving the viewport and returning during ETEX400 only', async () => {
 let snapshots = 0;
 for (const row of original.lifecycles) {
  const core = await create(); core._offscreen_lifecycle_begin(row.duration ?? -1);
  for (let i = 1; i < row.frames.length; i++) {
   core._offscreen_lifecycle_tick(); const expected = row.frames[i];
   const actual = snapshot(core, '_offscreen_lifecycle_snapshot', 26 + row.nodeCount * 5);
   const label = 'duration ' + row.duration + ' tick ' + (i - 1);
   assert.equal(actual[0], expected.count, label + ' live count');
   if (expected.count) {
    compareMotion(actual.slice(1, 15), expected, label);
    assert.equal(actual[15], expected.phase, label + ' phase');
    assert.deepEqual(actual.slice(16, 19), expected.age, label + ' manager age');
    expected.position.forEach((v, j) => close(actual[19 + j], v, label + ' position ' + j));
    assert.deepEqual(actual.slice(22, 25), expected.offscreenGrace, label + ' grace continues below zero');
    assert.equal(actual[25], expected.nodes.length, label + ' history count');
    expected.nodes.flat().forEach((v, j) => close(actual[26 + j], v, label + ' history ' + j));
   }
   snapshots++;
  }
 }
 assert.equal(snapshots, 396);
 assert.equal(original.lifecycles[0].frames[45].count, 0, 'without gate retires at tick44');
 assert.equal(original.lifecycles[2].frames[133 - 1].count, 1, 'protected curve returns and survives');
});
