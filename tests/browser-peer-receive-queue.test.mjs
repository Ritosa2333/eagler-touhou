import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';

// Run the shipped EM_JS bodies. Native transport tests do not execute queues.
const source = readFileSync(new URL('../src/netplay/BrowserPeerTransport.cpp', import.meta.url), 'utf8');
function embedded(name, args) {
  const start = source.indexOf(name);
  const body = source.indexOf('{', start) + 1;
  const end = source.indexOf('\n});', body);
  assert(start >= 0 && end > body);
  return new Function(...args, source.slice(body, end));
}
const connect = embedded('EM_JS(int, eagler_peer_connect,', ['relayUrlPtr', 'tagPtr', 'localPlayer', 'playerCount']);
const copy = embedded('EM_JS(int, eagler_peer_poll_copy,', ['out', 'capacity']);
globalThis.setInterval = () => 1;
globalThis.clearInterval = () => {};
globalThis.location = { href: 'https://example.test/' };
globalThis.Module = { eaglerOptions: {} };
globalThis.HEAPU8 = new Uint8Array(512);
HEAPU8.set(new TextEncoder().encode('wss://example.test/room'), 0);
HEAPU8.set(new TextEncoder().encode('th07'), 100);
class Socket {
  static OPEN = 1;
  static CONNECTING = 0;
  static CLOSING = 2;
  readyState = 1;
  send() {}
  close() { this.readyState = 3; }
}
globalThis.WebSocket = Socket;
const fresh = () => {
  assert.equal(connect(0, 100, 0, 2), 1);
  return globalThis.__eaglerPeerTransport;
};
const byte = new Uint8Array([7]);
let state = fresh();
assert.equal(state.maxReceivedPackets, 16384);
assert.equal(state.maxReceivedBytes, 16 * 1024 * 1024);
for (let i = 0; i < state.maxReceivedPackets; ++i) state.receiveBinary(byte);
assert.equal(state.receivedBytes, 16384);
state.receiveBinary(byte);
assert(state.failed && state.closed);
assert.match(state.error, /queue limit/);
assert.equal(state.receivedBytes, 0);
assert.equal(state.received.length, 0);
assert.equal(state.relay.readyState, 3);
state.receiveBinary(byte);
assert.equal(state.received.length, 0, 'closed queues cannot grow again');

state = fresh();
state.maxReceivedBytes = 8;
state.receiveBinary(new Uint8Array(8));
assert.equal(state.receivedBytes, 8);
let copied = false;
byte.buffer.slice = () => { copied = true; throw new Error('must reject before copying'); };
state.receiveBinary(byte);
assert.equal(copied, false);
delete byte.buffer.slice;
assert(state.failed);

state = fresh();
state.maxReceivedPackets = 300;
for (let i = 0; i < 300; ++i) state.receiveBinary(byte.buffer);
for (let i = 0; i < 256; ++i) assert.equal(copy(200, 1), 1);
assert.equal(state.receivedBytes, 44);
assert.equal(state.receivedHead, 0, 'compaction does not lose byte accounting');
for (let i = 0; i < 256; ++i) state.receiveBinary(byte);
assert.equal(state.receivedBytes, 300);
assert(!state.failed, 'consumption releases both byte and packet capacity');

class DeferredBlob extends Blob {
  constructor(bytes) {
    super([new Uint8Array(bytes)]);
    this.conversion = new Promise((resolve, reject) => { this.finish = resolve; this.fail = reject; });
  }
  arrayBuffer() { return this.conversion; }
}
state = fresh();
state.maxReceivedBytes = 8;
const blob = new DeferredBlob(8);
state.receiveBinary(blob);
assert.equal(state.pendingReceivedBytes, 8);
assert.equal(state.pendingReceivedPackets, 1);
state.receiveBinary(byte);
assert(state.failed && state.closed, 'in-flight conversions reserve capacity');
blob.finish(new ArrayBuffer(8));
await Promise.resolve();
assert.equal(state.received.length, 0, 'late conversion cannot revive a failed queue');
assert.equal(state.pendingReceivedBytes, 0);

state = fresh();
const rejected = new DeferredBlob(8);
state.receiveBinary(rejected);
rejected.fail(new Error('decode failure'));
await Promise.resolve();
assert.equal(state.pendingReceivedBytes, 0);
assert.equal(state.pendingReceivedPackets, 0);
const accepted = new DeferredBlob(8);
state.receiveBinary(accepted);
accepted.finish(new ArrayBuffer(8));
await Promise.resolve();
assert.equal(state.receivedBytes, 8);
assert.equal(state.pendingReceivedBytes, 0);
assert.equal(copy(200, 8), 8);
assert.equal(state.receivedBytes, 0);
state.close();
console.log('browser player receive queue bounds and lifecycle: PASS');
