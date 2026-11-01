import assert from 'node:assert/strict';
import {test} from 'vitest';
import {build} from 'esbuild';
import {authoredSourcesPlugin} from '../../lib/authored-source-resolution.mjs';
const result = await build({entryPoints:['src/launcher/i18n.mts'],bundle:true,platform:'node',format:'esm',write:false,plugins:[authoredSourcesPlugin(process.cwd())]});
const i18n = await import(`data:text/javascript;base64,${Buffer.from(result.outputFiles[0].text).toString('base64')}`);
test('Paraglide provides complete Chinese, English and Japanese catalogs', () => {
 assert.deepEqual(i18n.UI_LOCALES,['zh-CN','en','ja']);
 const keys=i18n.validateUiCatalogs();assert.ok(keys.length>=1273);
 for(const locale of i18n.UI_LOCALES)assert.deepEqual(Object.keys(i18n.UI_MESSAGES[locale]),keys);
});
test('explicit locale calls preserve interpolation and remain isolated', () => {
 assert.equal(i18n.translateUi('ja','nav.single'),'一人で遊ぶ');
 assert.equal(i18n.translateUi('ja','status.roomCreated',{code:'123456'}),'部屋 123456 を作成しました');
 assert.equal(i18n.translateUi('en','status.roomCreated',{code:'123456'}),'Room 123456 created');
 assert.equal(i18n.translateUi('zh-CN','status.roomCreated',{code:'123456'}),'已创建房间 123456');
 assert.equal(i18n.translateUi('ja','status.roomCreated'),'部屋 {code} を作成しました');
 assert.equal(i18n.translateUi('ja','status.roomCreated',{code:0}),'部屋 0 を作成しました');
 assert.equal(i18n.translateUi('ja','missing.fixture'),'missing.fixture');
});
test('Japanese language resolution and published home address are supported', () => {
 assert.equal(i18n.resolveUiLocale('ja-JP'),'ja');assert.equal(i18n.resolveUiLocale('zh-TW'),'zh-CN');assert.equal(i18n.resolveUiLocale('de-DE'),'en');
 assert.equal(i18n.localeHome('ja'),'ja.html');assert.equal(i18n.localeHome('en'),'en.html');assert.equal(i18n.localeHome('zh-CN'),'./');
 i18n.setUiLocale('ja',{persist:false,notify:false});assert.equal(i18n.t('nav.expandSidebar'),'サイドバーを展開');
 i18n.setUiLocale('zh-CN',{persist:false,notify:false});
});
