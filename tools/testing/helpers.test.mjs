import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp, mkdir, writeFile, readFile, rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {assertCliArguments} from './cli-options.mjs';
import {discoverNodeTests} from './discovery.mjs';
import {runWithReport} from './cli-report.mjs';

test('scenario switches reject typos, duplicates, empty values and values on flags', () => {
  const options = {values: ['game'], flags: ['lifecycle']};
  assert.doesNotThrow(() => assertCliArguments(['--game=th12', '--lifecycle'], options));
  for (const argv of [['--lifecyle'], ['--game=th12', '--game=th11'], ['--game='], ['--game'], ['--lifecycle=false']])
    assert.throws(() => assertCliArguments(argv, options));
});

test('discovery includes nested tests and excludes fixture programs', async () => {
  const directory = await mkdtemp(join(tmpdir(), 'eagler-discovery-'));
  try {
    await mkdir(join(directory, 'nested'), {recursive: true});
    await mkdir(join(directory, 'fixtures'), {recursive: true});
    for (const name of ['test-root.mjs', 'nested/behavior.test.mjs', 'fixtures/test-program.mjs', 'helper.mjs'])
      await writeFile(join(directory, name), '');
    assert.deepEqual(await discoverNodeTests(directory), ['nested/behavior.test.mjs', 'test-root.mjs']);
  } finally {await rm(directory, {recursive: true, force: true});}
});

test('input or browser failure replaces a stale passing report before work begins', async () => {
  const output = await mkdtemp(join(tmpdir(), 'eagler-report-'));
  try {
    const file = join(output, 'report.json');
    await writeFile(file, JSON.stringify({status: 'passed', passed: true}));
    const report = await runWithReport({output, schema: 'test/1'}, async () => {
      const pending = JSON.parse(await readFile(file, 'utf8'));
      assert.equal(pending.status, 'running'); assert.equal(pending.passed, false);
      throw Error('browser unavailable');
    });
    assert.equal(report.passed, false); assert.match(report.failure, /browser unavailable/);
    assert.deepEqual(JSON.parse(await readFile(file, 'utf8')), report);
    const next = await runWithReport({output, schema: 'test/1'}, async result => {result.passed = true;});
    assert.equal(next.status, 'passed'); assert.notEqual(next.runId, report.runId);
  } finally {await rm(output, {recursive: true, force: true});}
});
