import {test, beforeAll as before, afterAll as after} from 'vitest';
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


test('user cancellation stops both in-flight candidates', async () => {
  const controller = new AbortController(), visited = [], cancelled = [];
  const download = createResourceDownloader({fetchImpl: async (input, init) => {
    const url = new URL(input); if (url.pathname === '/_eagler/resource-routes') return route();
    visited.push(url.origin); init.signal.addEventListener('abort', () => cancelled.push(url.origin), {once: true});
    queueMicrotask(() => controller.abort()); return abortable(init.signal);
  }});
  await assert.rejects(download(source, {signal: controller.signal}), {name: 'AbortError'});
  assert.deepEqual(visited, nodes); assert.deepEqual(cancelled.sort(), nodes);
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

const payload = new Uint8Array(Array.from({length: 512}, (_, i) => i % 251));
function partial(input, init, data = payload, body) {
  const range = /^bytes=(\d+)-(\d*)$/.exec(init.headers.get('Range') || '');
  const from = range ? Number(range[1]) : 0, to = range?.[2] ? Number(range[2]) : data.length - 1;
  const bytes = data.slice(from, to + 1);
  return new Response(body?.(bytes) ?? bytes, {status: range ? 206 : 200, headers: {
    'Content-Length': String(bytes.length), ...(range ? {'Content-Range': 'bytes ' + from + '-' + to + '/' + data.length} : {}),
  }});
}
function paced(bytes, size, interval, {stall = false, onCancel = () => {}} = {}) {
  let tick, offset = 0, closed = false;
  return new ReadableStream({
    start(controller) {
      const send = () => {
        if (closed) return;
        if (offset === bytes.length) {if (!stall) {closed = true; clearInterval(tick); controller.close();} return;}
        controller.enqueue(bytes.slice(offset, offset + size)); offset += size;
      };
      tick = setInterval(send, interval); send();
    },
    cancel() {closed = true; clearInterval(tick); onCancel();},
  });
}
const downloadOptions = {idleMs: 80, hedgeDelayMs: 10, probeBytes: 16, slowWindowMs: 30, minimumRate: 100 * 1024};

test('cold selection cancels an unresponsive node and reuses every byte of the winning Range', async () => {
  const requests = []; let cancelled = false;
  const fetchImpl = async (input, init) => {
    const url = new URL(input);
    if (url.pathname === '/_eagler/resource-routes') return route(payload.length);
    requests.push([url.origin, init.headers.get('Range')]);
    if (url.origin === nodes[0]) {
      init.signal.addEventListener('abort', () => {cancelled = true;}, {once: true});
      return new Promise(() => {}); // Deliberately ignores AbortSignal.
    }
    return partial(input, init);
  };
  const actual = await (await createResourceDownloader({...downloadOptions, fetchImpl})(source)).arrayBuffer();
  assert.deepEqual(new Uint8Array(actual), payload); assert.equal(cancelled, true);
  assert.deepEqual(requests, [[nodes[0], 'bytes=0-15'], [nodes[1], 'bytes=0-15'], [nodes[1], 'bytes=16-']]);
  requests.length = 0;
  // A new factory, as used for a later language package, shares the session policy.
  await createResourceDownloader({...downloadOptions, fetchImpl})(source);
  assert.deepEqual(requests, [[nodes[1], null]]);
});

test('a stalled warmed node resumes on the backup from the last received byte', async () => {
  const requests = [], progress = []; let stall = false, cancelled = false;
  const fetchImpl = async (input, init) => {
    const url = new URL(input); if (url.pathname === '/_eagler/resource-routes') return route(payload.length);
    requests.push([url.origin, init.headers.get('Range')]);
    if (url.origin === nodes[0] && stall) return partial(input, init, payload,
      bytes => paced(bytes.slice(0, 8), 8, 5, {stall: true, onCancel: () => {cancelled = true;}}));
    if (url.origin === nodes[1] && !stall) return new Promise(() => {});
    return partial(input, init);
  };
  const download = createResourceDownloader({...downloadOptions, slowWindowMs: 0, fetchImpl});
  await download(source); stall = true; requests.length = 0;
  const actual = await (await download(source, {}, loaded => progress.push(loaded))).arrayBuffer();
  assert.deepEqual(new Uint8Array(actual), payload); assert.equal(cancelled, true);
  assert.deepEqual(requests, [[nodes[0], null], [nodes[1], 'bytes=8-']]);
  assert.ok(progress.every((value, i) => i === 0 || value >= progress[i - 1]));
});

test('an ignored continuation Range restarts cleanly instead of appending a full file to its prefix', async () => {
  let warm = true; const requested = [];
  const fetchImpl = async (input, init) => {
    const url = new URL(input); if (url.pathname === '/_eagler/resource-routes') return route(payload.length);
    if (url.origin === nodes[1]) {
      if (warm) return new Promise(() => {});
      requested.push(init.headers.get('Range')); return new Response(payload);
    }
    return partial(input, init, payload, warm ? undefined : bytes => paced(bytes.slice(0, 8), 8, 5, {stall: true}));
  };
  const download = createResourceDownloader({...downloadOptions, slowWindowMs: 0, fetchImpl});
  await download(source); warm = false;
  const response = await download(source);
  assert.deepEqual(new Uint8Array(await response.arrayBuffer()), payload);
  assert.deepEqual(requested, ['bytes=8-']); assert.equal(response.status, 200);
  assert.equal(response.headers.get('Content-Length'), String(payload.length)); assert.equal(response.headers.has('Content-Range'), false);
});

test('rolling low speed checks detect a later slowdown and switch only after a faster backup probe', async () => {
  let warm = true; const requests = [];
  const fetchImpl = async (input, init) => {
    const url = new URL(input); if (url.pathname === '/_eagler/resource-routes') return route(payload.length);
    requests.push([url.origin, init.headers.get('Range')]);
    if (url.origin === nodes[1] && warm) return new Promise(() => {});
    return partial(input, init, payload, url.origin === nodes[0] && !warm ? bytes => {
      let slow = false, tick, offset = 0;
      return new ReadableStream({start(controller) {
        controller.enqueue(bytes.slice(0, 128)); offset = 128;
        tick = setInterval(() => {if (offset >= bytes.length) {clearInterval(tick); controller.close(); return;}
          const end = Math.min(offset + (slow ? 2 : 64), bytes.length); controller.enqueue(bytes.slice(offset, end)); offset = end; slow = true;
        }, 15);
      }, cancel() {clearInterval(tick);}});
    } : undefined);
  };
  const download = createResourceDownloader({...downloadOptions, minimumRate: 1_000, fetchImpl});
  await download(source); warm = false; requests.length = 0;
  assert.deepEqual(new Uint8Array(await (await download(source)).arrayBuffer()), payload);
  assert.ok(requests.filter(([node]) => node === nodes[1]).length >= 2, 'probe followed by continuation');
  const backupRanges = requests.filter(([node]) => node === nodes[1]).map(([, range]) => range);
  assert.match(backupRanges[0], /^bytes=\d+-\d+$/); assert.match(backupRanges.at(-1), /^bytes=\d+-$/);
});

test('a slower backup does not interrupt a progressing download below 100 KiB/s', async () => {
  let warm = true, primaryCancelled = 0, backupRequests = 0;
  const fetchImpl = async (input, init) => {
    const url = new URL(input); if (url.pathname === '/_eagler/resource-routes') return route(payload.length);
    if (url.origin === nodes[1]) {
      if (warm) return new Promise(() => {});
      backupRequests++;
      return partial(input, init, payload, bytes => paced(bytes, 2, 15));
    }
    return partial(input, init, payload, warm ? undefined : bytes => paced(bytes, 8, 5, {onCancel: () => {primaryCancelled++;}}));
  };
  const download = createResourceDownloader({...downloadOptions, idleMs: 200, fetchImpl});
  await download(source); warm = false;
  assert.deepEqual(new Uint8Array(await (await download(source)).arrayBuffer()), payload);
  assert.equal(backupRequests, 1); assert.equal(primaryCancelled, 0);
});

test('all unresponsive nodes and a stuck route body have bounded deadlines', async () => {
  const began = performance.now();
  await assert.rejects(createResourceDownloader({...downloadOptions, idleMs: 25, fetchImpl: async input => {
    if (new URL(input).pathname === '/_eagler/resource-routes') return route(payload.length);
    return new Promise(() => {});
  }})(source), {name: 'TimeoutError'});
  assert.ok(performance.now() - began < 500);
  const direct = [];
  const download = createResourceDownloader({...downloadOptions, routeTimeoutMs: 25, fetchImpl: async input => {
    if (new URL(input).pathname === '/_eagler/resource-routes') return new Response(new ReadableStream(), {headers: {'Content-Type': 'application/json'}});
    direct.push(String(input)); return new Response(payload);
  }});
  assert.deepEqual(new Uint8Array(await (await download(source)).arrayBuffer()), payload);
  assert.deepEqual(direct, [source.href]);
});

test('an explicit partial response is checked against its own range length', async () => {
  const download = createResourceDownloader({...downloadOptions, fetchImpl: async (input, init) =>
    new URL(input).pathname === '/_eagler/resource-routes' ? route(payload.length) : partial(input, init)});
  const response = await download(source, {headers: {Range: 'bytes=4-7'}});
  assert.equal(response.status, 206); assert.deepEqual(new Uint8Array(await response.arrayBuffer()), payload.slice(4, 8));
});

test('mismatched Content-Range is rejected before bytes enter the prefix', async () => {
  const download = createResourceDownloader({...downloadOptions, fetchImpl: async (input, init) => {
    const url = new URL(input); if (url.pathname === '/_eagler/resource-routes') return route(payload.length);
    if (url.origin === nodes[0]) return new Response(payload.slice(1, 17), {status: 206, headers: {'Content-Range': 'bytes 1-16/512'}});
    return partial(input, init);
  }});
  assert.deepEqual(new Uint8Array(await (await download(source)).arrayBuffer()), payload);
});

test('a truncated continuation preserves its valid prefix and resumes on another node', async () => {
  const requests = [];
  const download = createResourceDownloader({...downloadOptions, fetchImpl: async (input, init) => {
    const url = new URL(input); if (url.pathname === '/_eagler/resource-routes') return route(payload.length);
    const range = init.headers.get('Range'); requests.push([url.origin, range]);
    if (url.origin === nodes[0] && range === 'bytes=16-') return partial(input, init, payload,
      bytes => new ReadableStream({start(controller) {controller.enqueue(bytes.slice(0, 8)); controller.close();}}));
    if (url.origin === nodes[1] && range === 'bytes=0-15') return new Promise(() => {});
    return partial(input, init);
  }});
  assert.deepEqual(new Uint8Array(await (await download(source)).arrayBuffer()), payload);
  assert.ok(requests.some(([node, range]) => node === nodes[1] && range === 'bytes=24-'));
});

test('failed nodes cool down across files, then return as a last resort', async () => {
  let unavailable = nodes[0]; const visits = [];
  const fetchImpl = async (input, init) => {
    const url = new URL(input);
    if (url.pathname === '/_eagler/resource-routes') return route(payload.length, {path: new URL(url.searchParams.get('path'), source).pathname,
      sources: nodes.map(origin => origin + url.searchParams.get('path'))});
    visits.push(url.origin);
    return url.origin === unavailable ? new Response('', {status: 503}) : partial(input, init);
  };
  const create = () => createResourceDownloader({...downloadOptions, fetchImpl});
  await create()(source); visits.length = 0;
  await create()(new URL('/language/th07/zh.zip', source)); assert.deepEqual(visits, [nodes[1]]);
  visits.length = 0; unavailable = nodes[1];
  assert.deepEqual(new Uint8Array(await (await create()(source)).arrayBuffer()), payload);
  assert.deepEqual(visits, [nodes[1], nodes[0]]);
});

test('a cold slow sample starts progress within its sampling window', async () => {
  let firstByte = 0; const began = performance.now();
  const download = createResourceDownloader({...downloadOptions, probeBytes: 256, probeWindowMs: 15, slowWindowMs: 0,
    fetchImpl: async (input, init) => {
      const url = new URL(input); if (url.pathname === '/_eagler/resource-routes') return route(payload.length);
      if (url.origin === nodes[1]) return new Promise(() => {});
      return partial(input, init, payload, bytes => paced(bytes, 16, 5));
    }});
  assert.deepEqual(new Uint8Array(await (await download(source, {}, loaded => {if (loaded && !firstByte) firstByte = performance.now() - began;})).arrayBuffer()), payload);
  assert.ok(firstByte < 100);
});

test('an unresponsive speed probe cools down across later downloads', async () => {
  let warm = true, probes = 0;
  const fetchImpl = async (input, init) => {
    const url = new URL(input); if (url.pathname === '/_eagler/resource-routes') return route(payload.length);
    if (url.origin === nodes[1]) {if (!warm) probes++; return new Promise(() => {});}
    return partial(input, init, payload, warm ? undefined : bytes => paced(bytes, 16, 5));
  };
  const options = {...downloadOptions, probeWindowMs: 20, fetchImpl};
  await createResourceDownloader(options)(source); warm = false;
  for (let i = 0; i < 2; i++) assert.deepEqual(new Uint8Array(await (await createResourceDownloader(options)(source)).arrayBuffer()), payload);
  assert.equal(probes, 1);
});

test('two timed-out nodes progress to a third node with no more than two active requests', async () => {
  const third = 'https://c.example'; let concurrent = 0, maximum = 0;
  const download = createResourceDownloader({...downloadOptions, idleMs: 25, fetchImpl: async (input, init) => {
    const url = new URL(input);
    if (url.pathname === '/_eagler/resource-routes') return route(payload.length, {sources: [...nodes, third].map(origin => origin + source.pathname + source.search)});
    concurrent++; maximum = Math.max(maximum, concurrent);
    init.signal.addEventListener('abort', () => {concurrent--;}, {once: true});
    return url.origin === third ? partial(input, init) : new Promise(() => {});
  }});
  assert.deepEqual(new Uint8Array(await (await download(source)).arrayBuffer()), payload);
  assert.equal(maximum, 2); assert.equal(concurrent, 0);
});
