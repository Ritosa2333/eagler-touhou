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
  const out=path.join(root,'artifacts/special-geometry-harness.mjs');
  const names=['reset','set_enemy','set_animation','set_answer','set_player','run_hurt','hurt_total','query_count','hurt_fault','run_collision','collision_fault','circle_code','line_code','graze_count','line_player_state','final_player_state','query_state','circle_state','line_state','graze_state','animation_state','animation_flags','order_count','order_kind','set_missing_animation'];
  const result=spawnSync(python,[emcc,'tests/native/special_geometry_harness.cpp','src/game/SpecialEnemyGeometry.cpp','-std=c++20','-O2','-o',out,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(n=>'_'+n)),'-sEXPORTED_RUNTIME_METHODS='+JSON.stringify(['HEAPF32'])],{
    cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python}});
  assert.equal(result.status,0,result.stdout+result.stderr);
  createHarness=(await import(pathToFileURL(out))).default;
  native=JSON.parse(await readFile(path.join(root,'tests/fixtures/special-enemy-geometry-native.json'),'utf8'));
  assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  assert.equal(native.hurtCases.length,21);assert.equal(native.collisionCases.length,61);
});
const floats=(h,p,n)=>Array.from(new Float32Array(h.HEAPF32.buffer,p,n));
const near=(a,b,where)=>assert.ok(Math.abs(a-b)<=.00002,`${where}: ${a} != ${b}`);
const nearList=(a,b,where)=>b.forEach((v,i)=>near(a[i],v,`${where}/${i}`));
const setupAnimation=(h,a)=>h._set_animation(a.rotationX,a.rotationZ,a.scaleY,a.spriteWidth,a.spriteHeight,a.flags);

async function compareHurt(c){
  const h=await createHarness();h._reset();
  const input=c.input;h._set_enemy(...input.position,input.health,input.maximumHealth,0,0,0,0);setupAnimation(h,input.animation);
  // Foreign query replies come from actual retail execution. This tests the
  // recovered geometry and query ordering, not the core damage bridge.
  c.queries.forEach((q,i)=>h._set_answer(i,q.nativeReturn));
  assert.equal(h._run_hurt(),c.damage,c.name);assert.equal(h._hurt_fault(),0);
  assert.equal(h._hurt_total(),c.queries.reduce((sum,q)=>sum+q.nativeReturn,0));
  assert.equal(h._query_count(),31,`${c.name} must not stop when cap80 is reached`);
  c.queries.forEach((q,i)=>nearList(floats(h,h._query_state(i),6),[...q.position,...q.size,i],`${c.name} query${i}`));
  const a=c.animationAfter;
  nearList(floats(h,h._animation_state(),5),[a.rotationX,a.rotationZ,a.scaleY,a.spriteWidth,a.spriteHeight],`${c.name} animation`);
  assert.equal(h._animation_flags()>>>0,a.flags);
}

test('native 53031-area geometry preserves HP arc, ANM rotation and scale radius',async()=>{
  for(const c of native.hurtCases)await compareHurt(c);
});
test('native 530 compression keeps 96→69 / 97→54 discontinuity and cap80',()=>{
  const damage=total=>native.hurtCases.find(c=>c.name===`curve_total_${total}`).damage;
  assert.deepEqual([damage(29),damage(30),damage(96),damage(97),damage(313),damage(314),damage(2480)],[29,30,69,54,79,80,80]);
});
test('real native damage queries demonstrate repeated score, Source exhaustion and one-shot consumption',()=>{
  const find=name=>native.hurtCases.find(c=>c.name===name);
  const source=find('actual_source_damage2_limit999999');assert.equal(source.damage,49);assert.equal(source.after.scoreStored,31);assert.equal(source.after.source.accumulatedDamage,62);
  const exhausted=find('actual_source_damage10_limit25');assert.equal(exhausted.damage,30);assert.equal(exhausted.after.scoreStored,3);assert.equal(exhausted.after.source.damage,0);assert.equal(exhausted.queries.length,31);
  const cap=find('actual_source_damage100_limit999999');assert.equal(cap.damage,80);assert.equal(cap.after.scoreStored,31);assert.equal(cap.after.source.accumulatedDamage,3100);
  const shot=find('actual_friendly_shot_consumed_once');assert.equal(shot.damage,7);assert.equal(shot.queries.filter(q=>q.nativeReturn>0).length,1);assert.equal(shot.after.friendlyPhase,2);
});
test('native 531 circle then directional line retain query/graze order and Player effects',async()=>{
  for(const c of native.collisionCases){
    const h=await createHarness();h._reset();const input=c.input;
    h._set_enemy(...input.position,0,0,input.flags,input.age,input.privateFloat0,input.privateFloat1);setupAnimation(h,input.animation);
    h._set_player(...input.playerPosition,c.before.playerState,c.queries[0].after.playerState,c.queries[1].after.playerState,c.queries[0].nativeReturn,c.queries[1].nativeReturn);
    assert.equal(h._run_collision(),0,c.name);assert.equal(h._collision_fault(),0);
    assert.equal(h._circle_code(),c.queries[0].nativeReturn);assert.equal(h._line_code(),c.queries[1].nativeReturn);
    nearList(floats(h,h._circle_state(),4),[...c.queries[0].position,c.queries[0].radius],`${c.name} circle`);
    nearList(floats(h,h._line_state(),6),[...c.queries[1].position,c.queries[1].angle,c.queries[1].transverseSize,c.queries[1].longitudinalSize],`${c.name} line`);
    assert.equal(h._line_player_state(),c.queries[1].before.playerState,c.name);
    assert.equal(h._final_player_state(),c.after.playerState,c.name);
    assert.equal(h._graze_count(),c.grazes.length,c.name);
    c.grazes.forEach((p,i)=>nearList(floats(h,h._graze_state(i),3),p.position,`${c.name} graze${i}`));
    const order=[1,...c.grazes.filter(g=>g.afterCircleQuery).map(()=>2),3,...c.grazes.filter(g=>!g.afterCircleQuery).map(()=>2)];
    assert.deepEqual(Array.from({length:h._order_count()},(_,i)=>h._order_kind(i)),order,c.name);
    assert.equal(h._animation_flags()>>>0,c.animationAfter.flags);
  }
});
test('actual native circle hit can start state4 before the line grazes in the same callback',()=>{
  const c=native.collisionCases.find(c=>c.name==='actual_angle0_at50_20');
  assert.equal(c.queries[0].nativeReturn,1);assert.equal(c.queries[0].after.playerState,4);
  assert.equal(c.queries[1].nativeReturn,2);assert.equal(c.misses.length,1);assert.equal(c.grazes.length,1);
});
test('missing animation and zero maximum health report undefined native boundaries',async()=>{
  const h=await createHarness();h._reset();h._set_enemy(0,0,0,1,0,0,0,0,0);
  assert.equal(h._run_hurt(),0);assert.equal(h._hurt_fault(),2);assert.equal(h._query_count(),0);
  h._set_missing_animation();h._run_hurt();assert.equal(h._hurt_fault(),1);
  h._run_collision();assert.equal(h._collision_fault(),1);assert.equal(h._order_count(),0);
});
