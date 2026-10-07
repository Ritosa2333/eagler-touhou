import test, {before} from 'node:test';
import assert from 'node:assert/strict';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {readFile} from 'node:fs/promises';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
let createHarness,native;
before(async()=>{
  const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe');
  const emcc=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
  const out=path.join(root,'artifacts/player-circle-harness.mjs');
  const result=spawnSync(python,[emcc,'tests/native/player_circle_harness.cpp','src/game/LaserCollision.cpp','-std=c++20','-O2','-o',out,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS='+JSON.stringify(['_query_circle','_begin_miss','_clears_laser'])],{
    cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python}});
  assert.equal(result.status,0,result.stdout+result.stderr);
  createHarness=(await import(pathToFileURL(out))).default;
  native=JSON.parse(await readFile(path.join(root,'tests/fixtures/player-circle-native.json'),'utf8'));
  assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  assert.equal(native.cases.length,842);
});
const query=(h,c)=>{const p=c.player;return h._query_circle(...c.center,c.radius,...p.position,p.circleHitRadius,p.hitWidth,p.hitHeight,p.state,p.invulnerability,p.flags,p.dialogue?1:0);};
test('842 native circle cases agree on geometry and miss-call boundaries',async()=>{
  const h=await createHarness();
  for(const c of native.cases){assert.equal(query(h,c),c.nativeReturn,c.name);assert.equal(h._begin_miss(),c.calls438370,c.name);assert.equal(h._clears_laser(),0);}
});
test('strict circle hit/graze equality differs from the sum-of-radii approximation',async()=>{
  const h=await createHarness();
  const edge=name=>native.cases.find(c=>c.name===name);
  assert.equal(query(h,edge('exact_radius0_distance2')),2);
  assert.equal(query(h,edge('exact_radius0_distance42')),0);
  assert.equal(query(h,edge('exact_radius40_graze58')),0);
});
test('invulnerable inner circle returns Hit1 without opening a new death window',async()=>{
  const h=await createHarness();const c=native.cases.find(c=>c.name==='gate_1_120_0_False_0');
  assert.equal(query(h,c),1);assert.equal(h._begin_miss(),0);
  for(const state of [2,3,4]){const graze=native.cases.find(c=>c.name===`gate_${state}_120_2_True_25`);assert.equal(query(h,graze),2);assert.equal(h._begin_miss(),0);}
});
