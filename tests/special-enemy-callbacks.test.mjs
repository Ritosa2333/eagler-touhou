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
  const out=path.join(root,'artifacts/special-enemy-harness.mjs');
  const names=['reset','set_enemy','set_resources','set_bullet','set_missing','run_callback','fault','affected','private_integer','score_stored','primary_counter','bullet_state','bullet_field','popup_count','popup_state','popup_field','animation_state','animation_flags'];
  const result=spawnSync(python,[emcc,'tests/native/special_enemy_harness.cpp','src/game/SpecialEnemyCallbacks.cpp','-std=c++20','-O2','-o',out,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(n=>'_'+n)),'-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPF32'])],{
    cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python}});
  assert.equal(result.status,0,result.stdout+result.stderr);
  createHarness=(await import(pathToFileURL(out))).default;
  native=JSON.parse(await readFile(path.join(root,'tests/fixtures/special-enemy-callbacks-native.json'),'utf8'));
  assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  assert.equal(native.dependency.sha256,'16fd909aeb68d0d1aca8529dc7f78880b97d6649d70ce8d03a2c858bc28e216b');
  assert.equal(native.cases.length,30);
});
const floats=(h,p,n)=>Array.from(new Float32Array(h.HEAPF32.buffer,p,n));
const near=(actual,expected,where)=>assert.ok(Math.abs(actual-expected)<=.000002,`${where}: ${actual} != ${expected}`);

async function compareCase(c){
  const h=await createHarness();h._reset();
  h._set_enemy(...c.enemyPosition,c.enemyAge,...c.before.privateIntegers);
  h._set_resources(c.before.scoreStored,c.before.primaryBossRewardCounter,c.aliasPrimaryBoss?1:0);
  for(const b of c.before.bullets)h._set_bullet(b.slot,b.objectFlags,b.phase,b.tag,...b.position,...b.velocity,b.nominalSpeed,b.heading);
  for(const [index,step] of c.steps.entries()){
    assert.equal(h._run_callback(c.selector),step.nativeReturn,`${c.name} return ${index}`);
    assert.equal(h._fault(),0,`${c.name} fault ${index}`);
    assert.deepEqual(Array.from({length:4},(_,i)=>h._private_integer(i)),step.after.privateIntegers,`${c.name} private ${index}`);
    assert.equal(h._score_stored(),step.after.scoreStored,`${c.name} score ${index}`);
    assert.equal(h._primary_counter(),step.after.primaryBossRewardCounter,`${c.name} counter ${index}`);
    for(const b of step.after.bullets){
      const actual=floats(h,h._bullet_state(b.slot),8),expected=[...b.position,...b.velocity,b.nominalSpeed,b.heading];
      expected.forEach((v,i)=>near(actual[i],v,`${c.name} tick ${index} bullet ${b.slot} field ${i}`));
      assert.deepEqual([h._bullet_field(b.slot,0)>>>0,h._bullet_field(b.slot,1),h._bullet_field(b.slot,2)],[b.objectFlags,b.phase,b.tag]);
    }
    const vm=floats(h,h._animation_state(),2);
    near(vm[0],step.after.animation.spriteWidth,`${c.name} width ${index}`);
    near(vm[1],step.after.animation.spriteHeight,`${c.name} height ${index}`);
    assert.equal(h._animation_flags()>>>0,step.after.animation.flags);
    assert.deepEqual(step.after.rng,[1234,0],'native callbacks do not consume the game RNG');
  }
  assert.equal(h._popup_count(),c.popups.length);
  c.popups.forEach((p,i)=>{
    const actual=floats(h,h._popup_state(i),5),expected=[...p.screenPosition,...p.scale];
    expected.forEach((v,j)=>near(actual[j],v,`${c.name} popup ${i} field ${j}`));
    assert.deepEqual([h._popup_field(i,0),h._popup_field(i,1),h._popup_field(i,2)>>>0,h._popup_field(i,3),h._popup_field(i,4),h._popup_field(i,5)],
      [p.displayedValue,p.font,p.color,...p.alignment,p.nativeTextFlag]);
    assert.equal(p.format,'%d');
    assert.deepEqual(c.steps[i].after.fontAfter,{color:0xffffffff,scale:[1,1],alignment:[1,1],font:0,nativeTextFlag:0});
  });
  return h;
}

test('native attraction radius, alive/object filters, coincident vector and slot 1999',async()=>{
  const h=await compareCase(native.cases.find(c=>c.name==='attraction_filters_radius_zero_and_last_pool_slot'));
  assert.equal(h._affected(),6);
});
test('native attraction 64-call trajectory preserves nominal speed and velocity Z',async()=>{
  await compareCase(native.cases.find(c=>c.name==='attraction_translated_origin_64_calls'));
});
test('native actual D3DX tiny-vector zero threshold is retained',async()=>{
  await compareCase(native.cases.find(c=>c.name==='attraction_tiny_distance_d3dx_zero_threshold'));
});
test('all 15 native Extra score entries, wrap, cap and popup-only triggers',async()=>{
  for(const c of native.cases.filter(c=>c.name.startsWith('extra_score_')))await compareCase(c);
});
test('native linked line tag, first-slot order, width-only update and age20 retirement',async()=>{
  for(const c of native.cases.filter(c=>c.selector===6))await compareCase(c);
  assert.deepEqual(native.immediateDispatch,{opcode:534,selector:6,enemyAge:20,vmReturn:0,pcOffset:40,
    trace:['0x4178bf','0x41bf50','0x41962b','0x468bea','0x468bea']});
});
test('invalid or absent world objects produce explicit faults without invented rewards',async()=>{
  const h=await createHarness();h._reset();h._set_enemy(0,0,0,0,1,0,0,0);h._set_missing(1,0);
  assert.equal(h._run_callback(2),0);assert.equal(h._fault(),4);assert.equal(h._score_stored(),0);assert.equal(h._popup_count(),0);
  h._set_missing(0,0);h._set_resources(100,15,0);h._run_callback(2);assert.equal(h._fault(),5);assert.equal(h._score_stored(),100);
  h._set_enemy(0,0,0,0,0,0,123,0);h._set_bullet(0,1,1,123,3,4,0,0,0,0,2,0);h._set_missing(0,1);
  h._run_callback(6);assert.equal(h._fault(),8);assert.deepEqual(floats(h,h._animation_state(),2),[13,17]);
  h._run_callback(3);assert.equal(h._fault(),1,'laser selector must be dispatched to its own module');
});
