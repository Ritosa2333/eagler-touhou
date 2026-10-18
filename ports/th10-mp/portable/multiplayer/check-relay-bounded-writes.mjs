// Run from the TH10 tree with the same Node executable used by the relay:
//   node portable/multiplayer/check-relay-bounded-writes.mjs
// This imports the locked ws 8.21.3 and original relay sender. It creates only
// loopback fixtures; it never edits or launches the frozen relay. Component
// cases use real Writable/Sender; integration cases use actual TCP Socket,
// actual ws Receiver/Sender, and complete byte/frame-order checks.
// Synthetic emit(data) supplies an exact synchronous raw boundary to the real
// Receiver. The destination is a real connected TCP socket/client. These are
// semantics and writev-coverage gates, not browser/game smoothness measurements.
import assert from 'node:assert/strict';
import { Writable } from 'node:stream';
import { performance } from 'node:perf_hooks';
import { Socket } from 'node:net';
import { TLSSocket } from 'node:tls';
import { once, EventEmitter } from 'node:events';
import { createRequire } from 'node:module';
import { fileURLToPath } from 'node:url';
import { createRelayBoundedWrites, BOUNDED_WRITES_SCHEMA, BOUNDED_WRITES_LIMITS } from './relay-bounded-writes.mjs';

const locked = new URL('../../artifacts/rollback-smoothness/relay-r6-locked/', import.meta.url);
const requireLocked = createRequire(new URL('server/netplay-relay.mjs', locked));
const requireHere = createRequire(import.meta.url);
const WebSocket = requireLocked('ws'), { WebSocketServer } = WebSocket;
const wsVersion = requireLocked('ws/package.json').version;
const Sender = requireLocked(fileURLToPath(new URL('node_modules/ws/lib/sender.js', locked)));
const { createBoundedRelaySender } = await import(new URL('server/relay-flow-control.mjs', locked).href);
let checks = 0, groups = 0;
const equal = (actual, expected, message) => { ++checks; assert.deepEqual(actual, expected, message); };
const same = (actual, expected, message) => { ++checks; assert.strictEqual(actual, expected, message); };
const yes = (value, message) => { ++checks; assert.ok(value, message); };
const throwsSame = (fn, error) => { ++checks; assert.throws(fn, value => value === error); };
const test = async (name, run) => { await run(); ++groups; console.log('PASS ' + name); };
const configuration = extra => ({ WebSocket, wsVersion, ...extra });

function payload(index, size = 48) {
  const buffer = Buffer.alloc(size, index & 255);
  if (size >= 4) buffer.writeUInt32LE(index >>> 0);
  return buffer;
}
function frame(data, { mask = true, opcode = 2 } = {}) {
  return Buffer.concat(Sender.frame(Buffer.from(data), {
    fin: true, rsv1: false, opcode, mask, readOnly: false,
    maskBuffer: Buffer.alloc(4), generateMask(bytes) { bytes.fill(0x6d); }
  }));
}
const binary = Object.freeze({ binary: true, delayMs: 0 });
function delayTurn() { return new Promise(resolve => setImmediate(resolve)); }
async function idle(raw) {
  for (let i = 0; i < 100 && raw.writableLength; ++i) await delayTurn();
  same(raw.writableLength, 0, 'fixture begins after its handshake write completes');
}
function bytesEqual(actual, expected) {
  equal(actual.length, expected.length, 'all frames remain present');
  for (let i = 0; i < expected.length; ++i) {
    same(actual[i].binary, true, 'binary kind stays binary');
    equal(actual[i].data, expected[i], 'complete frame bytes and ordering');
  }
}
async function fixture(extra = {}) {
  const server = new WebSocketServer({ host: '127.0.0.1', port: 0,
    perMessageDeflate: extra.compression === true });
  const serverErrors = [], clients = [], sockets = [], received = [];
  server.on('error', error => serverErrors.push(error));
  await once(server, 'listening');
  const controller = createRelayBoundedWrites(configuration({ enabled: true,
    observeWrites: true, observeTiming: true, ...extra }));
  const originals = [];
  async function connect(install = true) {
    const incoming = once(server, 'connection');
    const client = new WebSocket('ws://127.0.0.1:' + server.address().port +
      '/?room=perf-th10-bounded_fixture&run=1&player=' + clients.length + '&players=2',
      { perMessageDeflate: extra.compression === true });
    const errors = [], messages = [];
    client.on('error', error => errors.push(error));
    client.on('message', (data, isBinary) => messages.push({ data: Buffer.from(data), binary: isBinary }));
    await once(client, 'open');
    const [socket] = await incoming;
    socket.on('error', error => errors.push(error));
    await idle(socket._socket);
    originals.push({ socket, raw: socket._socket,
      emit: socket._socket.emit, cork: socket._socket.cork, uncork: socket._socket.uncork,
      write: socket._socket._write, writev: socket._socket._writev });
    clients.push(client); sockets.push(socket); received.push(messages);
    const installed = install ? controller.installSource(socket) : false;
    return { socket, client, installed, errors };
  }
  const a = await connect(), b = await connect();
  const sender = createBoundedRelaySender();
  const wrapped = controller.wrapSendBounded(sender);
  async function waitCount(index, count) {
    if (received[index].length >= count) return;
    const client = clients[index];
    await new Promise((resolve, reject) => {
      const timer = setTimeout(() => finish(new Error('loopback frame count timeout: ' + index + '/' + count)), 10000);
      function check() { if (received[index].length >= count) finish(); }
      function finish(error) {
        clearTimeout(timer); client.off('message', check);
        error ? reject(error) : resolve();
      }
      client.on('message', check); check();
    });
  }
  function inject(index, messages) {
    return sockets[index]._socket.emit('data', Buffer.concat(messages.map(data => frame(data))));
  }
  async function close() {
    try { controller.restore(); }
    finally {
      for (const socket of sockets) {
        socket.terminate();
        // A deliberately thrown user message handler can abort Receiver._write
        // before its callback. End that broken test stream explicitly so ws's
        // normal socketOnClose error/finish fence can release server tracking.
        if (socket._receiver?._writableState.writing)
          socket._receiver.destroy(new Error('intentional fixture dispatch aborted'));
      }
      for (const client of clients) client.terminate();
      await new Promise((resolve, reject) => {
        const timer = setTimeout(() => reject(new Error('loopback fixture cleanup timeout')), 10000);
        server.close(() => { clearTimeout(timer); resolve(); });
      });
    }
  }
  return { server, controller, sockets, clients, received, originals, a, b, sender, wrapped,
    connect, inject, waitCount, close, serverErrors };
}
async function using(extra, run) {
  const f = await fixture(extra);
  try { await run(f); } finally { await f.close(); }
}
function normal(f, after) {
  let count = 0;
  f.sockets[0].on('message', (data, isBinary) => {
    same(isBinary, true);
    same(f.wrapped(f.sockets[1], data, binary), true);
    ++count; if (after) after(count);
  });
}
function settled(report) {
  same(report.valid, true, report.firstFault);
  same(report.complete, true);
  same(report.totals.pendingBorrows, 0);
  same(report.totals.unsettledBorrows, 0);
  yes(report.totals.maxBatchMessages <= 8);
  yes(report.totals.maxBatchWireBytes <= 65536);
}

await test('actual runtime and locked ws binding are explicit', () => {
  same(process.versions.node, '24.13.0', 'run with the relay Node; unknown runtime must not pass this gate');
  same(wsVersion, '8.21.3');
  same(BOUNDED_WRITES_SCHEMA, 'th10-relay-bounded-writes/1');
  equal(BOUNDED_WRITES_LIMITS, { messages: 8, wireBytes: 65536, sockets: 32 });
  yes(requireLocked.resolve('ws').includes('relay-r6-locked'));
});

await test('normal OFF is the exact original function and never reads a socket', () => {
  let reads = 0, calls = 0;
  const forbidden = new Proxy({}, { get() { ++reads; throw new Error('private socket touched'); } });
  const controller = createRelayBoundedWrites(configuration());
  same(controller.installSource(forbidden), false);
  const receiver = {}, args = [forbidden, Buffer.from([1]), binary], result = {};
  function original() { ++calls; same(this, receiver); same(arguments.length, 3);
    for (let i = 0; i < 3; ++i) same(arguments[i], args[i]); return result; }
  same(controller.wrapSendBounded(original), original);
  same(Reflect.apply(controller.wrapSendBounded(original), receiver, args), result);
  equal(calls, 1); equal(reads, 0);
  settled(controller.snapshot()); equal(controller.snapshot().installedSockets, 0);
  same(controller.restore().restored, true); equal(reads, 0);
});

await test('unknown Node/ws and nonboolean switches bypass before private reads', () => {
  let reads = 0;
  const socket = new Proxy({}, { get() { ++reads; throw new Error('socket touched'); } });
  const original = function () { return this; };
  const unsupported = createRelayBoundedWrites(configuration({ enabled: true, wsVersion: '8.99.0' }));
  same(unsupported.installSource(socket), false); same(unsupported.wrapSendBounded(original), original);
  same(unsupported.snapshot().compatible, false);
  const descriptor = Object.getOwnPropertyDescriptor(process.versions, 'node');
  yes(descriptor.configurable, 'controlled runtime-version negative fixture');
  try {
    Object.defineProperty(process.versions, 'node', { ...descriptor, value: '99.0.0' });
    const other = createRelayBoundedWrites(configuration({ enabled: true, observeWrites: true }));
    same(other.installSource(socket), false); same(other.wrapSendBounded(original), original);
    same(other.snapshot().compatible, false);
  } finally { Object.defineProperty(process.versions, 'node', descriptor); }
  const nonboolean = createRelayBoundedWrites(configuration({ enabled: 'true' }));
  same(nonboolean.installSource(socket), false); same(nonboolean.wrapSendBounded(original), original);
  equal(reads, 0);
});

await test('real Writable and locked Sender prove nested-cork writev semantics', async () => {
  class Sink extends Writable {
    constructor() { super(); this.vectors = []; this.scalars = []; }
    _write(chunk, encoding, callback) { this.scalars.push(Buffer.from(chunk)); callback(); }
    _writev(chunks, callback) { this.vectors.push(chunks.map(entry => Buffer.from(entry.chunk))); callback(); }
  }
  const sink = new Sink(), sender = new Sender(sink, {});
  const input = Array.from({ length: 9 }, (_, i) => payload(i));
  const send = data => sender.send(data, { binary: true, compress: false, mask: false, fin: true });
  send(input[0]); same(sink.vectors.length, 1, 'the first normal ws send reaches the real Writable writer');
  sink.cork();
  for (const data of input.slice(1)) send(data);
  same(sink.writableCorked, 1, 'ws inner uncorks do not consume the outer layer');
  same(sink.vectors.length, 1);
  sink.uncork();
  same(sink.writableCorked, 0); same(sink.vectors.length, 2);
  same(sink.vectors[1].length, 16, 'eight complete ws frames in one real _writev request');
  equal(Buffer.concat(sink.vectors.flat()), Buffer.concat(input.map(data => frame(data, { mask: false }))));
  const baseline = new Sink(), ordinary = new Sender(baseline, {});
  for (const data of input) ordinary.send(data, { binary: true, compress: false, mask: false, fin: true });
  same(baseline.vectors.length, 9);
  equal(Buffer.concat(baseline.vectors.flat()), Buffer.concat(sink.vectors.flat()));
  same(baseline.vectors.length - sink.vectors.length, 7, 'real Writable/locked Sender writev delta');
  const finished = once(sink, 'finish'), baselineFinished = once(baseline, 'finish');
  sink.end(); baseline.end(); await Promise.all([finished, baselineFinished]);
});

await test('real Writable async writer already queues and has needDrain', async () => {
  let complete;
  class Slow extends Writable {
    constructor() { super({ highWaterMark: 4 }); this.calls = 0; }
    _write(chunk, encoding, callback) { ++this.calls; complete = callback; }
  }
  const stream = new Slow();
  same(stream.write(Buffer.alloc(8)), false);
  same(stream._writableState.writing, true); same(stream.writableNeedDrain, true);
  same(stream.writableLength, 8);
  const drained = once(stream, 'drain'); complete(); await drained;
  same(stream._writableState.writing, false); same(stream.writableLength, 0);
  const finished = once(stream, 'finish'); stream.end(); await finished;
});

await test('observation-only OFF keeps original sender and records actual TCP writes', async () => {
  await using({ enabled: false, observeWrites: true, observeTiming: true }, async f => {
    same(f.wrapped, f.sender);
    yes(f.a.installed && f.b.installed);
    const input = Array.from({ length: 12 }, (_, i) => payload(i));
    normal(f); same(f.inject(0, input), true);
    await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
    const report = f.controller.snapshot(); settled(report);
    same(report.enabled, false); same(report.totals.borrowedBatches, 0);
    yes(report.totals.writevCalls > 0); yes(report.totals.rawWallMs >= 0);
  });
});

await test('real TCP first send is immediate; subsequent batches keep every byte and frame', async () => {
  await using({}, async f => {
    const input = Array.from({ length: 26 }, (_, i) => payload(i));
    let atFirst = null;
    normal(f, count => { if (count === 1) atFirst = f.controller.snapshot(); });
    same(f.inject(0, input), true);
    await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
    same(atFirst.complete, false); same(atFirst.totals.borrowedBatches, 0);
    yes(atFirst.totals.writevCalls > 0, 'first actual ws write entered the writer before the second message');
    const report = f.controller.snapshot(); settled(report);
    yes(report.totals.borrowedBatches > 0, 'real Socket fixture must exercise batching; no silent coverage pass');
    same(report.totals.maxBatchMessages, 8);
    yes(report.totals.maxWritevChunks > 2, 'actual Node writer received more than one ws frame');
    yes(report.totals.writevCalls < input.length, 'check actual writev shape, not shorter send spans');
    same(report.totals.firstImmediate, 1);
    same(report.totals.rawDispatches, report.totals.rawCompleted);
  });
});

await test('64 KiB limit includes ws header and flushes before the next oversized batch', async () => {
  await using({}, async f => {
    // 8,194 framed bytes: seven fit, eight exceed 64 KiB.
    const input = Array.from({ length: 16 }, (_, i) => payload(i, 8190));
    normal(f); f.inject(0, input); await f.waitCount(1, input.length);
    bytesEqual(f.received[1], input);
    const report = f.controller.snapshot(); settled(report);
    yes(report.totals.borrowedBatches > 0);
    yes(report.totals.maxBatchMessages <= 7);
    yes(report.totals.maxBatchWireBytes <= 7 * 8194);
  });
});

await test('a frame larger than the batch bound passes unchanged and disables this raw scope', async () => {
  await using({}, async f => {
    const input = [payload(1, 65533), payload(2), payload(3)];
    normal(f); f.inject(0, input); await f.waitCount(1, input.length);
    bytesEqual(f.received[1], input); settled(f.controller.snapshot());
    same(f.controller.snapshot().totals.borrowedBatches, 0);
  });
});

await test('separate raw events never keep a borrowed tail or share first-packet state', async () => {
  await using({}, async f => {
    const input = Array.from({ length: 4 }, (_, i) => payload(i));
    normal(f);
    for (const value of input) {
      f.inject(0, [value]);
      same(f.controller.snapshot().totals.pendingBorrows, 0);
    }
    await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
    const report = f.controller.snapshot(); settled(report);
    same(report.totals.borrowedBatches, 0); same(report.totals.firstImmediate, input.length);
  });
});

await test('normal default OFF does not patch actual raw instances', async () => {
  await using({ enabled: false, observeWrites: false, observeTiming: false }, async f => {
    same(f.a.installed, false); same(f.b.installed, false);
    for (const item of f.originals) {
      same(item.raw.emit, item.emit); same(item.raw.cork, item.cork); same(item.raw.uncork, item.uncork);
      same(item.raw._write, item.write); same(item.raw._writev, item.writev);
    }
    normal(f); const input = [payload(1), payload(2)];
    f.inject(0, input); await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
    settled(f.controller.snapshot()); same(f.controller.snapshot().totals.rawDispatches, 0);
  });
});

await test('an existing foreign cork is preserved and the original sender still handles every frame', async () => {
  await using({}, async f => {
    const raw = f.sockets[1]._socket, input = [payload(1), payload(2), payload(3)];
    raw.cork();
    try {
      normal(f); f.inject(0, input);
      same(raw.writableCorked, 1);
      same(f.controller.snapshot().totals.borrowedBatches, 0);
    } finally { raw.uncork(); }
    await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
    settled(f.controller.snapshot());
  });
});

await test('foreign cork beginning after a borrowed send flushes ours before borrowing its own', async () => {
  await using({}, async f => {
    const raw = f.sockets[1]._socket, input = [payload(1), payload(2), payload(3)];
    let covered = false;
    normal(f, count => {
      if (count !== 2) return;
      same(f.controller.snapshot().totals.pendingBorrows, 1); covered = true;
      raw.cork(); same(f.controller.snapshot().totals.pendingBorrows, 0);
      same(raw.writableCorked, 1); raw.uncork();
    });
    f.inject(0, input); yes(covered);
    await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
    settled(f.controller.snapshot()); same(raw.writableCorked, 0);
  });
});

await test('foreign uncork consumption followed by a new cork is not uncorked by raw finally', async () => {
  await using({}, async f => {
    const raw = f.sockets[1]._socket, input = [payload(1), payload(2)];
    normal(f, count => {
      if (count === 2) {
        same(f.controller.snapshot().totals.pendingBorrows, 1);
        raw.uncork(); raw.cork();
      }
    });
    try {
      f.inject(0, input); same(raw.writableCorked, 1);
      const report = f.controller.snapshot(); settled(report);
      same(report.totals.consumedBorrows, 1);
    } finally { raw.uncork(); }
    await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
  });
});

await test('Writable.end consumes the borrow; a later layer survives the old raw finally', async () => {
  await using({}, async f => {
    const raw = f.sockets[1]._socket, input = [payload(1), payload(2)];
    normal(f, count => {
      if (count === 2) {
        same(f.controller.snapshot().totals.pendingBorrows, 1);
        raw.end(); same(raw.writableEnded, true); raw.cork();
      }
    });
    try {
      f.inject(0, input); same(raw.writableCorked, 1);
      const report = f.controller.snapshot(); settled(report);
      same(report.totals.consumedBorrows, 1);
    } finally { raw.uncork(); }
    await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
  });
});

await test('needDrain during a borrowed write releases promptly and bypasses remaining sends', async () => {
  await using({}, async f => {
    const raw = f.sockets[1]._socket, previous = raw._writableState.highWaterMark;
    const input = Array.from({ length: 6 }, (_, i) => payload(i));
    // Let the first frame finish at the original HWM. Lowering it sooner makes
    // that immediate write set needDrain and correctly prevents any borrowing.
    // The after-send hook runs before the next decoded message is forwarded.
    try {
      normal(f, count => {
        if (count === 1) {
          same(raw.writableNeedDrain, false); same(raw.writableLength, 0);
          same(f.controller.snapshot().totals.borrowedBatches, 0);
          // Only the real second write will set needDrain; do not fake its flag.
          raw._writableState.highWaterMark = 1;
        } else if (count === 2) {
          const during = f.controller.snapshot();
          same(during.totals.borrowedBatches, 1);
          yes(during.bypassChecks.needDrain > 0, 'the real borrowed write triggered needDrain');
          same(during.totals.releasedBatches, 1);
          same(during.totals.pendingBorrows, 0, 'settle occurs before the second sender returns');
          same(raw.writableCorked, 0);
        }
      });
      f.inject(0, input); await f.waitCount(1, input.length);
      bytesEqual(f.received[1], input);
      const report = f.controller.snapshot(); settled(report);
      yes(report.totals.borrowedBatches > 0); same(report.totals.maxBatchMessages, 1);
      same(report.totals.borrowedBatches, 1, 'the remaining messages bypass borrowing');
    } finally { raw._writableState.highWaterMark = previous; }
  });
});

await test('a real in-flight TCP write causes conservative bypass without dropping later frames', async () => {
  await using({}, async f => {
    const target = f.sockets[1], raw = target._socket, prefix = [];
    const input = [payload(1), payload(2), payload(3)];
    f.clients[1]._socket.pause();
    try {
      // Stop at the first real async write, keeping Node's pending bytes below
      // the default sender budget. An explicit large budget would only test the
      // options bypass and would no longer exercise inFlightWrite qualification.
      while (!raw._writableState.writing && prefix.length < 256) {
        const part = payload(10000 + prefix.length, 64 * 1024);
        prefix.push(part); target.send(part, { binary: true, compress: false });
      }
      yes(raw._writableState.writing && raw.writableLength > 0,
        'fixture must create a real async pending write');
      yes(raw.writableLength + input.length * (48 + 16) < 256 * 1024,
        'default admission remains safe; no synthetic writing flag or enlarged budget');
      f.sockets[0].on('message', data => same(f.wrapped(target, data, binary), true));
      f.inject(0, input); same(f.controller.snapshot().totals.borrowedBatches, 0);
      settled(f.controller.snapshot());
      yes(f.controller.snapshot().bypassChecks.inFlightWrite > 0);
      same(f.controller.snapshot().bypassChecks.optionsOrTarget, 0);
    } finally { f.clients[1]._socket.resume(); }
    await f.waitCount(1, prefix.length + input.length); bytesEqual(f.received[1], [...prefix, ...input]);
  });
});

await test('nested raw dispatch releases the outer scope and does not borrow in either remainder', async () => {
  await using({}, async f => {
    const input = [payload(1), payload(2), payload(3), payload(4)], nested = payload(99);
    f.sockets[1].on('message', data => same(f.wrapped(f.sockets[0], data, binary), true));
    normal(f, count => {
      if (count === 2) {
        same(f.controller.snapshot().totals.pendingBorrows, 1);
        same(f.inject(1, [nested]), true);
        same(f.controller.snapshot().totals.pendingBorrows, 0);
      }
    });
    f.inject(0, input); await Promise.all([f.waitCount(1, input.length), f.waitCount(0, 1)]);
    bytesEqual(f.received[1], input); bytesEqual(f.received[0], [nested]);
    const report = f.controller.snapshot(); settled(report);
    same(report.totals.nestedRaw, 1); same(report.totals.borrowedBatches, 1);
  });
});

await test('target identity replacement settles only the saved old raw socket', async () => {
  await using({}, async f => {
    const target = f.sockets[1], raw = target._socket, replacement = new Socket();
    const input = [payload(1), payload(2)];
    normal(f, count => {
      if (count === 2) { same(f.controller.snapshot().totals.pendingBorrows, 1); target._socket = replacement; }
    });
    try {
      f.inject(0, input); same(raw.writableCorked, 0); same(replacement.writableCorked, 0);
      settled(f.controller.snapshot());
    } finally { target._socket = raw; replacement.destroy(); }
    await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
  });
});

await test('unwrapped ws control send flushes a borrowed target first and keeps frame order', async () => {
  await using({}, async f => {
    const input = [payload(1), payload(2), payload(3)], events = [];
    f.clients[1].on('message', data => events.push('data:' + data.readUInt32LE(0)));
    f.clients[1].on('ping', data => events.push('ping:' + data.toString()));
    normal(f, count => {
      if (count === 2) {
        same(f.controller.snapshot().totals.pendingBorrows, 1);
        f.sockets[1].ping(Buffer.from('control'));
        same(f.controller.snapshot().totals.pendingBorrows, 0);
      }
    });
    f.inject(0, input); await f.waitCount(1, input.length);
    bytesEqual(f.received[1], input);
    equal(events, ['data:1', 'data:2', 'ping:control', 'data:3']);
    settled(f.controller.snapshot());
  });
});

await test('false admission return preserves each original gate and does not retain an empty borrow', async () => {
  await using({}, async f => {
    let calls = 0;
    const bounded = f.controller.wrapSendBounded(function () {
      ++calls; return calls === 2 ? false : Reflect.apply(f.sender, this, arguments);
    });
    const input = [payload(1), payload(2), payload(3)], returns = [];
    f.sockets[0].on('message', data => returns.push(bounded(f.sockets[1], data, binary)));
    f.inject(0, input); await f.waitCount(1, 2);
    equal(calls, 3); equal(returns, [true, false, true]);
    bytesEqual(f.received[1], [input[0], input[2]]);
    const report = f.controller.snapshot(); settled(report); same(report.totals.pendingBorrows, 0);
  });
});

await test('actual sender budget 100 rejects a held 50-byte frame; explicit-budget ON preserves OFF admission', async () => {
  const input = [payload(1, 48), payload(2, 48), payload(3, 48)];
  const options = { binary: true, delayMs: 0, maxBufferedBytes: 100 };
  for (const mode of [{ enabled: false, unsafe: false }, { enabled: true, unsafe: false },
                      { enabled: false, unsafe: true }]) {
    await using({ enabled: mode.enabled, observeWrites: mode.enabled, observeTiming: false }, async f => {
      const target = f.sockets[1], raw = target._socket, returns = [], gateSizes = [];
      let externalCork = false;
      f.sockets[0].on('message', data => {
        if (mode.unsafe && returns.length === 1) { raw.cork(); externalCork = true; }
        gateSizes.push(Number(target.bufferedAmount || 0) + data.byteLength + 16);
        returns.push(f.wrapped(target, data, options));
      });
      try {
        f.inject(0, input);
        if (mode.unsafe) {
          // This is the real original sender's counterexample, not a modeled
          // boolean: the third gate sees 50 held wire bytes + 48 payload + 16.
          equal(gateSizes, [64, 64, 114]); equal(returns, [true, true, false]);
          yes(raw.destroyed, 'the original budget gate terminates the target');
        } else {
          equal(gateSizes, [64, 64, 64]); equal(returns, [true, true, true]);
          await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
          same(raw.destroyed, false);
        }
        const report = f.controller.snapshot(); settled(report);
        same(report.totals.borrowedBatches, 0);
        if (mode.enabled) same(report.bypassChecks.optionsOrTarget, 1);
      } finally { if (externalCork) raw.uncork(); }
    });
  }
});

await test('an explicit numeric budget settles the existing batch before the original admission check', async () => {
  await using({}, async f => {
    const input = [payload(1, 48), payload(2, 48), payload(3, 48)];
    const options = { binary: true, delayMs: 0, maxBufferedBytes: 100 };
    const entries = [], returns = [];
    const wrapped = f.controller.wrapSendBounded(function (target, data, settings) {
      if (settings === options) entries.push({ pending: f.controller.snapshot().totals.pendingBorrows,
        buffered: target.bufferedAmount });
      return Reflect.apply(f.sender, this, arguments);
    });
    f.sockets[0].on('message', data => {
      const count = returns.length;
      returns.push(wrapped(f.sockets[1], data, count < 2 ? binary : options));
      if (count === 1) {
        same(f.controller.snapshot().totals.pendingBorrows, 1);
        same(f.sockets[1].bufferedAmount, 50, 'the second real WS frame is held before the budget boundary');
      }
    });
    f.inject(0, input); await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
    equal(returns, [true, true, true]); equal(entries, [{ pending: 0, buffered: 0 }]);
    const report = f.controller.snapshot(); settled(report);
    same(report.totals.borrowedBatches, 1); same(report.totals.releasedBatches, 1);
    same(report.bypassChecks.optionsOrTarget, 1);
  });
});

await test('an own budget getter is read once by the original sender after the existing batch settles', async () => {
  await using({}, async f => {
    const input = [payload(1, 48), payload(2, 48), payload(3, 48)], returns = [];
    let reads = 0;
    const options = { binary: true, delayMs: 0, get maxBufferedBytes() {
      ++reads;
      same(f.controller.snapshot().totals.pendingBorrows, 0, 'qualification must not pre-read this getter');
      same(f.sockets[1].bufferedAmount, 0, 'old writes settle before original option evaluation');
      return 100;
    } };
    f.sockets[0].on('message', data => {
      const count = returns.length;
      returns.push(f.wrapped(f.sockets[1], data, count < 2 ? binary : options));
      if (count === 1) same(f.controller.snapshot().totals.pendingBorrows, 1);
    });
    f.inject(0, input); await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
    equal(returns, [true, true, true]); same(reads, 1);
    const report = f.controller.snapshot(); settled(report);
    same(report.totals.borrowedBatches, 1); same(report.totals.releasedBatches, 1);
    same(report.bypassChecks.optionsOrTarget, 1);
  });
});

await test('getter options and delayed sender stay original; no duplicate option reads or timers', async () => {
  await using({}, async f => {
    let reads = 0;
    const getterOptions = { get binary() { ++reads; return true; }, delayMs: 0 };
    const input = [payload(1), payload(2)];
    f.sockets[0].on('message', data => same(f.wrapped(f.sockets[1], data, getterOptions), true));
    f.inject(0, input); await f.waitCount(1, input.length);
    bytesEqual(f.received[1], input); equal(reads, input.length);
    same(f.controller.snapshot().totals.borrowedBatches, 0);
    const jobs = [];
    const delayed = createBoundedRelaySender({ schedule(fn, ms) {
      const job = { fn, ms }; jobs.push(job); return job;
    }, cancel() {} });
    const boundDelayed = f.controller.wrapSendBounded(delayed);
    f.sockets[0].removeAllListeners('message');
    f.sockets[0].on('message', data => same(boundDelayed(f.sockets[1], data, { binary: true, delayMs: 12 }), true));
    const later = [payload(3), payload(4)];
    f.inject(0, later); equal(jobs.length, 2); yes(jobs.every(job => job.ms === 12));
    same(f.controller.snapshot().totals.borrowedBatches, 0);
    for (const job of jobs) job.fn();
    await f.waitCount(1, 4); bytesEqual(f.received[1], [...input, ...later]);
    settled(f.controller.snapshot());
  });
});

await test('wrapper preserves this, every argument and unusual return identity', async () => {
  await using({}, async f => {
    const receiver = {}, result = {}, extra = {}, records = [];
    const wrapped = f.controller.wrapSendBounded(function () {
      records.push({ receiver: this, args: Array.from(arguments) });
      Reflect.apply(f.sender, this, arguments); return result;
    });
    const input = [payload(1), payload(2)];
    f.sockets[0].on('message', data => same(wrapped.call(receiver, f.sockets[1], data, binary, extra), result));
    f.inject(0, input); await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
    equal(records.length, input.length);
    for (const record of records) {
      same(record.receiver, receiver); same(record.args.length, 4);
      same(record.args[0], f.sockets[1]); same(record.args[2], binary); same(record.args[3], extra);
    }
    settled(f.controller.snapshot());
  });
});

await test('original throw identity survives raw finally and borrowed bytes are settled', async () => {
  await using({}, async f => {
    const sentinel = new Error('original handler failure'); let calls = 0;
    const wrapped = f.controller.wrapSendBounded(function () {
      ++calls; const result = Reflect.apply(f.sender, this, arguments);
      if (calls === 2) throw sentinel;
      return result;
    });
    f.sockets[0].on('message', data => wrapped(f.sockets[1], data, binary));
    const input = [payload(1), payload(2), payload(3)];
    throwsSame(() => f.inject(0, input), sentinel);
    same(calls, 2); same(f.sockets[1]._socket.writableCorked, 0);
    const report = f.controller.snapshot(); settled(report); yes(report.totals.originalThrows > 0);
    await f.waitCount(1, 2); bytesEqual(f.received[1], input.slice(0, 2));
  });
});

await test('actual native writer throw at uncork is explicit and never silently leaves our cork', async () => {
  await using({}, async f => {
    const raw = f.sockets[1]._socket, handle = raw._handle;
    const descriptor = Object.getOwnPropertyDescriptor(handle, 'writev');
    const sentinel = new Error('fixture native writev failure');
    normal(f, count => {
      if (count === 1) Object.defineProperty(handle, 'writev', { configurable: true,
        writable: true, value() { throw sentinel; } });
    });
    try {
      throwsSame(() => f.inject(0, [payload(1), payload(2)]), sentinel);
      const report = f.controller.snapshot();
      same(report.valid, false); same(report.complete, true);
      same(report.totals.pendingBorrows, 0); same(raw.writableCorked, 0);
      same(report.totals.uncorkThrows, 1); same(report.firstFault, 'native-uncork-threw');
    } finally {
      if (descriptor) Object.defineProperty(handle, 'writev', descriptor);
      else delete handle.writev;
    }
  });
});


await test('original sender throw remains first when the final native uncork also throws', async () => {
  await using({}, async f => {
    const raw = f.sockets[1]._socket, handle = raw._handle;
    const descriptor = Object.getOwnPropertyDescriptor(handle, 'writev');
    const primary = new Error('original sender failure'), cleanup = new Error('cleanup writer failure');
    let calls = 0, cleanupCalls = 0;
    const wrapped = f.controller.wrapSendBounded(function () {
      ++calls; const result = Reflect.apply(f.sender, this, arguments);
      if (calls === 1) Object.defineProperty(handle, 'writev', { configurable: true,
        writable: true, value() { ++cleanupCalls; throw cleanup; } });
      if (calls === 2) throw primary;
      return result;
    });
    f.sockets[0].on('message', data => wrapped(f.sockets[1], data, binary));
    try {
      throwsSame(() => f.inject(0, [payload(1), payload(2), payload(3)]), primary);
      same(calls, 2); same(cleanupCalls, 1);
      const report = f.controller.snapshot();
      same(report.valid, false); same(report.complete, true);
      same(report.totals.uncorkThrows, 1); same(report.totals.pendingBorrows, 0);
      same(raw.writableCorked, 0);
    } finally {
      if (descriptor) Object.defineProperty(handle, 'writev', descriptor);
      else delete handle.writev;
    }
  });
});

await test('changed uncork owner is reported, never overwritten, and original throw wins cleanup', async () => {
  await using({}, async f => {
    const raw = f.sockets[1]._socket, ours = raw.uncork;
    const sentinel = new Error('original wins');
    const replacement = function () { return Reflect.apply(Writable.prototype.uncork, this, arguments); };
    let calls = 0;
    const wrapped = f.controller.wrapSendBounded(function () {
      ++calls; const result = Reflect.apply(f.sender, this, arguments);
      if (calls === 2) { raw.uncork = replacement; throw sentinel; }
      return result;
    });
    f.sockets[0].on('message', data => wrapped(f.sockets[1], data, binary));
    try {
      throwsSame(() => f.inject(0, [payload(1), payload(2)]), sentinel);
      const report = f.controller.snapshot();
      same(report.valid, false); same(report.complete, false);
      same(report.totals.unsettledBorrows, 1); same(raw.uncork, replacement);
      same(raw.writableCorked, 1, 'ambiguous ownership is never decremented by guessing');
    } finally {
      // Test owns this intentional replacement and explicitly releases it.
      raw.uncork = ours; Reflect.apply(Writable.prototype.uncork, raw, []);
    }
  });
});

await test('restore during a handler settles once and an old callback cannot recreate a borrow', async () => {
  await using({}, async f => {
    let during;
    const input = [payload(1), payload(2), payload(3)];
    normal(f, count => { if (count === 2) during = f.controller.restore(); });
    f.inject(0, input); await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
    same(during.restored, true); same(during.complete, false); same(during.totals.pendingBorrows, 0);
    const report = f.controller.snapshot(); settled(report);
    same(report.restored, true); same(report.active, false); same(report.totals.borrowedBatches, 1);
    for (const item of f.originals) {
      same(item.raw.emit, item.emit); same(item.raw.cork, item.cork); same(item.raw.uncork, item.uncork);
      same(item.raw._write, item.write); same(item.raw._writev, item.writev);
    }
    same(during.complete, false, 'earlier snapshot is not mutated into a later pass');
  });
});

await test('after restore, subsequent raw calls and saved wrappers leave the entire snapshot unchanged', async () => {
  await using({}, async f => {
    const input = Array.from({ length: 7 }, (_, i) => payload(i + 1));
    const raw = f.sockets[0]._socket, savedEmit = raw.emit;
    normal(f); f.inject(0, input.slice(0, 3)); await f.waitCount(1, 3);
    yes(f.controller.snapshot().totals.borrowedBatches > 0, 'prove the installed path was active');
    const frozen = f.controller.restore(); settled(frozen);
    same(frozen.active, false); same(frozen.restored, true);
    same(f.inject(0, input.slice(3, 5)), true);
    same(Reflect.apply(savedEmit, raw, ['data', frame(input[5])]), true);
    same(f.wrapped(f.sockets[1], input[6], binary), true);
    await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
    equal(f.controller.snapshot(), frozen, 'no raw/send/write/timing/bypass counter grows after restore');
    same(f.controller.installSource(f.sockets[0]), false);
    equal(f.controller.restore(), frozen, 'repeated restore and stale install are also inert');
  });
});

await test('borrowed emit receiver and nondata events retain EventEmitter semantics', async () => {
  await using({}, async f => {
    const raw = f.sockets[0]._socket, emitter = new EventEmitter(), marker = {}, seen = [];
    emitter.on('custom', function () { seen.push({ receiver: this, args: Array.from(arguments) }); });
    same(Reflect.apply(raw.emit, emitter, ['custom', marker, 7]), true);
    same(seen[0].receiver, emitter); same(seen[0].args[0], marker); same(seen[0].args[1], 7);
    same(f.controller.snapshot().totals.rawDispatches, 0);
    const previous = raw.listeners('data'); f.inject(0, []);
    equal(raw.listeners('data'), previous); settled(f.controller.snapshot());
  });
});

await test('TLS, different streams and negotiated compression are unsupported pass-through', async () => {
  const controller = createRelayBoundedWrites(configuration({ enabled: true }));
  const tls = new TLSSocket(new Socket());
  const fakeAccepted = new WebSocket(null, [], { autoPong: false, closeTimeout: 0 });
  fakeAccepted._socket = tls;
  same(controller.installSource(fakeAccepted), false);
  fakeAccepted._socket = new Writable({ write(chunk, encoding, callback) { callback(); } });
  same(controller.installSource(fakeAccepted), false);
  tls.destroy(); fakeAccepted._socket.destroy(); controller.restore();
  await using({ compression: true }, async f => {
    same(f.a.installed, false); same(f.b.installed, false);
    same(f.controller.snapshot().installedSockets, 0);
    normal(f); const input = [payload(1), payload(2)];
    f.inject(0, input); await f.waitCount(1, input.length); bytesEqual(f.received[1], input);
    same(f.controller.snapshot().totals.borrowedBatches, 0); settled(f.controller.snapshot());
  });
});


await test('real hop observer composition restores the outer candidate before freezing the inner observer', async () => {
  // The existing observer's own-property ownership checks stay unchanged.
  // Its raw span ends inside this module's span and excludes outer uncork.
  const old = process.env.TH10_RELAY_HOP_DIAGNOSTIC;
  let install;
  try {
    delete process.env.TH10_RELAY_HOP_DIAGNOSTIC;
    ({ install } = requireHere('./relay-hop-observer.cjs'));
  } finally {
    if (old === undefined) delete process.env.TH10_RELAY_HOP_DIAGNOSTIC;
    else process.env.TH10_RELAY_HOP_DIAGNOSTIC = old;
  }
  const ipc = new EventEmitter(); ipc.send = () => true;
  const timers = { setInterval() { return { unref() {} }; }, clearInterval() {} };
  const observer = install({ WebSocket, WebSocketServer, rawSocketTrace: true,
    clock: performance, ipc, timers, maxEvents: 1024, maxHeartbeats: 2 });
  try {
    await using({}, async f => {
      const input = Array.from({ length: 12 }, (_, i) => payload(i));
      normal(f);
      f.sockets[1].on('message', data => same(f.wrapped(f.sockets[0], data, binary), true));
      f.inject(0, input); f.inject(1, [payload(90)]);
      await Promise.all([f.waitCount(1, input.length), f.waitCount(0, 1)]);
      bytesEqual(f.received[1], input); bytesEqual(f.received[0], [payload(90)]);
      const own = f.controller.restore(); // Required ordering, before observer snapshot.
      settled(own);
      const hop = observer.snapshot();
      same(hop.valid, true, hop.errors.join(','));
      same(hop.complete, true); same(hop.incompleteSpans, 0);
      const gameplay = hop.sockets.filter(socket => socket.role === 1);
      same(gameplay.length, 2); yes(gameplay.every(socket => socket.rawSocketObserved));
      for (const socket of gameplay) {
        yes(socket.rawDataEvents > 0);
        yes(own.sockets[socket.seat].maxRawWallMs >= socket.maxRawDataMs,
          'outer raw wall includes inner observation and the final native uncork');
      }
    });
  } finally { observer.snapshot(); }
});

await test('fixed socket cap is explicit; snapshots contain no payloads or mutable references', async () => {
  await using({}, async f => {
    for (let i = 2; i < BOUNDED_WRITES_LIMITS.sockets + 1; ++i) {
      const added = await f.connect();
      same(added.installed, i < BOUNDED_WRITES_LIMITS.sockets);
    }
    const before = f.controller.snapshot(), frozenCopy = JSON.stringify(before);
    same(before.installedSockets, BOUNDED_WRITES_LIMITS.sockets);
    same(before.ignoredSockets, 1); same(before.sockets.length, 32);
    yes(!frozenCopy.includes('127.0.0.1')); yes(!frozenCopy.includes('EANP'));
    normal(f); const input = [payload(1), payload(2)];
    f.inject(0, input); await f.waitCount(1, input.length);
    same(JSON.stringify(before), frozenCopy, 'snapshots do not retain live metric objects');
    settled(f.controller.snapshot());
  });
});

console.log(JSON.stringify({
  schema: 'th10-relay-bounded-writes-test/1', passed: true, groups, checks,
  nodeVersion: process.versions.node, wsVersion, lockedWs: requireLocked.resolve('ws'),
  scope: 'real Node Writable/Socket and locked ws; synthetic raw source, real loopback destination; no game/browser/performance claim'
}));
