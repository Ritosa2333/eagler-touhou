import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe');
const compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const directory=path.join(root,'artifacts/bullet-cancel-lifecycle');fs.mkdirSync(directory,{recursive:true});
const output=path.join(directory,'core.mjs');
const names=['cancel_begin','cancel_scripted','cancel_tick','cancel_state','cancel_events_count','cancel_events'];
const build=spawnSync(python,[compiler,'tests/fixtures/bullet-cancel-lifecycle.cpp','src/game/BulletTargetMotion.cpp',
  '-std=c++20','-O2','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
  '-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(name=>'_'+name)),
  '-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPU8'])],
  {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(build.status,0,build.stdout+'\n'+build.stderr);
const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('fixtures/bullet-cancel-lifecycle-original.json',import.meta.url)));
assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
const state=c=>[...new Float32Array(c.HEAPU8.buffer,c._cancel_state(),8)];
const events=c=>[...new Int32Array(c.HEAPU8.buffer,c._cancel_events(),c._cancel_events_count())];
const begin=(c,r)=>c._cancel_begin(r.initial.phase,...r.initial.position,...r.initial.velocity,r.initial.age);
function compareState(actual,expected,label){
  assert.equal(actual[0],expected.phase,label+' phase');
  // Original retirement resets its reusable pool object. Only live bullets
  // are part of the portable state's semantic snapshot after pool release.
  if(expected.phase===0)return;
  assert.deepEqual(actual.slice(1,3),expected.position,label+' position');
  assert.equal(actual[3],expected.timer[1],label+' age');
  assert.equal(actual[4],expected.word4,label+' word4');
  assert.equal(actual[5],expected.interrupt,label+' IRQ');
  assert.equal(!!actual[6],!!(expected.flags&8),label+' pending retire');
}
test('original scripted cancel resets in-bounds lifetime before the same Bullet21 half-speed visit',async()=>{
  const c=await create();
  for(const row of native.rows.filter(r=>!(r.cancelled.flags&8))){
    begin(c,row);c._cancel_scripted();compareState(state(c),row.cancelled,row.case+' cancel');
    assert.deepEqual(events(c),row.cancelResult?[11]:[],row.case+' synchronous IRQ');
    for(const visit of row.passes){c._cancel_tick();compareState(state(c),visit,row.case+' visit'+visit.visit);assert.deepEqual(events(c),visit.events.map(()=>1));}
  }
});
test('original offscreen cancel retains its slot/timer and IRQ1 until the next Bullet21 retirement',async()=>{
  const c=await create();
  for(const row of native.rows.filter(r=>r.cancelled.flags&8)){
    begin(c,row);c._cancel_scripted();compareState(state(c),row.cancelled,row.case+' cancel');
    assert.deepEqual(events(c),[11],'IRQ is queued even outside the cancellation margin');
    for(const visit of row.passes){c._cancel_tick();compareState(state(c),visit,row.case+' visit'+visit.visit);assert.deepEqual(events(c),visit.visit===1?[2]:[],'no movement/ANM tail before pending retirement');}
  }
});
