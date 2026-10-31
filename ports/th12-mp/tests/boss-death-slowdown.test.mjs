import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {initializeCandidate,restoreStage,state} from '../tools/replay-verifier/capture-candidate.mjs';

test('retail BossDead preserves half/quarter speed, delayed result, explosion clocks and speed restoration', async()=>{
 const original=JSON.parse(fs.readFileSync(new URL('./fixtures/boss-death-slowdown-original.json',import.meta.url)));
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(original.rows.length,341);
 const core=await initializeCandidate(6),bytes=Buffer.from(original.mainBase64,'base64'),at=core._malloc(bytes.length);
 try {core.HEAPU8.set(bytes,at);assert.equal(core._th12_load_ecl(at,bytes.length),1);} finally {core._free(at);}
 restoreStage(core,original,{number:6,seed:original.seed,initial:original.initial});
 for(const row of original.rows){
  if(row.tick>=0)core._th12_tick(0,0);
  const s=state(core),spell=s.spellState;
  const actual={tick:row.tick,rng:[s.rng.seed,s.rng.calls],rate:s.gameSpeed,score:s.economy.resources[3],
   spellAge:spell.age,serialTicks:spell.serialTicks,spellFlags:spell.flags};
  assert.deepEqual(actual,Object.fromEntries(Object.entries(row).filter(([key])=>!['age','position','effects'].includes(key))),`tick${row.tick}`);
  if(row.position){
   const e=s.enemies.find(e=>e.life===13200);assert.ok(e,`boss/${row.tick}`);
   assert.equal(e.age,row.age,`age/${row.tick}`);
   for(let axis=0;axis<2;axis++)assert.ok(Math.abs(e.position[axis]-row.position[axis])<=4e-5,`position/${row.tick}/${axis}`);
  }
  if(row.effects){
   const effects=s.animations.nodes.filter(n=>n.alive&&n.bank===0&&[79,129,130].includes(n.script)).map(n=>[n.script,n.clock]).sort((a,b)=>a[0]-b[0]||a[1]-b[1]);
   assert.deepEqual(effects,row.effects,`effects/${row.tick}`);
  }
 }
 assert.equal(state(core).spellState.lastResult.displayElapsedTicks,120);
 core._th12_select_stage(6);assert.equal(state(core).gameSpeed,1);
});
