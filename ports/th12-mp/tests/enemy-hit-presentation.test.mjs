import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const directory=path.join(root,'artifacts/enemy-hit-presentation-tests');
fs.mkdirSync(directory,{recursive:true});
const python=emscripten(root).python;
const compiler=emscripten(root).compiler;
const output=path.join(directory,'core.mjs');
const built=spawnSync(python,[compiler,'tests/fixtures/enemy-hit-presentation.cpp',...runtimeSources(root),
 '-std=c++20','-O0','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432',
 '-sEXPORTED_FUNCTIONS=["_enemy_hit_pose","_enemy_hit_property","_th12_load_anm","_malloc","_free"]',
 '-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],{cwd:root,encoding:'utf8',
 env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,
 PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('fixtures/enemy-hit-presentation-original.json',import.meta.url)));
const properties=JSON.parse(fs.readFileSync(new URL('fixtures/enemy-hit-properties-original.json',import.meta.url)));
const core=await create();
const bytes=fs.readFileSync(new URL('../local/retail/stgenm01.anm',import.meta.url));
const at=core._malloc(bytes.length);core.HEAPU8.set(bytes,at);
assert.equal(core._th12_load_anm(2,at,bytes.length),1);core._free(at);

test('actual ECL453 hit sound and449 tinted bit are independent of death presentation',()=>{
 assert.equal(properties.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 for(const row of properties.cases){
  const p=core._enemy_hit_property(row.value??0,Number(row.value!==null));
  assert.deepEqual([...new Int32Array(core.HEAPU8.buffer,p,2)],[row.hitSound,Number(row.tinted)]);
 }
});
test('original hit flash cooldown, low health sound thresholds and secondary colors match411 CPU cases',()=>{
 assert.equal(native.originalExeSha256,properties.originalExeSha256);
 for(const row of native.cases){
  const i=row.initial,a=row.after;
  const p=core._enemy_hit_pose(i.flags,i.phaseLife,i.spellFlags,i.phaseAge,i.flash,i.hitSound,0);
  const actual=[...new Uint32Array(core.HEAPU8.buffer,p,5)];
  // The original embedded HUD VM is captured by the sampler, but this test
  // compares the Enemy slot0 VM and sound request block only.
  assert.deepEqual(actual,[a.flash,a.vmFlags,a.secondaryColor,a.sounds.length,a.sounds[0]?.id??0],JSON.stringify(i));
 }
});
test('missing Enemy slot0 skips hit flash timer and positioned sound request',()=>{
 const p=core._enemy_hit_pose(0x100000,100,0,0,4,37,1);
 assert.deepEqual([...new Uint32Array(core.HEAPU8.buffer,p,5)],[4,0,0,0,0]);
});
