import test from 'node:test';
import assert from 'node:assert/strict';
import {fileURLToPath} from 'node:url';
import {discoverNodeTests} from '../../../tools/testing/discovery.mjs';
import {TURN_NODE_TESTS} from '../scripts/test-plan.mjs';

test('TURN check entry selects the complete recursive test inventory', async () => {
  const directory = fileURLToPath(new URL('./', import.meta.url));
  assert.deepEqual(TURN_NODE_TESTS, (await discoverNodeTests(directory)).map(file => 'tests/' + file));
  assert(TURN_NODE_TESTS.includes('tests/test-netplay-service-config.mjs'));
  assert(TURN_NODE_TESTS.includes('tests/test-test-plan.mjs'));
});
