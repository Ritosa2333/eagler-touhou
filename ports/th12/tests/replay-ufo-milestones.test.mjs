import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import crypto from 'node:crypto';
import {parseReplay,replayInputEnded} from '../src/replay.mjs';
import {initializeCandidate,restoreStage,state} from '../tools/replay-verifier/capture-candidate.mjs';

const native=JSON.parse(fs.readFileSync(new URL('fixtures/replay-ufo-milestones-native.json',import.meta.url)));

test('production replay preserves native world through UFO defeat, bullet cancellation and the first laser graze',async()=>{
  assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  assert.equal(native.creationGuardPrimed,false);assert.equal(native.wholeGameOracle,false);
  const bytes=fs.readFileSync(new URL('../site/replays/'+native.replay,import.meta.url));
  assert.equal(crypto.createHash('sha256').update(bytes).digest('hex'),native.replaySha256);
  const replay=parseReplay(bytes),stage=replay.stages.find(s=>s.number===1),core=await initializeCandidate(1);
  restoreStage(core,replay,stage);core._th12_oracle_fixture(0);
  const identities=new Map(),occurrences=new Map(),excluded=new Set(native.excludedEnemyRoutines);
  function register(snapshot){
    for(const enemy of snapshot.enemies){
      if(identities.has(enemy.id))continue;
      const context=snapshot.ecl.find(c=>c.enemy===enemy.id),name=(context?.calls?.[0]??context)?.routine??'';
      const occurrence=occurrences.get(name)??0;occurrences.set(name,occurrence+1);
      identities.set(enemy.id,{routine:name,occurrence});
    }
  }
  register(state(core));
  for(let tick=0;tick<=Math.max(...native.records.map(r=>r.inputTick));tick++){
    const input=replay.input(1,tick);assert.equal(replayInputEnded(input),false);
    core._th12_tick(input.held,input.pressed);const snapshot=state(core);register(snapshot);
    const expected=native.records.find(r=>r.inputTick===tick);if(!expected)continue;
    const label='input'+tick,resources=snapshot.economy.resources;
    assert.deepEqual({seed:snapshot.rng.seed,calls:snapshot.rng.calls},expected.rng,label+' RNG');
    assert.deepEqual([snapshot.player.xFixed,snapshot.player.yFixed],expected.player.fixedPosition,label+' position');
    assert.equal(snapshot.player.state,expected.player.state,label+' player state');
    for(const [name,index] of [['power',0],['scoreStored',3],['pivStored',4],['maxPivStored',5],['rank',10]])
      assert.equal(resources[index],expected.economy[name],label+' '+name);
    assert.deepEqual([resources[6],resources[8],resources[7],resources[9]],expected.economy.lifeBombCounters,label+' life/Bomb');
    assert.deepEqual(snapshot.economy.ufoColors,expected.economy.ufoColors,label+' stock');
    const actual=snapshot.enemies.filter(e=>!excluded.has(identities.get(e.id).routine));
    assert.equal(actual.length,expected.combatEnemies.length,label+' combat count');
    for(const actor of expected.combatEnemies){
      const enemy=actual.find(e=>{const key=identities.get(e.id);return key.routine===actor.routine&&key.occurrence===actor.occurrence;});
      assert.ok(enemy,label+' '+actor.routine+'/'+actor.occurrence);
      assert.equal(enemy.life,actor.life,label+' HP');assert.equal(enemy.immunity,actor.hpImmunity,label+' immunity');
      for(let axis=0;axis<2;axis++)assert.ok(Math.abs(enemy.position[axis]-actor.position[axis])<.00002,label+' enemy axis'+axis);
      for(const component of ['absolute','relative'])if(actor[component])
        for(let axis=0;axis<2;axis++)assert.ok(Math.abs(enemy[component][axis]-actor[component][axis])<.00002,label+' '+component+' axis'+axis);
    }
    if(expected.cancelItems){
      const [firstSlot,lastSlot]=expected.cancelItemSlotRange;
      const actualItems=snapshot.items.filter(item=>item[1]>=firstSlot&&item[1]<=lastSlot);
      assert.deepEqual(actualItems.map(item=>item[1]),expected.cancelItems.map(item=>item.slot),label+' cancel item slots');
      for(const item of expected.cancelItems){
        const actual=actualItems.find(candidate=>candidate[1]===item.slot),itemLabel=label+' item'+item.slot;
        for(const [field,index] of [['type',2],['state',3],['age',8],['delay',10]])
          assert.equal(actual[index],item[field],itemLabel+' '+field);
        for(let axis=0;axis<2;axis++){
          assert.ok(Math.abs(actual[4+axis]-item.position[axis])<.00004,itemLabel+' position'+axis);
          assert.ok(Math.abs(actual[6+axis]-item.velocity[axis])<.00004,itemLabel+' velocity'+axis);
        }
      }
    }
  }
});
