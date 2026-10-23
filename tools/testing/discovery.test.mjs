import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp, mkdir, rm, writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {isAbsolute, relative, resolve} from 'node:path';
import {discoverNodeTests, isNodeTest} from './discovery.mjs';

test('the shared path predicate accepts test modules and excludes fixtures and helpers', () => {
  assert.equal(isNodeTest('test-root.mjs'), true);
  assert.equal(isNodeTest('nested/deep/relay.test.mjs'), true);
  assert.equal(isNodeTest('nested/fixtures/test-fixture.mjs'), false);
  assert.equal(isNodeTest('nested/helper.mjs'), false);
  assert.equal(isNodeTest('nested/test-helper.js'), false);
});

test('recursive discovery selects both supported test names and excludes fixtures/helpers', async () => {
  const root = await mkdtemp(resolve(tmpdir(), 'eagler-test-discovery-'));
  try {
    for (const directory of ['nested/deep', 'nested/fixtures', 'fixtures'])
      await mkdir(resolve(root, directory), {recursive:true});
    for (const file of ['test-root.mjs', 'nested/deep/relay.test.mjs', 'nested/fixtures/test-fixture.mjs',
      'fixtures/helper.test.mjs', 'nested/helper.mjs', 'nested/not-a-test.js'])
      await writeFile(resolve(root, file), '');

    assert.deepEqual(await discoverNodeTests(root), ['nested/deep/relay.test.mjs', 'test-root.mjs']);
  } finally {
    const fromTemp = relative(resolve(tmpdir()), resolve(root));
    assert(fromTemp && !fromTemp.startsWith('..') && !isAbsolute(fromTemp));
    await rm(root, {recursive:true, force:true});
  }
});
