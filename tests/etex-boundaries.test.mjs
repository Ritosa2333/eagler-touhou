import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const folder=path.join(root,'artifacts/etex-boundaries-tests');fs.mkdirSync(folder,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe');
const compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const built=spawnSync(python,[compiler,'tests/fixtures/etex-boundaries.cpp',
  'src/game/BulletTargetMotion.cpp','src/game/AnmLogic.cpp','-std=c++20','-O2',
  '-o',path.join(folder,'core.mjs'),'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
  '-sEXPORTED_FUNCTIONS=["_test_gate","_test_reset","_test_set","_test_initialize","_test_tick","_test_state","_test_blend"]',
  '-sEXPORTED_RUNTIME_METHODS=["HEAPF32","HEAPU32"]'],
  {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),
  EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stderr||built.stdout);
const create=(await import(pathToFileURL(path.join(folder,'core.mjs')).href)).default;
const core=await create();
const native=JSON.parse(fs.readFileSync(new URL('fixtures/etex-boundaries-native.json',import.meta.url)));
const timer=JSON.parse(fs.readFileSync(new URL('fixtures/bullet-gate-original.json',import.meta.url)));
const blend=JSON.parse(fs.readFileSync(new URL('fixtures/etex-blend-native.json',import.meta.url)));
function candidate(duration,rate,position,angle,sprite,d){
  const pointer=core._test_gate(duration,rate,...position,angle,...sprite,d)/4;
  return [...core.HEAPF32.subarray(pointer,pointer+3)];
}
function initializeBullet(row){
  core._test_reset(...(row.position??[0,120]),row.angle??0,0,16,16);
  row.records.forEach((record,index)=>core._test_set(index,+record.concurrent,
    record.kind,record.c,record.d,record.a,record.b));
  core._test_initialize();
}
function assertBullet(expected,label){
  const pointer=core._test_state()/4,actual=[...core.HEAPF32.subarray(pointer,pointer+13)];
  const original=[...expected.position,...expected.velocity,expected.speed,expected.angle,
    expected.active,expected.cursor,expected.gateRemaining,expected.gateValue,
    expected.waitRemaining,expected.accelerationTicks,0];
  // Nativex87 and portable trig may differ by float32 ULPs; frame/timer state
  // is exact, and the small position tolerance cannot conceal a retry frame.
  for(let i=0;i<6;i++)assert.ok(Math.abs(actual[i]-original[i])<=.000002,
    `${label} field${i}: candidate${actual[i]} native${original[i]}`);
  assert.deepEqual(actual.slice(6),original.slice(6),label);
}
test('kind400 d1 geometry matches original corner projection and boundary decisions',()=>{
  for(const row of native.geometryCases){
    const actual=candidate(row.duration,1,row.position,row.angle,row.sprite,row.d);
    assert.deepEqual(actual,[row.completed,row.after.gateRemaining,row.after.gateValue],
      JSON.stringify({sprite:row.sprite,position:row.position,angle:row.angle}));
  }
});
test('d0/1/2/-1 preserve native timer-first completion before same-frame retry',()=>{
  for(const row of native.rayRetryCases){
    const first=row.records[0];
    const completed=row.events.some(e=>e.context==='first-update'&&e.event==='complete400');
    assert.deepEqual(candidate(first.c,1,row.position,Math.fround(row.angle),[16,16],first.d),
      [+completed,row.after.gateRemaining,row.after.gateValue],JSON.stringify(first));
  }
});
test('geometry-disabled overload keeps all independently sampled fractional timer endpoints',()=>{
  for(const row of timer.rows){
    const first=row.frames[0];
    assert.deepEqual(candidate(row.duration,row.rate,[200,120],0,[16,16],0),
      [first.completed,first.remaining,first.value],`duration${row.duration} rate${row.rate}`);
  }
});
test('20 original creation and timer sequences preserve birth/no-countdown and same-frame motion',()=>{
  for(const row of native.sequenceCases){
    initializeBullet(row);
    for(const [index,snapshot] of row.samples.entries()){
      if(index)core._test_tick();
      assertBullet(snapshot,`${JSON.stringify(row.records)} ${snapshot.step}`);
    }
  }
});
test('five original concurrent and duplicate-active-kind sequences preserve repeated motion passes',()=>{
  for(const row of native.activeGateCases){
    initializeBullet(row);
    for(const [index,snapshot] of row.samples.entries()){
      if(index)core._test_tick();
      assertBullet(snapshot,`${JSON.stringify(row.records)} ${snapshot.step}`);
    }
  }
});
test('64 original d0/1/2/-1 ray completion sequences activate and update the next motion on the same frame',()=>{
  for(const row of native.rayRetryCases){
    initializeBullet(row);assertBullet(row.before,`${JSON.stringify(row.records)} birth`);
    core._test_tick();assertBullet(row.after,`${JSON.stringify(row.records)} first-update`);
  }
});
test('20 original ETEX10000000 blend cases update VM flags and preserve rotation',()=>{
  for(const row of blend.cases){
    const pointer=core._test_blend(row.beforeFlags,row.operandC,1.25)/4;
    assert.deepEqual([...core.HEAPU32.subarray(pointer,pointer+4)],
      [row.afterFlags,row.cursor,row.active,0],JSON.stringify(row));
    assert.equal(core.HEAPF32[pointer+4],row.rotationZ,JSON.stringify(row));
  }
});
