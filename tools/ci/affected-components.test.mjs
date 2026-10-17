import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {affectedComponents} from './affected-components.mjs';
const workspace = JSON.parse(await readFile(new URL('../../launcher/config/workspace.json', import.meta.url), 'utf8'));
const builds = JSON.parse(await readFile(new URL('../../launcher/config/runtime-builds.json', import.meta.url), 'utf8'));
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
});
