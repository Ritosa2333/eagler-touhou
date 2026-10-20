import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
const root = path.resolve(import.meta.dirname, '..'), workspace = path.dirname(root);
const folder = path.join(root, 'artifacts/ecl-power-reference-tests'), temp = path.join(folder, 'tmp');
fs.mkdirSync(temp, {recursive: true});
const python = emscripten(root).python;
const compiler = emscripten(root).compiler, output = path.join(folder, 'core.mjs');
const names = ['power_reference_begin', 'power_reference_int', 'power_reference_float', 'power_reference_tick',
 'power_reference_snapshot', 'th12_load_ecl', 'th12_load_anm', 'malloc', 'free'];
const built = spawnSync(python, [compiler, 'tests/fixtures/ecl-power-reference.cpp', ...runtimeSources(root),
 '-std=c++20', '-O0', '-o', output, '-sMODULARIZE=1', '-sEXPORT_ES6=1', '-sENVIRONMENT=node',
 '-sALLOW_MEMORY_GROWTH=1', '-sINITIAL_MEMORY=33554432',
 '-sEXPORTED_FUNCTIONS=' + JSON.stringify(names.map(n => '_' + n)), '-sEXPORTED_RUNTIME_METHODS=["HEAPU8","UTF8ToString"]'],
 {cwd: root, encoding: 'utf8', env: {...process.env, EM_CACHE: path.join(root, 'artifacts/emscripten-cache'),
 EMCC_TEMP_DIR: temp, EMSDK_PYTHON: python, PATH: path.dirname(python) + path.delimiter + process.env.PATH}});
assert.equal(built.status, 0, built.stdout + '\n' + built.stderr);
const create = (await import(pathToFileURL(output))).default;
const original = JSON.parse(fs.readFileSync(new URL('./fixtures/ecl-power-reference-original.json', import.meta.url)));
function load(core, bytes, method, ...prefix) {
 const at = core._malloc(bytes.length); core.HEAPU8.set(bytes, at);
 assert.equal(core[method](...prefix, at, bytes.length), 1); core._free(at);
}
async function prepare(row) {
 const core = await create();
 load(core, fs.readFileSync(new URL('../local/retail/bullet.anm', import.meta.url)), '_th12_load_anm', 0);
 load(core, Buffer.from(row.syntheticEclHex, 'hex'), '_th12_load_ecl');
 core._power_reference_begin(row.difficulty, row.seed, row.powers[0], row.initialDrop);
 return core;
}
const snapshot = core => JSON.parse(core.UTF8ToString(core._power_reference_snapshot()));
function compare(actual, expected, label) {
 for (const field of ['power', 'rng', 'enemies']) assert.deepEqual(actual[field], expected[field], label + ' ' + field);
 assert.equal(actual.items.length, expected.items.length, label + ' actual ItemManager count');
 for (let i = 0; i < actual.items.length; i++) {
  const a = actual.items[i], b = expected.items[i];
  for (const field of ['slot', 'type', 'state', 'age']) assert.equal(a[field], b[field], label + ' item' + i + ' ' + field);
  for (const field of ['position', 'velocity']) b[field].forEach((v, j) =>
   assert.ok(Math.abs(a[field][j] - v) < .00004, label + ' item' + i + ' ' + field + j + ': ' + a[field][j] + ' vs ' + v));
 }
}
test('actual ECL−9930 reads raw signed power before any stored global, preserving the integer and float distinction', async () => {
 assert.equal(original.originalExeSha256, '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const core = await prepare(original.records[0]);
 for (const row of original.getterCases) {
  assert.equal(core._power_reference_int(row.power) >>> 0, row.integerWord, 'raw int power ' + row.power);
  assert.equal(core._power_reference_float(row.power), row.floatValue, 'signed fild power ' + row.power);
 }
 assert.equal(original.getterCases.length, 6);
});
test('four native difficulty masks drop real exit blue points only at400 power and stop on parent deletion', async () => {
 let frames = 0;
 for (const row of original.records) {
  assert.equal(row.retailCrossCheck, true, 'native retail/synthetic worker equality');
  const core = await prepare(row);
  compare(snapshot(core), row.frames[0], 'difficulty ' + row.difficulty + ' birth'); frames++;
  for (let i = 0; i < row.powers.length; i++) {
   core._power_reference_tick(row.powers[i]);
   compare(snapshot(core), row.frames[i + 1], 'difficulty ' + row.difficulty + ' tick ' + i); frames++;
  }
  assert.equal(row.frames[1].items.length, 0, 'constructor guard prevents a premature drop');
  assert.deepEqual(row.frames[2].items.map(i => i.type), [1, 2], '406 retains initial primary smallP');
  assert.equal(row.frames.at(-1).items.length, 6, 'five blue points and the initial primary item');
  assert.equal(row.frames.at(-1).enemies, 0, 'main deletes the async trail too');
 }
 assert.equal(frames, 52);
});
