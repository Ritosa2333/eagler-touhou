// Explicit relay integration only. No global ws.send/prototype hooks, no timers.
// Source-reviewed contract: Node 24.13.0 and ws 8.21.3, plain accepted TCP.
// Default-OFF candidate; runtime tests and performance validation are separate.
// Normal forwarding calls the original sendBounded for every message. This
// batches Writable writes, not Input packets; it does not drop bytes or change
// the wire or retry policy. A pre-send settlement error is an explicit failed
// run and can prevent the current original call; it is never silently ignored.
//
// Integration:
//   const batches = createRelayBoundedWrites({ WebSocket, wsVersion,
//     enabled: false, observeWrites: false, observeTiming: false });
//   const sendBounded = batches.wrapSendBounded(createBoundedRelaySender());
//   // Only after admitting a gameplay connection, before its message handler:
//   batches.installSource(socket);
//   // After the checker fence, restore this outer hook before taking the old
//   // hop observer snapshot: const report = batches.restore().
//
// Install on every admitted gameplay socket: it can be both a source and target.
// Unknown streams, compression, delayed/blob writes, an existing cork, pending
// writes and needDrain bypass batching. Any own maxBufferedBytes option (even
// a getter) first settles our batch and then passes through without pre-reading.
// The integration MUST keep enabled false unless ALL effective relay delay,
// jitter and route-skew are zero, including control/route sends using this same
// sender. Its private timer bytes cannot be inspected here. A deferred/replaced
// sender or fault injection that can leave timers requires batching disabled.
// With original sender timer bytes == 0 and its default 262144-byte budget,
// our 65536-byte wire cap bounds queued + payload.byteLength + 16 at 65550.
// Explicit budgets have no such guarantee and always bypass.
// observeWrites and observeTiming are independent, explicit diagnostics; normal
// OFF returns the original sender and never reads a private socket field.
//
// A successful writev is a Node vector-write request, not proof of TCP segments,
// kernel arrival or remote receipt. Compare total raw wall (including uncork),
// actual write/writev counts, and client recovery. Costs can move into uncork.
// If the existing hop observer owns an inner raw.emit wrapper, call restore()
// BEFORE freezing that observer; otherwise its ownership check must fail.
// Callback/drain/error timing can change. No promise of identical lower-level
// completion timing, bounded milliseconds, or correction of an information gap.
import { Socket } from 'node:net';
import { Writable } from 'node:stream';
import { performance } from 'node:perf_hooks';

export const BOUNDED_WRITES_SCHEMA = 'th10-relay-bounded-writes/1';
export const BOUNDED_WRITES_LIMITS = Object.freeze({ messages: 8, wireBytes: 64 * 1024, sockets: 32 });
const OWNERS = new WeakMap();
const MAX = Number.MAX_SAFE_INTEGER;
const nativeCork = Writable.prototype.cork;
const nativeUncork = Writable.prototype.uncork;
const socketWrite = Socket.prototype.write;
const socketWriteOne = Socket.prototype._write;
const socketWriteVector = Socket.prototype._writev;
const socketEnd = Socket.prototype.end;
const socketDestroy = Socket.prototype.destroy;
// bufferedCalls counts wrapper attempts under a borrowed layer, including an
// original sender returning false. Only observed writer calls describe writes.
const metricNames = [
  'rawDispatches', 'rawCompleted', 'nestedRaw', 'forwardCalls', 'firstImmediate',
  'bypassedCalls', 'bufferedCalls', 'borrowedBatches', 'releasedBatches',
  'consumedBorrows', 'pendingBorrows', 'unsettledBorrows', 'maxBatchMessages',
  'maxBatchWireBytes', 'writeCalls', 'writevCalls', 'writevChunks',
  'maxWritevChunks', 'writeBytes', 'originalThrows', 'uncorkThrows',
  'ownershipFaults', 'rawWallMs', 'maxRawWallMs', 'uncorkWallMs', 'maxUncorkWallMs'
];
const metrics = () => Object.fromEntries(metricNames.map(key => [key, 0]));
const bump = (object, key, amount = 1) => { object[key] = Math.min(MAX, object[key] + amount); };
const peak = (object, key, value) => { object[key] = Math.max(object[key], value); };
function errorFor(code) {
  const error = new Error('relay bounded writes: ' + code);
  error.code = 'ERR_RELAY_BOUNDED_WRITES';
  return error;
}
function ownData(object, key, fallback) {
  const value = Object.getOwnPropertyDescriptor(object, key);
  return value === undefined ? { ok: true, value: fallback } :
    Object.hasOwn(value, 'value') ? { ok: true, value: value.value } : { ok: false };
}
// Inspect descriptors instead of evaluating option getters twice. The original
// sender remains the only consumer of getter/coercion-based or unusual options.
function wireSize(args) {
  if (args.length !== 3 || !Buffer.isBuffer(args[1])) return null;
  const options = args[2];
  if (!options || ![Object.prototype, null].includes(Object.getPrototypeOf(options))) return null;
  if (Object.hasOwn(options, 'maxBufferedBytes')) return null;
  const binary = ownData(options, 'binary', false), delay = ownData(options, 'delayMs', 0);
  if (!binary.ok || binary.value !== true || !delay.ok || delay.value !== 0) return null;
  const size = args[1].byteLength;
  if (!Number.isSafeInteger(size) || size < 0) return null;
  const framed = size + (size < 126 ? 2 : size < 65536 ? 4 : 10);
  return framed <= BOUNDED_WRITES_LIMITS.wireBytes ? framed : null;
}

export function createRelayBoundedWrites(options = {}) {
  const enabled = options.enabled === true;
  const observeWrites = options.observeWrites === true;
  const observeTiming = options.observeTiming === true;
  const requested = enabled || observeWrites || observeTiming;
  const WebSocket = options.WebSocket, wsVersion = options.wsVersion;
  const runtimeVersion = process.versions.node;
  const compatible = runtimeVersion === '24.13.0' && wsVersion === '8.21.3' &&
    typeof WebSocket === 'function';
  let active = requested && compatible, current = null, activeCall = null;
  let restored = false, firstFault = null, faultCount = 0, ignoredSockets = 0;
  let pending = 0, unsettled = 0;
  const totals = metrics(), states = [], bySocket = new WeakMap();
  // Counts are qualification checks, not packets; one call can check twice.
  const bypassChecks = { unsupportedInstall: 0, socketOrSender: 0, changedHooks: 0,
    lifecycle: 0, inFlightWrite: 0, needDrain: 0, existingCork: 0,
    existingBytes: 0, stateRead: 0, optionsOrTarget: 0, outsideScope: 0 };
  const bypass = key => { bump(bypassChecks, key); return false; };
  function fault(code, state) {
    firstFault ??= code; faultCount = Math.min(MAX, faultCount + 1);
    if (state) { state.disabled = true; bump(state.stats, 'ownershipFaults'); bump(totals, 'ownershipFaults'); }
    if (current) current.disabled = true;
    return errorFor(code);
  }
  function add(state, key, amount = 1) { bump(totals, key, amount); if (state) bump(state.stats, key, amount); }
  function maximum(state, key, value) { peak(totals, key, value); if (state) peak(state.stats, key, value); }
  function time() {
    if (!observeTiming) return null;
    try {
      const value = performance.now();
      if (!Number.isFinite(value) || value < 0) throw errorFor('invalid-clock');
      return value;
    } catch { fault('clock-read-failed'); return null; }
  }
  function duration(state, start, totalKey, maxKey) {
    if (start === null) return;
    const end = time();
    if (end === null || end < start) { fault('clock-went-backwards'); return; }
    add(state, totalKey, end - start); maximum(state, maxKey, end - start);
  }
  function hooksOwned(state) {
    return state.hooks.every(hook => state.raw[hook.key] === hook.value);
  }
  function patch(state, key, value) {
    const descriptor = Object.getOwnPropertyDescriptor(state.raw, key);
    Object.defineProperty(state.raw, key, { value, configurable: true, writable: true,
      enumerable: descriptor?.enumerable ?? false });
    state.hooks.push({ key, value, descriptor });
  }
  function undo(state) {
    for (const hook of state.hooks.slice().reverse()) {
      if (state.raw[hook.key] !== hook.value) { fault('hook-ownership-lost-on-restore', state); continue; }
      if (hook.descriptor) Object.defineProperty(state.raw, hook.key, hook.descriptor);
      else delete state.raw[hook.key];
    }
    state.hooks.length = 0;
    if (OWNERS.get(state.raw) === state) OWNERS.delete(state.raw);
  }
  function settle(state, reason) {
    const lease = state.lease;
    if (!lease) return null;
    const scope = lease.scope, wasFlushing = scope.flushing;
    scope.flushing = true;
    try {
      const raw = state.raw, ws = state.ws;
      // A method/stream replacement leaves ownership unknowable. Never guess
      // by decrementing a new owner's counter; expose an explicit failed run.
      if (!hooksOwned(state) || raw._writableState !== state.writable) {
        state.lease = null; --pending; ++unsettled;
        add(state, 'unsettledBorrows');
        return fault('cannot-settle-changed-owner', state);
      }
      if (raw.destroyed || raw.writableEnded || raw.writableFinished) {
        state.lease = null; --pending; add(state, 'consumedBorrows');
        lease.target.disabled = true;
        return null; // Closed streams cannot be reused; do not touch a later layer.
      }
      const count = raw.writableCorked;
      if (!Number.isSafeInteger(count) || count < 1) {
        state.lease = null; --pending; add(state, 'consumedBorrows');
        return fault('borrow-consumed-without-owned-hook', state);
      }
      // Native uncork decrements before clearBuffer can invoke a writer/callback.
      // Relinquish our token first so reentry cannot cancel a later scope.
      state.lease = null; --pending; add(state, 'releasedBatches');
      const unexpected = count !== 1 ? fault('unexpected-extra-cork-layer', state) : null;
      const start = time();
      try { Reflect.apply(nativeUncork, raw, []); }
      catch (error) { add(state, 'uncorkThrows'); fault('native-uncork-threw', state); return error; }
      finally { duration(state, start, 'uncorkWallMs', 'maxUncorkWallMs'); }
      if (reason !== 'limit') lease.target.disabled = true;
      if (ws._socket !== raw) lease.target.disabled = true;
      return unexpected;
    } catch (error) {
      if (state.lease === lease) {
        state.lease = null; --pending; ++unsettled; add(state, 'unsettledBorrows');
      }
      fault('borrow-settlement-read-failed', state);
      return error;
    } finally { scope.flushing = wasFlushing; }
  }
  function settleAll(scope, reason) {
    if (!scope) return null;
    let first = null;
    for (const target of scope.targets.values()) {
      const error = settle(target.state, reason); first ??= error;
    }
    return first;
  }
  function boundary(scope, reason) {
    if (!scope) return null;
    scope.disabled = true;
    return settleAll(scope, reason);
  }
  function live(state, idle) {
    try {
      if (state.disabled) return bypass('socketOrSender');
      const ws = state.ws, raw = state.raw, writable = state.writable, sender = ws._sender;
      if (ws._socket !== raw || ws.readyState !== 1 || ws._isServer !== true ||
          !sender || sender._socket !== raw || sender._state !== 0 ||
          sender._bufferedBytes !== 0 || !Array.isArray(sender._queue) || sender._queue.length ||
          sender._firstFragment !== true || !ws._extensions || Object.keys(ws._extensions).length ||
          ws._receiver?._allowSynchronousEvents !== true) return bypass('socketOrSender');
      if (!hooksOwned(state) || raw._writableState !== writable || raw.write !== socketWrite ||
          raw.end !== socketEnd || raw.destroy !== socketDestroy ||
          raw._write !== state.expectedWrite || raw._writev !== state.expectedWritev)
        return bypass('changedHooks');
      if (raw.connecting !== false || raw.destroyed || raw.writableEnded || raw.writableFinished ||
          raw.writable !== true || raw.writableObjectMode !== false || !raw._handle)
        return bypass('lifecycle');
      if (writable.writing !== false) return bypass('inFlightWrite');
      if (raw.writableNeedDrain !== false) return bypass('needDrain');
      if (!Number.isSafeInteger(raw.writableLength) || raw.writableLength < 0)
        return bypass('stateRead');
      if (idle && raw.writableCorked !== 0) return bypass('existingCork');
      if (idle && raw.writableLength !== 0) return bypass('existingBytes');
      return true;
    } catch { fault('socket-state-read-failed', state); return bypass('stateRead'); }
  }
  function beginLease(target, scope) {
    const state = target.state;
    if (!live(state, true)) return false;
    const lease = { scope, target, messages: 0, bytes: 0 };
    Reflect.apply(nativeCork, state.raw, []);
    state.lease = lease; ++pending; add(state, 'borrowedBatches');
    if (state.raw.writableCorked !== 1) throw fault('native-cork-did-not-borrow-one', state);
    return true;
  }
  function installWriteObservation(state, key, counter) {
    const original = state.raw[key];
    patch(state, key, function () {
      if (this !== state.raw || !active) return Reflect.apply(original, this, arguments);
      add(state, counter);
      if (key === '_writev') {
        const count = arguments[0]?.length;
        if (Number.isSafeInteger(count) && count >= 0) {
          add(state, 'writevChunks', count); maximum(state, 'maxWritevChunks', count);
        } else fault('writev-chunk-count-unavailable', state);
      }
      const bytes = state.writable.writelen;
      if (Number.isSafeInteger(bytes) && bytes >= 0) add(state, 'writeBytes', bytes);
      else fault('write-length-unavailable', state);
      return Reflect.apply(original, this, arguments);
    });
  }
  function installSource(ws) {
    if (!active) return false; // Includes normal OFF: zero private socket access.
    try {
      if (bySocket.has(ws)) return bySocket.get(ws).ws._socket === bySocket.get(ws).raw;
      if (states.length >= BOUNDED_WRITES_LIMITS.sockets) { ++ignoredSockets; return false; }
      if (!(ws instanceof WebSocket)) { ++ignoredSockets; return bypass('unsupportedInstall'); }
      const raw = ws._socket;
      if (!raw || Object.getPrototypeOf(raw) !== Socket.prototype || OWNERS.has(raw) ||
          raw.cork !== nativeCork || raw.uncork !== nativeUncork ||
          raw.write !== socketWrite || raw._write !== socketWriteOne ||
          raw._writev !== socketWriteVector || raw.end !== socketEnd ||
          raw.destroy !== socketDestroy || typeof raw.emit !== 'function' ||
          !raw._handle || ws.readyState !== 1 || raw.connecting !== false ||
          typeof raw._writableState?.writing !== 'boolean' ||
          ws._isServer !== true || ws._receiver?._allowSynchronousEvents !== true ||
          !ws._extensions || Object.keys(ws._extensions).length) {
        ++ignoredSockets; return bypass('unsupportedInstall');
      }
      const state = { id: states.length + 1, ws, raw, writable: raw._writableState,
        expectedWrite: raw._write, expectedWritev: raw._writev, hooks: [],
        stats: metrics(), lease: null, disabled: false };
      const emit = raw.emit;
      OWNERS.set(raw, state);
      try {
        if (enabled) {
          patch(state, 'cork', function () {
            if (this !== raw) return Reflect.apply(nativeCork, this, arguments);
            // Control/unwrapped sends start their own cork outside the admitted
            // sendBounded call. Flush our earlier bytes before that new owner.
            if (state.lease && activeCall?.state !== state) {
              const error = settle(state, 'external-cork');
              if (error) throw error;
            }
            return Reflect.apply(nativeCork, this, arguments);
          });
          patch(state, 'uncork', function () {
            if (this === raw && state.lease && raw.writableCorked === 1) {
              // Includes Writable.end(), which explicitly resets corked to 1.
              // The original call now consumes our layer; a subsequent cork
              // belongs to somebody else and the raw finally must not touch it.
              const lease = state.lease;
              state.lease = null; --pending; add(state, 'consumedBorrows');
              lease.target.disabled = true;
            }
            return Reflect.apply(nativeUncork, this, arguments);
          });
        }
        if (observeWrites) {
          installWriteObservation(state, '_write', 'writeCalls');
          installWriteObservation(state, '_writev', 'writevCalls');
          state.expectedWrite = raw._write; state.expectedWritev = raw._writev;
        }
        patch(state, 'emit', function () {
          if (!active || this !== raw) return Reflect.apply(emit, this, arguments);
          const event = arguments[0];
          if (event !== 'data') {
            if (event === 'end' || event === 'close' || event === 'error') {
              const error = boundary(current, 'lifecycle');
              if (error) throw error;
            }
            return Reflect.apply(emit, this, arguments);
          }
          if (ws._socket !== raw || !Buffer.isBuffer(arguments[1])) {
            const error = boundary(current, 'source-boundary');
            if (error) throw error;
            return Reflect.apply(emit, this, arguments);
          }
          const previous = current, start = time();
          add(state, 'rawDispatches');
          if (previous) {
            add(state, 'nestedRaw');
            const error = boundary(previous, 'nested-raw');
            if (error) throw error;
          }
          const scope = { source: state, targets: new Map(), disabled: !enabled || !!previous,
            flushing: false };
          current = scope;
          let threw = false;
          try { return Reflect.apply(emit, this, arguments); }
          catch (error) { threw = true; add(state, 'originalThrows'); throw error; }
          finally {
            const error = settleAll(scope, 'raw-finally');
            current = previous;
            add(state, 'rawCompleted'); duration(state, start, 'rawWallMs', 'maxRawWallMs');
            if (error && !threw) throw error;
          }
        });
        bySocket.set(ws, state); states.push(state);
        return true;
      } catch (error) {
        fault('socket-hook-installation-failed', state);
        try { undo(state); } catch { fault('socket-hook-rollback-failed', state); }
        return false;
      }
    } catch { fault('socket-installation-read-failed'); return false; }
  }
  function wrapSendBounded(original) {
    if (typeof original !== 'function') throw new TypeError('sendBounded must be a function');
    if (!enabled || !active) return original;
    return function () {
      if (!active) return Reflect.apply(original, this, arguments);
      const scope = current;
      add(null, 'forwardCalls');
      if (!scope || scope.disabled || scope.flushing) {
        add(null, 'bypassedCalls'); bypass('outsideScope');
        return Reflect.apply(original, this, arguments);
      }
      let state, size;
      try { state = bySocket.get(arguments[0]); size = wireSize(arguments); }
      catch { state = null; size = null; }
      if (!state || size === null) {
        bypass('optionsOrTarget');
        const error = boundary(scope, 'unsupported-send');
        if (error) throw error;
        add(state, 'bypassedCalls'); return Reflect.apply(original, this, arguments);
      }
      let target = scope.targets.get(state);
      if (!target) {
        target = { state, first: true, disabled: false }; scope.targets.set(state, target);
      }
      let leased = false;
      if (!target.disabled && !target.first) {
        try {
          if (state.lease && state.lease.bytes + size > BOUNDED_WRITES_LIMITS.wireBytes) {
            const error = settle(state, 'limit'); if (error) throw error;
          }
          if (!state.lease && !beginLease(target, scope)) target.disabled = true;
          if (state.lease) {
            if (!live(state, false) || state.raw.writableCorked !== 1 ||
                state.raw.writableLength !== state.lease.bytes) {
              const error = settle(state, 'no-longer-idle'); target.disabled = true;
              if (error) throw error;
            } else leased = true;
          }
        } catch (error) {
          target.disabled = true;
          const cleanup = settle(state, 'entry-error');
          if (cleanup) fault('entry-error-with-cleanup-error', state);
          throw error;
        }
      }
      if (target.first) { target.first = false; add(state, 'firstImmediate'); }
      else if (!leased) add(state, 'bypassedCalls');
      if (leased) {
        const lease = state.lease;
        ++lease.messages; lease.bytes += size;
        add(state, 'bufferedCalls');
        maximum(state, 'maxBatchMessages', lease.messages);
        maximum(state, 'maxBatchWireBytes', lease.bytes);
      }
      const previousCall = activeCall;
      activeCall = { state };
      let threw = false, result;
      try { result = Reflect.apply(original, this, arguments); return result; }
      catch (error) { threw = true; target.disabled = true; add(state, 'originalThrows'); throw error; }
      finally {
        activeCall = previousCall;
        let error = null;
        try {
          if (state.lease) {
            const lease = state.lease;
            if (threw || result !== true || !live(state, false) ||
                state.raw.writableCorked !== 1 || state.raw.writableLength !== lease.bytes) {
              target.disabled = true; error = settle(state, 'send-boundary');
            } else if (lease.messages >= BOUNDED_WRITES_LIMITS.messages ||
                       lease.bytes >= BOUNDED_WRITES_LIMITS.wireBytes) error = settle(state, 'limit');
          }
          if (!state.lease && !live(state, true)) target.disabled = true;
        } catch (cleanup) {
          fault('send-finally-read-failed', state);
          error = settle(state, 'send-finally-error') ?? cleanup;
        }
        if (error && !threw) throw error;
      }
    };
  }
  function snapshot() {
    // No payload, queue scan, URL, remote address or private socket reads here.
    const serialize = state => ({ id: state.id, disabled: state.disabled, ...state.stats,
      pendingBorrows: state.lease ? 1 : 0 });
    return {
      schema: BOUNDED_WRITES_SCHEMA, enabled, observeWrites, observeTiming,
      nodeVersion: runtimeVersion, wsVersion: wsVersion ?? null, compatible,
      active, restored, valid: firstFault === null && unsettled === 0,
      complete: current === null && activeCall === null && pending === 0 && unsettled === 0,
      firstFault, faultCount, ignoredSockets, installedSockets: states.length,
      bypassChecks: { ...bypassChecks },
      limits: BOUNDED_WRITES_LIMITS,
      scope: 'Node JS raw emit(data); finally includes our uncork; writev is not network receipt',
      totals: { ...totals, pendingBorrows: pending, unsettledBorrows: unsettled },
      sockets: states.map(serialize)
    };
  }
  function restore() {
    if (restored) return snapshot();
    const error = boundary(current, 'restore');
    active = false; restored = true;
    for (const state of states.slice().reverse()) {
      try { undo(state); } catch { fault('socket-hook-restore-failed', state); }
    }
    if (error) throw error;
    return snapshot();
  }
  return Object.freeze({ installSource, wrapSendBounded, snapshot, restore });
}
