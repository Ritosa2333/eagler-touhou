// Offline semantic checks for the opt-in relay preload. No real sockets or browser.
import assert from 'node:assert/strict';
import { EventEmitter } from 'node:events';
import { createRequire } from 'node:module';

const require = createRequire(import.meta.url);
let checks = 0, groups = 0;
const equal = (a, b, message) => { ++checks; assert.deepEqual(a, b, message); };
const same = (a, b, message) => { ++checks; assert.strictEqual(a, b, message); };
const yes = (value, message) => { ++checks; assert.ok(value, message); };
const throwsSame = (fn, error) => { ++checks; assert.throws(fn, value => value === error); };
const test = (name, run) => { run(); ++groups; console.log('PASS ' + name); };

let observer;
test('default-off preload has no timer or IPC listener', () => {
  const previous = process.env.TH10_RELAY_HOP_DIAGNOSTIC;
  const interval = globalThis.setInterval, listeners = process.listenerCount('message');
  let timers = 0;
  try {
    delete process.env.TH10_RELAY_HOP_DIAGNOSTIC;
    globalThis.setInterval = () => { ++timers; throw new Error('unexpected timer'); };
    observer = require('./relay-hop-observer.cjs');
    equal(timers, 0); equal(process.listenerCount('message'), listeners);
    equal(observer.controller, undefined);
    equal(observer.SCHEMA, 'th10-relay-server-hop/1');
  } finally {
    globalThis.setInterval = interval;
    if (previous === undefined) delete process.env.TH10_RELAY_HOP_DIAGNOSTIC;
    else process.env.TH10_RELAY_HOP_DIAGNOSTIC = previous;
  }
});
const { install, COLUMNS, RAW_DATA_COLUMNS } = observer;
const column = Object.fromEntries(COLUMNS.map((name, index) => [name, index]));
const read = (row, name) => row[column[name]];
const ofKind = (report, kind) => report.events.filter(row => read(row, 'kind') === kind);

function fixture(extra = {}) {
  let time = 100, clockThrows = false;
  class Socket extends EventEmitter {
    constructor() { super(); this.readyState = 1; this.bufferedAmount = 0; this.calls = []; }
    send() {
      this.calls.push({ receiver: this, args: Array.from(arguments) });
      return this.sendHook ? Reflect.apply(this.sendHook, this, arguments) : 'original-send';
    }
  }
  class Server extends EventEmitter {}
  const originalSend = Socket.prototype.send;
  const originalSocketEmit = Socket.prototype.emit, originalServerEmit = Server.prototype.emit;
  const socketEmitDescriptor = Object.getOwnPropertyDescriptor(Socket.prototype, 'emit');
  const serverEmitDescriptor = Object.getOwnPropertyDescriptor(Server.prototype, 'emit');
  const jobs = [];
  const timers = {
    setInterval(fn, ms) {
      const job = { fn, ms, active: true, unrefs: 0, unref() { ++this.unrefs; } };
      jobs.push(job); return job;
    },
    clearInterval(job) { job.active = false; }
  };
  const ipc = new EventEmitter();
  ipc.sent = [];
  ipc.send = function (message) { this.sent.push(message); return true; };
  const clock = { now() { if (clockThrows) throw new Error('clock unavailable'); return time; }, timeOrigin: 1700000000000 };
  const controller = install({ WebSocket: Socket, WebSocketServer: Server, clock, timers, ipc,
    maxEvents: 256, maxHeartbeats: 16, ...extra });
  const server = new Server();
  function connect(seat, players = 2, suffix = '', rawSocket = undefined) {
    const socket = new Socket();
    if (rawSocket !== undefined) socket._socket = rawSocket;
    const url = '/?room=perf-th10-private_room&run=1&player=' + seat + '&players=' + players + suffix;
    server.emit('connection', socket, { url });
    return socket;
  }
  return { Socket, Server, server, connect, controller, ipc, jobs, clock,
    originalSend, originalSocketEmit, originalServerEmit, socketEmitDescriptor, serverEmitDescriptor,
    advance(ms) { time += ms; }, setClockFailure(value) { clockThrows = value; },
    fire(ms = 500) { time += ms; for (const job of jobs) if (job.active) job.fn(); },
    snapshot() { return controller.snapshot(); }
  };
}

function packet({ sender = 0, players = 2, latest = 101, count = 1, target = null } = {}) {
  const offset = target === null ? 0 : 2;
  const bytes = Buffer.alloc(offset + 44 + count * 12);
  if (offset) { bytes[0] = 0xe7; bytes[1] = target; }
  Buffer.from('EANP').copy(bytes, offset);
  bytes[offset + 4] = 4; bytes[offset + 5] = 1;
  bytes[offset + 6] = sender; bytes[offset + 7] = players;
  bytes.writeUInt32LE(latest >>> 0, offset + 24);
  bytes.writeUInt32LE(count ? latest - count + 1 : 0xffffffff, offset + 32);
  bytes[offset + 42] = count;
  bytes.fill(0xab, offset + 44);
  return bytes;
}

test('real EventEmitter handler order and recv-to-send spans', () => {
  const f = fixture(), a = f.connect(0), b = f.connect(1);
  const input = packet({ target: 1 }), payload = input.subarray(2), calls = [];
  const options = { binary: true }, callback = () => { throw new Error('must not add a callback call'); };
  const result = { original: 'return identity' };
  b.sendHook = function () { f.advance(3); this.bufferedAmount = 24; return result; };
  const first = function (data, binary) {
    same(this, a); same(data, input); same(binary, true); calls.push('first');
    f.advance(2); same(b.send(payload, options, callback), result); f.advance(1);
  };
  const second = () => calls.push('second');
  a.on('message', first); a.on('message', second);
  const before = a.listeners('message');
  same(a.emit('message', input, true), true);
  equal(calls, ['first', 'second']); equal(a.listeners('message'), before);
  same(b.calls[0].receiver, b); same(b.calls[0].args[0], payload);
  same(b.calls[0].args[1], options); same(b.calls[0].args[2], callback);
  equal(b.calls[0].args.length, 3); equal(f.ipc.sent.length, 0);
  input.writeUInt32LE(999, 26); input.fill(0, 46);
  const report = f.snapshot(), [receive] = ofKind(report, 1), [send] = ofKind(report, 2);
  yes(report.valid && report.complete); equal(report.eventCount, 2);
  equal([read(receive, 'startMs'), read(receive, 'endMs')], [100, 106]);
  equal([read(send, 'startMs'), read(send, 'endMs')], [102, 105]);
  equal(read(send, 'messageContextId'), read(receive, 'eventId'));
  equal([read(send, 'sourceSocketId'), read(send, 'sourceSeat'), read(send, 'seat')], [1, 0, 1]);
  equal([read(receive, 'targetedPeer'), read(send, 'targetedPeer')], [1, -1]);
  equal([read(receive, 'headerLatestFrame'), read(send, 'headerLatestFrame')], [101, 101]);
  equal([read(send, 'bufferedBefore'), read(send, 'bufferedAfter')], [0, 24]);
  equal(read(receive, 'synchronousSendCalls'), 1);
  equal(report.sockets[1].maxReceiveToSendMs, 2);
  equal(report.sockets[0].maxMessageMs, 6); equal(report.sockets[1].maxSendMs, 3);
  yes(!JSON.stringify(report).includes('private_room'));
  yes(report.events.every(row => row.length === COLUMNS.length && row.every(Number.isFinite)));
});

test('send this arguments options getters and return are untouched', () => {
  const f = fixture(), a = f.connect(0);
  const data = { [Symbol.toPrimitive]() { throw new Error('must not coerce'); } };
  let getterReads = 0;
  const options = { get binary() { ++getterReads; throw new Error('must not read options'); } };
  const callback = () => {}, extra = { retainedByOriginal: true }, sentinel = Symbol('result');
  a.sendHook = function () { same(this, a); equal(arguments.length, 4); return sentinel; };
  same(a.send(data, options, callback, extra), sentinel);
  same(a.calls[0].args[0], data); same(a.calls[0].args[1], options);
  same(a.calls[0].args[2], callback); same(a.calls[0].args[3], extra);
  equal(getterReads, 0);
  const report = f.snapshot();
  yes(report.valid && report.complete); equal(read(report.events[0], 'bytes'), -1);
  equal(read(report.events[0], 'messageContextId'), 0);
});

test('original send exception escapes unchanged and closes both spans', () => {
  const f = fixture(), a = f.connect(0), b = f.connect(1), failure = new Error('original send failure');
  b.sendHook = () => { f.advance(4); throw failure; };
  a.on('message', data => b.send(data));
  throwsSame(() => a.emit('message', packet(), true), failure);
  b.sendHook = () => 'later';
  equal(b.send(packet({ latest: 102 })), 'later');
  const report = f.snapshot(), receive = ofKind(report, 1)[0], sends = ofKind(report, 2);
  yes(report.valid && report.complete);
  equal([read(receive, 'originalThrew'), read(sends[0], 'originalThrew')], [1, 1]);
  equal(read(sends[1], 'messageContextId'), 0);
  equal(report.sockets[0].messageThrows, 1); equal(report.sockets[1].sendThrows, 1);
});

test('nested throwing dispatch restores the outer synchronous context', () => {
  const f = fixture(), a = f.connect(0), b = f.connect(1), failure = new Error('nested');
  a.sendHook = () => { throw failure; };
  b.on('message', data => a.send(data));
  a.on('message', () => {
    throwsSame(() => b.emit('message', packet({ sender: 1, latest: 202 }), true), failure);
    equal(b.send(packet({ latest: 203 })), 'original-send');
  });
  a.emit('message', packet(), true);
  const report = f.snapshot(), messages = ofKind(report, 1), sends = ofKind(report, 2);
  equal(messages.length, 2); equal(sends.length, 2);
  equal(read(sends[0], 'messageContextId'), read(messages[1], 'eventId'));
  equal(read(sends[1], 'messageContextId'), read(messages[0], 'eventId'));
  equal([read(messages[0], 'originalThrew'), read(messages[1], 'originalThrew')], [0, 1]);
  yes(report.valid && report.complete);
});

test('later timer-style send has no leaked receive context', () => {
  const f = fixture(), a = f.connect(0), b = f.connect(1);
  let deferred;
  a.on('message', data => { deferred = () => b.send(data); });
  a.emit('message', packet(), true); f.advance(50); deferred();
  const report = f.snapshot(), send = ofKind(report, 2)[0];
  equal(read(send, 'messageContextId'), 0); equal(read(send, 'sourceSocketId'), 0);
  equal(report.sockets[1].pairedSends, 0); equal(read(send, 'startMs'), 150);
});

test('non-target sockets and signaling keep original behavior without packet rows', () => {
  const f = fixture(), outside = new f.Socket(), signal = f.connect(0, 2, '&signal=1');
  const invalid = f.connect(2, 2), directory = f.connect(0, 2, '&directory=1');
  const spectator = f.connect(0, 2, '&spectator=private_spectator');
  const largeUrl = new f.Socket();
  f.server.emit('connection', largeUrl, { url: 'x'.repeat(8193) });
  for (const socket of [outside, signal, invalid, directory, spectator, largeUrl]) {
    equal(socket.send(packet()), 'original-send'); equal(socket.emit('message', packet(), true), false);
  }
  const report = f.snapshot();
  equal(report.eventCount, 0); equal(report.sockets.length, 1);
  equal(report.sockets[0].role, 2); equal(report.ignoredConnections, 4);
  yes(report.valid && report.complete);
  yes(!JSON.stringify(report).includes('private_spectator'));
});

test('socket cap and duplicate connection observation are explicit', () => {
  const f = fixture({ maxSockets: 1 }), a = f.connect(0), b = f.connect(1);
  f.server.emit('connection', a, { url: '/?room=perf-th10-private_room&player=0&players=2' });
  same(a.send(packet()), 'original-send'); same(b.send(packet({ sender: 1 })), 'original-send');
  const report = f.snapshot();
  equal(report.sockets.length, 1); equal(report.droppedSockets, 1);
  equal(report.dropped, 1); equal(report.eventCount, 1);
  yes(report.valid); same(report.complete, false); equal(b.calls.length, 1);
});

test('event cap preserves forwarding and records incomplete evidence', () => {
  const f = fixture({ maxEvents: 3 }), a = f.connect(0), b = f.connect(1);
  a.on('message', data => b.send(data));
  a.emit('message', packet(), true); a.emit('message', packet({ latest: 102 }), true);
  const report = f.snapshot();
  equal(report.eventCount, 3); equal(report.eventAttempts, 4);
  equal(report.droppedEvents, 1); equal(report.dropped, 1);
  equal(a.listenerCount('message'), 1); equal(b.calls.length, 2);
  equal(report.sockets[0].messages, 2); equal(report.sockets[1].sendCalls, 2);
  yes(report.valid); same(report.complete, false);
  yes(report.events.every(row => read(row, 'completed') === 1));
});

test('500ms unref heartbeat has bounded storage and observable gaps', () => {
  const f = fixture({ maxHeartbeats: 2 });
  equal(f.jobs.length, 1); equal(f.jobs[0].ms, 500); equal(f.jobs[0].unrefs, 1);
  f.fire(500); f.fire(800); f.fire(500);
  const report = f.snapshot(), beats = ofKind(report, 3);
  equal(beats.map(row => read(row, 'heartbeatGapMs')), [500, 800]);
  equal(report.heartbeatCount, 2); equal(report.droppedHeartbeats, 1);
  equal(report.maxHeartbeatGapMs, 800); equal(report.dropped, 1);
  equal(f.jobs[0].active, false); yes(report.valid); same(report.complete, false);
});

test('snapshot IPC is end-only id-bound frozen and restores owned hooks', () => {
  const f = fixture(), a = f.connect(0);
  a.send(packet()); f.fire();
  equal(f.ipc.sent.length, 0); equal(f.ipc.listenerCount('message'), 1);
  f.ipc.emit('message', { type: 'other', requestId: 'x' });
  f.ipc.emit('message', { type: 'th10-relay-hop-snapshot', requestId: {} });
  equal(f.ipc.sent.length, 0); equal(f.jobs[0].active, true);
  f.ipc.emit('message', { type: 'th10-relay-hop-snapshot', requestId: 'request-42' });
  equal(f.ipc.sent.length, 1); equal(f.ipc.sent[0].requestId, 'request-42');
  equal(f.ipc.sent[0].type, 'th10-relay-hop-snapshot');
  const report = f.ipc.sent[0].report, count = report.eventCount;
  yes(report.valid && report.complete); same(f.snapshot(), report);
  equal(f.ipc.listenerCount('message'), 0); equal(f.jobs[0].active, false);
  same(f.Socket.prototype.send, f.originalSend);
  same(f.Socket.prototype.emit, f.originalSocketEmit); same(f.Server.prototype.emit, f.originalServerEmit);
  equal(Object.getOwnPropertyDescriptor(f.Socket.prototype, 'emit'), f.socketEmitDescriptor);
  equal(Object.getOwnPropertyDescriptor(f.Server.prototype, 'emit'), f.serverEmitDescriptor);
  a.send(packet({ latest: 999 })); f.jobs[0].fn();
  f.ipc.emit('message', { type: 'th10-relay-hop-snapshot', requestId: 'too-late' });
  equal(report.eventCount, count); equal(f.ipc.sent.length, 1);
  equal(report.clock.timeOrigin, 1700000000000);
  equal([report.clock.startedAtMs, report.clock.stoppedAtMs], [100, 600]);
});

test('observer read failure cannot change send return or swallow game errors', () => {
  const f = fixture(), a = f.connect(0);
  Object.defineProperty(a, 'bufferedAmount', { get() { throw new Error('diagnostic getter'); } });
  equal(a.send(packet()), 'original-send');
  const failure = new Error('still original');
  a.sendHook = () => { throw failure; };
  throwsSame(() => a.send(packet()), failure);
  const report = f.snapshot();
  same(report.valid, false); same(report.complete, false);
  yes(report.errors.length > 0); equal(a.calls.length, 2);
  yes(report.incompleteSpans > 0);
});

test('clock error and backwards time invalidate evidence without altering dispatch', () => {
  const f = fixture(), a = f.connect(0);
  f.setClockFailure(true); equal(a.send(packet()), 'original-send');
  f.setClockFailure(false); f.advance(-20); equal(a.send(packet()), 'original-send');
  const report = f.snapshot();
  same(report.valid, false); same(report.complete, false);
  yes(report.errors.includes('clock-read-failed')); yes(report.errors.includes('nonmonotonic-clock'));
  equal(a.calls.length, 2);
});

test('bounded v4 headers use view offsets and never serialize input samples', () => {
  const f = fixture(), a = f.connect(0), backing = Buffer.alloc(200, 0x7f);
  const valid = packet({ latest: 777, count: 2, target: 1 });
  Buffer.from('INPUT_SECRET_SENTINEL').copy(valid, 46);
  valid.copy(backing, 13);
  const view = new DataView(backing.buffer, backing.byteOffset + 13, valid.length);
  const cases = [Buffer.from([0xe7]), Buffer.alloc(27), packet(), packet(), packet(), packet(), view,
    packet({ latest: 0xffffffff, count: 0 })];
  cases[2][4] = 3; cases[3][5] = 2; cases[4][42] = 33; cases[5] = cases[5].subarray(0, 44);
  for (const data of cases) equal(a.send(data), 'original-send');
  const report = f.snapshot(), sends = ofKind(report, 2);
  equal(sends.slice(0, 6).map(row => read(row, 'eanpInputHeader')), [0, 0, 0, 0, 0, 0]);
  equal([read(sends[6], 'headerLatestFrame'), read(sends[6], 'headerSampleCount'), read(sends[6], 'targetedPeer')], [777, 2, 1]);
  equal([read(sends[7], 'headerLatestFrame'), read(sends[7], 'headerSampleCount')], [4294967295, 0]);
  yes(report.valid && report.complete); yes(!JSON.stringify(report).includes('INPUT_SECRET'));
  for (let i = 0; i < cases.length; ++i) same(a.calls[i].args[0], cases[i]);
});

test('partial hook failure restores earlier mutations and remains reportable', () => {
  class Socket extends EventEmitter { send() { return 'unchanged'; } }
  class Server extends EventEmitter {}
  const original = Socket.prototype.send;
  Object.defineProperty(Socket.prototype, 'send', { value: original, configurable: false, writable: false });
  const ipc = new EventEmitter(); ipc.send = () => {};
  const token = { unref() {} };
  const control = install({ WebSocket: Socket, WebSocketServer: Server, ipc,
    clock: { now: () => 1, timeOrigin: 1000 }, timers: { setInterval: () => token, clearInterval() {} }, maxEvents: 2 });
  equal(new Socket().send(), 'unchanged');
  equal(Object.getOwnPropertyDescriptor(Server.prototype, 'emit'), undefined);
  equal(Object.getOwnPropertyDescriptor(Socket.prototype, 'emit'), undefined);
  const report = control.snapshot();
  same(report.valid, false); same(report.complete, false);
  yes(report.errors.includes('ws-hook-installation-failed'));
});

test('snapshot never overwrites a later foreign hook', () => {
  const f = fixture(), observed = f.Socket.prototype.send;
  function foreign() { return Reflect.apply(observed, this, arguments); }
  f.Socket.prototype.send = foreign;
  const report = f.snapshot();
  same(f.Socket.prototype.send, foreign); same(report.valid, false);
  yes(report.errors.includes('prototype-ownership-lost'));
  equal(f.connect(0).send(packet()), 'original-send');
});

test('empty and invalid-short lobby parameters follow the real relay branches', () => {
  const f = fixture(), empty = f.connect(0, 2, '&lobby=&spectator=');
  const short = f.connect(1, 2, '&lobby=x'), lobby = f.connect(0, 2, '&lobby=private_lobby');
  const rejectedSpectator = f.connect(0, 2, '&spectator=0');
  for (const socket of [empty, short, lobby, rejectedSpectator]) equal(socket.send(packet()), 'original-send');
  const report = f.snapshot();
  equal(report.sockets.length, 2); equal(report.ignoredConnections, 2); equal(report.eventCount, 2);
  equal(report.sockets.map(socket => socket.role), [1, 1]); yes(report.valid && report.complete);
});

test('heartbeat and message spans share the same total event cap', () => {
  const f = fixture({ maxEvents: 1 }), a = f.connect(0);
  a.send(packet()); f.fire();
  const report = f.snapshot();
  equal(report.eventCount, 1); equal(report.eventAttempts, 2); equal(report.heartbeatCount, 1);
  equal(report.droppedEvents, 1); same(report.complete, false);
});

test('snapshot inside dispatch remains explicitly incomplete after the handler returns', () => {
  const f = fixture(), a = f.connect(0);
  let during;
  a.on('message', () => { during = f.snapshot(); });
  same(a.emit('message', packet(), true), true);
  same(f.snapshot(), during); same(during.valid, false); same(during.complete, false);
  yes(during.errors.includes('snapshot-during-message-dispatch'));
  equal(during.incompleteSpans, 1); equal(read(during.events[0], 'completed'), 0);
});

test('IPC send failure stays outside gameplay; parent must reject missing response', () => {
  const f = fixture(), a = f.connect(0);
  f.ipc.send = () => { throw new Error('IPC unavailable'); };
  f.ipc.emit('message', { type: 'th10-relay-hop-snapshot', requestId: 'delivery-fails' });
  equal(f.ipc.sent.length, 0); equal(f.jobs[0].active, false);
  equal(f.ipc.listenerCount('message'), 0); equal(a.send(packet()), 'original-send');
  // The already frozen report cannot attest its own subsequent IPC delivery.
  same(f.Socket.prototype.send, f.originalSend);
});

test('test overrides cannot raise production storage ceilings', () => {
  const f = fixture({ maxEvents: 32769, maxSockets: 17, maxHeartbeats: 2049 });
  const report = f.snapshot();
  equal(report.limits.events, 32768); equal(report.limits.sockets, 16);
  equal(report.limits.heartbeats, 2048); same(report.valid, false);
  equal(report.errors, ['invalid-event-limit', 'invalid-socket-limit', 'invalid-heartbeat-limit']);
});


function rawSocketFixture() {
  class RawSocket extends EventEmitter {
    constructor() { super(); this.paused = false; this.pauseReads = 0; this.pauseCalls = 0; this.resumeCalls = 0; }
    isPaused() { ++this.pauseReads; return this.paused; }
    pause() { ++this.pauseCalls; this.paused = true; return this; }
    resume() { ++this.resumeCalls; this.paused = false; return this; }
  }
  return new RawSocket();
}
const rawColumn = Object.fromEntries(RAW_DATA_COLUMNS.map((name, index) => [name, index]));
const readRaw = (row, name) => row[rawColumn[name]];

test('raw tracing OFF never reads the transport socket or changes its emit', () => {
  for (const rawSocketTrace of [undefined, false]) {
    const f = fixture({ rawSocketTrace }), a = new f.Socket(), raw = rawSocketFixture();
    let socketReads = 0;
    Object.defineProperty(a, '_socket', { get() { ++socketReads; throw new Error('OFF socket getter'); } });
    const emit = raw.emit, descriptor = Object.getOwnPropertyDescriptor(raw, 'emit');
    f.server.emit('connection', a, { url: '/?room=raw_off&player=0&players=2' });
    same(raw.emit('data', Buffer.from([1, 2, 3])), false);
    const report = f.snapshot();
    equal(socketReads, 0); equal(raw.pauseReads, 0); same(raw.emit, emit);
    equal(Object.getOwnPropertyDescriptor(raw, 'emit'), descriptor);
    same(report.rawSocketTrace, false); equal(ofKind(report, 4).length, 0);
    equal(report.sockets[0].rawDataEvents, null); equal(report.sockets[0].rawDataBytes, null);
    equal(report.sockets[0].lastRawSocketIsPaused, null);
    same(report.sockets[0].rawSocketObserved, false); yes(report.valid && report.complete);
  }
});

test('raw data wraps real EventEmitter decode-message-send nesting without changing handlers', () => {
  const f = fixture({ rawSocketTrace: true }), raw = rawSocketFixture(), otherRaw = rawSocketFixture();
  const originalEmit = raw.emit, descriptor = Object.getOwnPropertyDescriptor(raw, 'emit');
  const a = f.connect(0, 2, '', raw), b = f.connect(1, 2, '', otherRaw);
  const chunk = packet({ latest: 777, count: 2, target: 1 }), decoded = packet({ latest: 101 });
  const extra = { identity: true }, order = [];
  b.sendHook = () => { f.advance(3); return 'forwarded'; };
  a.on('message', function (data, binary) {
    same(this, a); same(data, decoded); same(binary, true); order.push('message');
    f.advance(2); equal(b.send(data), 'forwarded'); f.advance(1);
  });
  raw.on('data', function (data, token) {
    same(this, raw); same(data, chunk); same(token, extra); equal(arguments.length, 2);
    order.push('data-first'); f.advance(1); same(a.emit('message', decoded, true), true);
    raw.pause(); f.advance(2);
  });
  raw.on('data', () => order.push('data-last'));
  const before = raw.listeners('data'), messageListeners = a.listeners('message');
  same(raw.emit('data', chunk, extra), true);
  equal(order, ['data-first', 'message', 'data-last']);
  equal(raw.listeners('data'), before); equal(a.listeners('message'), messageListeners);
  equal(raw.pauseCalls, 1); equal(raw.resumeCalls, 0); equal(f.jobs.length, 1);
  const report = f.snapshot(), [data] = ofKind(report, 4), [message] = ofKind(report, 1), [send] = ofKind(report, 2);
  yes(report.valid && report.complete); same(report.rawSocketTrace, true);
  equal(report.eventSchema, COLUMNS); equal(report.rawDataEventSchema, RAW_DATA_COLUMNS);
  equal(COLUMNS.length, 25); equal(RAW_DATA_COLUMNS.length, 25);
  equal(RAW_DATA_COLUMNS.slice(0, 23), COLUMNS.slice(0, 23));
  equal(RAW_DATA_COLUMNS.slice(23), ['rawSocketPausedBefore', 'rawSocketPausedAfter']);
  equal(report.events.map(row => read(row, 'kind')), [4, 1, 2]);
  equal([read(data, 'startMs'), read(data, 'endMs')], [100, 109]);
  equal([read(message, 'startMs'), read(message, 'endMs')], [101, 107]);
  equal([read(send, 'startMs'), read(send, 'endMs')], [103, 106]);
  equal(read(send, 'messageContextId'), read(message, 'eventId'));
  equal(read(data, 'messageContextId'), 0);
  equal([readRaw(data, 'rawSocketPausedBefore'), readRaw(data, 'rawSocketPausedAfter')], [0, 1]);
  equal([read(data, 'bytes'), read(data, 'receiveIsBinary')], [chunk.length, -1]);
  equal([read(data, 'eanpInputHeader'), read(data, 'headerSender'), read(data, 'headerLatestFrame'),
    read(data, 'headerSampleCount'), read(data, 'targetedPeer')], [0, -1, -1, -1, -1]);
  equal([read(data, 'readyStateBefore'), read(data, 'readyStateAfter')], [1, 1]);
  const s = report.sockets[0];
  same(s.rawSocketObserved, true); equal(s.rawDataEvents, 1); equal(s.rawDataBytes, chunk.length);
  equal(s.lastRawDataMs, 100); equal(s.rawDataThrows, 0); equal(s.maxRawDataMs, 9);
  same(s.lastRawSocketIsPaused, true); equal(s.rawSocketReadErrors, 0);
  equal(report.sockets[1].rawDataEvents, 0); equal(report.sockets[1].rawDataBytes, 0);
  same(raw.emit, originalEmit); equal(Object.getOwnPropertyDescriptor(raw, 'emit'), descriptor);
  yes(report.events.every(row => row.length === 25 && row.every(Number.isFinite)));
  yes(!JSON.stringify(report).includes('rawSocket":'));
});

test('raw own emit descriptor false return and borrowed receiver are preserved', () => {
  const f = fixture({ rawSocketTrace: true }), raw = rawSocketFixture(), foreign = rawSocketFixture();
  const seen = [], original = function () {
    seen.push({ self: this, args: Array.from(arguments) });
    return Reflect.apply(EventEmitter.prototype.emit, this, arguments);
  };
  Object.defineProperty(raw, 'emit', { value: original, configurable: true, writable: true, enumerable: true });
  const descriptor = Object.getOwnPropertyDescriptor(raw, 'emit');
  f.connect(0, 2, '', raw);
  const observed = raw.emit, chunk = Buffer.from([9, 8]), token = {};
  same(raw.emit('data', chunk, token), false);
  same(Reflect.apply(observed, foreign, ['data', chunk, token]), false);
  same(raw.emit('other', chunk), false);
  const report = f.snapshot();
  equal(seen.length, 3); same(seen[0].self, raw); same(seen[1].self, foreign);
  same(seen[0].args[1], chunk); same(seen[0].args[2], token); equal(seen[0].args.length, 3);
  equal(ofKind(report, 4).length, 1); equal(report.sockets[0].rawDataEvents, 1);
  equal(foreign.pauseReads, 0); same(raw.emit, original);
  equal(Object.getOwnPropertyDescriptor(raw, 'emit'), descriptor); yes(report.valid && report.complete);
});

test('raw tracing ignores signaling rejected and unobserved sockets', () => {
  const f = fixture({ rawSocketTrace: true });
  const signalRaw = rawSocketFixture(), rejectedRaw = rawSocketFixture(), outsideRaw = rawSocketFixture();
  const originals = [signalRaw.emit, rejectedRaw.emit, outsideRaw.emit];
  f.connect(0, 2, '&signal=1', signalRaw); f.connect(2, 2, '', rejectedRaw);
  for (const [i, raw] of [signalRaw, rejectedRaw, outsideRaw].entries()) {
    same(raw.emit, originals[i]); same(raw.emit('data', packet()), false);
    equal(raw.pauseReads, 0);
  }
  const report = f.snapshot();
  equal(report.eventCount, 0); equal(report.sockets.length, 1);
  same(report.sockets[0].rawSocketObserved, false); equal(report.sockets[0].rawDataEvents, null);
  yes(report.valid && report.complete);
});

test('nested raw throwing dispatch restores the original message context', () => {
  const f = fixture({ rawSocketTrace: true }), rawA = rawSocketFixture(), rawB = rawSocketFixture();
  const a = f.connect(0, 2, '', rawA), b = f.connect(1, 2, '', rawB), failure = new Error('nested raw original');
  const chunkA = Buffer.from([1]), chunkB = Buffer.from([2]);
  a.sendHook = () => { f.advance(2); throw failure; };
  rawB.on('data', () => b.emit('message', packet({ sender: 1, latest: 202 }), true));
  b.on('message', data => a.send(data));
  rawA.on('data', () => a.emit('message', packet(), true));
  a.on('message', () => {
    throwsSame(() => rawB.emit('data', chunkB), failure);
    equal(b.send(packet({ latest: 203 })), 'original-send');
  });
  same(rawA.emit('data', chunkA), true);
  equal(b.send(packet({ latest: 204 })), 'original-send');
  const report = f.snapshot(), rawRows = ofKind(report, 4), messages = ofKind(report, 1), sends = ofKind(report, 2);
  yes(report.valid && report.complete); equal(rawRows.length, 2);
  equal([read(rawRows[0], 'originalThrew'), read(rawRows[1], 'originalThrew')], [0, 1]);
  equal(read(sends[0], 'messageContextId'), read(messages[1], 'eventId'));
  equal(read(sends[1], 'messageContextId'), read(messages[0], 'eventId'));
  equal(read(sends[2], 'messageContextId'), 0);
  equal(report.sockets.map(s => s.rawDataThrows), [0, 1]);
  yes(report.events.every(row => read(row, 'completed') === 1));
});

test('raw listener exception is rethrown unchanged with a completed raw span', () => {
  const f = fixture({ rawSocketTrace: true }), raw = rawSocketFixture(), failure = new Error('original raw listener');
  const a = f.connect(0, 2, '', raw);
  raw.on('data', () => { f.advance(4); throw failure; });
  throwsSame(() => raw.emit('data', Buffer.from([1, 2])), failure);
  equal(a.send(packet()), 'original-send');
  const report = f.snapshot(), [data] = ofKind(report, 4), [send] = ofKind(report, 2);
  equal([read(data, 'originalThrew'), read(data, 'completed')], [1, 1]);
  equal(read(send, 'messageContextId'), 0); equal(report.sockets[0].rawDataThrows, 1);
  equal(report.sockets[0].maxRawDataMs, 4); yes(report.valid && report.complete);
});

test('unavailable raw sockets or locked emit fail evidence without changing forwarding', () => {
  for (const locked of [false, true]) {
    const f = fixture({ rawSocketTrace: true }), raw = locked ? rawSocketFixture() : undefined;
    let emit;
    if (raw) {
      emit = raw.emit;
      Object.defineProperty(raw, 'emit', { value: emit, configurable: false, writable: false });
    }
    const a = f.connect(0, 2, '', raw);
    equal(a.send(packet()), 'original-send');
    if (raw) { same(raw.emit, emit); same(raw.emit('data', Buffer.from([1])), false); }
    const report = f.snapshot();
    same(report.valid, false); same(report.complete, false);
    yes(report.errors.includes(locked ? 'raw-socket-hook-installation-failed' : 'raw-socket-unavailable'));
    same(report.sockets[0].rawSocketObserved, false);
    equal(report.sockets[0].rawDataEvents, null); equal(report.sockets[0].rawDataBytes, null);
    equal(report.sockets[0].lastRawSocketIsPaused, null);
    same(f.Socket.prototype.send, f.originalSend);
  }
});

test('raw pause getter failures stay unavailable and cannot change data dispatch', () => {
  for (const bad of ['throws', 'nonboolean']) {
    const f = fixture({ rawSocketTrace: true }), raw = rawSocketFixture();
    f.connect(0, 2, '', raw); let calls = 0;
    raw.on('data', () => { ++calls; });
    raw.isPaused = () => { if (bad === 'throws') throw new Error('raw pause failed'); return 1; };
    same(raw.emit('data', Buffer.from([3, 4, 5])), true);
    const report = f.snapshot(), [data] = ofKind(report, 4);
    same(report.valid, false); same(report.complete, false); equal(calls, 1);
    equal([readRaw(data, 'rawSocketPausedBefore'), readRaw(data, 'rawSocketPausedAfter')], [-1, -1]);
    equal(read(data, 'completed'), 1); equal(report.sockets[0].rawDataEvents, 1);
    equal(report.sockets[0].rawDataBytes, 3); equal(report.sockets[0].lastRawSocketIsPaused, null);
    yes(report.sockets[0].rawSocketReadErrors >= 3); yes(report.errors.includes('raw-socket-state-read-failed'));
    equal(raw.pauseCalls, 0); equal(raw.resumeCalls, 0);
  }
});

test('raw byte lengths are intrinsic and unknown values never become zero bytes', () => {
  const f = fixture({ rawSocketTrace: true }), raw = rawSocketFixture();
  f.connect(0, 2, '', raw); const received = [];
  raw.on('data', data => received.push(data));
  const backing = new ArrayBuffer(20), view = new DataView(backing, 3, 7);
  let getterReads = 0;
  Object.defineProperty(view, 'byteLength', { get() { ++getterReads; throw new Error('view getter'); } });
  const opaque = { get byteLength() { ++getterReads; throw new Error('opaque getter'); },
    [Symbol.toPrimitive]() { throw new Error('must not coerce raw payload'); } };
  same(raw.emit('data', view), true); same(raw.emit('data', opaque), true);
  same(raw.emit('data', Buffer.alloc(2)), true);
  const report = f.snapshot(), dataRows = ofKind(report, 4);
  equal(getterReads, 0); same(received[0], view); same(received[1], opaque);
  equal(dataRows.map(row => read(row, 'bytes')), [7, -1, 2]);
  equal(report.sockets[0].rawDataEvents, 3); equal(report.sockets[0].rawDataBytes, null);
  same(report.valid, false); same(report.complete, false);
  yes(report.errors.includes('raw-data-byte-length-unavailable'));
  yes(dataRows.every(row => read(row, 'eanpInputHeader') === 0 && read(row, 'completed') === 1));
});

test('raw spans messages sends and heartbeat share the original event ceiling', () => {
  const f = fixture({ rawSocketTrace: true, maxEvents: 3 }), rawA = rawSocketFixture(), rawB = rawSocketFixture();
  const a = f.connect(0, 2, '', rawA), b = f.connect(1, 2, '', rawB), chunk = Buffer.from([7, 8, 9]);
  rawA.on('data', () => a.emit('message', packet(), true)); a.on('message', data => b.send(data));
  same(rawA.emit('data', chunk), true); same(rawA.emit('data', chunk), true); f.fire();
  const report = f.snapshot();
  equal(report.eventCount, 3); equal(report.eventAttempts, 7); equal(report.droppedEvents, 4);
  equal(report.events.map(row => read(row, 'kind')), [4, 1, 2]);
  equal(report.sockets[0].rawDataEvents, 2); equal(report.sockets[0].rawDataBytes, 6);
  equal(report.sockets[0].messages, 2); equal(report.sockets[1].sendCalls, 2);
  equal(b.calls.length, 2); equal(report.heartbeatCount, 1);
  equal(f.jobs.length, 1); yes(report.valid); same(report.complete, false);
  yes(report.events.every(row => read(row, 'completed') === 1));
});

test('raw snapshot restores owned hooks and never overwrites a later owner', () => {
  const f = fixture({ rawSocketTrace: true }), raw = rawSocketFixture(), original = raw.emit;
  f.connect(0, 2, '', raw); const observed = raw.emit;
  function foreign() { return Reflect.apply(observed, this, arguments); }
  raw.emit = foreign;
  const report = f.snapshot(), count = report.eventCount;
  same(raw.emit, foreign); same(report.valid, false);
  yes(report.errors.includes('raw-socket-hook-ownership-lost'));
  same(raw.emit('data', Buffer.from([1])), false); equal(report.eventCount, count);
  same(f.Socket.prototype.emit, f.originalSocketEmit); same(f.Socket.prototype.send, f.originalSend);
  same(original, EventEmitter.prototype.emit);
});

test('snapshot inside raw data is explicitly incomplete and cannot become a later pass', () => {
  const f = fixture({ rawSocketTrace: true }), raw = rawSocketFixture(), original = raw.emit;
  f.connect(0, 2, '', raw); let during;
  raw.on('data', () => { during = f.snapshot(); });
  same(raw.emit('data', Buffer.from([1])), true);
  same(f.snapshot(), during); same(during.valid, false); same(during.complete, false);
  yes(during.errors.includes('snapshot-during-raw-data-dispatch'));
  equal(during.incompleteSpans, 1); equal(read(during.events[0], 'completed'), 0);
  same(raw.emit, original); equal(f.jobs[0].active, false);
});

test('replacing the current raw socket invalidates the old hook without observing stale data', () => {
  const f = fixture({ rawSocketTrace: true }), raw = rawSocketFixture(), replacement = rawSocketFixture();
  const a = f.connect(0, 2, '', raw); let received = 0;
  raw.on('data', () => { ++received; }); a._socket = replacement;
  same(raw.emit('data', Buffer.from([1])), true);
  const report = f.snapshot();
  equal(received, 1); equal(report.sockets[0].rawDataEvents, 0); equal(ofKind(report, 4).length, 0);
  same(report.valid, false); yes(report.errors.includes('raw-socket-identity-changed'));
  equal(report.sockets[0].lastRawSocketIsPaused, null); equal(replacement.pauseReads, 0);
});

test('raw option requires an explicit boolean and cannot silently enable hooks', () => {
  const f = fixture({ rawSocketTrace: 1 }), raw = rawSocketFixture();
  f.connect(0, 2, '', raw);
  const report = f.snapshot();
  same(report.rawSocketTrace, false); same(report.valid, false);
  yes(report.errors.includes('invalid-raw-socket-trace')); equal(raw.pauseReads, 0);
  same(report.sockets[0].rawSocketObserved, false); equal(report.sockets[0].rawDataEvents, null);
});

console.log(JSON.stringify({ schema: 'th10-relay-hop-observer-test/1', passed: true, groups, checks }));
