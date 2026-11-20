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

function regionalEntry({country = 'JP', url = '/site/?game=th03&q=a%20b#settings', manual, cookie = '', online = true, fetchCountry} = {}) {
  const origin = 'https://launcher.invalid', store = new Map(), requests = [];
  if (manual) store.set(api.UI_LOCALE_STORAGE_KEY, manual);
  const storage = {getItem: key => store.get(key), setItem: (key, value) => store.set(key, value)};
  const document = {cookie};
  const enter = (path = url) => api.initialUiLocaleAddress({href: new URL(path, origin).href, base: '/site/',
    storage, document, online, timeoutMs: 20, async fetchCountry(endpoint, init) {
      requests.push([endpoint.href, init.cache, init.credentials, init.mode]);
      return fetchCountry ? fetchCountry(endpoint, init) : new Response(country);
    }});
  return {enter, store, requests, document};
}

test('first visit selects the country language and keeps the URL payload intact', async () => {
  for (const country of ['CN', 'HK', 'MO', 'TW', 'JP', 'US', 'GB', 'SG']) {
    const expected = api.localeForCountry(country);
    const {enter, requests, store} = regionalEntry({country});
    const first = await enter(), second = await enter('/site/lobby.html?game=th03mp&room=1234#watch');
    assert.equal(first?.href, expected === 'zh-CN' ? undefined : `https://launcher.invalid/site/${expected}.html?game=th03&q=a%20b#settings`);
    assert.equal(second?.href, expected === 'zh-CN' ? undefined : `https://launcher.invalid/site/lobby.html?game=th03mp&room=1234&lang=${expected}#watch`);
    assert.equal(expected, ['CN','HK','MO','TW'].includes(country) ? 'zh-CN' : country === 'JP' ? 'ja' : 'en');
    assert.deepEqual(requests, [['https://launcher.invalid/site/_eagler/ui-country', 'no-store', 'omit', 'same-origin']]);
    assert.equal(store.get(api.UI_LOCALE_STORAGE_KEY), undefined, 'automatic locale is not a manual choice');
    store.set(api.UI_LOCALE_STORAGE_KEY, 'ja');
    assert.equal((await enter())?.pathname, '/site/ja.html');
  }
});

test('explicit links and saved manual language never query IP location', async () => {
  const {enter, requests} = regionalEntry({manual: 'ja', country: 'CN'});
  assert.equal((await enter())?.pathname, '/site/ja.html');
  assert.equal(await enter('/site/lobby.html?lang=zh-CN&room=1234'), undefined);
  assert.equal(await enter('/site/en.html?game=th03'), undefined);
  assert.equal(await enter('/site/ja.html'), undefined);
  assert.equal(await enter('/site/ui-review.html'), undefined);
  assert.deepEqual(requests, []);
});

test('manual language cookie overrides the old storage value and is shared across entry pages', async () => {
  const {enter, store, requests, document} = regionalEntry({manual: 'en', cookie: `other=1; ${api.UI_LOCALE_STORAGE_KEY}=ja`});
  assert.equal((await enter())?.pathname, '/site/ja.html');
  assert.equal(await enter('/site/lobby.html?lang=en'), undefined);
  assert.equal((await enter('/site/faq.html#guide'))?.href, 'https://launcher.invalid/site/faq.html?lang=ja#guide');
  assert.equal(store.get(api.UI_LOCALE_STORAGE_KEY), 'en');
  assert.deepEqual(requests, []);
  api.saveUiLocalePreference(document, null, new URL('https://launcher.invalid/site/'), 'zh-CN');
  assert.equal(document.cookie, `${api.UI_LOCALE_STORAGE_KEY}=zh-CN; Path=/site/; Max-Age=31536000; SameSite=Lax; Secure`);
  assert.equal(await enter('/site/lobby.html'), undefined);
  assert.deepEqual(requests, []);
});

test('offline, unknown country, HTTP errors and timeouts keep the first English default without another lookup', async () => {
  for (const options of [
    {online: false}, {country: 'ZZ'}, {country: '<!doctype html>'},
    {fetchCountry: () => new Response('JP', {status: 404})},
    {fetchCountry: (_, {signal}) => new Promise((_, reject) => signal.addEventListener('abort', () => reject(signal.reason), {once: true}))},
  ]) {
    const {enter, store, requests} = regionalEntry(options);
    assert.equal((await enter())?.pathname, '/site/en.html');
    assert.equal((await enter())?.pathname, '/site/en.html');
    assert.equal(store.get(api.UI_LOCALE_STORAGE_KEY), undefined);
    assert.equal(requests.length, options.online === false ? 0 : 1);
  }
});
