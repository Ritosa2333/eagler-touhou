import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
const root=path.resolve(import.meta.dirname,'..'), workspace=path.dirname(root);
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe');
const compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const cache=path.join(root,'artifacts/emscripten-cache');
const output=path.join(root,'artifacts/bullet-collision-order');
fs.mkdirSync(output,{recursive:true});
const names=['collision_run','collision_result','collision_queries','collision_query_count','collision_events','collision_event_count'];
const built=spawnSync(python,[compiler,'tests/fixtures/bullet-collision-order.cpp','src/game/BulletTargetMotion.cpp',
 '-std=c++20','-O2','-o',path.join(output,'core.mjs'),'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(name=>'_'+name)),
 '-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPU8','HEAPF32'])],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:cache,EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const createFixture=(await import('../artifacts/bullet-collision-order/core.mjs')).default;
const original=JSON.parse(fs.readFileSync(new URL('fixtures/bullet-collision-order-original.json',import.meta.url)));
const eventCode={query:1,miss:2,graze:3,'anm-tail':4,'interrupt:1':11};
test('original4099e0 preserves both spawning collision slices and jumps to IRQ1/ANM tail on hit',async()=>{
 const core=await createFixture();
 for(const row of original.rows.filter(row=>row.initial.phase===2)){
  const i=row.initial;
  core._collision_run(i.phase,i.x,i.vx,i.age,i.anmInt0,i.word4,i.extension200?1:0);
  const result=[...new Float32Array(core.HEAPU8.buffer,core._collision_result(),8)];
  assert.deepEqual(result.slice(0,4),[row.after.phase,...row.after.position,row.after.word4],row.case);
  if(row.after.phase===3)assert.equal(result[4],row.after.interrupt,row.case);
  assert.deepEqual(result.slice(5),[row.missCallbacks,row.grazeCallbacks,row.anmTailCalls],row.case);
  const queries=[...new Float32Array(core.HEAPU8.buffer,core._collision_queries(),core._collision_query_count()*2)];
  assert.deepEqual(queries,row.queryPositions.flat(),row.case);
  const events=[...new Int32Array(core.HEAPU8.buffer,core._collision_events(),core._collision_event_count())];
  assert.deepEqual(events,row.events.map(name=>eventCode[name]),row.case);
 }
});
test('original0x200 countdown does not suppress hit and nonzero negative words also decrement',async()=>{
 const core=await createFixture();
 for(const row of original.rows.filter(row=>row.initial.phase===1)){
  const i=row.initial;
  core._collision_run(i.phase,i.x,i.vx,i.age,i.anmInt0,i.word4,i.extension200?1:0);
  const result=[...new Float32Array(core.HEAPU8.buffer,core._collision_result(),8)];
  assert.deepEqual(result,[row.after.phase,...row.after.position,row.after.word4,row.after.interrupt,row.missCallbacks,row.grazeCallbacks,row.anmTailCalls],row.case);
 }
});
