import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
import {loadLiveMain} from './helpers/live-main.mjs';
const root = path.resolve(import.meta.dirname, '..'), workspace = path.dirname(root);
const directory = path.join(root, 'artifacts/phase-player-counters-tests'), temp = path.join(directory, 'tmp');
fs.mkdirSync(temp, {recursive: true});
const python = emscripten(root).python;
const compiler = emscripten(root).compiler, output = path.join(directory, 'core.mjs');
const names = ['death_resource_begin', 'death_resource_collision', 'death_resource_window',
 'death_resource_tick', 'death_resource_snapshot', 'phase_counter_begin', 'phase_counter_snapshot', 'th12_load_ecl', 'th12_load_anm', 'th12_load_sht', 'malloc', 'free'];
const result = spawnSync(python, [compiler, 'tests/fixtures/phase-player-counters.cpp', ...runtimeSources(root),
 '-std=c++20', '-O0', '-o', output, '-sMODULARIZE=1', '-sEXPORT_ES6=1', '-sENVIRONMENT=node',
 '-sALLOW_MEMORY_GROWTH=1', '-sINITIAL_MEMORY=33554432',
 '-sEXPORTED_FUNCTIONS=' + JSON.stringify(names.map(n => '_' + n)), '-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],
 {cwd: root, encoding: 'utf8', env: {...process.env, EM_CACHE: path.join(root, 'artifacts/emscripten-cache'),
 EMSDK_PYTHON: python, EMCC_TEMP_DIR: temp, PATH: path.dirname(python) + path.delimiter + process.env.PATH}});
assert.equal(result.status, 0, result.stdout + '\n' + result.stderr);
const create = (await import(pathToFileURL(output))).default;
const original = JSON.parse(fs.readFileSync(new URL('./fixtures/phase-player-counters-original.json', import.meta.url)));
async function prepared() {
 const core = await create(); loadLiveMain(core);
 const resources = [['bullet.anm', '_th12_load_anm', 0], ['pl00.anm', '_th12_load_anm', 4],
  ['pl01.anm', '_th12_load_anm', 5], ['pl02.anm', '_th12_load_anm', 6], ['front.anm', '_th12_load_anm', 7]];
 for (let i = 0; i < 6; i++) resources.push([`pl0${i >> 1}${i % 2 ? 'b' : 'a'}.sht`, '_th12_load_sht', i]);
 for (const [name, method, index] of resources) {
  const bytes = fs.readFileSync(new URL('../local/retail/' + name, import.meta.url)), at = core._malloc(bytes.length);
  core.HEAPU8.set(bytes, at); assert.equal(core[method](index, at, bytes.length), 1); core._free(at);
 }
 return core;
}

test('all six original loadouts count accepted Bombs and committed deaths, preserving the deathbomb window', async () => {
 assert.equal(original.originalExeSha256, '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(original.rows.length, 24);
 const core = await prepared();
 let snapshots = 0;
 for (const row of original.rows) {
  const label = `loadout${row.loadout}/${row.mode}`;
  const compare = expected => {
   assert.deepEqual([...new Uint32Array(core.HEAPU8.buffer, core._phase_counter_snapshot(), 3)], expected, label);
   snapshots++;
  };
  core._phase_counter_begin(row.loadout, row.stock);
  compare(row.frames[0]);
  let at = 1;
  if (row.collision) { core._death_resource_collision(); compare(row.frames[at++]); }
  if (row.mode === 'deathbomb') core._death_resource_window();
  for (let tick = 0; tick < 10; tick++) {
   core._death_resource_tick(Number(row.mode !== 'miss' && tick === 0) * 2);
   compare(row.frames[at++]);
  }
 }
 assert.equal(snapshots, 276);
});
