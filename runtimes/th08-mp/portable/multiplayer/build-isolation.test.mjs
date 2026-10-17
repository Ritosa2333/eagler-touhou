import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';

const root=resolve(import.meta.dirname,'../..');
function plan(args){return JSON.parse(execFileSync(process.execPath,[resolve(root,'portable/build.mjs'),'--th08','--print-plan',...args],{encoding:'utf8'}));}
function exports(profile){
  const bytes=readFileSync(resolve(root,'th08_web/artifacts',profile,'th08-sdl.wasm'));
  return new Set(WebAssembly.Module.exports(new WebAssembly.Module(bytes)).map(entry=>entry.name));
}
test('fixture owner is separate from ordinary and production multiplayer source plans',()=>{
  const ordinary=plan([]),multiplayer=plan(['--multiplayer']),fixture=plan(['--multiplayer-fixtures']);
  const source='../portable/multiplayer/FixtureExports.cpp';
  assert.equal(ordinary.variant,'normal');
  assert.equal(multiplayer.variant,'multiplayer');
  assert.equal(fixture.variant,'multiplayer-fixtures');
  assert.equal(new Set([ordinary.outputDirectory,multiplayer.outputDirectory,fixture.outputDirectory]).size,3);
  assert.ok(!ordinary.sources.includes(source));
  assert.ok(!multiplayer.sources.includes(source));
  assert.ok(fixture.sources.includes(source));
  assert.ok(!ordinary.flags.includes('-DTH_ENABLE_MULTIPLAYER_GAMEPLAY=1'));
  assert.ok(!multiplayer.flags.includes('-DTH_MULTIPLAYER_FIXTURES=1'));
  assert.ok(fixture.flags.includes('-DTH_MULTIPLAYER_FIXTURES=1'));
  for(const name of ['NetplayRuntime','EnemyJournal','PoolsJournal','ResourcesJournal','WorldJournal','AudioEvents','FileEvents','TextureJournal','RollbackDriver','NetworkConnection','CanonicalExports','AnalogMovement']){
    const owner='cpp/multiplayer/'+name+'.cpp';
    assert.ok(!ordinary.sources.includes(owner));
    assert.ok(multiplayer.sources.includes(owner));
    assert.ok(fixture.sources.includes(owner));
  }
});
test('diagnostic fixture exports never enter ordinary or production multiplayer WASM',()=>{
  const ordinary=exports('sdl3'),multiplayer=exports('multiplayer'),fixture=exports('multiplayer-fixtures');
  for(const [profile,names] of [['ordinary',ordinary],['multiplayer',multiplayer]])
    for(const name of names)assert.ok(!name.startsWith('mp_fixture_'),name+' leaked into '+profile+' WASM');
  for(const name of ['mp_fixture_die','mp_fixture_place','mp_fixture_power','mp_fixture_status','mp_fixture_items','mp_fixture_item_status','mp_fixture_enemy_journal','mp_fixture_screen_journal','mp_fixture_pools_journal','mp_fixture_resources_journal','mp_fixture_world_journal','mp_fixture_native_correction','mp_fixture_audio_clock','mp_fixture_audio_routing'])
    assert.ok(fixture.has(name),name+' missing from diagnostic WASM');
});
test('network admission exports are multiplayer-only, not fixture-only',()=>{
  const ordinary=exports('sdl3'),multiplayer=exports('multiplayer'),fixture=exports('multiplayer-fixtures');
  for(const name of ['multiplayer_spectator_connect','multiplayer_spectator_status','multiplayer_session_build','multiplayer_wire_apply','multiplayer_session_ready',
    'multiplayer_capture_local','multiplayer_capture_input','multiplayer_input_build','multiplayer_netplay_status',
    'multiplayer_connect','multiplayer_network_poll','multiplayer_network_error','multiplayer_driver_status','multiplayer_reconcile','multiplayer_canonical_state']){
    assert.ok(!ordinary.has(name),name+' leaked into ordinary WASM');
    assert.ok(multiplayer.has(name),name+' missing from production multiplayer');
    assert.ok(fixture.has(name),name+' missing from diagnostic multiplayer');
  }
});
test('diagnostic native headers participate in fixture identity only',()=>{
  const fixture=JSON.parse(readFileSync(resolve(root,'th08_web/artifacts/multiplayer-fixtures/build.json')));
  const production=JSON.parse(readFileSync(resolve(root,'th08_web/artifacts/multiplayer/build.json')));
  const ordinary=JSON.parse(readFileSync(resolve(root,'th08_web/artifacts/sdl3/build.json')));
  for(const file of [...['enemy','screen','pools','resources','world'].map(name=>'portable/multiplayer/'+name+'-journal-fixture.hpp'),'portable/multiplayer/correction-fixture.hpp']){
    assert.ok(fixture.sourceFiles[file],file+' missing from fixture source inventory');
    assert.ok(!production.sourceFiles[file],file+' leaked into production source identity');
    assert.ok(!ordinary.sourceFiles[file],file+' leaked into ordinary source identity');
  }
});
