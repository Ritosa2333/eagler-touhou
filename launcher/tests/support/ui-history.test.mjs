import {test} from 'vitest';
import assert from 'node:assert/strict';
import {readUiHistoryText, verifyUiHistoryFixtures, UI_HISTORY_FIXTURES} from './ui-history.mjs';

test('fixed UI history fixtures are complete and match their recorded SHA-256', () => {
  assert.equal(verifyUiHistoryFixtures(), UI_HISTORY_FIXTURES.length);
  assert.deepEqual([...new Set(UI_HISTORY_FIXTURES.map(entry => entry.revision))].sort(), [
    '8bdf6dbf4a81032f0725cc841b86d03e1465fe07',
    'edee9633e5e3ee79cd2e1aa334f84f6caf755090',
  ]);
});

test('history reader rejects unknown revisions, paths and traversal', () => {
  assert.throws(() => readUiHistoryText('edee9633', 'public/index.html'), /revision/);
  assert.throws(() => readUiHistoryText('edee9633e5e3ee79cd2e1aa334f84f6caf755090', '../public/index.html'), /path/);
  assert.throws(() => readUiHistoryText('edee9633e5e3ee79cd2e1aa334f84f6caf755090', 'not-a-fixture.txt'), /Unregistered/);
});
