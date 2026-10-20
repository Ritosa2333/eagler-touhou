import test, {before, after} from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp, rm} from 'node:fs/promises';
import {resolve} from 'node:path';
import {pathToFileURL} from 'node:url';
import {build} from 'esbuild';
let directory, createResourceDownloader;
before(async () => {
  directory = await mkdtemp(resolve(import.meta.dirname, '../../.cache/resource-download-test-'));
  const file = resolve(directory, 'download.mjs');
  await build({entryPoints: [resolve(import.meta.dirname, '../../src/launcher/resource-download.mts')], outfile: file,
    bundle: true, platform: 'node', format: 'esm', logLevel: 'silent'});
  ({createResourceDownloader} = await import(pathToFileURL(file)));
});
after(async () => {await rm(directory, {recursive: true, force: true});});
const source = new URL('https://play.example/games/th07/th07.data?v=abc');
const nodes = ['https://a.example', 'https://b.example'];
const route = (bytes = 4, overrides = {}) => Response.json({schema: 'eagler-touhou/resource-routes/1',
  path: source.pathname, bytes, sha256: 'a'.repeat(64), sources: nodes.map(origin => origin + source.pathname + source.search), ...overrides});
const abortable = (signal, stream) => {
  let owner; const body = new ReadableStream({start(controller) {owner = controller; stream?.(controller);}});
  signal.addEventListener('abort', () => owner.error(signal.reason), {once: true});
  return new Response(body);
};

test('failed node switches automatically, preserves version and Range, then prefers the working node', async () => {
  const requests = [], progress = [];
  const download = createResourceDownloader({fetchImpl: async (input, init) => {
    const url = new URL(input); requests.push([url.href, init]);
    if (url.pathname === '/_eagler/resource-routes') return route();
    if (url.origin === nodes[0]) return new Response('unavailable', {status: 503});
    return new Response('game');
  }});
  for (let i = 0; i < 2; i++) assert.equal(await (await download(source, {headers: {Range: 'bytes=0-3', Authorization: 'local-only'}},
    (loaded, total, url) => progress.push({loaded, total, url}))).text(), 'game');
  assert.deepEqual(requests.filter(([url]) => !url.includes('/_eagler/resource-routes')).map(([url]) => new URL(url).origin), [nodes[0], nodes[1], nodes[1]]);
  for (const [url, init] of requests.filter(([url]) => !url.includes('/_eagler/resource-routes'))) {
    assert.equal(new URL(url).search, '?v=abc'); assert.equal(init.headers.get('Range'), 'bytes=0-3');
    assert.equal(init.headers.has('Authorization'), false); assert.equal(init.credentials, 'omit');
  }
  assert.equal(progress.at(-1).loaded, 4);
});

for (const failure of ['headers', 'body', 'truncated', 'slow']) test(`${failure} failure selects the next node`, async () => {
  const visited = []; let cancelled = false, tick;
  const download = createResourceDownloader({idleMs: 25, slowWindowMs: 15, minimumRate: 100_000,
    fetchImpl: async (input, init) => {
      const url = new URL(input); if (url.pathname === '/_eagler/resource-routes') return route();
      visited.push(url.origin);
      if (url.origin === nodes[1]) return new Response('game');
      init.signal.addEventListener('abort', () => {cancelled = true; clearInterval(tick);}, {once: true});
      if (failure === 'headers') return new Promise((_, reject) => init.signal.addEventListener('abort', () => reject(init.signal.reason), {once: true}));
      if (failure === 'truncated') return new Response('bad');
      return abortable(init.signal, controller => {
        controller.enqueue(new Uint8Array([1]));
        if (failure === 'slow') tick = setInterval(() => controller.enqueue(new Uint8Array([1])), 3);
      });
    }});
  try {
    assert.equal(await (await download(source)).text(), 'game'); assert.deepEqual(visited, nodes);
    if (failure !== 'truncated') assert.equal(cancelled, true);
  } finally {clearInterval(tick);}
});

test('user cancellation during a stalled body cancels the attempt without contacting the next node', async () => {
  const controller = new AbortController(), visited = [];
  const download = createResourceDownloader({fetchImpl: async (input, init) => {
    const url = new URL(input); if (url.pathname === '/_eagler/resource-routes') return route();
    visited.push(url.origin); queueMicrotask(() => controller.abort()); return abortable(init.signal);
  }});
  await assert.rejects(download(source, {signal: controller.signal}), {name: 'AbortError'});
  assert.deepEqual(visited, [nodes[0]]);
});

test('ordinary static hosting probes once and preserves a single source', async () => {
  let probes = 0, downloads = 0;
  const download = createResourceDownloader({fetchImpl: async input => {
    if (new URL(input).pathname === '/_eagler/resource-routes') {probes++; return new Response('game');}
    downloads++; return new Response('game');
  }});
  for (let i = 0; i < 2; i++) assert.equal(await (await download(source)).text(), 'game');
  assert.equal(probes, 1); assert.equal(downloads, 2);
});

for (const bad of [{path: '/different'}, {sources: ['https://a.example/different']},
  {sources: ['https://a.example' + source.pathname]}, {sources: ['https://user:secret@a.example' + source.pathname + source.search]}]) {
  test(`invalid route locations remain on the declared source: ${JSON.stringify(bad)}`, async () => {
    const visited = [];
    const download = createResourceDownloader({fetchImpl: async input => {
      const url = new URL(input); if (url.pathname === '/_eagler/resource-routes') return route(4, bad);
      visited.push(url.href); return new Response('game');
    }});
    assert.equal(await (await download(source)).text(), 'game'); assert.deepEqual(visited, [source.href]);
  });
}
