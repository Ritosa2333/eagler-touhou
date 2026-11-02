// Real browser CORS/Range, throughput, cancellation and resumable byte identity.
import assert from 'node:assert/strict';
import {createServer} from 'node:https';
import {readFile, mkdtemp, rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {createHash} from 'node:crypto';
import {build} from 'esbuild';
import puppeteer from 'puppeteer-core';
import {findChromiumExecutable} from '../../lib/chromium-executable.mjs';

const tls = {cert: await readFile(process.env.EAGLER_TEST_TLS_CERT), key: await readFile(process.env.EAGLER_TEST_TLS_KEY)};
const work = await mkdtemp(join(tmpdir(), 'resource-download-browser-'));
const payload = Buffer.from(Array.from({length: 2 * 1024 * 1024}, (_, i) => i % 251));
const sha = bytes => createHash('sha256').update(bytes).digest('hex');
const servers = [], origins = [], requests = [], modes = ['fast', 'fast', 'fast'], reports = [];
let browser;
try {
  const file = join(work, 'download.mjs');
  await build({entryPoints: [new URL('../../src/launcher/resource-download.mts', import.meta.url).pathname.replace(/^\/([A-Za-z]:)/, '$1')],
    outfile: file, bundle: true, format: 'esm', platform: 'browser', logLevel: 'silent'});
  const module = await readFile(file);
  for (let node = -1; node < 3; node++) {
    const server = createServer(tls, (request, response) => {
      const url = new URL(request.url, 'https://fixture.test');
      response.setHeader('Access-Control-Allow-Origin', '*');
      response.setHeader('Access-Control-Expose-Headers', 'Content-Length, Content-Range, Accept-Ranges');
      response.setHeader('Cache-Control', 'no-store');
      if (node === -1) {
        if (url.pathname === '/download.mjs') {response.setHeader('Content-Type', 'text/javascript'); response.end(module); return;}
        if (url.pathname === '/_eagler/resource-routes') {
          const path = new URL(url.searchParams.get('path'), origins[0]);
          const bytes = Number(path.searchParams.get('bytes')) || payload.length;
          response.setHeader('Content-Type', 'application/json');
          response.end(JSON.stringify({schema: 'eagler-touhou/resource-routes/1', path: path.pathname,
            bytes, sha256: sha(payload.subarray(0, bytes)), sources: origins.slice(1).map(origin => origin + path.pathname + path.search)}));
          return;
        }
        response.setHeader('Content-Type', 'text/html');
        response.end('<script type="module">import {createResourceDownloader} from "/download.mjs"; window.createDownload=createResourceDownloader; window.loaders=new Map();</script>');
        return;
      }
      const mode = modes[node], bytes = Number(url.searchParams.get('bytes')) || payload.length;
      const range = /^bytes=(\d+)-(\d*)$/.exec(request.headers.range || '');
      const from = range ? Number(range[1]) : 0, to = range?.[2] ? Number(range[2]) : bytes - 1;
      const record = {node, case: url.searchParams.get('case'), range: request.headers.range || null, sent: 0, cancelled: false};
      requests.push(record);
      let tick;
      response.on('close', () => {clearInterval(tick); record.cancelled = !response.writableFinished;});
      if (mode === 'headers') return;
      const ignored = mode === 'ignore-range', start = ignored ? 0 : from, end = ignored ? bytes - 1 : to;
      response.writeHead(range && !ignored ? 206 : 200, {'Content-Type': 'application/octet-stream',
        'Content-Length': end - start + 1, 'Accept-Ranges': 'bytes',
        ...(range && !ignored ? {'Content-Range': 'bytes ' + start + '-' + end + '/' + bytes} : {})});
      const send = chunk => {record.sent += chunk.length; response.write(chunk);};
      if (mode === 'stall') {send(payload.subarray(start, Math.min(start + 8192, end + 1))); return;}
      if (mode === 'slow' || mode === 'slower') {
        let offset = start;
        const size = mode === 'slow' ? 2048 : 1024;
        const step = () => {
          const next = Math.min(offset + size, end + 1); send(payload.subarray(offset, next)); offset = next;
          if (offset > end) {clearInterval(tick); response.end();}
        };
        tick = setInterval(step, 50); step(); return;
      }
      record.sent = end - start + 1; response.end(payload.subarray(start, end + 1));
    });
    await new Promise(resolve => server.listen(0, '127.0.0.1', resolve));
    servers.push(server); origins.push('https://127.0.0.1:' + server.address().port);
  }
  browser = await puppeteer.launch({headless: true, executablePath: await findChromiumExecutable(),
    args: ['--ignore-certificate-errors', '--no-proxy-server']});
  const page = await browser.newPage();
  await page.goto(origins[0]); await page.waitForFunction(() => !!window.createDownload);
  async function download(key, name, {bytes = payload.length, cancelAfter, options = {}} = {}) {
    const result = await page.evaluate(async ({key, name, bytes, cancelAfter, options}) => {
      let loader = window.loaders.get(key);
      if (!loader) {loader = window.createDownload({...options, fetchImpl: (...args) => fetch(...args)}); window.loaders.set(key, loader);}
      const url = new URL('/games/th09/th09.data', location.href);
      url.searchParams.set('case', name); url.searchParams.set('bytes', bytes);
      const controller = new AbortController(), began = performance.now();
      let firstByteMs = null, timer;
      if (cancelAfter) timer = setTimeout(() => controller.abort(), cancelAfter);
      try {
        const response = await loader(url, {signal: controller.signal}, loaded => {if (loaded && firstByteMs === null) firstByteMs = performance.now() - began;});
        const data = await response.arrayBuffer();
        const hash = [...new Uint8Array(await crypto.subtle.digest('SHA-256', data))].map(value => value.toString(16).padStart(2, '0')).join('');
        return {ms: performance.now() - began, firstByteMs, bytes: data.byteLength, hash, status: response.status};
      } catch (error) {return {ms: performance.now() - began, error: error.name};}
      finally {clearTimeout(timer);}
    }, {key, name, bytes, cancelAfter, options});
    if (!cancelAfter) {assert.equal(result.bytes, bytes); assert.equal(result.hash, sha(payload.subarray(0, bytes))); assert.equal(result.status, 200);}
    return result;
  }

  modes.splice(0, 3, 'headers', 'fast', 'fast');
  let result = await download('cold', 'cold');
  assert.ok(result.firstByteMs < 1500); assert.ok(result.ms < 2500);
  assert.ok(requests.some(r => r.case === 'cold' && r.node === 0 && r.cancelled));
  assert.equal(requests.filter(r => r.case === 'cold' && r.node === 1).reduce((n, r) => n + r.sent, 0), payload.length);
  reports.push({case: 'cold-with-hanging-primary', ...result});
  requests.length = 0;
  await download('cold', 'shared');
  assert.deepEqual([...new Set(requests.map(r => r.node))], [1]);

  modes.splice(0, 3, 'fast', 'headers', 'headers');
  result = await download('cold', 'warm-headers');
  assert.ok(result.firstByteMs < 1800, 'a formerly healthy node does not hold up first bytes until its idle deadline');
  reports.push({case: 'known-node-now-unresponsive', ...result});

  for (const mode of ['stall', 'ignore-range', 'slow']) {
    modes.splice(0, 3, 'fast', 'headers', 'headers');
    await download(mode, mode + '-warm', {bytes: 128 * 1024});
    modes.splice(0, 3, mode === 'ignore-range' ? 'stall' : mode, mode === 'ignore-range' ? 'ignore-range' : 'fast', 'headers');
    result = await download(mode, mode);
    assert.ok(result.ms < 6500);
    const resumed = requests.filter(r => r.case === mode && r.node === 1);
    assert.ok(resumed.some(r => /^bytes=[1-9]\d*-/.test(r.range)), 'resumes after downloaded prefix');
    if (mode === 'ignore-range') assert.equal(resumed.length, 1, 'reuse a full response that ignores Range');
    else {
      const sent = resumed.reduce((n, r) => n + r.sent, 0) + requests.find(r => r.case === mode && r.node === 0).sent;
      assert.ok(sent >= payload.length && sent <= payload.length + 64 * 1024, 'only bounded overlap while the original download continues');
    }
    reports.push({case: mode, ...result});
  }
  modes.splice(0, 3, 'fast', 'headers', 'headers');
  await download('all-slow', 'all-slow-warm', {bytes: 128 * 1024});
  modes.splice(0, 3, 'slow', 'slower', 'headers');
  result = await download('all-slow', 'all-slow', {bytes: 256 * 1024, options: {slowWindowMs: 500}});
  assert.ok(result.ms < 12000);
  assert.equal(requests.filter(r => r.case === 'all-slow' && r.node === 1).length, 1, 'one bounded speed comparison');
  assert.equal(requests.filter(r => r.case === 'all-slow' && r.node === 0).length, 1, 'no restart on a slower backup');
  reports.push({case: 'backup-slower-keeps-current-download', ...result});

  modes.splice(0, 3, 'headers', 'headers', 'headers');
  result = await download('cancel', 'cancel', {cancelAfter: 100});
  assert.equal(result.error, 'AbortError'); assert.ok(result.ms < 1000);
  await new Promise(resolve => setTimeout(resolve, 50));
  assert.ok(requests.filter(r => r.case === 'cancel').every(r => r.cancelled));
  reports.push({case: 'user-cancel', ...result});
  console.log(JSON.stringify({reports, requests}, null, 2));
} finally {
  if (browser) await browser.close();
  await Promise.all(servers.map(server => new Promise(resolve => {server.closeAllConnections(); server.close(resolve);})));
  await rm(work, {recursive: true, force: true});
}
