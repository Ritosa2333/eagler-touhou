import assert from 'node:assert/strict';
import test from 'node:test';
import {loadGoldenManifest} from './golden.mjs';

test('published TH10 golden set is complete and content-addressed', async () => {
  const {manifest} = await loadGoldenManifest();
  assert.deepEqual(manifest.suites.quick, ['demo1', 'demo2', 'demo3', 'demo0']);
  assert.deepEqual(manifest.suites.daily, ['lunatic', 'extra']);
  assert.equal(manifest.assets.demos.rawTicks, 12000);
  assert.equal(manifest.assets.lunatic.comparedTicks, 84797);
  assert.equal(manifest.assets.extra.comparedTicks, 39111);
});
