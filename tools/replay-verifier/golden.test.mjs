import assert from 'node:assert/strict';
import test from 'node:test';
import {loadGoldenManifest} from './golden.mjs';

test('published TH08 golden set is complete and content-addressed', async () => {
  const {manifest} = await loadGoldenManifest();
  assert.deepEqual(manifest.suites.quick, ['demo0', 'demo1', 'demo2', 'demo3']);
  assert.deepEqual(manifest.suites.daily, ['lunatic', 'extra']);
  assert.equal(manifest.assets.lunatic.comparedTicks, 110851);
  assert.equal(manifest.assets.extra.comparedTicks, 41505);
});
