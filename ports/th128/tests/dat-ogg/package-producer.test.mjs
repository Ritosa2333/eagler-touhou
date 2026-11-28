// Meaningful unit tests for the TH128 DAT+OGG package producer (package-v3).
// Node only: no runtime, no browser, no World, no install.

import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';

import {
  TH15_PINNED_DATA_LAYOUT,
  classifyEntry,
  createPackage,
  layoutForDataFileName,
  readPackageZip,
  sha256Hex,
  th128DataLayout,
  validatePackageDescriptor,
  validatePackageDirectory,
  validatePackageFiles,
  planPackage,
} from '../../tools/package-import-dat-ogg.mjs';

const OGG = Buffer.concat([Buffer.from('OggS', 'ascii'), Buffer.from([0x00, 0x02, 0x00, 0x00])]);
const ZIP = Buffer.concat([Buffer.from([0x50, 0x4b, 0x03, 0x04]), Buffer.from('language-pack', 'utf8')]);
const DAT = Buffer.concat([Buffer.from('TH128DAT', 'ascii'), Buffer.alloc(64, 7)]);

function workspace() {
  const root = fs.mkdtempSync(path.join(os.tmpdir(), 'th128-pkg-v3-'));
  const write = (rel, bytes) => {
    const abs = path.join(root, ...rel.split('/'));
    fs.mkdirSync(path.dirname(abs), { recursive: true });
    fs.writeFileSync(abs, bytes);
    return abs;
  };
  write('games/th128/th128.dat', DAT);
  write('games/th128/music/ogg/th128_08.ogg', OGG);
  write('shared/msgothic.ttc', Buffer.from('ttc-font', 'utf8'));
  write('games/th128/language/zh.zip', ZIP);
  return root;
}

const INPUT = {
  game: 'th128',
  revision: 'v3-0001',
  files: [
    { id: 'game-data', source: 'games/th128/th128.dat', target: '/th128.dat', sourceFile: 'games/th128/th128.dat' },
    { id: 'music-th128-08', source: 'games/th128/music/ogg/th128_08.ogg', target: '/music/th128_08.ogg', sourceFile: 'games/th128/music/ogg/th128_08.ogg' },
    { id: 'shared-msgothic', source: 'shared/msgothic.ttc', target: '/msgothic.ttc', sourceFile: 'shared/msgothic.ttc' },
    { id: 'lang-zh', source: 'games/th128/language/zh.zip', target: '/__eagler/language/zh.zip', sourceFile: 'games/th128/language/zh.zip' },
  ],
  languageEntries: [{ id: 'lang-zh', file: 'lang-zh', title: '中文（简体）' }],
};

const createOnce = (root, name = 'out') => createPackage({ input: INPUT, rootDir: root, outDir: path.join(root, name) });

test('mount identity: TH15 pins the convention, TH128 uses the /th128.dat mount', () => {
  assert.equal(layoutForDataFileName('/th15.dat'), TH15_PINNED_DATA_LAYOUT);
  assert.equal(th128DataLayout(), layoutForDataFileName('/th128.dat'));
  assert.match(th128DataLayout(), /^sha256-[0-9a-f]{64}$/);
  assert.notEqual(th128DataLayout(), TH15_PINNED_DATA_LAYOUT);
});

test('creates a deterministic DAT+OGG package with canonical components', () => {
  const root = workspace();
  const first = createOnce(root, 'a');
  const second = createOnce(root, 'b');

  assert.equal(first.descriptor.runtimeRequirement.dataFile, 'game-data');
  assert.equal(first.descriptor.runtimeRequirement.dataLayout, th128DataLayout());
  assert.equal(first.descriptor.files['game-data'].sha256, sha256Hex(DAT));
  assert.deepEqual(first.descriptor.base.files, ['game-data', 'shared-msgothic']);
  assert.deepEqual(first.descriptor.components.ogg, { type: 'ogg', files: ['music-th128-08'] });
  assert.equal(first.descriptor.components.language.entries.length, 1);
  assert.equal(first.descriptor.files['game-data'].sha256.length, 64);

  assert.equal(first.zipSha256, second.zipSha256);
  assert.equal(first.descriptorBytes.toString('utf8'), second.descriptorBytes.toString('utf8'));
});

test('ZIP uses DEFLATE for DAT/language and STORE for OGG', () => {
  const root = workspace();
  const created = createOnce(root);
  const zip = fs.readFileSync(created.zipPath);
  const entries = readPackageZip(zip);
  assert.equal(entries.get('games/th128/th128.dat').method, 8);
  assert.equal(entries.get('games/th128/language/zh.zip').method, 8);
  assert.equal(entries.get('games/th128/music/ogg/th128_08.ogg').method, 0);
  assert.deepEqual(entries.get('games/th128/th128.dat').bytes, DAT);
  assert.deepEqual(entries.get('games/th128/music/ogg/th128_08.ogg').bytes, OGG);
  assert.deepEqual(entries.get('shared/msgothic.ttc').bytes, Buffer.from('ttc-font', 'utf8'));
});

test('validatePackageDirectory accepts the extracted package', () => {
  const root = workspace();
  const created = createOnce(root);
  const report = validatePackageDirectory(path.join(created.outDir, 'extracted'));
  assert.equal(report.ok, true);
  assert.equal(report.checked.length, 4);
});

test('rejects an asset ZIP renamed to .dat and a WAV renamed to .ogg', () => {
  const root = workspace();
  fs.writeFileSync(path.join(root, 'games', 'th128', 'th128.dat'), ZIP);
  assert.throws(() => planPackage({ input: INPUT, rootDir: root }), /dat-is-zip/);

  fs.writeFileSync(path.join(root, 'games', 'th128', 'th128.dat'), DAT);
  fs.writeFileSync(path.join(root, 'games', 'th128', 'music', 'ogg', 'th128_08.ogg'), Buffer.from('RIFF....WAVE', 'ascii'));
  assert.throws(() => planPackage({ input: INPUT, rootDir: root }), /not-ogg/);
});

test('enforces the source/target layout family', () => {
  assert.equal(classifyEntry({ source: 'games/th128/th128.dat', target: '/th128.dat' }), 'data');
  assert.equal(classifyEntry({ source: 'games/th128/music/ogg/a.ogg', target: '/music/a.ogg' }), 'ogg');
  assert.throws(() => classifyEntry({ source: 'games/th128/th128.dat', target: '/assets/th128.dat' }), /invalid-layout/);
  assert.throws(() => classifyEntry({ source: 'games/th128/music/ogg/a.ogg', target: '/assets/music/a.ogg' }), /invalid-layout/);
  assert.throws(() => classifyEntry({ source: 'assets/catalog.json', target: '/assets/catalog.json' }), /invalid-layout/);
  assert.throws(() => classifyEntry({ source: 'shared/msgothic.ttc', target: '/fonts/msgothic.ttc' }), /invalid-layout/);
});

test('rejects a foreign dataLayout and a package without language entries', () => {
  const root = workspace();
  const created = createOnce(root);
  const descriptor = JSON.parse(created.descriptorBytes.toString('utf8'));

  const foreign = structuredClone(descriptor);
  foreign.runtimeRequirement.dataLayout = TH15_PINNED_DATA_LAYOUT;
  assert.throws(() => validatePackageDescriptor(foreign), /data-layout-mismatch/);

  const noLanguage = structuredClone(descriptor);
  delete noLanguage.components.language;
  assert.throws(() => validatePackageDescriptor(noLanguage), /invalid-components/);
});

test('base may only hold DAT and shared fonts; output directory is never overwritten', () => {
  const root = workspace();
  const created = createOnce(root);
  const descriptor = JSON.parse(created.descriptorBytes.toString('utf8'));
  const badBase = structuredClone(descriptor);
  badBase.base.files = ['game-data', 'music-th128-08'];
  assert.throws(() => validatePackageDescriptor(badBase), /invalid-base/);

  assert.throws(() => createOnce(root), /output-exists/);
});

test('validatePackageFiles reports byte tampering after creation', () => {
  const root = workspace();
  const created = createOnce(root);
  const extracted = path.join(created.outDir, 'extracted');
  fs.appendFileSync(path.join(extracted, 'games', 'th128', 'th128.dat'), 'x');
  const report = validatePackageFiles(created.descriptor, { rootDir: extracted });
  assert.equal(report.ok, false);
  assert.equal(report.problems[0].code, 'size-mismatch');
});

test('DAT content is pinned by expectedDatSha256 while dataLayout stays the mount identity', () => {
  const root = workspace();
  const plan = planPackage({ input: INPUT, rootDir: root, options: { expectedDatSha256: sha256Hex(DAT) } });
  assert.equal(plan.descriptor.runtimeRequirement.dataLayout, th128DataLayout());
  assert.throws(() => planPackage({ input: INPUT, rootDir: root, options: { expectedDatSha256: 'f'.repeat(64) } }), /dat-hash-mismatch/);
});

test('OGG loop metadata is carried verbatim when MAIN supplies it', () => {
  const root = workspace();
  const loopMetadata = { schema: 'th128-ogg-loop/1', tracks: [{ file: 'th128_08.ogg', loopStart: 1198848 }] };
  const created = createPackage({ input: { ...INPUT, loopMetadata }, rootDir: root, outDir: path.join(root, 'loops') });
  assert.deepEqual(created.descriptor.components.ogg.loopMetadata, loopMetadata);
  assert.deepEqual(JSON.parse(created.descriptorBytes.toString('utf8')).components.ogg.loopMetadata, loopMetadata);
});

test('TH15-style lang_zh-hans entry and target are accepted for the shipped Chinese component', () => {
  const root=workspace();
  const old=path.join(root,'games/th128/language/zh.zip');
  const canonical=path.join(root,'games/th128/language/lang_zh-hans.zip');
  fs.copyFileSync(old,canonical);
  const input=structuredClone(INPUT);
  const language=input.files.find(f=>f.id==='lang-zh');
  language.id='language:lang_zh-hans';
  language.source='games/th128/language/lang_zh-hans.zip';
  language.target='/__eagler/language/lang_zh-hans.zip';
  language.sourceFile=language.source;
  input.languageEntries=[{id:'lang_zh-hans',title:'Chinese',file:'language:lang_zh-hans'}];
  const made=createPackage({input,rootDir:root,outDir:path.join(root,'canonical')});
  assert.deepEqual(made.descriptor.components.language.entries,input.languageEntries);
  assert.equal(validatePackageDirectory(path.join(made.outDir,'extracted')).ok,true);
});
