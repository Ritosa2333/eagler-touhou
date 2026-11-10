import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const dir=path.join(root,'artifacts/moving-laser-prefix-clear-tests');
fs.mkdirSync(dir,{recursive:true});
const python=emscripten(root).python;
const compiler=emscripten(root).compiler;
const output=path.join(dir,'core.mjs');
const built=spawnSync(python,[compiler,'tests/fixtures/moving-laser-prefix-clear.cpp','src/game/Laser.cpp',
 '-std=c++20','-O2','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sEXPORTED_FUNCTIONS=["_moving_laser_prefix_clear"]','-sEXPORTED_RUNTIME_METHODS=["UTF8ToString"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),
 EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('fixtures/moving-laser-prefix-clear-original.json',import.meta.url)));
test('original partial MovingLaser clear stores the removed length as head offset and suppresses its cap',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const core=await create();
 for(const row of native.records){
  const actual=JSON.parse(core.UTF8ToString(core._moving_laser_prefix_clear(row.initialOffset,row.radius)));
  const expected=JSON.parse(JSON.stringify({removed:row.removed,afterClear:row.afterClear,afterTick:row.afterTick}),
   (key,value)=>typeof value==='number'?Number(value.toPrecision(9)):value);
  assert.deepEqual(actual,expected,`initial head ${row.initialOffset}, radius ${row.radius}`);
  assert.ok(actual.afterClear.geometry[4]>0,'actual cap predicate requires zero head offset');
 }
});
