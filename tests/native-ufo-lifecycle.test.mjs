// Research diagnostics only: actual retail x86, no candidate behavior/golden acceptance.
import test from 'node:test';
import assert from 'node:assert/strict';
import path from 'node:path';
import {readFile} from 'node:fs/promises';
import {createHash} from 'node:crypto';
import {spawnSync} from 'node:child_process';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const digest=async file=>createHash('sha256').update(await readFile(file)).digest('hex');
test('original UFO lifecycle probe keeps death, escape, HUD, quota and source paths distinct',async()=>{
  const watched=['src/core.cpp','src/items.hpp','src/ufo.hpp'].map(f=>path.join(root,f));watched.push(path.join(workspace,'th12/th12.exe'));
  const before=await Promise.all(watched.map(digest));
  const run=spawnSync(path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),['scripts/reverse/inspect-ufo-lifecycle.py'],{cwd:root,encoding:'utf8'});
  assert.equal(run.status,0,run.stdout+run.stderr);
  const result=JSON.parse(await readFile(path.join(root,'artifacts/reverse/ufo-lifecycle-inspection.json'),'utf8'));
  assert.equal(result.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');assert.equal(result.partial,true);assert.equal(result.wholeGameOracle,false);assert.ok(result.cases.length>=41);
  const get=name=>{const found=result.cases.find(c=>c.name===name);assert.ok(found,name);return found;};
  const summoned=get('third_token_manager19_summon');assert.equal(summoned.before.manager.enemyId,0);assert.equal(summoned.after.manager.age,1);assert.equal(summoned.after.enemy.health,1900);assert.deepEqual(summoned.after.inventory.slice(0,3),[1,2,3]);
  const mismatch=get('inventory_mismatch_hud_rotation');assert.deepEqual(mismatch.extra.map(x=>x.hudRotation),[0,0,1,2,2]);
  const dialogue=get('dialogue_forced_death_callback');assert.ok(dialogue.calls.some(c=>c.entry==='0x44a890'));assert.deepEqual(dialogue.events.filter(e=>e.kind==='itemSpawn').map(e=>e.type),[4,13]);assert.equal(dialogue.after.manager.enemyId,0);
  const escaped=get('natural_escape_without_hurt');assert.equal(escaped.after.manager.enemyId,0);assert.ok(!escaped.calls.some(c=>c.entry==='0x44af00'));assert.ok(!escaped.events.some(e=>e.kind==='itemSpawn'));
  for(const [age,expected] of [[419,-999],[420,999],[421,123]])assert.equal(get('escape_signal_'+age).after.enemy.private0,expected);
  assert.equal(get('escape_signal_100_boss').after.enemy.private0,999);
  for(const [name,irq] of [['token_hint_(1, 1)_10',7],['token_hint_(2, 2)_11',8],['token_hint_(3, 3)_12',9],['token_hint_(1, 2)_12',10],['token_hint_(1, 3)_11',10],['token_hint_(2, 3)_10',10]]){
    const c=get(name),shown=c.extra.shown.items[0];assert.ok(shown.hintHandle);assert.equal(c.extra.persisted.items[0].hintHandle,shown.hintHandle);assert.equal(c.after.items[0].hintHandle,0);
    assert.ok(c.extra.shown.animations.some(vm=>vm.bankScriptSprite[1]===211&&vm.interrupt===irq));
  }
  assert.equal(get('full_inventory_collect_2000000').after.pivStored,2100000);assert.equal(get('full_inventory_collect_39950000').after.pivStored,40000000);
  const quota=get('full_token_pool');assert.equal(quota.extra.lastReturn,quota.extra.poolEnd);assert.equal(quota.extra.sideCounter,17);assert.equal(quota.after.items.length,16);
  const adjacent=get('aggregate_reward_full_token_pool').extra.adjacentSmallPointSlot0;assert.deepEqual(adjacent.stateType,[0,0]);assert.deepEqual(adjacent.aggregateKindBucketsMultiplier,[1,3,5,1]);
  const hpExpected=[80,16,16,80,80,2,80,80],sourceCases=result.cases.filter(c=>c.name.startsWith('ufo_source_spell_'));assert.equal(sourceCases.length,8);
  for(let i=0;i<8;i++){assert.equal(sourceCases[i].extra.healthDelta,hpExpected[i]);assert.equal(sourceCases[i].extra.sourceAccumulatedDamage,80);}
  assert.deepEqual(await Promise.all(watched.map(digest)),before,'research must preserve original and candidate files');
});
