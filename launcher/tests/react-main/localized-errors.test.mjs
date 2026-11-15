import {test, beforeAll as before, afterAll as after, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {fileURLToPath, pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';

const project = fileURLToPath(new URL('../../', import.meta.url));
let env, work, api, React, createRoot, root;
before(async () => {
  env = installMountedDom(); React = await import('react'); ({createRoot} = await import('react-dom/client'));
  work = await mkdtemp(resolve(project, '.cache/localized-errors-'));
  const output = resolve(work, 'errors.mjs');
  await build({absWorkingDir: project, stdin: {resolveDir: project, loader: 'ts', contents: `
    export {localizeKnownError} from './src/launcher/localized-error-text.mts';
    export {errorText} from './app/services/error-text.ts';
    export {createStartupErrorModel} from './app/models/startup-error.ts';
    export {StartupError} from './app/components/feedback/StartupError.tsx';
    export {LocaleProvider, translate} from './app/i18n.tsx';
  `}, outfile: output, bundle: true, platform: 'node', format: 'esm', jsx: 'automatic', packages: 'external',
  logLevel: 'silent', plugins: [authoredSourcesPlugin(project)]});
  api = await import(pathToFileURL(output).href);
});
afterEach(async () => {if (root) await React.act(async () => root.unmount()); root = null; env.document.body.replaceChildren(); assert.deepEqual(env.errors.splice(0), []);});
after(async () => {env.close(); await rm(work, {recursive: true, force: true});});

test('known error templates translate literal punctuation and captured values with an explicit locale', () => {
  const english = api.translate.bind(null, 'en'), japanese = api.translate.bind(null, 'ja');
  assert.equal(api.localizeKnownError('已取消下载', english), english('error.downloadCancelled'));
  assert.equal(api.localizeKnownError('Site Manifest 没有可用游戏', japanese), japanese('error.manifestEmpty'));
  assert.equal(api.localizeKnownError('美化版资源目录不可用：HTTP 503', english), english('error.beautyCatalog', {status: '503'}));
  assert.equal(api.localizeKnownError('  已取消下载', english), '  已取消下载');
  assert.equal(api.localizeKnownError('TypeError: custom user detail {x}\n[a.b]', japanese), 'TypeError: custom user detail {x}\n[a.b]');
});

test('raw error text and structured cross-realm errors stay available independently from presentation', () => {
  const error = new Error('已取消下载'), japanese = api.translate.bind(null, 'ja');
  assert.equal(api.errorText(error), '已取消下载');
  assert.equal(api.errorText({message: '已取消下载'}), '已取消下载');
  assert.equal(api.errorText(error, japanese), japanese('error.downloadCancelled'));
  assert.equal(error.message, '已取消下载'); assert.match(error.stack, /已取消下载/);
});

test('error locale can change while open; diagnostic text and copied stack stay byte-identical', async () => {
  const copied = [], original = new Error('Site Manifest 没有可用游戏');
  const model = api.createStartupErrorModel({translate: api.translate.bind(null, 'en'), launched: () => false,
    copyText: async text => {copied.push(text); return true;}, toast() {}, now: () => new Date(0)});
  model.show(original, 'Original context');
  const detail = model.getSnapshot().detail;
  assert.ok(detail.endsWith(original.stack));
  const container = env.document.createElement('div'); env.document.body.append(container); root = createRoot(container);
  for (const locale of ['en', 'ja', 'zh-CN']) {
    await React.act(async () => root.render(React.createElement(api.LocaleProvider, {locale}, React.createElement(api.StartupError, {model}))));
    const summary = env.document.querySelector('.startup-error-reason');
    if (locale !== 'zh-CN') assert.equal(summary.textContent, api.translate(locale, 'error.manifestEmpty'));
    else assert.equal(summary, null, 'the original language does not repeat identical copy');
    assert.equal(env.document.querySelector('#startupErrorText').textContent, detail);
    await model.copy(); assert.equal(copied.at(-1), detail);
  }
  await React.act(async () => model.close()); assert.equal(model.getSnapshot().reason, ''); model.dispose();
});
