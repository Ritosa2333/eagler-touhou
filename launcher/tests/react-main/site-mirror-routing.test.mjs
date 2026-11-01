// Use Undici's reference entry directly: Bun aliases the bare undici import
// to its server Request, whose credentials getter does not model the browser.
import {Request as WhatwgRequest} from 'undici/index.js';
import {test} from 'vitest';
import assert from 'node:assert/strict';
import vm from 'node:vm';
import {readFile, mkdtemp, writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {buildAppShell} from '../../lib/app-shell-build.mjs';
import {resolveRuntimeGenerationWorkerSource} from '../../lib/contracts-build.mjs';

const source = await readFile(new URL('../../src/site-mirror-sw.js', import.meta.url), 'utf8');
const scopeUrl = new URL('https://primary.test/nested/');
const config = {schema:'eagler-touhou/site-network/1', mirrors:['https://second.test/site/', 'https://third.test:8443/']};
function harness(fetchImpl, options = {}) {
  const requests = [], logs = [];
  const context = vm.createContext({URL, Request: WhatwgRequest, Response, Headers, AbortController, setTimeout, clearTimeout,
    console:{info:(...args)=>logs.push(args),warn:(...args)=>logs.push(args)},
    fetch: request => {requests.push(request); return fetchImpl(request);}});
  vm.runInContext(source, context);
  return {requests, logs, validate: context.validateSiteNetwork,
    router:context.createSiteMirrorRouting({scopeUrl, loadConfig:async () => config, timeoutMs:15, ...options})};
}
const read = response => response.text();

test('failed primary uses ordered mirrors and remembers failure across requests', async () => {
  const h = harness(request => {
    if (request.url.startsWith(scopeUrl.origin)) throw new Error('connection reset');
    return new Response('second');
  });
  assert.equal(await h.router.run(new URL('a.js?v=1', scopeUrl), read), 'second');
  assert.deepEqual(h.requests.map(r => r.url), ['https://primary.test/nested/a.js?v=1','https://second.test/site/a.js?v=1']);
  assert.equal(h.requests[1].credentials, 'omit');
  assert.equal(h.requests[1].redirect, 'error');
  await h.router.run(new URL('b.js', scopeUrl), read);
  assert.equal(h.requests[2].url, 'https://second.test/site/b.js');
  assert.equal(h.logs[0][0],'[EAGLER|入口回退]');
  assert.equal(h.logs[0][1].node,'https://primary.test');
  assert.equal(h.logs[1][0],'[EAGLER|入口]');
  assert.equal(h.logs[1][1].node,'https://second.test');
  assert.equal(h.logs[1][1].path,'/nested/a.js');
  assert.ok(!JSON.stringify(h.logs).includes('?v='),'diagnostics omit query strings');
  assert.equal(h.logs.length,2,'unchanged successful entry does not log every file');
});

test('connection stalls, body stalls, 5xx and failed integrity advance to the next mirror', async () => {
  for (const mode of ['connect', 'body', 'server', 'integrity']) {
    const h = harness(request => {
      if (request.url.startsWith('https://third.test')) return new Response('verified');
      if (mode === 'connect') return new Promise((_, reject) => request.signal.addEventListener('abort', () => reject(request.signal.reason)));
      if (mode === 'body') return new Response(new ReadableStream({start(controller) {
        request.signal.addEventListener('abort', () => controller.error(request.signal.reason));
      }}));
      return new Response(mode === 'integrity' ? 'corrupt' : 'unavailable', {status: mode === 'server' ? 503 : 200});
    });
    const value = await h.router.run(new URL('app.js', scopeUrl), async response => {
      const bytes = await response.text();
      if (bytes !== 'verified') throw new Error('integrity mismatch');
      return bytes;
    });
    assert.equal(value, 'verified', mode);
    assert.equal(h.requests.length, 3, mode);
  }
});

test('access restrictions are terminal and untrusted paths/configuration are rejected', async () => {
  const h = harness(() => new Response('denied', {status:403}));
  await assert.rejects(h.router.run(new URL('a.js', scopeUrl), read), /access denied/);
  assert.equal(h.requests.length, 1);
  await assert.rejects(h.router.run('https://primary.test/private/a.js', read), /outside application scope/);
  for (const url of ['http://second.test/', 'https://u:p@second.test/', 'https://second.test/?x=1', 'https://second.test/no-slash']) {
    assert.throws(() => h.validate({...config, mirrors:[url]}), /Invalid/);
  }
  assert.throws(() => h.validate({...config, mirrors:[...config.mirrors,...config.mirrors]}), /Invalid/);
});

test('nested shells discover resources at the mirror origin root', async () => {
  const h = harness(request => {
    if (request.url.startsWith(scopeUrl.origin)) throw new Error('primary unavailable');
    return new Response('routes');
  });
  assert.equal(await h.router.run('https://primary.test/_eagler/resource-routes?path=%2Fgames%2Fth09%2Fth09.data',read),'routes');
  assert.equal(h.requests[1].url,'https://second.test/_eagler/resource-routes?path=%2Fgames%2Fth09%2Fth09.data');
});

test('build pins deployment configuration, metadata and descriptors to the shell release', async t => {
  const directory = await mkdtemp(join(tmpdir(), 'site-mirror-build-'));
  t.onTestFinished(async () => {const {rm} = await import('node:fs/promises'); await rm(directory, {recursive:true,force:true});});
  for (const [path, value] of Object.entries({
    'index.html':'<h1>launcher</h1>', 'site-network.json':JSON.stringify(config),
    'site-manifest.json':'{}', 'host-manifest.json':'{}',
    'release-catalog.json':JSON.stringify({games:{th09:{descriptor:'th09.package.json'}}}), 'th09.package.json':'{}',
  })) await writeFile(join(directory, path), value);
  const workerContractSource = await readFile(await resolveRuntimeGenerationWorkerSource(), 'utf8');
  const options = {globDirectory:directory, shellFiles:['index.html'], quiet:true, workerContractSource};
  const result = await buildAppShell(options);
  for (const path of ['site-network.json','site-manifest.json','host-manifest.json','th09.package.json']) {
    assert.ok(result.contract.entries.includes(path), path);
  }
  assert.ok(result.worker.toString().includes('createSiteMirrorRouting'));
  const first = result.buildId;
  await writeFile(join(directory,'site-network.json'), JSON.stringify({...config,mirrors:['https://new.test/']}));
  assert.notEqual((await buildAppShell(options)).buildId, first);
  const {unlink} = await import('node:fs/promises');
  await unlink(join(directory,'th09.package.json'));
  await assert.rejects(buildAppShell(options), /Missing mirrored Package Descriptor/);
});
