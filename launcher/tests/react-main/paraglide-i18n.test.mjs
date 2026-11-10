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
  await build({stdin: {resolveDir: project, loader: 'ts', contents: 'export * from "./src/launcher/i18n.mts";'}, outfile, bundle: true, platform: 'node', format: 'esm', logLevel: 'silent'});
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
