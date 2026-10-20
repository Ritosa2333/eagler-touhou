import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {initializeCandidate,restoreStage,state} from '../tools/replay-verifier/capture-candidate.mjs';

for(let stage=1;stage<=7;stage++)test(`stage${stage} retail BossDead explosion, RNG, clear, Spell end and rate restoration`,async()=>{
 const fixture=JSON.parse(fs.readFileSync(new URL(`fixtures/boss-death-stage${String(stage).padStart(2,'0')}-original.json`,import.meta.url)));
 assert.equal(fixture.stage,stage);
 assert.equal(fixture.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(fixture.wholeGameOracle,false);
 assert.equal(fixture.rows.length,stage>=6?341:81);
 const core=await initializeCandidate(stage),bytes=Buffer.from(fixture.mainBase64,'base64'),at=core._malloc(bytes.length);
 try{core.HEAPU8.set(bytes,at);assert.equal(core._th12_load_ecl(at,bytes.length),1);}finally{core._free(at);}
 restoreStage(core,fixture,{number:stage,seed:fixture.seed,initial:fixture.initial});
 for(const row of fixture.rows){
  if(row.tick>=0)core._th12_tick(0,0);
  const s=state(core),spell=s.spellState;
  assert.deepEqual({tick:row.tick,rng:[s.rng.seed,s.rng.calls],rate:s.gameSpeed,
   score:s.economy.resources[3],spellAge:spell.age,serialTicks:spell.serialTicks,spellFlags:spell.flags},
   Object.fromEntries(Object.entries(row).filter(([key])=>!['age','position','effects'].includes(key))),`stage${stage}/tick${row.tick}`);
  if(row.position){const boss=s.enemies.find(e=>e.life===13200);assert.ok(boss);assert.equal(boss.age,row.age);
   for(let axis=0;axis<2;axis++)assert.ok(Math.abs(boss.position[axis]-row.position[axis])<=4e-5,`position/${row.tick}/${axis}`);
  }
  if(row.effects){const effects=s.animations.nodes.filter(n=>n.alive&&n.bank===0&&[79,129,130].includes(n.script))
   .map(n=>[n.script,n.clock]).sort((a,b)=>a[0]-b[0]||a[1]-b[1]);assert.deepEqual(effects,row.effects,`effects/${row.tick}`);}
 }
 assert.equal(state(core).gameSpeed,1);
});
