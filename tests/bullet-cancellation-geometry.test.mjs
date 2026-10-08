import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe');
const compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const directory=path.join(root,'artifacts/bullet-cancellation-geometry');fs.mkdirSync(directory,{recursive:true});
const output=path.join(directory,'core.mjs');
const build=spawnSync(python,[compiler,'tests/fixtures/bullet-cancellation-geometry.cpp','-std=c++20','-O2','-o',output,
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_point_visible","_circle_contains"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(build.status,0,build.stdout+'\n'+build.stderr);
const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('fixtures/bullet-cancellation-geometry-native.json',import.meta.url)));
test('native conversion viewport keeps the2px margin and excludes its exact edges',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const c=await create();for(const row of native.points)assert.equal(!!c._point_visible(...row.position),row.visible,JSON.stringify(row));
 assert.ok(native.points.some(r=>r.position[0]===0&&r.position[1]<0&&r.visible));
});
test('native circle cancellation preserves its original float stores and conversion decisions',async()=>{
 const c=await create();let distinguishesFloatStores=0;for(const row of native.circles){
  const hit=!!c._circle_contains(...row.position,...row.origin,row.radius,row.hitbox);
  assert.equal(hit,row.cancelled,JSON.stringify(row));
  assert.equal(hit&&!!c._point_visible(...row.position),row.converted,'point conversion '+JSON.stringify(row));
  const dx=row.position[0]-row.origin[0],dy=row.position[1]-row.origin[1],reach=row.radius+row.hitbox*.5;
  if((dx*dx+dy*dy<=reach*reach)!==row.cancelled)++distinguishesFloatStores;
 }
 assert.equal(distinguishesFloatStores,1,'a true native boundary rejects the former unrounded circle calculation');
});
