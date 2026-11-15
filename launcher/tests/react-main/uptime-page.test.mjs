import {test, vi, beforeAll, afterAll, afterEach} from 'vitest';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {build} from 'esbuild';
import {installMountedDom} from './mounted-dom-environment.mjs';
import {authoredSourcesPlugin} from './authored-sources.mjs';

let env, React, createRoot, owners, work, root, previousFetch;
const summary = (range = '24h') => ({schema: 'eagler-uptime/1', range, updatedAt: new Date().toISOString(), services: [{
  id: 'main', label: 'EAGLER TOUHOU', group: 'site', status: 'operational', checkedAt: new Date().toISOString(),
  latencyMs: 42, availability: 1, points: [{at: new Date().toISOString(), samples: 1, availability: 1, latencyMs: 42}],
}]});
const act = async fn => React.act(async () => {await fn?.(); for (let i = 0; i < 12; i++) await Promise.resolve();});
const element = selector => {const node = env.document.querySelector(selector); assert.ok(node, selector); return node;};
const respond = value => ({ok: true, json: async () => value});
beforeAll(async () => {
  env = installMountedDom(); React = await import('react'); ({createRoot} = await import('react-dom/client'));
  previousFetch = globalThis.fetch;
  work = await mkdtemp(resolve('.cache/uptime-page-'));
  await build({absWorkingDir: process.cwd(), stdin: {resolveDir: process.cwd(), loader: 'ts', contents:
    "export * from './app/components/uptime/UptimePage.tsx'; export * from './app/i18n.tsx';"},
    bundle: true, platform: 'node', format: 'esm', jsx: 'automatic', outfile: resolve(work, 'page.mjs'),
    packages: 'external', plugins: [authoredSourcesPlugin(process.cwd())], logLevel: 'silent'});
  owners = await import(pathToFileURL(resolve(work, 'page.mjs')));
});
afterEach(async () => {
  if (root) await act(() => root.unmount()); root = null;
  globalThis.fetch = previousFetch; vi.useRealTimers(); env.document.body.replaceChildren();
  assert.deepEqual(env.errors.splice(0), []);
});
afterAll(async () => {env.close(); await rm(work, {recursive: true, force: true});});
async function mount(locale = 'zh-CN') {
  const container = env.document.createElement('div'); env.document.body.append(container); root = createRoot(container);
  await act(() => root.render(React.createElement(owners.LocaleProvider, {locale},
    React.createElement(owners.UptimePage, {endpoint: 'https://status.test/uptime/summary'}))));
}

test('public status validates numeric, time and identity boundaries before rendering', () => {
  assert.equal(owners.parseUptimeSummary(summary(), '24h').services[0].latencyMs, 42);
  const invalid = [null, {...summary(), range: '7d'}, {...summary(), updatedAt: 'invalid'}];
  for (const change of [{label: {}}, {availability: NaN}, {latencyMs: -1}, {checkedAt: 'invalid'}, {points: [null]}]) {
    const value = summary(); Object.assign(value.services[0], change); invalid.push(value);
  }
  const duplicate = summary(); duplicate.services.push(duplicate.services[0]); invalid.push(duplicate);
  for (const value of invalid) assert.throws(() => owners.parseUptimeSummary(value, '24h'), /Invalid uptime/);
});

test('range changes retain the list, abort the old request and ignore late completion', async () => {
  const pending = [];
  globalThis.fetch = (url, options) => new Promise(resolve => pending.push({url, options, resolve}));
  await mount('ja');
  await act(() => pending[0].resolve(respond(summary())));
  const row = element('.uptime-row'); assert.equal(row.dataset.status, 'operational');
  const button = range => [...env.document.querySelectorAll('button')].find(node => node.textContent === owners.translate('ja', range));
  await act(() => button('uptime.range7d').click());
  assert.equal(element('.uptime-row'), row); assert.equal(element('.uptime-list').getAttribute('aria-busy'), 'true');
  await act(() => button('uptime.range90d').click());
  assert.equal(pending[1].options.signal.aborted, true);
  const current = summary('90d'); current.services[0].latencyMs = 90;
  await act(() => pending[2].resolve(respond(current)));
  await act(() => pending[1].resolve(respond(summary('7d'))));
  assert.equal(element('.uptime-row'), row);
  assert.equal(element('.uptime-latency').textContent, '90 ms');
  assert.equal(element('.uptime-list').getAttribute('aria-busy'), 'false');
  assert.equal(element('h1').textContent, owners.translate('ja', 'nav.serviceStatus'));
});

test('invalid refresh keeps historical data and exposes the failure as unknown', async () => {
  let response = summary();
  globalThis.fetch = async () => respond(response);
  await mount('en');
  const row = element('.uptime-row'); response = {schema: 'wrong'};
  await act(() => env.document.dispatchEvent(new env.window.Event('visibilitychange')));
  assert.equal(element('.uptime-row'), row); assert.equal(row.dataset.status, 'unknown');
  assert.equal(element('[role=alert]').textContent, owners.translate('en', 'uptime.failed'));
  assert.equal(element('.uptime-history').children.length, 1);
});

test('an initial failure shows one error without empty table headings and recovers on refresh', async () => {
  let response={ok:false};globalThis.fetch=async()=>response;
  await mount('en');
  assert.equal(element('[role=alert]').textContent,owners.translate('en','uptime.failed'));
  assert.equal(element('.uptime-list').dataset.emptyFailure,'true');
  assert.equal(env.document.querySelector('.uptime-columns'),null);
  response=respond(summary());await act(()=>env.document.dispatchEvent(new env.window.Event('visibilitychange')));
  assert.equal(element('.uptime-list').hasAttribute('data-empty-failure'),false);
  assert.ok(element('.uptime-columns'));assert.equal(element('.uptime-latency').textContent,'42 ms');
});

test('hidden pages pause polling and unmount cancels requests and timers', async () => {
  vi.useFakeTimers();
  const calls = [];
  globalThis.fetch = (url, options) => {calls.push(options); return calls.length === 1 ? Promise.resolve(respond(summary())) : new Promise(() => {});};
  await mount();
  assert.equal(calls.length, 1);
  Object.defineProperty(env.document, 'hidden', {configurable: true, value: true});
  await act(() => vi.advanceTimersByTime(30_000)); assert.equal(calls.length, 1);
  Object.defineProperty(env.document, 'hidden', {configurable: true, value: false});
  await act(() => env.document.dispatchEvent(new env.window.Event('visibilitychange')));
  assert.equal(calls.length, 2);
  await act(() => root.unmount()); root = null;
  assert.equal(calls[1].signal.aborted, true); assert.equal(vi.getTimerCount(), 0);
  delete env.document.hidden;
});
