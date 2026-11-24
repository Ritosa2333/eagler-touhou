import {test, beforeAll as before, afterAll as after} from 'vitest';
import assert from 'node:assert/strict';
import {mkdir, mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {ensureI18nBuild} from '../../lib/i18n-build.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
let work, api;
before(async () => {
  await ensureI18nBuild(); await mkdir(resolve(project, '.cache'), {recursive: true});
  work = await mkdtemp(resolve(project, '.cache/paraglide-tests-'));
  const outfile = resolve(work, 'actual.mjs');
  await build({stdin: {resolveDir: project, loader: 'ts', contents: 'export * from "./src/launcher/i18n.mts"; export * from "./app/navigation/initial-ui-locale.ts";'}, outfile, bundle: true, platform: 'node', format: 'esm', logLevel: 'silent'});
  api = await import(pathToFileURL(outfile).href);
});
after(async () => {if (work) await rm(work, {recursive: true, force: true});});
test('all three compiled catalogs expose the same semantic keys', () => {
  assert.deepEqual(api.UI_LOCALES, ['zh-CN', 'en', 'ja']);
  const keys = api.validateUiCatalogs();
  for (const locale of api.UI_LOCALES) assert.deepEqual(Object.keys(api.UI_MESSAGES[locale]), keys);
  assert.equal(api.resolveUiLocale('ja-JP'), 'ja'); assert.equal(api.isUiLocale('ja'), true);
  assert.equal(api.isUiMessageKey('room.rollbackDelayHint'), true);
});
test('compiled interpolation preserves numeric zero, empty values and missing parameters', () => {
  assert.equal(api.translateUi('en', 'status.roomCreated', {code: 0}), 'Room 0 created');
  assert.equal(api.translateUi('en', 'status.roomCreated', {code: ''}), 'Room  created');
  assert.equal(api.translateUi('en', 'status.roomCreated'), 'Room {code} created');
  assert.equal(api.translateUi('ja', 'status.roomCreated', {code: '1234'}).includes('1234'), true);
});
test('explicit locales do not depend on the legacy current-locale state', () => {
  api.setUiLocale('en', {persist: false, notify: false});
  const ja = api.translateUi('ja', 'nav.lessMotion');
  assert.notEqual(ja, api.translateUi('en', 'nav.lessMotion'));
  for (let index = 0; index < 100; index++) {
    assert.equal(api.translateUi('zh-CN', 'nav.lessMotion'), '更少动画');
    assert.equal(api.translateUi('ja', 'nav.lessMotion'), ja);
    assert.equal(api.t('nav.lessMotion'), 'Less motion');
  }
});
test('a missing semantic key keeps the diagnostic fallback', () => {
  const warn = console.warn, warnings = [];
  try {console.warn = value => warnings.push(value); assert.equal(api.translateUi('ja', 'fixture.unknown'), 'fixture.unknown');}
  finally {console.warn = warn;}
  assert.equal(warnings.length, 1);
});
test('language replacement preserves unrelated query bytes and removes duplicate locale fields', () => {
  const search = '?game=th10&name=a%20b&encoded=%2f%2F&lang=en&l%61ng=zh-CN&value=a+b';
  assert.equal(api.uiLocaleSearch(search, 'ja', true), '?game=th10&name=a%20b&encoded=%2f%2F&value=a+b&lang=ja');
  assert.equal(api.uiLocaleSearch(search, 'en'), '?game=th10&name=a%20b&encoded=%2f%2F&value=a+b');
  assert.equal(api.withUiLanguage('faq.html?q=a%20b#touch', 'ja'), 'faq.html?q=a%20b&lang=ja#touch');
});
test('localized entry addresses distinguish all three published documents', () => {
  assert.equal(api.localeHome('ja'), 'ja.html'); assert.equal(api.localeFromPath('/site/ja.html'), 'ja');
  assert.equal(api.localeFromPath('/site/en.html'), 'en'); assert.equal(api.localeHome('zh-CN'), './');
});

function launcherEntry({url = '/site/?game=th03&q=a%20b#settings', manual, cookie = '', oldRegionalChoice} = {}) {
  const origin = 'https://launcher.invalid', store = new Map();
  if (manual) store.set(api.UI_LOCALE_STORAGE_KEY, manual);
  if (oldRegionalChoice) store.set('eagler-touhou-region-locale-v1', oldRegionalChoice);
  const storage = {getItem: key => store.get(key), setItem: (key, value) => store.set(key, value)};
  const document = {cookie};
  const enter = (path = url, reload = false) => api.initialUiLocaleAddress({href: new URL(path, origin).href, base: '/site/',
    storage, document, reload});
  return {enter, store, document};
}

test('plain entries default to Chinese for new visits and ignore old automatic choices', () => {
  for (const oldRegionalChoice of [undefined, 'en', 'ja', 'zh-CN']) {
    const {enter} = launcherEntry({oldRegionalChoice});
    assert.equal(enter(), undefined);
    assert.equal(enter('/site/index.html'), undefined);
    assert.equal(enter('/site/lobby.html?game=th03mp&room=1234#watch'), undefined);
  }
});

test('explicit links and saved manual languages retain their values and URL payload', () => {
  const {enter} = launcherEntry({manual: 'ja', oldRegionalChoice: 'en'});
  assert.equal(enter()?.href, 'https://launcher.invalid/site/ja.html?game=th03&q=a%20b#settings');
  assert.equal(enter('/site/lobby.html?lang=zh-CN&room=1234'), undefined);
  assert.equal(enter('/site/en.html?game=th03'), undefined);
  assert.equal(enter('/site/ja.html'), undefined);
  assert.equal(enter('/site/ui-review.html'), undefined);
});

test('reloading a legacy automatic English address restores the default language', () => {
  const {enter} = launcherEntry({oldRegionalChoice: 'en'});
  assert.equal(enter('/site/en.html?game=th03&q=a%20b#settings'), undefined);
  assert.equal(enter('/site/en.html?lang=en', true), undefined);
  assert.equal(enter('/site/en.html?game=th03&q=a%20b#settings', true)?.href,
    'https://launcher.invalid/site/?game=th03&q=a%20b#settings');
  assert.equal(launcherEntry({manual: 'en', oldRegionalChoice: 'en'}).enter('/site/en.html', true), undefined);
});

test('manual language cookie overrides the old storage value and is shared across entry pages', () => {
  const {enter, store, document} = launcherEntry({manual: 'en', cookie: `other=1; ${api.UI_LOCALE_STORAGE_KEY}=ja`});
  assert.equal(enter()?.pathname, '/site/ja.html');
  assert.equal(enter('/site/lobby.html?lang=en'), undefined);
  assert.equal(enter('/site/faq.html#guide')?.href, 'https://launcher.invalid/site/faq.html?lang=ja#guide');
  assert.equal(store.get(api.UI_LOCALE_STORAGE_KEY), 'en');
  api.saveUiLocalePreference(document, null, new URL('https://launcher.invalid/site/'), 'zh-CN');
  assert.equal(document.cookie, `${api.UI_LOCALE_STORAGE_KEY}=zh-CN; Path=/site/; Max-Age=31536000; SameSite=Lax; Secure`);
  assert.equal(enter('/site/lobby.html'), undefined);
});
