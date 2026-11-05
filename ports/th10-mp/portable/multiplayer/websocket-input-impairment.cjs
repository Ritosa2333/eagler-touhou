/* Test-only TH10 APPLICATION input-send delay on the actual current relay.
 * Install before Module/transport creation. This is not wire latency, RTT or
 * TCP loss/retransmission emulation. Only valid TH10 Input messages are queued;
 * text, Session, spectator and other control messages keep their original path.
 * Input messages retain FIFO order, so a spike also holds later Input messages.
 * env timers, when supplied by tests, must have setTimeout's deferred semantics.
 */
(function (root) {
  "use strict";
  function installWebSocketInputImpairment(options = {}, env = {}) {
    const Socket = env.WebSocket || root.WebSocket;
    const transport = env.transport || (() => root.__eaglerPeerTransport);
    const now = env.now || (() => root.performance.now());
    const later = env.later || ((fn, ms) => root.setTimeout(fn, ms));
    const cancel = env.cancel || (id => root.clearTimeout(id));
    const latency = options.oneWayMs ?? 50, jitter = options.jitterMs ?? 10;
    const dropEvery = options.dropEvery ?? 0, maxBytes = options.maxQueuedBytes ?? 1048576;
    const spikeFrame = options.spikeFrame ?? 450, spikeMs = options.spikeMs ?? 0;
    const spikeDelayMs = options.spikeDelayMs ?? 800, initialSeed = options.seed ?? 7135;
    const INVALID_FRAME = 0xffffffff, MAX_JOBS = 1024;
    if (![latency, jitter, spikeMs, spikeDelayMs].every(v => Number.isFinite(v) && v >= 0 && v <= 10000) ||
        !Number.isSafeInteger(dropEvery) || dropEvery < 0 ||
        !Number.isSafeInteger(maxBytes) || maxBytes <= 0 || maxBytes > 1048576 ||
        !Number.isInteger(spikeFrame) || spikeFrame < 0 || spikeFrame >= INVALID_FRAME ||
        !Number.isInteger(initialSeed) || initialSeed <= 0 || initialSeed > INVALID_FRAME ||
        ![transport, now, later, cancel].every(v => typeof v === "function"))
      throw new Error("invalid WebSocket impairment settings");
    if (!Socket?.prototype || typeof Socket.prototype.send !== "function")
      throw new Error("WebSocket unavailable: impairment was not installed");
    const proto = Socket.prototype, original = proto.send;
    if (original.__th10WebSocketInputImpairment) throw new Error("WebSocket impairment already installed");
    let seed = initialSeed >>> 0, active = true;
    const pending = new Set(), lanes = new WeakMap();
    const stats = { schema: "th10-websocket-input-impairment/1", transportMode: "relay",
      scope: "application-websocket-input-send-delay-not-wire-latency",
      orderingScope: "FIFO among matched Input messages on each current relay socket; non-Input controls bypass this test queue",
      dropScope: "optional whole Input-message omission before WebSocket.send; not TCP packet loss or retransmission",
      bindingScope: "current TH10 transport object, relay socket, local seat and player count checked again at delivery",
      oneWayMs: latency, jitterMs: jitter, dropEvery, maxQueuedBytes: maxBytes, maxPendingJobs: MAX_JOBS,
      matched: 0, broadcastInputs: 0, targetedInputs: 0, ackOnlyInputs: 0,
      scheduled: 0, sent: 0, dropped: 0, canceled: 0, overflow: 0, errors: 0,
      sendErrors: 0, scheduleErrors: 0, canceledStaleOrClosed: 0, canceledUninstalled: 0,
      queuedBytes: 0, pendingJobs: 0, peakQueuedBytes: 0, peakPendingJobs: 0,
      requestedDelayTotalMs: 0, plannedDelayTotalMs: 0, maxPlannedDelayMs: 0,
      fifoDelayed: 0, fifoDelayTotalMs: 0, maxFifoDelayMs: 0,
      deliveredDelayTotalMs: 0, minDeliveredDelayMs: null, maxDeliveredDelayMs: 0,
      timerOverrunTotalMs: 0, maxTimerOverrunMs: 0,
      spikeStartMs: null, spikeEndMs: null, spikeDelayed: 0, spikeLastDeliveryMs: null };
    function random() {
      seed ^= seed << 13; seed ^= seed >>> 17; seed ^= seed << 5; seed >>>= 0;
      return seed / 4294967296;
    }
    function current(owner, socket, seat = owner?.localPlayer, count = owner?.playerCount) {
      return owner && transport() === owner && owner.tag === "th10" && owner.route === "relay" &&
        !owner.closed && !owner.failed && !owner.disconnected && owner.relay === socket &&
        socket.readyState === 1 && owner.localPlayer === seat && owner.playerCount === count &&
        Number.isInteger(seat) && Number.isInteger(count) && count >= 2 && count <= 3 &&
        seat >= 0 && seat < count;
    }
    function inputPacket(bytes, owner) {
      // BrowserPeerTransport::send_to has a two-byte target envelope. The
      // broadcast form has no envelope. Reject every other envelope/length.
      const targeted = bytes[0] === 0xe7, offset = targeted ? 2 : 0;
      if (bytes.byteLength < offset + 44 || (targeted &&
          (bytes[1] >= owner.playerCount || bytes[1] === owner.localPlayer))) return null;
      if (bytes[offset] !== 0x45 || bytes[offset + 1] !== 0x41 ||
          bytes[offset + 2] !== 0x4e || bytes[offset + 3] !== 0x50 ||
          bytes[offset + 4] !== 4 || bytes[offset + 5] !== 1 ||
          bytes[offset + 6] !== owner.localPlayer || bytes[offset + 7] !== owner.playerCount ||
          bytes[offset + 43] !== 0) return null;
      const count = bytes[offset + 42];
      if (count > 32 || bytes.byteLength !== offset + 44 + count * 12) return null;
      const view = new DataView(bytes.buffer, bytes.byteOffset + offset, bytes.byteLength - offset);
      const latest = view.getUint32(24, true), first = view.getUint32(32, true);
      if (count === 0 ? first !== INVALID_FRAME :
          first === INVALID_FRAME || latest === INVALID_FRAME || first + count - 1 !== latest) return null;
      // Payload sample values are opaque: the production decoder retains its
      // full validity authority. The injector never edits or re-encodes them.
      return { targeted, count, latest };
    }
    function release(job) {
      pending.delete(job); --stats.pendingJobs; stats.queuedBytes -= job.bytes.byteLength;
    }
    function drain(lane) {
      // Even if a test/host fires equal-deadline timers out of order, no later
      // Input reaches send before its accepted predecessor.
      while (lane.head?.ready) {
        const job = lane.head; lane.head = job.next;
        if (!lane.head) lane.tail = null;
        release(job);
        if (!active || !current(job.owner, job.socket, job.seat, job.count)) {
          ++stats.canceled; ++stats.canceledStaleOrClosed; continue;
        }
        const deliveredAt = now(), elapsed = Math.max(0, deliveredAt - job.start);
        const overrun = Math.max(0, elapsed - job.delay);
        try { original.call(job.socket, job.bytes); }
        catch (error) {
          ++stats.errors; ++stats.sendErrors; stats.lastError = String(error?.message || error).slice(0, 240);
          continue;
        }
        ++stats.sent; stats.deliveredDelayTotalMs += elapsed;
        stats.minDeliveredDelayMs = stats.minDeliveredDelayMs === null ? elapsed : Math.min(stats.minDeliveredDelayMs, elapsed);
        stats.maxDeliveredDelayMs = Math.max(stats.maxDeliveredDelayMs, elapsed);
        stats.timerOverrunTotalMs += overrun;
        stats.maxTimerOverrunMs = Math.max(stats.maxTimerOverrunMs, overrun);
        if (job.spiking) stats.spikeLastDeliveryMs = deliveredAt;
      }
    }
    function wrapped(data) {
      if (!active) return original.call(this, data);
      const bytes = data instanceof ArrayBuffer ? new Uint8Array(data) :
        ArrayBuffer.isView(data) ? new Uint8Array(data.buffer, data.byteOffset, data.byteLength) : null;
      if (!bytes || bytes.byteLength < 44) return original.call(this, data);
      const owner = transport();
      if (!current(owner, this)) return original.call(this, data);
      const packet = inputPacket(bytes, owner);
      if (!packet) return original.call(this, data);
      ++stats.matched;
      if (packet.targeted) ++stats.targetedInputs; else ++stats.broadcastInputs;
      if (packet.count === 0) ++stats.ackOnlyInputs;
      const start = now();
      if (spikeMs && stats.spikeStartMs === null && packet.latest !== INVALID_FRAME && packet.latest >= spikeFrame) {
        stats.spikeStartMs = start; stats.spikeEndMs = start + spikeMs;
      }
      const spiking = stats.spikeEndMs !== null && start < stats.spikeEndMs;
      if (dropEvery && stats.matched % dropEvery === 0) { ++stats.dropped; return; }
      if (pending.size >= MAX_JOBS || stats.queuedBytes + bytes.byteLength > maxBytes) {
        ++stats.overflow; throw new Error("WebSocket impairment queue capacity exceeded");
      }
      let lane = lanes.get(this);
      if (!lane || lane.owner !== owner || lane.seat !== owner.localPlayer || lane.count !== owner.playerCount) {
        lane = { owner, seat: owner.localPlayer, count: owner.playerCount, head: null, tail: null };
        lanes.set(this, lane);
      }
      const requested = Math.max(0, latency + (random() * 2 - 1) * jitter) + (spiking ? spikeDelayMs : 0);
      const due = Math.max(start + requested, lane.tail?.due ?? -Infinity), delay = due - start;
      const fifo = Math.max(0, delay - requested);
      const job = { bytes: bytes.slice(), socket: this, owner, seat: owner.localPlayer,
        count: owner.playerCount, start, due, delay, spiking, ready: false, timer: null, next: null, lane };
      // Real setTimeout cannot invoke its callback before this stack returns.
      // A timer installation failure remains synchronous and queues no bytes.
      try {
        job.timer = later(() => {
          if (!pending.has(job)) return;
          job.ready = true; drain(lane);
        }, delay);
      } catch (error) {
        ++stats.errors; ++stats.scheduleErrors; stats.lastError = String(error?.message || error).slice(0, 240);
        throw error;
      }
      if (lane.tail) lane.tail.next = job; else lane.head = job;
      lane.tail = job; pending.add(job);
      ++stats.scheduled; ++stats.pendingJobs; stats.queuedBytes += job.bytes.byteLength;
      stats.peakQueuedBytes = Math.max(stats.peakQueuedBytes, stats.queuedBytes);
      stats.peakPendingJobs = Math.max(stats.peakPendingJobs, stats.pendingJobs);
      stats.requestedDelayTotalMs += requested; stats.plannedDelayTotalMs += delay;
      stats.maxPlannedDelayMs = Math.max(stats.maxPlannedDelayMs, delay);
      if (fifo > 0) { ++stats.fifoDelayed; stats.fifoDelayTotalMs += fifo; stats.maxFifoDelayMs = Math.max(stats.maxFifoDelayMs, fifo); }
      if (spiking) ++stats.spikeDelayed;
    }
    Object.defineProperty(wrapped, "__th10WebSocketInputImpairment", { value: true });
    proto.send = wrapped;
    return { stats, uninstall() {
      if (!active) return;
      active = false;
      for (const job of pending) {
        cancel(job.timer); release(job); ++stats.canceled; ++stats.canceledUninstalled;
        job.lane.head = job.lane.tail = null;
      }
      if (proto.send === wrapped) proto.send = original;
    } };
  }
  root.installWebSocketInputImpairment = installWebSocketInputImpairment;
  if (typeof module !== "undefined") module.exports = { installWebSocketInputImpairment };
})(globalThis);
