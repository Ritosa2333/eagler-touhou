import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {affectedComponents,nodeBuildTargets} from './affected-components.mjs';
const workspace = JSON.parse(await readFile(new URL('../../launcher/config/workspace.json', import.meta.url), 'utf8'));
const builds = JSON.parse(await readFile(new URL('../../launcher/config/runtime-builds.json', import.meta.url), 'utf8'));

test('source build matrix follows actual recipes and retains both shared-source product modes',()=>{
  assert.deepEqual(nodeBuildTargets(['launcher'],workspace,builds),[]);
  const affected=affectedComponents(['ports/th08/web/cpp/game/Player.cpp'],workspace,builds);
  assert.deepEqual(nodeBuildTargets(affected,workspace,builds).map(({game,variant})=>`${game}/${variant}`),['th08/normal','th08/multiplayer']);
  const targets=nodeBuildTargets(affectedComponents(null,workspace,builds),workspace,builds);
  assert.equal(targets.filter(target=>target.game==='th09').length,2);
  assert.equal(targets.filter(target=>target.game==='th20').length,1);
  assert.ok(targets.every(target=>!['th06','th07'].includes(target.game)));
});
test('Launcher-only contribution does not schedule native consumers', () => {
  assert.deepEqual(affectedComponents(['launcher/app/routes/home.tsx'], workspace, builds), ['launcher']);
});
test('an ordinary fix schedules its independently maintained MP source', () => {
  const ordinary = workspace.repositories.th08, mp = workspace.repositories.th08mp;
  assert.notEqual(ordinary, mp);
  const affected = affectedComponents([ordinary + '/portable/build.mjs'], workspace, builds);
  assert(affected.includes(ordinary));
  assert(affected.includes(mp));
  assert(!affected.includes(workspace.repositories.th20));
});
test('shared common changes schedule every declared Runtime consumer', () => {
  const affected = affectedComponents(['common/include/eagler/netplay/NetplayCore.hpp'], workspace, builds);
  for (const entry of Object.values(builds.games)) for (const variant of Object.values(entry.variants))
    assert(affected.includes(workspace.repositories[variant.workspaceRepository ?? entry.workspaceRepository]));
  assert(affected.includes('common'));
  for (const name of ['relay', 'turn', 'telemetry']) assert(affected.includes(workspace.repositories[name]));
});

test('standalone service changes schedule their own gate and Launcher integration', () => {
  for (const name of ['relay', 'turn', 'telemetry']) {
    const directory = workspace.repositories[name];
    assert.deepEqual(affectedComponents([directory + '/README.md'], workspace, builds), ['launcher', directory].sort());
  }
});
