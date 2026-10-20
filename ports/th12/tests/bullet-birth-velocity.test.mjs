import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import crypto from 'node:crypto';
import {parseReplay} from '../src/replay.mjs';
import {initializeCandidate,restoreStage,state} from '../tools/replay-verifier/capture-candidate.mjs';

const native=JSON.parse(fs.readFileSync(new URL('fixtures/bullet-birth-velocity-native.json',import.meta.url)));
const milestones=JSON.parse(fs.readFileSync(new URL('fixtures/world-milestones-th12_14-native.json',import.meta.url)));
test('retail replay startup, friendly physical slots and raw-angle bullet births match selected native milestones',async()=>{
  const bytes=fs.readFileSync(new URL('../site/replays/'+native.replay,import.meta.url));
  assert.equal(crypto.createHash('sha256').update(bytes).digest('hex'),native.replaySha256);
  assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  assert.equal(milestones.replaySha256,native.replaySha256);assert.equal(milestones.creationGuardPrimed,false);
  const replay=parseReplay(bytes),stage=replay.stages.find(s=>s.number===1),core=await initializeCandidate(1);
  restoreStage(core,replay,stage);core._th12_oracle_fixture(0);
  const last=Math.max(...[...native.records,...milestones.records].map(r=>r.inputTick));
  for(let tick=0;tick<=last;tick++){
    const input=replay.input(1,tick);core._th12_tick(input.held,input.pressed);
    const samples=native.records.filter(r=>r.inputTick===tick),milestone=milestones.records.find(r=>r.inputTick===tick);if(!samples.length&&!milestone)continue;
    const snapshot=state(core);
    if(milestone){
      assert.equal(snapshot.rng.seed,milestone.rngSeed,'RNG seed input'+tick);assert.equal(snapshot.rng.calls,milestone.rngCalls,'RNG calls input'+tick);
      assert.deepEqual([snapshot.player.xFixed,snapshot.player.yFixed],milestone.playerFixed);
      for(const enemy of milestone.enemies){
        const actor=snapshot.enemies.find(e=>e.id===enemy.id);assert.ok(actor,'native enemy'+enemy.id+' input'+tick);assert.equal(actor.life,enemy.life);
        for(let axis=0;axis<2;axis++)assert.ok(Math.abs(actor.position[axis]-enemy.position[axis])<.00002);
      }
      for(const shot of milestone.friendly){
        const bullet=snapshot.bullets.find(b=>b.friendly&&b.physicalSlot===shot.slot);assert.ok(bullet,'native friendly slot'+shot.slot);
        assert.equal(bullet.age,shot.age);assert.equal(bullet.weapon[0],shot.phase);assert.equal(bullet.weapon[4],shot.damage);assert.equal(bullet.weapon[2],shot.hitLastFrame,'slot'+shot.slot+' input'+tick);
        for(let axis=0;axis<2;axis++)assert.ok(Math.abs(bullet.position[axis]-shot.position[axis])<.00004);
      }
    }
    for(const sample of samples){
      const bullet=snapshot.bullets.find(b=>!b.friendly&&b.physicalSlot===sample.physicalSlot);
      assert.ok(bullet,'native slot '+sample.physicalSlot+' on input '+tick);
      assert.equal(bullet.age,1);assert.deepEqual([bullet.type,bullet.color],sample.typeColor);
      assert.ok(Math.abs(bullet.motion[0]-sample.storedAngle)<1e-7);
      for(let axis=0;axis<2;axis++)assert.ok(Math.abs(bullet.hostile[axis]-sample.velocity[axis])<1e-7,
        `input${tick} slot${sample.physicalSlot} axis${axis}: ${bullet.hostile[axis]} versus native ${sample.velocity[axis]}`);
    }
  }
});
