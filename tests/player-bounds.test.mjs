import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';import {spawnSync} from 'node:child_process';import{pathToFileURL}from'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),folder=path.join(root,'artifacts/player-bounds-tests');fs.mkdirSync(folder,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const build=spawnSync(python,[compiler,'tests/fixtures/player-bounds.cpp','src/game/PlayerBounds.cpp','-std=c++20','-O2','-o',path.join(folder,'core.mjs'),'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_bounds_quad","_bounds_corners","_bounds_survives","_bounds_hit"]','-sEXPORTED_RUNTIME_METHODS=["HEAPF32"]'],{cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python}});assert.equal(build.status,0,build.stderr||build.stdout);
const create=(await import(pathToFileURL(path.join(folder,'core.mjs')).href)).default,original=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/player-bounds-original.json')));
test('C++ quad positions agree with all36 original mode0/1 anchor and rotation samples',async()=>{
 const c=await create();for(const row of original.geometry){const p=c._bounds_quad(row.mode,row.anchorX,row.anchorY,row.angle,...row.position,...row.dimensions)/4,actual=[...c.HEAPF32.slice(p,p+12)],expected=row.vertices.flat();actual.forEach((v,i)=>assert.ok(Math.abs(v-expected[i])<.00002,`${JSON.stringify(row)} coordinate${i} ${v} != ${expected[i]}`));}
});
test('C++ friendly offscreen decisions match original age/type and strict viewport boundaries',async()=>{
 const c=await create();for(const row of original.bounds){c.HEAPF32.set(row.vertices.flatMap(([x,y])=>[x,y,0]),c._bounds_corners()/4);assert.equal(!!c._bounds_survives(row.type,row.age,...row.shotPosition),row.active,JSON.stringify(row));}
});
test('original friendly and attached beam damage excludes targets wholly above the playfield',async()=>{
 const c=await create();for(const r of original.damage)assert.equal(!!c._bounds_hit(r.type,r.shotY,r.shotHeight,r.enemyY,r.enemyHeight),r.damage>0,JSON.stringify(r));
});
