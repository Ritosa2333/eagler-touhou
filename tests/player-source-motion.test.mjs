import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';import {spawnSync} from 'node:child_process';import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),directory=path.join(root,'artifacts/player-source-motion');fs.mkdirSync(directory,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py'),output=path.join(directory,'core.mjs');
const build=spawnSync(python,[compiler,'tests/fixtures/player-source-motion.cpp','-std=c++20','-O2','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_source_begin","_source_tick","_source_state","_source_damage"]','-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],{cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(build.status,0,build.stdout+'\n'+build.stderr);const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('fixtures/player-source-motion-original.json',import.meta.url)));
const values=c=>[...new Float32Array(c.HEAPU8.buffer,c._source_state(),6)];
const expected=s=>[...s.position,s.radius,s.timer[0],s.timer[1],s.flags];
test('original Source16 stationary circles quantize their center after birth and before radius/timer updates',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const c=await create();for(const row of native.rows){const i=row.initial;c._source_begin(...i.position,i.radius,i.growth,i.ticks);assert.deepEqual(values(c),expected(row.birth));for(const frame of row.frames){c._source_tick();assert.deepEqual(values(c),expected(frame));}}
});
test('original circle collision retains unrounded coordinate deltas until the distance store',async()=>{
 const rows=JSON.parse(fs.readFileSync(new URL('fixtures/player-source-circle-original.json',import.meta.url))).rows;
 const c=await create();let changed=0;
 for(const row of rows){
  c._source_begin(...row.source,row.radius,0,15);
  assert.equal(c._source_damage(...row.target),row.damage,JSON.stringify(row));
  const dx=Math.fround(row.source[0]-row.target[0]),dy=Math.fround(row.source[1]-row.target[1]);
  const oldDamage=row.radius*row.radius>=Math.fround(dx*dx+dy*dy)?4:0;
  if(oldDamage!==row.damage)++changed;
 }
 assert.equal(changed,8,'eight native boundaries distinguish rounded deltas from the original x87 calculation');
});
