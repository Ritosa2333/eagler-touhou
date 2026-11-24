import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {test} from 'vitest';
import {createGameSettingsModel} from '../../app/models/game-settings.ts';
import {createMetadataService} from '../../app/services/metadata.ts';
import {liveRuntimeEntryAuthorized} from '../../app/services/runtime-entry-authority.ts';
import {HOST_PROTOCOL, productEnabledForBuild, productRequiresImport} from '../../src/contracts/product-catalog.mts';
import {RELEASE_CATALOG_SCHEMA} from '../../src/contracts/release-catalog.mts';
import {SITE_MANIFEST_SCHEMA, validateSiteManifest} from '../../src/contracts/site-manifest.mts';
import {createManagedRuntimeGenerationLease} from '../../src/launcher/runtime-generation-lease.mts';
import {gamePreferenceStorageKey} from '../../src/launcher/game-preferences.mts';
import {th128InstalledConfigure, th128InstalledMusic} from '../../src/launcher/th128-installed-config.mts';
import {provideTh128ManagedPackage, th128MountedLanguage} from '../../src/launcher/th128-managed-package.mts';
import {publishedLanguageSelection} from '../../package/package-launcher.mjs';

// The DATA layout is a public format identifier. Bodies and installation
// references below are synthetic; no DAT, OGG, ZIP, service or browser is used.
const layout = 'sha256-58cd45fe7cac9af281d9de993a527ccfbbe6f27b89bcd0ca98a7031333fce200';
const hostOrigin = 'https://launcher.example.invalid/';
const entry = '/fixtures/th128/th128.html';
const language = {id: 'lang_zh-hans', title: '简体中文', pack: null};

function packageFixture() {
  const generation = {id: 'install-1', game: 'th128', files: {}, descriptor: {
    schema: 'eagler-touhou/package/1', game: 'th128', revision: 'fixture', files: {},
    runtimeRequirement: {protocol: HOST_PROTOCOL, target: 'th128', dataFile: 'game-data', dataLayout: layout},
    base: {files: ['game-data']},
    components: {ogg: {type: 'ogg', files: []}, language: {type: 'language', entries: [{id: language.id, file: 'language-zh'}]}},
  }};
  const objects = new Map();
  const add = (id, target, value) => {
    const body = Uint8Array.of(value, 0);
    const sha256 = createHash('sha256').update(body).digest('hex');
    generation.descriptor.files[id] = {source: `fixture/${id}`, target, bytes: body.length, sha256, revision: sha256.slice(0, 16)};
    generation.files[id] = {objectId: `fixture-${id}`, revision: sha256.slice(0, 16)};
    objects.set(id, body);
  };
  add('game-data', '/th128.dat', 1);
  add('language-zh', '/__eagler/language/lang_zh-hans.zip', 2);
  for (const [index, track] of ['00', '01', '02', '05', '06', '07', '08', '09', '10', '11'].entries()) {
    const id = `music-${track}`;
    add(id, `/music/th128_${track}.ogg`, index + 3);
    generation.descriptor.components.ogg.files.push(id);
  }
  const lease = createManagedRuntimeGenerationLease();
  lease.bind('th128', generation);
  const reads = [];
  const read = (selected, id) => {
    assert.equal(selected, generation);
    reads.push(id);
    const body = objects.get(id);
    return {fileId: id, path: generation.descriptor.files[id].target, bytes: body.length, buffer: body.slice().buffer};
  };
  return {lease, generation, reads, ports: {
    generation, epoch: 4,
    checkCurrent() {assert.equal(lease.resolve({game: 'th128', generation: generation.id}), generation);},
    async readData(selected) {return read(selected, 'game-data');},
    async readResource(selected, id) {return read(selected, id);},
  }};
}

test('TH128 publishes three choices and mounts only the installed edition', async () => {
  const {generation, ports} = packageFixture();
  const {descriptor} = generation;
  descriptor.components.language.entries.unshift({id: 'ja', file: 'language-ja'});
  descriptor.files['language-ja'] = {...descriptor.files['language-zh'],
    source: 'fixture/language-ja', target: '/__eagler/language/lang_ja.zip'};
  descriptor.components.language.entries.push({id: 'lang_en', file: 'language-en'});
  descriptor.files['language-en'] = {...descriptor.files['language-zh'],
    source: 'fixture/language-en', target: '/__eagler/language/lang_en.zip'};
  assert.deepEqual(publishedLanguageSelection(descriptor, 'ja'), {language: ['ja']});
  assert.deepEqual(publishedLanguageSelection(descriptor, 'lang_zh-hans'), {language: ['lang_zh-hans']});
  assert.deepEqual(publishedLanguageSelection(descriptor, 'lang_en'), {language: ['lang_en']});
  assert.equal(th128InstalledConfigure('lang_en', 'ogg-full', {}, 'lang_en').configure.language, 'en');
  assert.equal(th128MountedLanguage(generation), 'lang_zh-hans');
  const {managedPackage} = await provideTh128ManagedPackage(ports);
  const mounted = JSON.parse(new TextDecoder().decode(managedPackage.descriptor));
  assert.deepEqual(mounted.components.language.entries, [{id: 'lang_zh-hans', file: 'language-zh'}]);
  assert.ok(!Object.hasOwn(mounted.files, 'language-ja'));
  assert.ok(!Object.hasOwn(mounted.files, 'language-en'));
  await assert.rejects(managedPackage.readFile('language-ja'), /not declared/);
});

function hostFixture() {
  const {bytes, sha256} = packageFixture().generation.descriptor.files['game-data'];
  return {schema: SITE_MANIFEST_SCHEMA, protocol: HOST_PROTOCOL, profile: 'test-local-adapter',
    shared: {resourceMode: 'import', testBuild: true},
    games: {th128: {runtime: entry, gameData: {path: 'th128.dat', bytes, sha256, version: `sha256-${sha256}`, layout},
      music: {midi: {files: [], supported: false}, ogg: null}, features: {thprac: true},
      languages: [], languageOptions: [language], offlineCompatibility: {
        schema: 'eagler-touhou/offline-game-pack/1',
        runtimeCompatibility: {protocol: HOST_PROTOCOL, dataLayout: layout, versionSource: 'offline-pack'},
        requiredShared: [], languages: {source: 'offline-pack', baseline: [language.id]},
      },
    }},
  };
}

test('TH128 preview authorizes only the declared same-origin single-player entry', () => {
  const host = validateSiteManifest(hostFixture());
  const plan = {game: 'th128', runtimeVariant: 'normal', entry, runtimeHost: host};
  assert.equal(productRequiresImport('th128'), true);
  assert.equal(productEnabledForBuild('th128', false), false);
  assert.equal(productEnabledForBuild('th128', host.shared.testBuild), true);
  assert.equal(liveRuntimeEntryAuthorized(plan, hostOrigin), true);
  for (const changed of [{entry: entry + '?other=1'}, {entry: entry + '#other'}, {runtimeVariant: 'multiplayer'}]) {
    assert.equal(liveRuntimeEntryAuthorized({...plan, ...changed}, hostOrigin), false);
  }
  const foreign = structuredClone(host);
  foreign.games.th128.runtime = 'https://other.example.invalid/th128.html';
  assert.equal(liveRuntimeEntryAuthorized({...plan, entry: foreign.games.th128.runtime, runtimeHost: foreign}, hostOrigin), false);
});

test('offline language baseline is a property of the published product', () => {
  const missingChinese = structuredClone(hostFixture());
  missingChinese.games.th128.offlineCompatibility.languages.baseline = ['ja'];
  assert.throws(() => validateSiteManifest(missingChinese), /invalid Site Manifest games/);
  const downloadHost = structuredClone(hostFixture());
  downloadHost.games.th128.gameData.source = '/th128.dat';
  assert.throws(() => validateSiteManifest(downloadHost), /invalid Site Manifest games/);

  const {games: {th128: other}, ...host} = structuredClone(hostFixture());
  other.gameData.path = 'th06.data';
  other.runtime = '/fixtures/th06/th06.html';
  other.languageOptions = [{id: 'ja', pack: null}];
  other.offlineCompatibility.languages.baseline = ['ja'];
  other.offlineCompatibility.requiredShared = ['/msgothic.ttc', '/unifont.otf'];
  const japaneseHost = {...host, games: {th06: other}};
  assert.doesNotThrow(() => validateSiteManifest(japaneseHost));
  other.offlineCompatibility.languages.baseline = [language.id];
  assert.throws(() => validateSiteManifest(japaneseHost), /invalid Site Manifest games/);
});

test('the real Launcher offers only the installed game language even in its Japanese interface', async () => {
  const installed = packageFixture().generation;
  const metadata = createMetadataService({
    baseUrl: hostOrigin, storage: null, mobile: false, webAudioAvailable: true, webMidiAvailable: false,
    translate: (_locale, key) => key,
    indexedDBFactory: {open() {throw new Error('This test does not open an installation database');}},
    async fetchImpl(request) {
      const path = new URL(request).pathname;
      if (path === '/site-manifest.json') return Response.json(hostFixture());
      if (path === '/release-catalog.json') return Response.json({schema: RELEASE_CATALOG_SCHEMA, games: {}});
      throw new Error(`Unexpected fixture metadata request: ${path}`);
    },
  });
  try {
    await metadata.initialize();
    assert.equal(metadata.getSnapshot().siteManifestError, null);
    assert.deepEqual(metadata.getSnapshot().products, ['th128']);
    metadata.acceptInstalledHint('th128', installed);
    const options = metadata.settingsContext('th128', 'ja');
    assert.deepEqual(options.languages.map(({id}) => id), [language.id]);
    const model = createGameSettingsModel({storage: null});
    model.hydrate(options);
    assert.equal(model.getSnapshot().language, language.id);
    model.setLanguage('ja');
    assert.equal(model.getSnapshot().language, language.id);
    assert.equal(model.getSnapshot().context.uiLocale, 'ja');
    assert.deepEqual(metadata.settingsContext('th06', 'ja').languages.map(({id}) => id), ['ja']);
  } finally {metadata.dispose();}
});

test('the parent requires the whole installation before exposing any resource', async () => {
  const missing = packageFixture();
  delete missing.generation.files['music-11'];
  await assert.rejects(provideTh128ManagedPackage(missing.ports), /missing installed file: music-11/);
  assert.deepEqual(missing.reads, []);

  const complete = packageFixture();
  const {buffer, managedPackage} = await provideTh128ManagedPackage(complete.ports);
  assert.deepEqual([...new Uint8Array(buffer)], [1, 0]);
  assert.equal(managedPackage.version, 'th128-dat-ogg/1');
  assert.equal(managedPackage.generation, 'install-1');
  assert.equal(managedPackage.epoch, 4);
  assert.deepEqual(JSON.parse(new TextDecoder().decode(managedPackage.descriptor)), complete.generation.descriptor);
  await assert.rejects(managedPackage.readFile('toString'), /not declared/);
  await assert.rejects(managedPackage.readFile('game-data'), /not declared/);
  const track = await managedPackage.readFile('music-05');
  assert.equal(track.path, '/music/th128_05.ogg');
  assert.deepEqual([...new Uint8Array(track.buffer)], [6, 0]);
  assert.deepEqual(complete.reads, ['game-data', 'music-05']);
});

test('a deferred read cannot escape after leaving one installation and reopening another', async () => {
  const fixture = packageFixture();
  let resume;
  const hold = new Promise(resolve => {resume = resolve;});
  const {managedPackage} = await provideTh128ManagedPackage({...fixture.ports,
    async readResource(...args) {const receipt = await fixture.ports.readResource(...args); await hold; return receipt;},
  });
  const rejected = assert.rejects(managedPackage.readFile('music-11'), /inactive game generation/);
  try {
    assert.deepEqual(fixture.reads, ['game-data', 'music-11']);
    fixture.lease.clear();
    fixture.lease.bind('th128', {...fixture.generation, id: 'install-2'});
  } finally {resume();}
  await rejected;
  await assert.rejects(managedPackage.readFile('language-zh'), /inactive game generation/);
  assert.deepEqual(fixture.reads, ['game-data', 'music-11']);
});

test('direct touch remains the TH128 default without overwriting another game and OGG is a full track', () => {
  const values = new Map();
  const storage = {getItem: key => values.get(key) ?? null, setItem: (key, value) => {values.set(key, value);}};
  const model = createGameSettingsModel({storage});
  const {generation} = packageFixture();
  const context = {uiLocale: 'ja', mobile: false, webMidiAvailable: false,
    musicAvailability: {audio: true, midiAvailable: false, importServer: true, remoteOggAdvertised: false},
  };
  const other = {...context, productId: 'th06', languages: [{id: 'ja', pack: null}]};
  const th128 = {...context, productId: 'th128', languages: [language]};
  th128.musicAvailability = {...context.musicAvailability, installed: {files: generation.files, oggFileIds: generation.descriptor.components.ogg.files}};
  model.hydrate(other);
  model.setOption('touchMovementMode', 'touch-unlimited');
  model.hydrate(th128);
  assert.equal(model.getSnapshot().options.touchMovementMode, 'touch');
  assert.equal(model.getSnapshot().music, 'ogg-full');
  assert.equal(model.getSnapshot().language, language.id);
  model.setMusic('none');
  model.hydrate(other);
  assert.equal(model.getSnapshot().options.touchMovementMode, 'touch-unlimited');
  model.hydrate(th128);
  assert.equal(model.getSnapshot().options.touchMovementMode, 'touch');
  assert.equal(model.getSnapshot().music, 'none');

  const options = {thpracEnabled: true, thpracLocale: 'ja', limitPresentationTo60: false, touchEnabled: true, touchMovementMode: 'touch'};
  const config = th128InstalledConfigure(language.id, 'ogg-full', options).configure;
  assert.deepEqual(config, {language: 'zh', music: 'ogg', runtimePack: null, options: {...options, oggDecodeMode: 'full'}});
  assert.equal(th128InstalledConfigure(language.id, 'none', options).configure.music, 'none');
});

test('TH128 can choose full OGG before the local installation hint is ready', () => {
  const stored = new Map();
  const storage = {getItem: key => stored.get(key) ?? null, setItem: (key, value) => {stored.set(key, value);}};
  const settings = createGameSettingsModel({storage});
  const source = {audio: true, midiAvailable: false, importServer: true, remoteOggAdvertised: false};
  const context = {productId: 'th128', uiLocale: 'zh-CN', mobile: false, webMidiAvailable: false,
    languages: [language], musicAvailability: source};
  settings.hydrate(context);
  assert.equal(settings.getSnapshot().music, 'none');
  settings.setMusic('none');
  settings.setMusic('ogg-full');
  assert.equal(settings.getSnapshot().musicPreference, 'ogg-full');
  assert.equal(settings.getSnapshot().musicPreferenceExplicit, true);
  assert.equal(settings.getSnapshot().music, 'none');
  settings.hydrate(context);
  assert.equal(settings.getSnapshot().musicPreference, 'ogg-full');
  const {generation} = packageFixture();
  settings.refreshContext({...context, musicAvailability: {...source, installed: {
    revision: generation.descriptor.revision, oggFileIds: generation.descriptor.components.ogg.files, files: generation.files,
  }}});
  assert.equal(settings.getSnapshot().music, 'ogg-full');
  assert.equal(th128InstalledConfigure(language.id, settings.getSnapshot().music, {}).configure.music, 'ogg');
});

test('an implicit no-resource fallback does not persist TH128 with music off', () => {
  const key = gamePreferenceStorageKey('th128');
  let saved = JSON.stringify({music: 'none', musicPreferenceExplicit: false});
  const storage = {getItem: id => id === key ? saved : null, setItem: (id, value) => {if (id === key) saved = value;}};
  const settings = createGameSettingsModel({storage});
  const context = {productId: 'th128', uiLocale: 'zh-CN', mobile: false, webMidiAvailable: false, languages: [language],
    musicAvailability: {audio: true, midiAvailable: false, importServer: true, remoteOggAdvertised: false}};
  settings.hydrate(context);
  assert.equal(settings.getSnapshot().musicPreference, 'ogg-full');
  settings.commitPreferences();
  assert.equal(JSON.parse(saved).music, 'ogg-full');
  settings.setMusic('none');
  settings.hydrate(context);
  assert.equal(settings.getSnapshot().musicPreference, 'none');
  assert.equal(settings.getSnapshot().musicPreferenceExplicit, true);
});

test('TH128 music consent and configuration require the whole installed soundtrack', () => {
  const {generation} = packageFixture();
  assert.equal(th128InstalledMusic(generation, 'ogg-full', true), 'ogg-full');
  assert.equal(th128InstalledMusic(generation, 'none', true), 'none');
  assert.equal(th128InstalledMusic(generation, 'ogg-full', false), 'none');
  delete generation.files['music-11'];
  assert.throws(() => th128InstalledMusic(generation, 'ogg-full', true), /missing installed file: music-11/);
});
