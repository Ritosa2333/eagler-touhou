import assert from 'node:assert/strict';
import { mkdtemp, mkdir, writeFile, rm } from 'node:fs/promises';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';
import { test } from 'node:test';
import { parseRetailAssetArgs, resolveRetailFontInputs } from '../../scripts/retail-font-inputs.mjs';

test('TH09 font root prefers an explicit path, then EAGLER_FONT_ROOT, then legacy fallback', () => {
  const base = resolve(tmpdir(), 'th09-font-root-selection');
  const explicit = join(base, 'explicit');
  const environment = join(base, 'environment');
  const legacy = join(base, 'legacy');
  assert.equal(
    parseRetailAssetArgs([join(base, 'TH09'), '--font-root', explicit], { EAGLER_FONT_ROOT: environment }, legacy).fontRoot,
    explicit,
  );
  assert.equal(
    parseRetailAssetArgs([join(base, 'TH09')], { EAGLER_FONT_ROOT: environment }, legacy).fontRoot,
    environment,
  );
  assert.equal(
    parseRetailAssetArgs([join(base, 'TH09')], {}, legacy).fontRoot,
    legacy,
  );
});

test('TH09 accepts a small standalone font fixture and reports missing required files', async t => {
  const temp = await mkdtemp(join(tmpdir(), 'th09-font-fixture-'));
  t.after(() => rm(temp, { recursive: true, force: true }));
  const fontRoot = join(temp, 'fonts');
  await mkdir(fontRoot);
  await writeFile(join(fontRoot, 'cp932.bin'), Buffer.alloc(131072, 1));
  await writeFile(join(fontRoot, 'blend.bin'), Buffer.alloc(131072, 2));
  await writeFile(join(fontRoot, 'msgothic.ttc'), Buffer.from('fixture font'));
  const inputs = resolveRetailFontInputs(fontRoot);
  assert.equal(inputs.codepageName, 'cp932.bin');
  assert.equal(inputs.root, fontRoot);

  await rm(join(fontRoot, 'blend.bin'));
  assert.throws(() => resolveRetailFontInputs(fontRoot), /Missing TH09 font input\(s\).*blend\.bin/);
});

test('TH09 can derive CP932 from the first half of a standalone codepages table', async t => {
  const temp = await mkdtemp(join(tmpdir(), 'th09-codepages-fixture-'));
  t.after(() => rm(temp, { recursive: true, force: true }));
  await writeFile(join(temp, 'codepages.bin'), Buffer.alloc(262144, 3));
  await writeFile(join(temp, 'blend.bin'), Buffer.alloc(131072, 4));
  await writeFile(join(temp, 'msgothic.ttc'), Buffer.from('fixture font'));
  assert.equal(resolveRetailFontInputs(temp).codepageName, 'codepages.bin');
});
