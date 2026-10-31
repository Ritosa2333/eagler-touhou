import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),
 dir=path.join(root,'artifacts/last-enemy-id-tests'),temp=path.join(dir,'tmp');
fs.mkdirSync(temp,{recursive:true});
const python=emscripten(root).python,
 compiler=emscripten(root).compiler,output=path.join(dir,'core.mjs');
const built=spawnSync(python,[compiler,'tests/fixtures/last-enemy-id.cpp',...runtimeSources(root),
 '-std=c++20','-O0','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432',
 '-sEXPORTED_FUNCTIONS=["_last_enemy_setup","_last_enemy_tick","_last_enemy_registered","_last_enemy_next","_last_enemy_get_int","_last_enemy_get_float","_th12_load_ecl","_th12_state","_th12_state_size","_malloc","_free"]',
 '-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],{cwd:root,encoding:'utf8',env:{...process.env,
  EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,EMCC_TEMP_DIR:temp,
  PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default,
 native=JSON.parse(fs.readFileSync(new URL('fixtures/last-enemy-id-original.json',import.meta.url)));
const snapshot=c=>{const at=c._th12_state(),size=c._th12_state_size();
 return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(at,at+size)));};
function load(c,hex){
 const raw=Buffer.from(hex,'hex'),at=c._malloc(raw.length);c.HEAPU8.set(raw,at);
 try{assert.equal(c._th12_load_ecl(at,raw.length),1);}finally{c._free(at);}
}
test('Native last-successful-registration reference survives consecutive and nested constructors and gates ECL455',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(native.records.length,7);
 assert.deepEqual(native.records.filter(row=>row.name.startsWith('anchor-lifetime-poll'))
  .map(row=>row.difficulty),[0,1,2,3],'four real native difficulty configurations');
 for(const row of native.records){
  const c=await create();load(c,row.syntheticEclHex);c._last_enemy_setup(row.seed,row.difficulty);
  for(const expected of row.frames){
   if(expected.inputTick>=0)c._last_enemy_tick();
   const actual=snapshot(c),label=row.name+' input'+expected.inputTick;
   assert.equal(expected.nativeDifficulty,row.difficulty,label+' captured native difficulty');
   assert.equal(c._last_enemy_registered()>>>0,expected.lastRegisteredId,label+' last registered ID');
   assert.equal(c._last_enemy_next()>>>0,expected.nextId,label+' next allocation ID');
   assert.deepEqual([actual.rng.seed,actual.rng.calls],expected.gameSeedCounter,label+' original RNG');
   assert.deepEqual(actual.enemies.map(e=>e.id),expected.enemies.map(e=>e.id),label+' live linked actors');
   for(const enemy of expected.enemies){
    assert.equal(enemy.difficultyMask,1<<row.difficulty,label+' actual ctor difficulty mask');
    const e=actual.enemies.find(e=>e.id===enemy.id),ctx=actual.ecl.find(e=>e.enemy===enemy.id);
    assert.deepEqual([ctx.routine,ctx.byteOffset,ctx.time],[enemy.routine,enemy.pcOffset,enemy.time],label+' context'+enemy.id);
    const locals=new Map(ctx.locals);
    assert.deepEqual(Array.from({length:4},(_,i)=>locals.get(i)??0),enemy.localsWords,label+' real locals'+enemy.id);
    assert.deepEqual(e.position.map(Math.fround),enemy.position,label+' movement position'+enemy.id);
   }
   assert.equal(actual.fixtureFlags,0,label+' no diagnostic mask');
   assert.deepEqual(actual.referenceFaults,[],label+' reference faults');
   assert.deepEqual(actual.runtimeFaults,[],label+' runtime faults');
  }
  if(row.name.startsWith('anchor-lifetime-poll-while-moving')){
   const births=row.actualEntries.filter(e=>e.entry==='0x412b70');
   assert.deepEqual(births.map(e=>[e.inputTick,e.registeredId]),[[-1,1],[-1,2],[6,3],[14,4]],'actual linked lifetime gates subsequent anchors');
   for(const frame of row.frames)assert.ok(frame.enemies.filter(e=>e.routine==='Anchor').length<=1,'never overlap live anchors');
  }
 }
});
test('Native last-ID integer and float getters use raw int32 and signed-fild float conversion',async()=>{
 const c=await create();load(c,native.records[0].syntheticEclHex);c._last_enemy_setup(1234,1);
 for(const row of native.getterCases){
  assert.equal(c._last_enemy_get_int(row.managerWord)>>>0,row.integerWord,'raw int word '+row.managerWord);
  assert.equal(c._last_enemy_get_float(row.managerWord),row.floatValue,'signed-fild float '+row.managerWord);
 }
 // Restarting the actual production session resets last-ID state before its
 // first constructor; it cannot retain the controlled previous high ID.
 c._last_enemy_setup(1234,1);
 const main=snapshot(c).ecl.find(e=>e.routine==='main');
 assert.equal(new Map(main.locals).get(0)??0,0,'fresh first constructor sees last-ID0');
});
