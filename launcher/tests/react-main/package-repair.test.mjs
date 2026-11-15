import {test, beforeAll, beforeEach, afterAll} from 'vitest';
import assert from 'node:assert/strict';
import {mkdir, mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {IDBFactory, IDBKeyRange} from 'fake-indexeddb';
import {authoredSourcesPlugin} from './authored-sources.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
const baseUrl = 'https://repair.invalid/';
const data = new Uint8Array([1, 2, 3, 4]), music = new Uint8Array([5, 6, 7]);
const nextData = new Uint8Array([4, 3, 2, 1]);
const previousFactory = globalThis.indexedDB, previousKeyRange = globalThis.IDBKeyRange;
let api, directory, dataHash, musicHash;
beforeAll(async () => {
  await mkdir(resolve(project, '.cache'), {recursive: true});
  directory = await mkdtemp(resolve(project, '.cache/package-repair-'));
  const outfile = resolve(directory, 'actual-package.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, contents: `
    export * as store from './package/package-store.mjs';
    export {installPackageFromAcquisition} from './package/package-installer.mjs';
    export {repairInstalledPackage, installPublishedPackage} from './package/package-launcher.mjs';
    export {createPackageAcquisition} from './app/services/package-acquisition.ts';
    export {sha256Hex} from './package/package-integrity.mjs';
  `}, outfile, bundle: true, format: 'esm', platform: 'node', packages: 'external',
    plugins: [authoredSourcesPlugin(project)]});
  api = await import(pathToFileURL(outfile).href);
  dataHash = await api.sha256Hex(data); musicHash = await api.sha256Hex(music);
});
beforeEach(() => {
  globalThis.indexedDB = new IDBFactory();
  globalThis.IDBKeyRange = IDBKeyRange;
});
afterAll(async () => {
  if (previousFactory === undefined) delete globalThis.indexedDB;
  else globalThis.indexedDB = previousFactory;
  if (previousKeyRange === undefined) delete globalThis.IDBKeyRange;
  else globalThis.IDBKeyRange = previousKeyRange;
  if (directory) await rm(directory, {recursive: true, force: true});
});
function descriptor(revision = 'package-a', gameVersion = 'original') {
  return {schema: 'eagler-touhou/package/1', game: 'th06', revision, gameVersion,
    runtimeRequirement: {protocol: 'eagler-touhou/1', target: 'th06', dataFile: 'data', dataLayout: 'layout-test'},
    files: {
      data: {source: 'game/data.bin', target: '/th06.data', bytes: data.length, revision: dataHash.slice(0, 16), sha256: dataHash},
      music: {source: 'music/01.ogg', target: '/bgm-ogg/01.ogg', bytes: music.length, revision: musicHash.slice(0, 16), sha256: musicHash},
    }, base: {files: ['data']}, components: {ogg: {type: 'ogg', files: ['music']}}};
}
async function install(value = descriptor()) {
  await api.installPackageFromAcquisition({descriptor: value, desiredFileIds: ['data', 'music'], source: 'local',
    acquire: async id => id === 'data' ? data : music});
  return api.store.readCurrentPackageGeneration('th06', {gameVersion: value.gameVersion});
}
async function damage(installed, id, bytes = new Uint8Array(id === 'data' ? 4 : 3)) {
  await api.store.putPackageObject(bytes, {objectId: installed.generation.files[id].objectId});
}
function publication(value = descriptor(), replacement = data) {
  const requests = [];
  const catalog = {schema: 'eagler-touhou/release-catalog/1',
    games: {th06: {revision: value.revision, descriptor: 'th06.package.json'}}};
  const fetchImpl = async input => {
    const pathname = new URL(input).pathname; requests.push(pathname);
    if (pathname === '/th06.package.json') return Response.json(value);
    if (pathname === '/game/data.bin') return new Response(replacement);
    if (pathname === '/music/01.ogg') return new Response(music);
    throw Error('Unexpected download ' + pathname);
  };
  return {requests, catalog, fetchImpl, options: {
    getPublication: async () => ({catalog, catalogUrl: baseUrl + 'release-catalog.json'}), fetchImpl,
  }};
}
function acquisition(published) {
  return api.createPackageAcquisition({baseUrl, translate: key => key,
    metadata: {getSnapshot: () => ({siteManifest: null, releaseCatalog: published.catalog}), initialize: async () => {}},
    fetchImpl: published.fetchImpl});
}
const request = {productId: 'th06', music: 'none', language: 'ja'};

test('healthy installed resources launch offline without consulting the publication', async () => {
  const installed = await install();
  assert.equal(await api.repairInstalledPackage(installed, {
    getPublication: async () => {throw Error('Unexpected network');},
  }), installed.generation);
});
for (const size of [3, 4]) test(`damaged required file (${size} bytes) is verified and replaced atomically`, async () => {
  const installed = await install(), published = publication();
  await damage(installed, 'data', new Uint8Array(size));
  const result = await api.repairInstalledPackage(installed, published.options);
  assert.notEqual(result.id, installed.generation.id);
  assert.equal(result.files.music.objectId, installed.generation.files.music.objectId);
  assert.deepEqual(published.requests, ['/th06.package.json', '/game/data.bin']);
  assert.equal((await api.store.readCurrentPackageGeneration('th06')).generation.id, result.id);
});
test('a rejected replacement leaves the current installation unchanged', async () => {
  const installed = await install(); await damage(installed, 'data');
  await assert.rejects(api.repairInstalledPackage(installed, publication(descriptor(), new Uint8Array(4)).options), /SHA-256 mismatch/);
  assert.equal((await api.store.readCurrentPackageGeneration('th06')).generation.id, installed.generation.id);
});
test('legacy content revisions match the published full SHA-256 declaration', async () => {
  const legacy = descriptor('legacy'); delete legacy.files.data.sha256;
  const installed = await install(legacy); await damage(installed, 'data', new Uint8Array(3));
  const result = await api.repairInstalledPackage(installed, publication(descriptor('published')).options);
  assert.notEqual(result.files.data.objectId, installed.generation.files.data.objectId);
});
test('damaged optional music does not block a silent offline launch', async () => {
  const installed = await install(); await damage(installed, 'music');
  const service = acquisition({catalog: null, fetchImpl: async () => {throw Error('Unexpected network');}});
  const result = await service.acquire(request);
  assert.equal(result.kind, 'managed');
  assert.equal(result.generation.files.data.objectId, installed.generation.files.data.objectId);
  assert.equal(result.generation.files.music, undefined);
  assert.deepEqual(result.generation.descriptor.components.ogg.files, ['music']);
});
test('required repairs also drop damaged optional references and reuse healthy data', async () => {
  const installed = await install(); await damage(installed, 'data'); await damage(installed, 'music');
  const published = publication(), result = await api.repairInstalledPackage(installed, published.options);
  assert.equal(result.files.music, undefined);
  assert.deepEqual(published.requests, ['/th06.package.json', '/game/data.bin']);
});
for (const updateBeforePrepare of [false, true]) test(`obsolete damaged data can update before launch (room policy ${updateBeforePrepare})`, async () => {
  const installed = await install(); await damage(installed, 'data');
  const next = descriptor('package-b'); next.files.data.sha256 = await api.sha256Hex(nextData);
  next.files.data.revision = next.files.data.sha256.slice(0, 16);
  const published = publication(next, nextData), decisions = [];
  const result = await acquisition(published).acquire({...request, updateBeforePrepare,
    decideUpdate: async choice => {decisions.push(choice); return 'confirm';}});
  assert.equal(decisions.length, 1);
  assert.equal(result.generation.descriptor.revision, 'package-b');
  assert.equal(result.generation.files.music.objectId, installed.generation.files.music.objectId);
  assert.equal((await api.store.readCurrentPackageGeneration('th06')).generation.id, result.generation.id);
});
test('declining the replacement never launches the damaged old generation', async () => {
  const installed = await install(); await damage(installed, 'data');
  const next = descriptor('package-b'); next.files.data.sha256 = 'a'.repeat(64);
  next.files.data.revision = 'changed-data';
  const published = publication(next); let decisions = 0;
  await assert.rejects(acquisition(published).acquire({...request,
    decideUpdate: async () => {decisions++; return 'cancel';}}), /no matching cloud resource/);
  assert.equal(decisions, 1);
  assert.deepEqual(published.requests, ['/th06.package.json']);
  assert.equal((await api.store.readCurrentPackageGeneration('th06')).generation.id, installed.generation.id);
});
test('updating obsolete damaged data does not require unused damaged music', async () => {
  const installed = await install(); await damage(installed, 'data'); await damage(installed, 'music');
  const next = descriptor('package-b'); next.files.data.sha256 = await api.sha256Hex(nextData);
  next.files.data.revision = next.files.data.sha256.slice(0, 16);
  const published = publication(next, nextData), fetchPublished = published.fetchImpl;
  published.fetchImpl = async input => {
    assert.notEqual(new URL(input).pathname, '/music/01.ogg', 'unused music must not gate the replacement');
    return fetchPublished(input);
  };
  const result = await acquisition(published).acquire({...request, decideUpdate: async () => 'confirm'});
  assert.equal(result.generation.descriptor.revision, 'package-b');
  assert.equal(result.generation.files.music, undefined);
});
test('an explicitly requested damaged optional file is still acquired and verified', async () => {
  const installed = await install(); await damage(installed, 'music');
  const published = publication();
  const result = await api.installPublishedPackage('th06', {
    catalog: published.catalog, catalogUrl: baseUrl + 'release-catalog.json', addFileIds: ['music'], fetchImpl: published.fetchImpl,
  });
  assert.notEqual(result.generation.files.music.objectId, installed.generation.files.music.objectId);
  assert.deepEqual(published.requests, ['/th06.package.json', '/music/01.ogg']);
});
test('cancelling repair does not open an update dialog or replace current', async () => {
  const installed = await install(); await damage(installed, 'data');
  const published = publication(), controller = new AbortController(); let decisions = 0;
  published.fetchImpl = async () => {controller.abort(); throw new DOMException('Cancelled', 'AbortError');};
  await assert.rejects(acquisition(published).acquire({...request, signal: controller.signal,
    decideUpdate: async () => {decisions++; return 'confirm';}}), {name: 'AbortError'});
  assert.equal(decisions, 0);
  assert.equal((await api.store.readCurrentPackageGeneration('th06')).generation.id, installed.generation.id);
});
test('repairing beauty resources leaves the original installation untouched', async () => {
  const original = await install(), beauty = await install(descriptor('beauty-a', 'beauty'));
  await damage(beauty, 'data');
  const result = await api.repairInstalledPackage(beauty, publication(descriptor('beauty-a', 'beauty')).options);
  assert.equal(result.descriptor.gameVersion, 'beauty');
  assert.equal((await api.store.readCurrentPackageGeneration('th06')).generation.id, original.generation.id);
});
