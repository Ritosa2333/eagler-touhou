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
  for(const name of ['NetplayRuntime','EnemyJournal','PoolsJournal']){
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
  for(const name of ['mp_fixture_die','mp_fixture_place','mp_fixture_power','mp_fixture_status','mp_fixture_items','mp_fixture_item_status','mp_fixture_enemy_journal','mp_fixture_screen_journal','mp_fixture_pools_journal'])
    assert.ok(fixture.has(name),name+' missing from diagnostic WASM');
});
test('network admission exports are multiplayer-only, not fixture-only',()=>{
  const ordinary=exports('sdl3'),multiplayer=exports('multiplayer'),fixture=exports('multiplayer-fixtures');
  for(const name of ['multiplayer_session_build','multiplayer_wire_apply','multiplayer_session_ready',
    'multiplayer_capture_local','multiplayer_input_build','multiplayer_netplay_status']){
    assert.ok(!ordinary.has(name),name+' leaked into ordinary WASM');
    assert.ok(multiplayer.has(name),name+' missing from production multiplayer');
    assert.ok(fixture.has(name),name+' missing from diagnostic multiplayer');
  }
});
