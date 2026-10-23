import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync, mkdirSync, rmSync, writeFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {isAbsolute, join, relative, resolve} from 'node:path';
import {discoverNodeTests} from '../../../tools/testing/discovery.mjs';
import {RELAY_NODE_TESTS} from '../scripts/test-plan.mjs';

test('Relay test discovery includes nested regressions and excludes fixture programs', async () => {
  const directory = mkdtempSync(join(tmpdir(), 'eagler-relay-test-plan-'));
  try {
    mkdirSync(join(directory, 'integration', 'deep'), {recursive:true});
    mkdirSync(join(directory, 'fixtures'), {recursive:true});
    for (const path of ['test-root.mjs', 'integration/deep/test-nested.mjs', 'fixtures/test-fixture.mjs'])
      writeFileSync(join(directory, path), '');
    const selected = await discoverNodeTests(directory);
    assert.deepEqual(selected, ['integration/deep/test-nested.mjs', 'test-root.mjs']);
    assert(RELAY_NODE_TESTS.includes('tests/test-test-plan.mjs'));
  } finally {
    const fromTemp = relative(resolve(tmpdir()), resolve(directory));
    assert(fromTemp && !fromTemp.startsWith('..') && !isAbsolute(fromTemp));
    rmSync(directory, {recursive:true, force:true});
  }
});
