import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';
import createFixture from '../artifacts/player-weapons-tests/core.mjs';
const native=JSON.parse(fs.readFileSync(new URL('../local/reverse/player-weapons-probe.json',import.meta.url)));
const bombs=JSON.parse(fs.readFileSync(new URL('../local/reverse/bomb-probe.json',import.meta.url)));
const animationOrder=JSON.parse(fs.readFileSync(new URL('../local/reverse/player-anm-order.json',import.meta.url)));
const state=(c,i=0)=>[...new Float32Array(c.HEAPU8.buffer,c._test_state(i),12)];
const source=c=>[...new Float32Array(c.HEAPU8.buffer,c._test_source(),6)];
const close=(a,b,label)=>assert.ok(Math.abs(a-b)<.00003,`${label}: ${a} vs original ${b}`);
test('Sanae B hits create six original particles and consume exactly one native RNG32',async()=>{
 const c=await createFixture();for(const r of native.extraCallback1){c._test_reset(2,r.seed,32,120,-Math.PI/2,10,1,0,11,12,15);assert.equal(c._test_query(32,120,24,24),27);assert.equal(c._test_count(),7);assert.equal(c._test_rng_seed(),r.finalSeed);assert.equal(c._test_rng_calls(),2);
  for(let i=0;i<6;i++){const a=state(c,i+1),b=r.emissions[i];close(a[2],b.angle,'particle angle');close(a[0]+Math.cos(a[2])*3,b.position[0],'particle source x');close(a[1]+Math.sin(a[2])*3,b.position[1],'particle source y');assert.equal(a[5],13);assert.equal(a[9],2);}
  assert.equal(state(c)[4],2);assert.equal(state(c)[5],12);assert.equal(c._test_query(32,120,24,24),12);
 }
});
test('Sanae B particle deceleration and source creation match original 43aa50',async()=>{
 const c=await createFixture();c._test_reset(2,0,0,120,0,3,0,4,13,14,2);
 for(const r of native.particleUpdate){c._test_update();const a=state(c);close(a[3],r.speed,'particle speed '+r.tick);assert.equal(a[4],r.phase);if(r.tick===31){const s=source(c);close(s[0],r.sourceRadius,'initial radius');close(s[1],r.sourceGrowth,'radius growth');assert.equal(s[3],15);assert.equal(s[4],4);assert.equal(s[5],3);assert.equal(a[7],2);assert.equal(a[6],0);}}
});
test('expanding circle sources retain original timer, cadence and center-only collision',async()=>{
 const c=await createFixture();c._test_source_reset();for(const r of native.sources){const s=source(c);close(s[0],r.radius,'circle radius '+r.tick);assert.equal(s[2],r.previous);assert.equal(s[3],r.ticks);assert.equal(s[5],r.flags);assert.equal(c._test_source_damage(0,120,24,24),r.damage);assert.equal(c._test_source_damage(45,120,400,400),r.outsideWithHugeEnemy);assert.equal(c._test_source_damage(r.radius,120,1,1),r.boundary);c._test_source_tick();}
});
test('ordinary and laser rectangles use distinct enemy width and height with inclusive edges',async()=>{
 const c=await createFixture();for(const r of native.rectangles)assert.equal(c._test_rect(r.type,r.x,r.y,r.width,r.height),r.damage?1:0,JSON.stringify(r));
});
test('moving lasers and particles damage only on the original four-tick cadence',async()=>{
 const c=await createFixture();for(const r of native.projectileCadence)assert.equal(c._test_cadence(r.age,r.age-1,r.type),r.damage?1:0,JSON.stringify(r));assert.equal(c._test_cadence(4,4,2),0);
});
test('Marisa B Bomb sweep and damage match 80 original timer/position queries',async()=>{
 const c=await createFixture();for(const r of native.marisaBBomb){assert.equal(c._test_marisa_b_damage(r.age,r.y),r.damage,JSON.stringify(r));const a=[...new Float32Array(c.HEAPU8.buffer,c._test_marisa_b_area(r.age),5)];assert.equal(a[0],r.clearRectangles.length?1:0);if(a[0]){const b=r.clearRectangles[0];close(a[1],b.position[0],'sweep x');close(a[2],b.position[1],'sweep y');close(a[3],b.size[0],'sweep width');close(a[4],b.size[1],'sweep height');}}
});
test('Sanae B clear-radius growth and leaf global RNG match isolated original callbacks',async()=>{
 const c=await createFixture();for(const r of bombs.sanaeB.filter(r=>r.circles.length))close(c._test_sanae_b_radius(r.age),r.circles[0].radius,'Sanae B clear radius');
 c._test_leaf_begin();for(let i=0;i<bombs.leaf.length;i++){const a=[...new Float32Array(c.HEAPU8.buffer,c._test_leaf_state(),5)],b=bombs.leaf[i];assert.equal(a[0],b.age);close(a[1],b.angle,'leaf heading '+i);assert.equal(a[3],b.seed);assert.equal(a[4],b.rng32Calls);if(i+1<bombs.leaf.length)c._test_leaf_tick();}
});
test('Marisa A Bomb uses original three damage bands and separate clear rectangles',async()=>{
 const c=await createFixture();for(const r of bombs.marisaADamage)assert.equal(c._test_marisa_a_damage(r.age,r.x,r.y,r.width,r.height),r.damage,JSON.stringify(r));
 for(let i=0;i<3;i++){const a=[...new Float32Array(c.HEAPU8.buffer,c._test_marisa_a_area(i),4)],b=bombs.marisaAClear[i];assert.deepEqual(a,[...b.position.slice(0,2),...b.size]);}
});
test('Reimu A emitter offsets and lifespan match the original profile4 callback',async()=>{
 const c=await createFixture();c._test_reimu_a_emitter_begin();for(const r of bombs.reimuAEmitter){c._test_reimu_a_emitter_tick();const a=[...new Float32Array(c.HEAPU8.buffer,c._test_reimu_a_emitter_state(),6)];assert.equal(a[0],r.age);assert.deepEqual(a.slice(1,5),r.offsets);assert.equal(a[5],r.result?0:1);}
});
test('Reimu A beam history, hundredth-pixel flooring and rectangular source match original callbacks',async()=>{
 const c=await createFixture();for(const r of bombs.reimuABeams){c._test_reimu_a_beam_begin(r.offset);for(const s of r.samples){const a=[...new Float32Array(c.HEAPU8.buffer,c._test_reimu_a_beam_state(),10)];close(a[0],s.head[0],'beam x');close(a[1],s.head[1],'beam y');close(a[2],s.angle,'beam heading');assert.equal(a[3],s.age);close(a[4],s.sourcePosition[0],'source x');close(a[5],s.sourcePosition[1],'source y');close(a[6],s.sourceAngle,'source rotation');assert.deepEqual(a.slice(7,9),s.sourceSize);assert.equal(a[9],s.sourceFlags);c._test_reimu_a_beam_tick();}}
});
test('Reimu B orbits, staggered release and homing match 600 original updates',async()=>{
 const c=await createFixture();for(const r of bombs.reimuBOrbs){c._test_reimu_b_begin(r.index);for(const s of r.samples){c._test_reimu_b_tick(r.target?1:0);const a=[...new Float32Array(c.HEAPU8.buffer,c._test_reimu_b_state(),11)];close(a[0],s.position[0],`orb x ${r.index}:${s.tick}`);close(a[1],s.position[1],`orb y ${r.index}:${s.tick}`);close(a[2],s.center[0],'orb center x');close(a[3],s.center[1],'orb center y');close(a[4],s.heading,'orb heading');close(a[5],s.radius,'orb radius');close(a[6],s.speed,'orb speed');close(a[7],s.delta[0],'orb delta x');close(a[8],s.delta[1],'orb delta y');assert.equal(a[9],s.flags);assert.equal(a[10],s.age);}}
});
test('Reimu B explosion preserves original burst source, clear radius and old-source retirement',async()=>{
 const c=await createFixture(),r=bombs.reimuBExplosion,a=[...new Float32Array(c.HEAPU8.buffer,c._test_reimu_b_explosion(),8)];assert.deepEqual(a,[r.circles[0].radius,r.newSource.radius,r.newSource.growth,r.newSource.ticks,r.newSource.damage,r.newSource.flags,r.oldSourceFlags,r.orbActive]);
});
test('Sanae A Bomb full-field clear, 14 damage and handle termination match original samples',async()=>{
 const c=await createFixture();for(const r of bombs.sanaeADamage)assert.equal(c._test_sanae_a_damage(r.y),r.damage);c._test_sanae_a_begin();for(let age=0;age<=241;++age){const a=[...new Float32Array(c.HEAPU8.buffer,c._test_bomb_tick(),5)],r=bombs.sanaeATiming.find(r=>r.age===age);if(!r)continue;assert.equal(a[1],r.result===0?1:0);assert.equal(a[2],r.iframes);assert.equal(a[3],r.clearRectangles.length);if(age===240)assert.equal(a[4],1);}
});
test('a new Bomb preserves unfinished game-RNG leaves and old beam source updates',async()=>{
 const c=await createFixture(),a=[...new Float32Array(c.HEAPU8.buffer,c._test_effect_restart(),7)];assert.equal(a[0],1);assert.equal(a[1],bombs.leaf[1].age);assert.equal(a[2],bombs.leaf[1].seed);assert.equal(a[3],bombs.leaf[1].rng32Calls);assert.equal(a[4],1);close(a[5],bombs.reimuABeams[0].samples[1].head[1],'retained beam head');close(a[6],bombs.reimuABeams[0].samples[1].sourceAngle,'retained source rotation');
});
test('Sanae B leaves match original primary-list creation order and game RNG assignments',async()=>{
 const c=await createFixture(),r=animationOrder.records.find(r=>r.scenario==='sanaeBLeaves');c._test_leaf_order_begin();for(const f of r.frames){if(f.frame>=0)c._test_leaf_order_tick(f.frame===1);assert.equal(c._test_rng_seed(),f.rng[0]);assert.equal(c._test_rng_calls(),f.rng[1]);assert.equal(new Float32Array(c.HEAPU8.buffer,c._test_leaf_order_state(-1),1)[0],f.list.length);for(let i=0;i<f.list.length;++i){const a=[...new Float32Array(c.HEAPU8.buffer,c._test_leaf_order_state(i),3)],n=f.list[i];assert.equal(a[0],n.id);assert.equal(a[1],n.leafAge);close(a[2],n.leafHeading,'registered leaf heading');}}
});
