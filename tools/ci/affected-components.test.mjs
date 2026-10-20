import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {affectedComponents,ciJobCoverage,nodeBuildTargets} from './affected-components.mjs';
import {PRODUCT_GAMES} from '../../common/contracts/product-catalog.mjs';
const expectedVariantCount=Object.values(PRODUCT_GAMES).reduce((total,product)=>total+1+Number(!!product.multiplayerRuntime),0);
const expectedNodeCount=expectedVariantCount-4;
const workspace = JSON.parse(await readFile(new URL('../../launcher/config/workspace.json', import.meta.url), 'utf8'));
const builds = JSON.parse(await readFile(new URL('../../launcher/config/runtime-builds.json', import.meta.url), 'utf8'));
const workflow = await readFile(new URL('../../.github/workflows/ci.yml', import.meta.url), 'utf8');

test('source build matrix follows actual recipes and retains both shared-source product modes',()=>{
  assert.deepEqual(nodeBuildTargets(['launcher'],workspace,builds),[]);
  const affected=affectedComponents(['ports/th08/web/cpp/game/Player.cpp'],workspace,builds);
  assert.deepEqual(nodeBuildTargets(affected,workspace,builds).map(({game,variant})=>`${game}/${variant}`),['th08/normal','th08/multiplayer']);
  const targets=nodeBuildTargets(affectedComponents(null,workspace,builds),workspace,builds);
  assert.equal(targets.filter(target=>target.game==='th09').length,2);
  assert.equal(targets.filter(target=>target.game==='th20').length,1);
  assert.deepEqual(targets.filter(target=>target.game==='th075').map(target=>target.variant), ['normal','multiplayer']);
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

test('test orchestration changes run policy and portability lanes without rebuilding products', () => {
  const testOnlyChanges = [
    'tests/suites.mjs',
    'tests/run.mjs',
    'tools/testing/runner.mjs',
    'tools/testing/runner.test.mjs',
    'tools/ci/required-checks.mjs',
    'tools/ci/required-checks.test.mjs',
    'tools/build/runtime-evidence.test.mjs',
    'tools/build/source-dependencies.mjs',
    'tools/build/emscripten_sdk.py',
  ];
  for (const input of testOnlyChanges) {
    const affected = affectedComponents([input], workspace, builds);
    assert.deepEqual(affected, ['launcher'], input);
    assert.deepEqual(nodeBuildTargets(affected, workspace, builds), [], input);
    assert.deepEqual(ciJobCoverage([input]), {sourceContracts:true, portAdaptation:true}, input);
  }
});

test('standalone service changes schedule their own gate and Launcher integration', () => {
  for (const name of ['relay', 'turn', 'telemetry']) {
    const directory = workspace.repositories[name];
    assert.deepEqual(affectedComponents([directory + '/README.md'], workspace, builds), ['launcher', directory].sort());
  }
});

test('source validation lanes follow their actual component inputs', () => {
  assert.deepEqual(ciJobCoverage(['services/relay/src/netplay-relay.mjs']),
    {sourceContracts:false, portAdaptation:false});
  assert.deepEqual(ciJobCoverage(['launcher/app/routes/home.tsx']),
    {sourceContracts:false, portAdaptation:true});
  assert.deepEqual(ciJobCoverage(['docs/adaptation/checklist.json']),
    {sourceContracts:false, portAdaptation:true});
  assert.deepEqual(ciJobCoverage(['tests/suites.mjs']),
    {sourceContracts:true, portAdaptation:true});
  assert.deepEqual(ciJobCoverage(null), {sourceContracts:true, portAdaptation:true});
});

test('runtime source-build implementation changes compile every CI Runtime variant', () => {
  const inputs = [
    'launcher/scripts/build-runtime.mjs',
    'launcher/lib/runtime-source-plan.mjs',
    'launcher/lib/runtime-build-evidence.mjs',
    'launcher/lib/runtime-build-profiles.mjs',
    'launcher/lib/source-layout.mjs',
    'launcher/lib/workspace-layout.mjs',
    'launcher/lib/release-manifest.mjs',
    'launcher/lib/contracts/product-catalog.mjs',
    'launcher/config/runtime-builds.json',
    'launcher/config/workspace.json',
    'tools/testing/discovery.mjs',
    'tools/build/compiler-arguments.mjs',
    'tools/build/runtime-inputs.mjs',
    'tools/build/source-object.mjs',
  ];
  const allTargets = nodeBuildTargets(affectedComponents(null, workspace, builds), workspace, builds)
    .map(({game, variant}) => `${game}/${variant}`).sort();
  assert.equal(allTargets.length, expectedNodeCount, 'the node source-build matrix should contain every catalogued Node variant');
  const allBuildOwners = [...new Set(Object.entries(builds.games).flatMap(([game, entry]) =>
    Object.values(entry.variants).map(definition => workspace.repositories[definition.workspaceRepository ?? entry.workspaceRepository])))];
  const affectedOwners = [...new Set(['launcher', ...allBuildOwners])].sort();
  const cmakeWebVariants = [];
  for (const game of ['th06', 'th07']) {
    const owner = workspace.repositories[game];
    assert(allBuildOwners.includes(owner), `${game} CMake Web consumers must be in the build owner set`);
    assert(workflow.includes("if: ${{ contains(fromJSON(needs.scope.outputs.affected), '" + owner + "') }}"));
    assert(workflow.includes('name: ' + owner + ' / Web ${{ matrix.variant }}'));
    assert(builds.games[game].variants.normal && builds.games[game].variants.multiplayer);
    const jobHeader = `  ports_${game}_eagler_ci_web-build:`;
    const jobStart = workflow.indexOf(jobHeader);
    assert.notEqual(jobStart, -1, `${game} Web build job must exist`);
    const nextJob = /\n  [a-z0-9_]+:\n/g.exec(workflow.slice(jobStart + jobHeader.length));
    const job = workflow.slice(jobStart, nextJob ? jobStart + jobHeader.length + nextJob.index : undefined);
    for (const variant of ['normal', 'netplay']) {
      assert(job.includes(`- ${variant}`), `${game} Web matrix must include ${variant}`);
      cmakeWebVariants.push(`${game}/${variant}`);
    }
  }
  assert.equal(allTargets.length + cmakeWebVariants.length, expectedVariantCount,
    'the CI build consumers cover every catalogued normal and multiplayer product');

  for (const input of inputs) {
    const affected = affectedComponents([input], workspace, builds);
    assert.deepEqual(affected, affectedOwners, input);
    assert.deepEqual(nodeBuildTargets(affected, workspace, builds)
      .map(({game, variant}) => `${game}/${variant}`).sort(), allTargets, input);
    assert.deepEqual(ciJobCoverage([input]), {sourceContracts:true, portAdaptation:true}, input);
    assert(!affected.some(directory => directory.startsWith('services/')), `${input} must not schedule service suites`);
  }
});

test('unrelated Launcher scripts do not schedule the Runtime source-build matrix', () => {
  const affected = affectedComponents(['launcher/scripts/audit-publication.mjs'], workspace, builds);
  assert.deepEqual(affected, ['launcher']);
  assert.deepEqual(nodeBuildTargets(affected, workspace, builds), []);
  assert.deepEqual(ciJobCoverage(['launcher/scripts/audit-publication.mjs']),
    {sourceContracts:false, portAdaptation:false});
});

test('Runtime build infrastructure scope composes with an independently changed service', () => {
  const relay = workspace.repositories.relay;
  const affected = affectedComponents(['launcher/lib/runtime-source-plan.mjs', relay + '/src/netplay-relay.mjs'], workspace, builds);
  assert(affected.includes(relay));
  assert.equal(nodeBuildTargets(affected, workspace, builds).length, expectedNodeCount);
});

test('mixed diffs keep orchestration-only changes out of builds and preserve actual build consumers', () => {
  const relay = workspace.repositories.relay;
  const orchestrationOnly = ['tests/suites.mjs', 'tools/testing/runner.mjs', `${relay}/README.md`];
  const scoped = affectedComponents(orchestrationOnly, workspace, builds);
  assert.deepEqual(scoped, ['launcher', relay].sort());
  assert.deepEqual(nodeBuildTargets(scoped, workspace, builds), []);
  assert.deepEqual(ciJobCoverage(orchestrationOnly), {sourceContracts:true, portAdaptation:true});

  const withRuntimeBuilder = ['launcher/lib/runtime-source-plan.mjs', 'tests/suites.mjs', `${relay}/src/netplay-relay.mjs`];
  const built = affectedComponents(withRuntimeBuilder, workspace, builds);
  assert(built.includes(relay), 'the independently edited service retains its service gate');
  assert.equal(nodeBuildTargets(built, workspace, builds).length, expectedNodeCount,
    'a real Runtime planner change still selects all Node product builds');
  assert.deepEqual(ciJobCoverage(withRuntimeBuilder), {sourceContracts:true, portAdaptation:true});
});
