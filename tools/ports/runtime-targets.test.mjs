import test from 'node:test';
import assert from 'node:assert/strict';
import {runtimeTestTargets, buildRuntimeTestTargets, selectRuntimeInputs, resultCoverage, runtimeTargetKey} from './runtime-targets.mjs';
import {mkdtemp, writeFile, readFile, rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve, join} from 'node:path';
import {spawnSync} from 'node:child_process';

const input = target => ({...target, id: runtimeTargetKey(target).replace('/', '-'), runtime: '/runtime', entry: 'game.html', data: '/private/data'});

test('catalog selection covers separate MP owners, shared owners and th12', async () => {
  const all = await runtimeTestTargets();
  assert.equal(all.find(row => row.game === 'th12' && row.variant === 'multiplayer').directory, 'ports/th12-mp');
  assert.equal(all.find(row => row.game === 'th06' && row.variant === 'multiplayer').directory, 'ports/th06');
  const cooperative = await runtimeTestTargets({variant: 'multiplayer', gameplay: 'cooperative'});
  assert.ok(cooperative.some(row => row.game === 'th12'));
  assert.ok(cooperative.every(row => row.variant === 'multiplayer' && row.gameplay === 'cooperative'));
  for (const game of ['th12,typo', 'th12,th12', '']) await assert.rejects(runtimeTestTargets({game}));
  await assert.rejects(runtimeTestTargets({variant: 'multiplayer', game: 'th20'}), /no applicable/);
});

test('a new product joins the matrix through build and workspace declarations', () => {
  assert.deepEqual(buildRuntimeTestTargets({newGame: {}}, {games: {newGame: {workspaceRepository: 'newOwner', variants: {normal: {}}}}},
    {repositories: {newOwner: 'ports/newGame'}}).map(runtimeTargetKey), ['newGame/normal']);
  assert.throws(() => buildRuntimeTestTargets({newGame: {}}, {games: {}}, {repositories: {}}), /catalog/);
});

test('a private manifest cannot shrink required coverage or repeat a Runtime', async () => {
  const required = await runtimeTestTargets({game: 'th12'}), targets = required.map(input);
  assert.deepEqual(selectRuntimeInputs({targets}, required), targets);
  assert.throws(() => selectRuntimeInputs({targets: targets.slice(0, 1)}, required), /th12\/multiplayer/);
  assert.throws(() => selectRuntimeInputs({targets: [...targets, targets[0]]}, required), /Duplicate manifest Runtime/);
  assert.throws(() => selectRuntimeInputs({targets: [targets[0], {...targets[1], id: targets[0].id}]}, required), /duplicate Runtime id/);
});

test('missing, repeated, failed and unexpected cases never form a passing run', () => {
  const expected = ['normal', 'challenge'];
  assert.equal(resultCoverage(expected, expected.map(id => ({id, passed: true}))).complete, true);
  for (const rows of [[], [{id: 'normal', passed: true}], [{id: 'normal', passed: true}, {id: 'normal', passed: true}],
    [{id: 'normal', passed: true}, {id: 'challenge', passed: false}], [...expected, 'extra'].map(id => ({id, passed: true}))])
    assert.equal(resultCoverage(expected, rows).complete, false);
});

test('both browser drivers fail before launch on bad switches or missing targets and replace stale success', async () => {
  const output = await mkdtemp(join(tmpdir(), 'eagler-driver-selection-'));
  try {
    const manifest = join(output, 'inputs.json'), file = join(output, 'report.json');
    await writeFile(manifest, JSON.stringify({targets: []}));
    for (const driver of ['runtime-browser.mjs', 'mp-browser.mjs']) {
      for (const extra of [['--lifecyle'], []]) {
        await writeFile(file, JSON.stringify({status: 'passed', passed: true}));
        const child = spawnSync(process.execPath, [resolve(import.meta.dirname, driver), `--output=${output}`, `--manifest=${manifest}`, '--game=th12', ...extra],
          {encoding: 'utf8', timeout: 15000, windowsHide: true});
        assert.equal(child.error, undefined); assert.notEqual(child.status, 0);
        const report = JSON.parse(await readFile(file, 'utf8'));
        assert.equal(report.status, 'failed'); assert.equal(report.passed, false);
        assert.match(report.failure, extra.length ? /Unknown option/ : /Missing required Runtime inputs/);
      }
    }
  } finally {await rm(output, {recursive: true, force: true});}
});
