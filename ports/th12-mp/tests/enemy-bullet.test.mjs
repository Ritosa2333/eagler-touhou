import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';
import createFixture from '../artifacts/bullet-tests/core.mjs';
const native=JSON.parse(fs.readFileSync(new URL('../local/reverse/bullet-probe.json',import.meta.url)));
const state=c=>[...new Float32Array(c.HEAPU8.buffer,c._test_state(),18)];
const close=(a,b,label)=>assert.ok(Math.abs(a-b)<.00004,`${label}: ${a} vs original ${b}`);
function reset(c,initial,records,type=0){c._test_reset(initial.x,initial.y,initial.angle,initial.speed,type);for(let i=0;i<records.length;i++){const r=records[i];assert.equal(c._test_set(i,r.concurrent?1:0,r.kind,r.c,r.d,r.a,r.b),1);}c._test_initialize();}
test('TH12 ordinary rectangles and round types match original geometry queries',async()=>{
  const c=await createFixture();for(const r of native.collision){const type=r.shape==='circle'?26:r.size===10?17:0;assert.equal(c._test_collide(type,r.x,r.y,r.playerHit),r.result,JSON.stringify(r));}
});
test('TH12 vector acceleration, polar change and initial speed-up match original motion callbacks',async()=>{
  const c=await createFixture();for(const r of native.motion.filter(r=>[1,4,8].includes(r.record.kind))){reset(c,r.initial,[r.record]);
    for(let frame=0;frame<r.samples.length;frame++){
      const a=state(c),b=r.samples[frame];close(a[2],b.velocity[0],`velocity x ${r.record.kind}:${frame}`);close(a[3],b.velocity[1],`velocity y ${r.record.kind}:${frame}`);close(a[4],b.speed,`speed ${r.record.kind}:${frame}`);close(a[5],b.angle,`angle ${r.record.kind}:${frame}`);assert.equal(c._test_active(),b.active);
      if(frame+1<r.samples.length)c._test_tick();
    }
  }
});
test('TH12 bounce mask 16 detects an edge without reflecting; ordinary masks reflect',async()=>{
  const c=await createFixture();for(const r of native.motion.filter(r=>r.record.kind===256)){reset(c,r.initial,[r.record]);c._test_tick();const a=state(c),b=r.samples[1];close(a[0]-a[2],b.position[0],'pre-movement reflected x');close(a[1]-a[3],b.position[1],'pre-movement reflected y');close(a[5],b.angle,'reflected angle');assert.equal(c._test_active(),b.active);}
});
test('TH12 80000/100000 pair decodes original packed child emitter and start index',async()=>{
  const c=await createFixture();reset(c,{x:0,y:120,angle:0,speed:3},native.split.records);assert.equal(state(c)[16],1);const a=[...new Float32Array(c.HEAPU8.buffer,c._test_emission(),13)],b=native.split.emissions[0];
  assert.deepEqual(a.slice(0,2),[b.type,b.color]);for(let i=0;i<2;i++)close(a[i+2],b.position[i],'child position');for(let i=0;i<2;i++)close(a[i+4],b.angleSpread[i],'child angle/spread');for(let i=0;i<2;i++)close(a[i+6],b.speedSlow[i],'child speed/slow');assert.deepEqual(a.slice(8,12),[...b.countsMode,b.first]);assert.equal(a[12],b.flags);assert.equal(state(c)[6],native.split.after.cursor);
});
test('TH12 first-stage wait/protection completes before sequential acceleration begins',async()=>{
  const c=await createFixture();for(const r of native.sequences){reset(c,r.initial,r.records);for(let frame=0;frame<r.samples.length;frame++){
    const a=state(c),b=r.samples[frame];close(a[0],b.position[0],`sequence x ${r.records[0].kind}:${frame}`);close(a[1],b.position[1],`sequence y ${r.records[0].kind}:${frame}`);close(a[2],b.velocity[0],`sequence vx ${frame}`);close(a[4],b.speed,`sequence speed ${frame}`);assert.equal(c._test_active(),b.active);assert.equal(a[6],b.cursor);if(frame+1<r.samples.length)c._test_tick();
  }}
});
test('spawn variants retain original 8/15/25 timing and cancel disables collision',async()=>{
  const c=await createFixture();for(let variant=0;variant<3;variant++){reset(c,{x:0,y:120,angle:0,speed:0},[{kind:2,c:variant,d:0,a:0,b:0}]);assert.equal(state(c)[12],[8,15,25][variant]);assert.equal(c._test_current_collision(),0);for(let i=0;i<8;i++)c._test_tick();assert.equal(c._test_current_collision(),0);c._test_tick();assert.equal(c._test_current_collision(),1);c._test_cancel();assert.equal(c._test_current_collision(),0);for(let i=0;i<8;i++)c._test_tick();assert.equal(state(c)[7],0);}
});
