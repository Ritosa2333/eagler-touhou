import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {initializeCandidate,restoreStage,state} from '../tools/replay-verifier/capture-candidate.mjs';
test('ECL275 forwards the original handle IRQ to the laser root and immediate children',async()=>{
 const o=JSON.parse(fs.readFileSync(new URL('fixtures/enemy-anm-irq-original.json',import.meta.url)));
 assert.equal(o.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');assert.equal(o.rows.length,43);
 const c=await initializeCandidate(6),raw=Buffer.from(o.mainBase64,'base64'),at=c._malloc(raw.length);
 c.HEAPU8.set(raw,at);assert.equal(c._th12_load_ecl(at,raw.length),1);c._free(at);
 restoreStage(c,o,{number:6,seed:o.seed,initial:o.initial});
 for(const expected of o.rows){
  if(expected.tick>=0)c._th12_tick(0,0);
  const s=state(c),nodes=s.animations.nodes.filter(n=>n.bank===2&&n.owner===1).sort((a,b)=>b.script-a.script);
  const native=[...expected.nodes].sort((a,b)=>b.script-a.script);
  assert.deepEqual([s.rng.seed,s.rng.calls],expected.rng,`RNG/${expected.tick}`);
  assert.equal(nodes.length,native.length,`nodes/${expected.tick}`);
  for(let i=0;i<nodes.length;i++){
   const a=nodes[i],b=native[i],label=`${expected.tick}/script${b.script}`;
   assert.equal(a.script,b.script,label);assert.equal(a.clock,b.clock,label+' clock');
   assert.equal(a.render[0]>>>0,b.primary>>>0,label+' color');
   // Native x87 retains interpolation intermediates until the final store;
   // allow its sub-pixel float rounding, while clocks/colors stay exact.
   for(let axis=0;axis<2;axis++)assert.ok(Math.abs(a.pose[12+axis]-b.scale[axis])<=4e-7*Math.max(1,Math.abs(b.scale[axis])),label+' scale'+axis);
   if(b.script!==69)assert.equal(a.render[4],b.sprite,label+' sprite');
  }
 }
});
