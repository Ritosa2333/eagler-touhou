import assert from 'node:assert/strict';
import test from 'node:test';
import { filterItems, safeAssetUrl, words } from '../site/inspector.mjs';

test('asset URLs stay inside the selected prepared generation', () => {
  const base = 'http://127.0.0.1:8128/assets/generations/aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa/zh/';
  assert.equal(safeAssetUrl(base, 'textures/title-001.png'), `${base}textures/title-001.png`);
  for (const invalid of ['../manifest.json', '/catalog.json', 'https://elsewhere/x', 'a\\b', 'a/%2e%2e/b', '//x', 'x#bad']) {
    assert.throws(() => safeAssetUrl(base, invalid));
  }
});

test('raw words preserve integer and float interpretations without executing', () => {
  assert.deepEqual(words('0000803f'), [{ integer: 1065353216, float: 1 }]);
  assert.deepEqual(words('ffffffff'), [{ integer: -1, float: null }]);
  assert.deepEqual(words(''), []);
  assert.throws(() => words('bad'));
});

test('case-insensitive filtering retains original items', () => {
  const rows = [{ name: 'title.anm' }, { name: 'ice.anm' }];
  assert.deepEqual(filterItems(rows, 'ICE'), [rows[1]]);
  assert.deepEqual(filterItems(rows, ''), rows);
});
