import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';import {spawnSync}from'node:child_process';import{pathToFileURL}from'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),out=path.join(root,'artifacts/enemy-life-test.mjs'),python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const result=spawnSync(python,[compiler,'tests/fixtures/enemy-life.cpp','-std=c++20','-O2','-o',out,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_reset_life","_begin_spell","_end_spell","_hurt_life","_life_value","_raw_value","_life_flags"]'],{cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python}});assert.equal(result.status,0,result.stderr||result.stdout);
const create=(await import(pathToFileURL(out))).default,native=JSON.parse(fs.readFileSync(new URL('./fixtures/enemy-life-original.json',import.meta.url)));
test('sevenfold spell life preserves all native cumulative remainders, thresholds and end flags',async()=>{
  const c=await create(),check=s=>assert.deepEqual([c._life_value(),c._raw_value(),c._life_flags()],[s.life,s.raw,s.flags]);
  for(const record of native.records){c._reset_life(record.initial,record.threshold,record.flags);if(record.spell)c._begin_spell();check(record.birth);for(const frame of record.frames){c._hurt_life(frame.damage);check(frame);}c._end_spell();check(record.end);}
});
