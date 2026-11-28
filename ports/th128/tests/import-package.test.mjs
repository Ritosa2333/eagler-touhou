// Discriminating unit tests for the TH128 Package producer/validator.
// No product runtime, no browser, no World: only Node fs/path/zlib-free code
// under the package-v2 candidate.

import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';

import {
  PACKAGE_DESCRIPTOR_SCHEMA,
  TH128_GAME_ID,
  canonicalJson,
  createPackage,
  dataLayoutOf,
  planPackage,
  readStoredZip,
  sha256Hex,
  validatePackageDescriptor,
  validatePackageDirectory,
  validatePackageFiles,
} from '../tools/package-import.mjs';

function makeWorkspace() {
  const root = fs.mkdtempSync(path.join(os.tmpdir(), 'th128-package-v2-'));
  const assets = path.join(root, 'assets');
  fs.mkdirSync(path.join(assets, 'generations', 'g1', 'zh', 'music'), { recursive: true });
  fs.writeFileSync(path.join(assets, 'catalog.json'), `${JSON.stringify({ schema: 'th128-asset-catalog/1', playable: false })}\n`);
  fs.writeFileSync(path.join(assets, 'generations', 'g1', 'zh', 'manifest.json'), `${JSON.stringify({ schema: 'th128-prepared-assets/1', game: 'th128', files: [] })}\n`);
  fs.writeFileSync(path.join(assets, 'generations', 'g1', 'zh', 'music', 'th128_07.wav'), Buffer.from([0x52, 0x49, 0x46, 0x46, 0x01, 0x02, 0x03, 0x04]));
  return root;
}

const BASE_INPUT = {
  game: TH128_GAME_ID,
  revision: 'r1',
  dataFile: 'game-data',
  files: [
    { id: 'game-data', source: 'assets/catalog.json', target: '/assets/catalog.json', sourceFile: 'assets/catalog.json' },
    { id: 'title-music', source: 'assets/generations/g1/zh/music/th128_07.wav', target: '/assets/generations/g1/zh/music/th128_07.wav', sourceFile: 'assets/generations/g1/zh/music/th128_07.wav' },
  ],
};

function createOnce(workspace, name = 'out') {
  return createPackage({ input: BASE_INPUT, rootDir: workspace, outDir: path.join(workspace, name) });
}

test('createPackage writes a deterministic descriptor, extracted tree and ZIP', () => {
  const workspace = makeWorkspace();
  const first = createOnce(workspace, 'a');
  const second = createOnce(workspace, 'b');

  assert.equal(first.descriptor.schema, PACKAGE_DESCRIPTOR_SCHEMA);
  assert.equal(first.descriptor.game, TH128_GAME_ID);
  assert.equal(first.descriptor.runtimeRequirement.protocol, 'eagler-touhou/1');
  assert.equal(first.descriptor.runtimeRequirement.target, TH128_GAME_ID);
  assert.equal(first.descriptor.runtimeRequirement.dataFile, 'game-data');
  assert.equal(first.descriptor.runtimeRequirement.dataLayout, dataLayoutOf(first.descriptor));
  assert.ok(first.descriptor.base.files.includes('game-data'));

  const catalogBytes = fs.readFileSync(path.join(workspace, 'assets', 'catalog.json'));
  assert.equal(first.descriptor.files['game-data'].sha256, sha256Hex(catalogBytes));
  assert.equal(first.descriptor.files['game-data'].revision, sha256Hex(catalogBytes).slice(0, 16));

  // Same input twice => byte-identical ZIP and descriptor.
  assert.equal(first.zipSha256, second.zipSha256);
  assert.equal(first.descriptorBytes.toString('utf8'), second.descriptorBytes.toString('utf8'));

  // ZIP: root package.json first, remaining entries sorted, bytes equal to extracted.
  const entries = readStoredZip(fs.readFileSync(first.zipPath));
  const names = [...entries.keys()];
  assert.equal(names[0], 'package.json');
  const rest = names.slice(1);
  assert.deepEqual(rest, [...rest].sort());
  assert.equal(entries.get('package.json').bytes.toString('utf8'), canonicalJson(first.descriptor));
  for (const name of rest) {
    const extracted = fs.readFileSync(path.join(first.outDir, 'extracted', ...name.split('/')));
    assert.deepEqual(entries.get(name).bytes, extracted);
  }
});

test('validatePackageDirectory accepts the extracted package and reports every payload', () => {
  const workspace = makeWorkspace();
  const created = createOnce(workspace);
  const report = validatePackageDirectory(path.join(created.outDir, 'extracted'));
  assert.equal(report.ok, true);
  assert.deepEqual(report.problems, []);
  assert.equal(report.checked.length, 2);
  assert.deepEqual(report.checked.map((row) => row.id).sort(), ['game-data', 'title-music']);
});

test('rejects a foreign game id (TH12 payload renamed to TH128 must fail)', () => {
  const workspace = makeWorkspace();
  assert.throws(
    () => planPackage({ input: { ...BASE_INPUT, game: 'th12' }, rootDir: workspace }),
    /unsupported-game/,
  );
});

test('rejects the TH12 dataLayout constant', () => {
  const workspace = makeWorkspace();
  const created = createOnce(workspace);
  const descriptor = JSON.parse(created.descriptorBytes.toString('utf8'));
  descriptor.runtimeRequirement.dataLayout = `sha256-94ee7257${'a'.repeat(56)}`;
  assert.throws(() => validatePackageDescriptor(descriptor), /foreign-data-layout/);
});

test('rejects traversal, mount escapes and duplicate targets', () => {
  const workspace = makeWorkspace();
  const traversal = {
    ...BASE_INPUT,
    files: [{ id: 'bad', source: 'assets/catalog.json', target: '/assets/../escape', sourceFile: 'assets/catalog.json' }],
  };
  assert.throws(() => planPackage({ input: traversal, rootDir: workspace }), /unsafe-target/);

  const outsideMount = {
    ...BASE_INPUT,
    files: [{ id: 'bad', source: 'assets/catalog.json', target: '/th12.data', sourceFile: 'assets/catalog.json' }],
  };
  assert.throws(() => planPackage({ input: outsideMount, rootDir: workspace }), /unsafe-target/);

  const duplicateTarget = {
    ...BASE_INPUT,
    files: [
      { id: 'a', source: 'assets/catalog.json', target: '/assets/catalog.json', sourceFile: 'assets/catalog.json' },
      { id: 'b', source: 'assets/generations/g1/zh/manifest.json', target: '/assets/catalog.json', sourceFile: 'assets/generations/g1/zh/manifest.json' },
    ],
  };
  assert.throws(() => planPackage({ input: duplicateTarget, rootDir: workspace }), /duplicate-target/);
});

test('rejects forbidden payloads (logs/config/saves) and replay files', () => {
  const workspace = makeWorkspace();
  const logInput = {
    ...BASE_INPUT,
    files: [{ id: 'run-log', source: 'logs/run.log', target: '/assets/logs/run.log', sourceFile: 'assets/catalog.json' }],
  };
  assert.throws(() => planPackage({ input: logInput, rootDir: workspace }), /forbidden-payload/);

  const savesInput = {
    ...BASE_INPUT,
    files: [{ id: 'save', source: 'assets/saves/a.bin', target: '/assets/saves/a.bin', sourceFile: 'assets/catalog.json' }],
  };
  assert.throws(() => planPackage({ input: savesInput, rootDir: workspace }), /forbidden-payload/);
});

test('rejects tampered payload bytes and truncated ZIPs', () => {
  const workspace = makeWorkspace();
  const created = createOnce(workspace);
  const extracted = path.join(created.outDir, 'extracted');
  fs.appendFileSync(path.join(extracted, 'assets', 'catalog.json'), 'x');
  const report = validatePackageFiles(created.descriptor, { rootDir: extracted });
  assert.equal(report.ok, false);
  assert.equal(report.problems[0].code, 'size-mismatch');
  const targetFile = path.join(extracted, 'assets', 'catalog.json');
  const originalSize = created.descriptor.files['game-data'].bytes;
  const sameSize = fs.readFileSync(targetFile).subarray(0, originalSize);
  sameSize[0] ^= 1;
  fs.writeFileSync(targetFile, sameSize);
  const hashReport = validatePackageFiles(created.descriptor, { rootDir: extracted });
  assert.equal(hashReport.ok, false);
  assert.equal(hashReport.problems[0].code, 'hash-mismatch');

  const zip = fs.readFileSync(created.zipPath);
  assert.throws(() => readStoredZip(zip.subarray(0, zip.length - 10)), /invalid-zip/);
});

test('rejects unsupported language and audio components', () => {
  const workspace = makeWorkspace();
  const created = createOnce(workspace);
  const descriptor = JSON.parse(created.descriptorBytes.toString('utf8'));
  const langFile = {
    source: 'assets/lang/en.zip',
    target: '/assets/lang/en.zip',
    revision: 'en00000000000000',
    bytes: 4,
    sha256: 'b'.repeat(64),
  };
  const withLanguage = structuredClone(descriptor);
  withLanguage.files['lang-en'] = langFile;
  withLanguage.base.files.push('lang-en');
  withLanguage.components = { language: { type: 'language', entries: [{ id: 'lang_en', file: 'lang-en', title: 'English' }] } };
  withLanguage.runtimeRequirement.dataLayout = dataLayoutOf(withLanguage);
  assert.throws(() => validatePackageDescriptor(withLanguage), /unsupported-language/);

  const withOgg = structuredClone(descriptor);
  withOgg.components = { ogg: { type: 'ogg', files: ['title-music'] } };
  assert.throws(() => validatePackageDescriptor(withOgg), /unsupported-component/);
});

test('refuses to overwrite an existing non-empty output directory', () => {
  const workspace = makeWorkspace();
  createOnce(workspace);
  assert.throws(() => createOnce(workspace), /output-exists/);
});

test('rejects a descriptor whose dataLayout does not describe its own file list', () => {
  const workspace = makeWorkspace();
  const created = createOnce(workspace);
  const descriptor = JSON.parse(created.descriptorBytes.toString('utf8'));
  descriptor.runtimeRequirement.dataLayout = `sha256-${'c'.repeat(64)}`;
  assert.throws(() => validatePackageDescriptor(descriptor), /data-layout-mismatch/);
});

test('validatePackageDescriptor reports byte/sha requirement and base coverage', () => {
  const workspace = makeWorkspace();
  const created = createOnce(workspace);
  const descriptor = JSON.parse(created.descriptorBytes.toString('utf8'));
  const noBase = structuredClone(descriptor);
  noBase.base.files = noBase.base.files.filter((id) => id !== 'game-data');
  assert.throws(() => validatePackageDescriptor(noBase), /invalid-base/);

  const negativeBytes = structuredClone(descriptor);
  negativeBytes.files['game-data'].bytes = -1;
  assert.throws(() => validatePackageDescriptor(negativeBytes), /invalid-bytes/);
});
