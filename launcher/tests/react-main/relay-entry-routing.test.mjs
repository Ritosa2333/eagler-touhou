import {test, afterAll} from 'vitest';
import assert from 'node:assert/strict';
import {build} from 'esbuild';
import {mkdtemp, rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {pathToFileURL, fileURLToPath} from 'node:url';
const work = await mkdtemp(join(tmpdir(), 'relay-entry-'));
await build({entryPoints: [fileURLToPath(new URL('../../src/launcher/relay-entry-routing.mts', import.meta.url))], bundle: true, platform: 'node', format: 'esm', outfile: join(work, 'routing.mjs')});
const {createRelayEntryRouting} = await import(pathToFileURL(join(work, 'routing.mjs')));
afterAll(() => rm(work, {recursive: true, force: true}));
const primary = 'wss://primary.example/netplay/';
const backup = 'wss://backup.example/netplay/';
class Socket extends EventTarget {
  readyState = 0;
  sent = [];
  constructor(url) {super(); this.url = url;}
  open() {this.readyState = 1; this.dispatchEvent(new Event('open'));}
  send(data) {this.sent.push(data);}
  close() {this.readyState = 3; this.dispatchEvent(new CloseEvent('close', {code: 1000}));}
  fail() {this.dispatchEvent(new Event('error'));}
}
function fixture(timeout = 1000) {
  const sockets = [];
  const router = createRelayEntryRouting({shared: () => ({netplayRelay: primary, netplayRelayFallbacks: [backup]}), connectTimeoutMs: timeout,
    createSocket: url => {const socket = new Socket(url); sockets.push(socket); return socket;}});
  return {router, sockets};
}
const flush = () => new Promise(resolve => queueMicrotask(resolve));
test('primary success makes no backup connection', async () => {
  const {router, sockets} = fixture();
  const routed = router.createSocket(primary + '?room=th09-1234&member=a%2Bb');
  await flush(); sockets[0].open();
  assert.equal(routed.readyState, 1); assert.equal(sockets.length, 1);
  assert.equal(router.resolve(primary), primary);
  routed.close();
});
test('failed connection retries backup preserving room and identity, subsequent connections reuse it', async () => {
  const {router, sockets} = fixture();
  const query = '?room=th09-1234&run=abc&member=a%2Bb&lobby=1';
  const routed = router.createSocket(primary + query);
  let errors = 0, opens = 0;
  routed.addEventListener('error', () => errors++); routed.addEventListener('open', () => opens++);
  await flush(); sockets[0].fail();
  assert.equal(sockets[1].url, backup + query);
  sockets[0].open(); assert.equal(opens, 0);
  sockets[1].open(); assert.equal(opens, 1); assert.equal(errors, 0);
  assert.equal(router.resolve(primary), backup);
  routed.send('take-seat'); assert.deepEqual(sockets[1].sent, ['take-seat']);
  const second = router.createSocket(primary + '?directory=1');
  await flush(); assert.equal(sockets[2].url, backup + '?directory=1');
  routed.close(); second.close();
});
test('blackholed primary is bounded and late events do not leak', async () => {
  const {router, sockets} = fixture(10);
  const routed = router.createSocket(primary + '?diagnostic=1');
  await new Promise(resolve => setTimeout(resolve, 15));
  assert.equal(sockets.length, 2); sockets[1].open(); assert.equal(routed.readyState, 1);
  routed.close();
});
test('explicit close cancels pending fallback', async () => {
  const {router, sockets} = fixture(10);
  const routed = router.createSocket(primary); await flush(); routed.close();
  await new Promise(resolve => setTimeout(resolve, 15));
  assert.equal(sockets.length, 1); assert.equal(routed.readyState, 3);
});
test('established connection is not replayed or silently replaced', async () => {
  const {router, sockets} = fixture();
  const routed = router.createSocket(primary); await flush(); sockets[0].open();
  let code = 0; routed.addEventListener('close', event => code = event.code);
  sockets[0].close(); assert.equal(code, 1000); assert.equal(sockets.length, 1);
});
test('all failed entries report one terminal failure', async () => {
  const {router, sockets} = fixture();
  const routed = router.createSocket(primary); let errors = 0, closes = 0;
  routed.addEventListener('error', () => errors++); routed.addEventListener('close', () => closes++);
  await flush(); sockets[0].fail(); sockets[1].fail();
  assert.equal(errors, 1); assert.equal(closes, 1); assert.equal(routed.readyState, 3);
});
test('application rejection does not try another entry', async () => {
  const {router, sockets} = fixture();
  const routed = router.createSocket(primary); await flush(); sockets[0].dispatchEvent(new CloseEvent('close', {code: 1008}));
  assert.equal(sockets.length, 1); assert.equal(routed.readyState, 3);
});
test('messages from abandoned sockets are ignored and current messages preserve payload', async () => {
  const {router, sockets} = fixture();
  const routed = router.createSocket(primary); const messages = [];
  routed.addEventListener('message', event => messages.push(event.data));
  await flush(); sockets[0].fail(); sockets[1].open();
  sockets[0].dispatchEvent(new MessageEvent('message', {data: 'old'}));
  sockets[1].dispatchEvent(new MessageEvent('message', {data: 'state'}));
  assert.deepEqual(messages, ['state']); routed.close();
});
test('cancellation before the first microtask never opens a connection', async () => {
  const {router, sockets} = fixture();
  const routed = router.createSocket(primary); routed.close(); await flush();
  assert.equal(sockets.length, 0); assert.equal(routed.readyState, 3);
});
test('a removed backup is no longer returned by route resolution', async () => {
  const shared = {netplayRelay: primary, netplayRelayFallbacks: [backup]}; const sockets = [];
  const router = createRelayEntryRouting({shared: () => shared, createSocket: url => {const socket = new Socket(url); sockets.push(socket); return socket;}});
  const routed = router.createSocket(primary); await flush(); sockets[0].fail(); sockets[1].open();
  assert.equal(router.resolve(primary), backup);
  shared.netplayRelayFallbacks = []; assert.equal(router.resolve(primary), primary); routed.close();
});
test('a synchronous constructor error can fall back without escaping the caller', async () => {
  const sockets = [];
  const router = createRelayEntryRouting({shared: () => ({netplayRelay: primary, netplayRelayFallbacks: [backup]}), createSocket: url => {
    if (url.startsWith(primary)) throw new Error('connect failed');
    const socket = new Socket(url); sockets.push(socket); return socket;
  }});
  const routed = router.createSocket(primary); await flush(); sockets[0].open();
  assert.equal(routed.readyState, 1); assert.equal(router.resolve(primary), backup); routed.close();
});
