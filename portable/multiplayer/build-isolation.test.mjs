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
});
test('diagnostic fixture exports never enter ordinary or production multiplayer WASM',()=>{
  const ordinary=exports('sdl3'),multiplayer=exports('multiplayer'),fixture=exports('multiplayer-fixtures');
  for(const [profile,names] of [['ordinary',ordinary],['multiplayer',multiplayer]])
    for(const name of names)assert.ok(!name.startsWith('mp_fixture_'),name+' leaked into '+profile+' WASM');
  for(const name of ['mp_fixture_die','mp_fixture_place','mp_fixture_power','mp_fixture_status','mp_fixture_items','mp_fixture_item_status'])
    assert.ok(fixture.has(name),name+' missing from diagnostic WASM');
});
