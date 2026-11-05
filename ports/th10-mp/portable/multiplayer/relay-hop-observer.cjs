'use strict';

// Diagnostic preload for the owned local relay only. No packet copies, logs,
// files, hot-window IPC, send callbacks, or changes to relay message handlers.
const { performance } = require('node:perf_hooks');
const SCHEMA = 'th10-relay-server-hop/1';
const MAX_EVENTS = 32768, MAX_SOCKETS = 16, MAX_HEARTBEATS = 2048;
const HEARTBEAT_MS = 500, MAX_ERRORS = 16;
const MARK = Symbol.for('th10.relay-hop-observer');
const COLUMNS = Object.freeze([
  'kind', 'eventId', 'socketId', 'role', 'seat', 'messageContextId',
  'sourceSocketId', 'sourceSeat', 'startMs', 'endMs', 'receiveIsBinary',
  'bytes', 'readyStateBefore', 'readyStateAfter', 'bufferedBefore',
  'bufferedAfter', 'eanpInputHeader', 'headerSender', 'headerLatestFrame',
  'headerSampleCount', 'targetedPeer', 'originalThrew', 'completed',
  'synchronousSendCalls', 'heartbeatGapMs'
]);
// Kind 4 retains the same width. Its last two columns are raw socket pause
// observations, not message send counts or heartbeat gaps. Legacy kinds keep
// COLUMNS exactly as before; consumers must select the schema by event kind.
const RAW_DATA_COLUMNS = Object.freeze(COLUMNS.map((name, index) =>
  index === 23 ? 'rawSocketPausedBefore' : index === 24 ? 'rawSocketPausedAfter' : name));
const N = COLUMNS.length;
const typed = Object.getPrototypeOf(Uint8Array.prototype);
const typedGet = name => Object.getOwnPropertyDescriptor(typed, name).get;
const viewLength = typedGet('byteLength'), viewOffset = typedGet('byteOffset');
const viewBuffer = typedGet('buffer');
const dataViewLength = Object.getOwnPropertyDescriptor(DataView.prototype, 'byteLength').get;
const dataViewOffset = Object.getOwnPropertyDescriptor(DataView.prototype, 'byteOffset').get;
const dataViewBuffer = Object.getOwnPropertyDescriptor(DataView.prototype, 'buffer').get;
const arrayBufferLength = Object.getOwnPropertyDescriptor(ArrayBuffer.prototype, 'byteLength').get;

function packetMetadata(value) {
  let bytes = null, size = -1;
  if (typeof value === 'string') size = Buffer.byteLength(value);
  else if (ArrayBuffer.isView(value)) {
    const dataView = value instanceof DataView;
    size = (dataView ? dataViewLength : viewLength).call(value);
    const offset = (dataView ? dataViewOffset : viewOffset).call(value);
    const buffer = (dataView ? dataViewBuffer : viewBuffer).call(value);
    bytes = new Uint8Array(buffer, offset, size);
  } else if (value instanceof ArrayBuffer) {
    size = arrayBufferLength.call(value);
    bytes = new Uint8Array(value);
  }
  const result = { size, input: 0, sender: -1, latest: -1, count: -1, target: -1 };
  if (!bytes) return result;
  const offset = size >= 2 && bytes[0] === 0xe7 ? 2 : 0;
  if (offset) result.target = bytes[1];
  // Read only the v4 EANP Input header. The 12-byte input samples stay opaque.
  if (size < offset + 44 || bytes[offset] !== 0x45 || bytes[offset + 1] !== 0x41 ||
      bytes[offset + 2] !== 0x4e || bytes[offset + 3] !== 0x50 ||
      bytes[offset + 4] !== 4 || bytes[offset + 5] !== 1 ||
      bytes[offset + 7] < 2 || bytes[offset + 7] > 3 ||
      bytes[offset + 6] >= bytes[offset + 7] || bytes[offset + 43] !== 0) return result;
  const count = bytes[offset + 42];
  if (count > 32 || size !== offset + 44 + 12 * count) return result;
  result.input = 1; result.sender = bytes[offset + 6]; result.count = count;
  result.latest = (bytes[offset + 24] | bytes[offset + 25] << 8 |
    bytes[offset + 26] << 16 | bytes[offset + 27] << 24) >>> 0;
  return result;
}

function rawByteLength(value) {
  // Raw TCP chunks are opaque. Only intrinsic byte lengths are read: no view,
  // header decoding, copying, string scan, or user-provided byteLength getter.
  if (ArrayBuffer.isView(value))
    return (value instanceof DataView ? dataViewLength : viewLength).call(value);
  if (value instanceof ArrayBuffer) return arrayBufferLength.call(value);
  return null;
}

function connectionRole(request) {
  // Parse once; never retain the request, URL, room name, or member identity.
  if (typeof request?.url !== 'string' || request.url.length > 8192) return null;
  const q = new URL(request.url, 'http://relay.invalid').searchParams;
  if (q.get('directory') === '1' || q.get('diagnostic') === '1') return null;
  // Match the relay's branch conditions: empty role parameters remain gameplay.
  const lobby = q.get('lobby') || '', spectator = q.get('spectator') || '';
  if (/^[A-Za-z0-9_-]{8,64}$/.test(lobby) || spectator) return null;
  const room = q.get('room') || '', run = q.get('run') || '0';
  const countText = q.get('players') || '2', seatText = q.get('player') || '';
  if (!/^[A-Za-z0-9_-]{1,64}$/.test(room) || !/^[A-Za-z0-9_-]{1,64}$/.test(run) ||
      !/^[23]$/.test(countText) || !/^[0-2]$/.test(seatText)) return null;
  const seat = Number(seatText), count = Number(countText);
  if (seat >= count) return null;
  return { role: q.get('signal') === '1' ? 2 : 1, seat, players: count };
}

function install(options = {}) {
  const ipc = options.ipc === undefined ? process : options.ipc;
  const clock = options.clock || { now: () => performance.now(), timeOrigin: performance.timeOrigin };
  const timers = options.timers || { setInterval, clearInterval };
  const errors = [], restores = [], sockets = [], states = new WeakMap();
  let errorCount = 0, droppedErrors = 0, droppedEvents = 0, droppedSockets = 0;
  let droppedHeartbeats = 0, ignoredConnections = 0, active = true, frozen = null;
  let timer = null, ipcAttached = false, context = null, lastNow = 0, rawDispatchDepth = 0;
  let eventCount = 0, eventAttempts = 0, heartbeatCount = 0, maxHeartbeatGapMs = 0;
  let installed = false, stoppedAtMs = null, lastHeartbeatMs = 0;

  function fault(code) {
    ++errorCount;
    if (errors.length < MAX_ERRORS) errors.push(code);
    else ++droppedErrors;
  }
  function limit(value, maximum, code) {
    if (value === undefined) return maximum;
    if (Number.isInteger(value) && value > 0 && value <= maximum) return value;
    fault(code); return maximum;
  }
  const rawSocketTrace = options.rawSocketTrace === true;
  if (options.rawSocketTrace !== undefined && typeof options.rawSocketTrace !== 'boolean')
    fault('invalid-raw-socket-trace');
  const eventLimit = limit(options.maxEvents, MAX_EVENTS, 'invalid-event-limit');
  const socketLimit = limit(options.maxSockets, MAX_SOCKETS, 'invalid-socket-limit');
  const heartbeatLimit = limit(options.maxHeartbeats, MAX_HEARTBEATS, 'invalid-heartbeat-limit');
  let rows;
  try { rows = new Float64Array(eventLimit * N); }
  catch { fault('event-storage-allocation-failed'); rows = new Float64Array(0); }
  function now() {
    try {
      const value = clock.now();
      if (!Number.isFinite(value) || value < lastNow) { fault('nonmonotonic-clock'); return lastNow; }
      lastNow = value; return value;
    } catch { fault('clock-read-failed'); return lastNow; }
  }
  let timeOrigin = null;
  try {
    if (Number.isFinite(clock.timeOrigin)) timeOrigin = clock.timeOrigin;
    else fault('missing-clock-time-origin');
  } catch { fault('clock-time-origin-failed'); }
  const startedAtMs = now();
  lastHeartbeatMs = startedAtMs;
  if (options.initialError) fault('preload-resolution-failed');

  function stateValues(socket) {
    const ready = socket.readyState, buffered = socket.bufferedAmount;
    if (!Number.isInteger(ready) || ready < 0 || ready > 3 ||
        !Number.isFinite(buffered) || buffered < 0) throw new Error('socket-state');
    return [ready, buffered];
  }
  function row(kind, state, meta, at, owner, binary) {
    const id = ++eventAttempts;
    if (eventCount >= eventLimit || rows.length === 0) { ++droppedEvents; return { id, offset: -1 }; }
    const offset = eventCount++ * N;
    rows.fill(-1, offset, offset + N);
    rows[offset] = kind; rows[offset + 1] = id;
    rows[offset + 2] = state?.id || 0; rows[offset + 3] = state?.role || 0;
    rows[offset + 4] = state?.seat ?? -1;
    rows[offset + 5] = owner?.id || 0;
    rows[offset + 6] = owner?.socketId || 0;
    rows[offset + 7] = owner?.seat ?? -1;
    rows[offset + 8] = at; rows[offset + 10] = binary;
    rows[offset + 11] = meta?.size ?? 0;
    rows[offset + 16] = meta?.input || 0; rows[offset + 17] = meta?.sender ?? -1;
    rows[offset + 18] = meta?.latest ?? -1; rows[offset + 19] = meta?.count ?? -1;
    rows[offset + 20] = meta?.target ?? -1;
    rows[offset + 21] = 0; rows[offset + 22] = 0; rows[offset + 23] = kind === 4 ? -1 : 0;
    if (state) {
      const values = stateValues(state.socket);
      rows[offset + 12] = values[0]; rows[offset + 14] = values[1];
    }
    return { id, offset };
  }
  function complete(record, state, end, threw, sendCalls = 0) {
    if (!record || record.offset < 0) return;
    const offset = record.offset;
    rows[offset + 9] = end; rows[offset + 21] = threw ? 1 : 0;
    if (rows[offset] !== 4) rows[offset + 23] = sendCalls;
    if (state) {
      const values = stateValues(state.socket);
      rows[offset + 13] = values[0]; rows[offset + 15] = values[1];
    }
    rows[offset + 22] = 1;
  }
  function observeConnection(socket, request) {
    if (states.has(socket)) return;
    const role = connectionRole(request);
    if (!role) { ++ignoredConnections; return; }
    if (sockets.length >= socketLimit) { ++droppedSockets; return; }
    const state = { socket, id: sockets.length + 1, ...role, openedAtMs: now(), closedAtMs: null,
      messages: 0, binaryMessages: 0, inputMessages: 0, receivedBytes: 0, lastReceiveMs: null,
      lastInputReceivedFrame: null, messageThrows: 0, maxMessageMs: 0,
      sendCalls: 0, inputSends: 0, sentArgumentBytes: 0, lastSendMs: null,
      lastInputSentFrame: null, sendThrows: 0, maxSendMs: 0, pairedSends: 0,
      maxReceiveToSendMs: 0, lastReadyState: null, lastBufferedAmount: null,
      rawSocket: null, rawSocketObserved: false, rawDataEvents: null, rawDataBytes: null,
      lastRawDataMs: null, rawDataThrows: null, maxRawDataMs: null,
      lastRawSocketIsPaused: null, rawSocketReadErrors: rawSocketTrace && role.role === 1 ? 0 : null };
    states.set(socket, state); sockets.push(state);
    if (rawSocketTrace && state.role === 1) observeRawSocket(state);
  }
  function patch(proto, key, wrapper, ownershipError = 'prototype-ownership-lost') {
    const descriptor = Object.getOwnPropertyDescriptor(proto, key);
    if (proto[key]?.[MARK]) throw new Error('already-installed');
    Object.defineProperty(wrapper, MARK, { value: true });
    Object.defineProperty(proto, key, { value: wrapper, configurable: true,
      enumerable: descriptor?.enumerable ?? false, writable: true });
    restores.push(() => {
      if (proto[key] !== wrapper) { fault(ownershipError); return; }
      if (descriptor) Object.defineProperty(proto, key, descriptor);
      else delete proto[key];
    });
  }
  function rawFault(state, code) {
    ++state.rawSocketReadErrors; state.lastRawSocketIsPaused = null; fault(code);
  }
  function rawPaused(state) {
    try {
      const raw = state.rawSocket;
      if (state.socket._socket !== raw || typeof raw?.isPaused !== 'function')
        throw new Error('raw-socket-state');
      const paused = Reflect.apply(raw.isPaused, raw, []);
      if (typeof paused !== 'boolean') throw new Error('raw-socket-paused');
      state.lastRawSocketIsPaused = paused;
      return paused ? 1 : 0;
    } catch { rawFault(state, 'raw-socket-state-read-failed'); return -1; }
  }
  function observeRawSocket(state) {
    let raw, emitRaw, installationStage = false;
    try {
      raw = state.socket._socket;
      if (!raw || typeof raw.emit !== 'function') throw new Error('raw-socket');
      emitRaw = raw.emit; state.rawSocket = raw; installationStage = true;
      patch(raw, 'emit', function () {
        // This is an own-property hook on this one observed gameplay socket.
        // Never change listeners, pause/resume, write, or the Receiver itself.
        if (!active || this !== raw || arguments[0] !== 'data')
          return Reflect.apply(emitRaw, this, arguments);
        let current = false;
        try { current = state.socket._socket === raw; }
        catch { rawFault(state, 'raw-socket-identity-read-failed'); return Reflect.apply(emitRaw, this, arguments); }
        if (!current) {
          rawFault(state, 'raw-socket-identity-changed');
          return Reflect.apply(emitRaw, this, arguments);
        }
        let record = null, start = lastNow, threw = false;
        ++rawDispatchDepth;
        try {
          try {
            start = now(); ++state.rawDataEvents; state.lastRawDataMs = start;
            let size = null;
            try { size = rawByteLength(arguments[1]); }
            catch { /* Unavailable length is recorded below; dispatch still proceeds. */ }
            if (!Number.isSafeInteger(size) || size < 0) {
              size = -1; state.rawDataBytes = null;
              rawFault(state, 'raw-data-byte-length-unavailable');
            } else if (state.rawDataBytes !== null) state.rawDataBytes += size;
            record = row(4, state, { size }, start, context, -1);
            const paused = rawPaused(state);
            if (record.offset >= 0) rows[record.offset + 23] = paused;
          } catch { rawFault(state, 'raw-data-entry-observation-failed'); }
          try { return Reflect.apply(emitRaw, this, arguments); }
          catch (error) { threw = true; throw error; }
        } finally {
          --rawDispatchDepth;
          try {
            const end = now(); state.rawDataThrows += threw ? 1 : 0;
            state.maxRawDataMs = Math.max(state.maxRawDataMs, end - start);
            const paused = rawPaused(state);
            if (record && record.offset >= 0) rows[record.offset + 24] = paused;
            complete(record, state, end, threw);
          } catch { rawFault(state, 'raw-data-exit-observation-failed'); }
        }
      }, 'raw-socket-hook-ownership-lost');
      state.rawSocketObserved = true;
      state.rawDataEvents = 0; state.rawDataBytes = 0;
      state.rawDataThrows = 0; state.maxRawDataMs = 0;
      rawPaused(state);
    } catch {
      rawFault(state, installationStage ? 'raw-socket-hook-installation-failed' : 'raw-socket-unavailable');
    }
  }
  try {
    const wp = options.WebSocket?.prototype, sp = options.WebSocketServer?.prototype;
    if (!wp || !sp || typeof wp.send !== 'function' ||
        typeof wp.emit !== 'function' || typeof sp.emit !== 'function') throw new Error('ws-api');
    const emitServer = sp.emit, emitSocket = wp.emit, send = wp.send;
    patch(sp, 'emit', function () {
      if (active && arguments[0] === 'connection') {
        try { observeConnection(arguments[1], arguments[2]); } catch { fault('connection-observation-failed'); }
      }
      return Reflect.apply(emitServer, this, arguments);
    });
    patch(wp, 'emit', function () {
      const state = active ? states.get(this) : null;
      if (state && arguments[0] === 'close') {
        try { state.closedAtMs = now(); } catch { fault('close-observation-failed'); }
      }
      if (!state || state.role !== 1 || arguments[0] !== 'message')
        return Reflect.apply(emitSocket, this, arguments);
      const previous = context;
      let record = null, owner = null, start = lastNow, threw = false;
      try {
        start = now();
        const meta = packetMetadata(arguments[1]);
        ++state.messages; state.binaryMessages += arguments[2] === true ? 1 : 0;
        state.inputMessages += meta.input; state.receivedBytes += Math.max(0, meta.size);
        state.lastReceiveMs = start;
        if (meta.input) state.lastInputReceivedFrame = meta.latest;
        record = row(1, state, meta, start, null, arguments[2] === true ? 1 : 0);
        owner = { id: record.id, socketId: state.id, seat: state.seat, at: start, sends: 0 };
        if (record.offset >= 0) {
          rows[record.offset + 5] = owner.id;
          rows[record.offset + 6] = state.id; rows[record.offset + 7] = state.seat;
        }
      } catch { fault('message-entry-observation-failed'); }
      context = owner;
      try { return Reflect.apply(emitSocket, this, arguments); }
      catch (error) { threw = true; throw error; }
      finally {
        context = previous;
        try {
          const end = now(); state.messageThrows += threw ? 1 : 0;
          state.maxMessageMs = Math.max(state.maxMessageMs, end - start);
          complete(record, state, end, threw, owner?.sends || 0);
        } catch { fault('message-exit-observation-failed'); }
      }
    });
    patch(wp, 'send', function () {
      const state = active ? states.get(this) : null;
      if (!state || state.role !== 1) return Reflect.apply(send, this, arguments);
      const owner = context;
      let record = null, start = lastNow, threw = false;
      try {
        start = now();
        // Do not read options or add/wrap callbacks. Send argument identity is unchanged.
        const meta = packetMetadata(arguments[0]);
        ++state.sendCalls; state.inputSends += meta.input;
        state.sentArgumentBytes += Math.max(0, meta.size); state.lastSendMs = start;
        if (meta.input) state.lastInputSentFrame = meta.latest;
        if (owner) {
          ++owner.sends; ++state.pairedSends;
          state.maxReceiveToSendMs = Math.max(state.maxReceiveToSendMs, start - owner.at);
        }
        record = row(2, state, meta, start, owner, -1);
      } catch { fault('send-entry-observation-failed'); }
      try { return Reflect.apply(send, this, arguments); }
      catch (error) { threw = true; throw error; }
      finally {
        try {
          const end = now(); state.sendThrows += threw ? 1 : 0;
          state.maxSendMs = Math.max(state.maxSendMs, end - start);
          complete(record, state, end, threw);
        } catch { fault('send-exit-observation-failed'); }
      }
    });
    installed = true;
  } catch {
    fault('ws-hook-installation-failed');
    for (const restore of restores.splice(0).reverse()) { try { restore(); } catch { fault('hook-restore-failed'); } }
  }

  function heartbeat() {
    if (!active) return;
    try {
      const at = now(), gap = at - lastHeartbeatMs;
      lastHeartbeatMs = at; maxHeartbeatGapMs = Math.max(maxHeartbeatGapMs, gap);
      if (heartbeatCount >= heartbeatLimit) { ++droppedHeartbeats; return; }
      ++heartbeatCount;
      const record = row(3, null, null, at, null, -1);
      complete(record, null, at, false);
      if (record.offset >= 0) rows[record.offset + 24] = gap;
    } catch { fault('heartbeat-observation-failed'); }
  }
  try {
    timer = timers.setInterval(heartbeat, HEARTBEAT_MS);
    if (typeof timer?.unref !== 'function') fault('heartbeat-unref-unavailable');
    else timer.unref();
  } catch { fault('heartbeat-installation-failed'); }

  function snapshot() {
    if (frozen) return frozen;
    stoppedAtMs = now(); active = false;
    if (context) fault('snapshot-during-message-dispatch');
    if (rawDispatchDepth) fault('snapshot-during-raw-data-dispatch');
    try { if (timer !== null) timers.clearInterval(timer); } catch { fault('heartbeat-clear-failed'); }
    timer = null;
    if (ipcAttached) {
      try { ipc.removeListener('message', onMessage); } catch { fault('ipc-listener-remove-failed'); }
      ipcAttached = false;
    }
    for (const restore of restores.splice(0).reverse()) { try { restore(); } catch { fault('hook-restore-failed'); } }
    const socketRows = sockets.map(state => {
      try {
        const values = stateValues(state.socket);
        state.lastReadyState = values[0]; state.lastBufferedAmount = values[1];
      } catch { fault('snapshot-socket-state-failed'); }
      if (rawSocketTrace && state.role === 1 && state.rawSocketObserved) rawPaused(state);
      const { socket, rawSocket, ...plain } = state;
      return plain;
    });
    const events = [];
    let incompleteSpans = 0;
    for (let i = 0; i < eventCount; ++i) {
      const record = Array.from(rows.subarray(i * N, (i + 1) * N));
      if (record[22] !== 1) ++incompleteSpans;
      events.push(record);
    }
    const dropped = droppedEvents + droppedSockets + droppedHeartbeats + droppedErrors;
    frozen = {
      schema: SCHEMA, enabled: true, valid: errorCount === 0 && installed,
      complete: errorCount === 0 && installed && dropped === 0 && incompleteSpans === 0,
      errors: errors.slice(), errorCount, dropped, droppedEvents, droppedSockets,
      droppedHeartbeats, droppedErrors, incompleteSpans,
      clock: { scope: 'one Node relay process performance.now milliseconds', timeOrigin,
        startedAtMs, stoppedAtMs, epochScope: 'timeOrigin + relative sample; compare with each browser own origin, not raw performance.now' },
      identity: options.identity || null,
      limits: { events: eventLimit, sockets: socketLimit, heartbeats: heartbeatLimit, heartbeatMs: HEARTBEAT_MS },
      scope: 'diagnostic only; message dispatch, ws.send calls and optional raw socket data dispatch, not kernel delivery, remote receipt, thread CPU, or pure performance',
      contextScope: 'synchronous message dispatch only; later timer sends have context 0; a context is not a wire packet identity',
      packetScope: 'byte count and bounded v4 EANP Input header only; no input samples, strings, URLs, or payload retention',
      rawSocketTrace, rawDataEventSchema: RAW_DATA_COLUMNS,
      rawDataScope: 'kind 4 is current gameplay ws._socket.emit(data) entry-to-exit; opaque chunk byte length only; no raw header or payload read; not kernel-arrival time',
      rawPauseScope: 'kind 4 columns 23/24 are rawSocketPausedBefore/After: 0=false, 1=true, -1=unavailable; socket summary lastRawSocketIsPaused is the final actual isPaused read; no heartbeat scan',
      rawUnavailableScope: 'disabled or unobserved raw counters are null; an unavailable chunk length makes rawDataBytes null and evidence invalid; raw state read failures are explicit',
      eventSchema: COLUMNS, eventKinds: { 1: 'message-entry-to-exit', 2: 'ws-send-before-to-after', 3: 'heartbeat', 4: 'raw-socket-data-entry-to-exit' },
      roleSchema: { 0: 'none', 1: 'gameplay', 2: 'signaling-metadata-only' },
      fieldScope: 'select rawDataEventSchema for kind 4, eventSchema for kinds 1/2/3; times use the Node clock; -1 means unavailable; receiveIsBinary is -1 for sends and raw data; bufferedAmount is local ws buffering; spans include original throws',
      eventCount, eventAttempts, heartbeatCount, maxHeartbeatGapMs,
      ignoredConnections, sockets: socketRows, events
    };
    return frozen;
  }
  function onMessage(message) {
    if (!message || message.type !== 'th10-relay-hop-snapshot') return;
    const id = message.requestId;
    if (!(typeof id === 'string' && id.length > 0 && id.length <= 128 ||
          typeof id === 'number' && Number.isSafeInteger(id))) return;
    const report = snapshot();
    try {
      if (typeof ipc.send !== 'function') throw new Error('ipc-send');
      ipc.send({ type: 'th10-relay-hop-snapshot', requestId: id, report });
    } catch { fault('snapshot-ipc-send-failed'); }
  }
  try {
    if (!ipc || typeof ipc.on !== 'function' || typeof ipc.removeListener !== 'function')
      throw new Error('ipc-api');
    ipc.on('message', onMessage); ipcAttached = true;
  } catch { fault('ipc-installation-failed'); }
  return { snapshot };
}

module.exports = { install, SCHEMA, COLUMNS, RAW_DATA_COLUMNS };

if (process.env.TH10_RELAY_HOP_DIAGNOSTIC === '1') {
  let resolved = {};
  try {
    const { createRequire } = require('node:module');
    const atEntry = createRequire(process.argv[1]);
    const ws = atEntry('ws');
    resolved = { WebSocket: ws.WebSocket, WebSocketServer: ws.WebSocketServer,
      identity: { entryPath: process.argv[1], wsEntryPath: atEntry.resolve('ws'),
        wsVersion: atEntry('ws/package.json').version, nodeVersion: process.versions.node, pid: process.pid } };
  } catch { resolved.initialError = true; }
  module.exports.controller = install({ ...resolved, rawSocketTrace: true });
}
