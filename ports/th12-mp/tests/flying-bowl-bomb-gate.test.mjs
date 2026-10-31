import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {initializeCandidate, restoreStage, state} from '../tools/replay-verifier/capture-candidate.mjs';

test('Flying Bowl stops hurt queries during an actual Bomb, then restores them without RNG or cancellation drift', async () => {
 const original=JSON.parse(fs.readFileSync(new URL('./fixtures/flying-bowl-bomb-gate-original.json',import.meta.url)));
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(original.rows.length,741);
 const core=await initializeCandidate(6),bytes=Buffer.from(original.mainBase64,'base64'),at=core._malloc(bytes.length);
 try {core.HEAPU8.set(bytes,at);assert.equal(core._th12_load_ecl(at,bytes.length),1);} finally {core._free(at);}
 restoreStage(core,original,{number:6,seed:original.seed,initial:original.initial});
 for(const expected of original.rows){
  if(expected.tick>=0)core._th12_tick(expected.tick===380?3:1,0);
  const s=state(core),e=s.enemies.find(e=>!e.hidden&&e.active);
  assert.ok(e,`boss/${expected.tick}`);
  const actual={tick:expected.tick,rng:[s.rng.seed,s.rng.calls],health:e.life,graze:s.player.graze,
   score:s.economy.resources[3],bombActive:s.player.bombState.active,bombFlags:e.flags&0x18000001};
  assert.deepEqual(actual,Object.fromEntries(Object.entries(expected).filter(([key])=>key!=='bullets')),`tick${expected.tick}`);
  if(expected.bullets){
   const bullets=s.bullets.filter(b=>!b.friendly&&b.active);
   assert.equal(bullets.length,expected.bullets.length,`bullet count/${expected.tick}`);
   for(const b of expected.bullets){
    const c=bullets.find(c=>c.physicalSlot===b.slot);assert.ok(c,`slot${b.slot}/${expected.tick}`);
    assert.deepEqual([c.type,c.color],b.typeColor);
    assert.equal(c.hostile[2],b.state);
    assert.equal(c.hostile[10],b.flags);
    for(let axis=0;axis<2;axis++)assert.ok(Math.abs(c.position[axis]-b.position[axis])<=4e-5,`position/${expected.tick}/${b.slot}/${axis}`);
   }
  }
 }
 const entering=original.rows[381],lastActive=original.rows[660],leaving=original.rows[661];
 assert.equal(entering.bombActive,1);assert.equal(leaving.bombActive,0);
 assert.equal(entering.health,lastActive.health);
 assert.equal(entering.score,lastActive.score);
 assert.equal(leaving.bombFlags&0x10000001,0);
});
