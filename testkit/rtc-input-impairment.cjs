/* Test-only APPLICATION send impairment, not a kernel/wire latency emulator.
 * Inject in the game realm before RTC channel creation. A test MUST require
 * matched/sent > 0; relay delay alone never impairs the real RTC input lane.
 */
(function (root) {
  "use strict";
  function installRtcInputImpairment(options = {}, env = {}) {
    const DataChannel = env.DataChannel || root.RTCDataChannel;
    const now = env.now || (() => root.performance.now());
    const later = env.later || ((fn, ms) => root.setTimeout(fn, ms));
    const cancel = env.cancel || (id => root.clearTimeout(id));
    const latency = options.oneWayMs ?? 50, jitter = options.jitterMs ?? 10;
    const dropEvery = options.dropEvery ?? 0;
    const inputLabel = options.inputLabel ?? "eagler-input";
    const controlLabel = options.controlLabel ?? "eagler-control";
    const blackoutFrame = options.blackoutFrame ?? 900, blackoutMs = options.blackoutMs ?? 0;
    const includeControlInputs = options.includeControlInputs !== false;
    const maxBytes = options.maxQueuedBytes ?? 1048576;
    if (![latency, jitter].every(v => Number.isFinite(v) && v >= 0 && v <= 10000) ||
        typeof inputLabel !== "string" || !inputLabel ||
        typeof controlLabel !== "string" || !controlLabel || inputLabel === controlLabel ||
        !Number.isSafeInteger(dropEvery) || dropEvery < 0 ||
        !Number.isSafeInteger(maxBytes) || maxBytes <= 0 ||
        !Number.isInteger(blackoutFrame) || blackoutFrame < 0 ||
        !Number.isFinite(blackoutMs) || blackoutMs < 0 || blackoutMs > 10000 ||
        !Number.isInteger(options.seed ?? 7135) || (options.seed ?? 7135) <= 0 ||
        (options.seed ?? 7135) > 0xffffffff)
      throw new Error("invalid RTC impairment settings");
    if (!DataChannel?.prototype || typeof DataChannel.prototype.send !== "function")
      throw new Error("RTCDataChannel unavailable: impairment was not installed");
    const proto = DataChannel.prototype, original = proto.send;
    if (original.__eaglerRtcInputImpairment) throw new Error("RTC impairment already installed");
    let seed = (options.seed ?? 7135) >>> 0, active = true;
    let blackoutUntil = null;
    const pending = new Set();
    const stats = { scope: "application-send-delay-not-wire-latency", oneWayMs: latency,
      jitterMs: jitter, matched: 0, scheduled: 0, sent: 0, dropped: 0, canceled: 0,
      controlInputs: 0, blackoutDropped: 0,
      overflow: 0, errors: 0, queuedBytes: 0, peakQueuedBytes: 0,
      plannedDelayTotalMs: 0, deliveredDelayTotalMs: 0, maxDeliveredDelayMs: 0,
      timerOverrunTotalMs: 0, maxTimerOverrunMs: 0 };
    function random() {
      seed ^= seed << 13; seed ^= seed >>> 17; seed ^= seed << 5; seed >>>= 0;
      return seed / 4294967296;
    }
    function release(job) { pending.delete(job); stats.queuedBytes -= job.bytes.byteLength; }
    function wrapped(data) {
      const bytes = data instanceof ArrayBuffer ? new Uint8Array(data) :
        ArrayBuffer.isView(data) ? new Uint8Array(data.buffer, data.byteOffset, data.byteLength) : null;
      const reliableInput = includeControlInputs && this.label === controlLabel;
      if (!active || (this.label !== inputLabel && !reliableInput) || !bytes || bytes.length < 32 || bytes[5] !== 1 ||
          this.readyState !== "open") return original.call(this, data);
      ++stats.matched;
      if (reliableInput) ++stats.controlInputs;
      else {
        const frame = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength).getUint32(24, true);
        if (blackoutMs && blackoutUntil === null && frame >= blackoutFrame) blackoutUntil = now() + blackoutMs;
        if (blackoutUntil !== null && now() < blackoutUntil) { ++stats.dropped; ++stats.blackoutDropped; return; }
        if (dropEvery && stats.matched % dropEvery === 0) { ++stats.dropped; return; }
      }
      if (pending.size >= 1024 || stats.queuedBytes + bytes.byteLength > maxBytes) {
        ++stats.overflow; throw new Error("RTC impairment queue capacity exceeded");
      }
      const delay = Math.max(0, latency + (random() * 2 - 1) * jitter);
      const job = { bytes: bytes.slice(), channel: this, start: now(), timer: null };
      pending.add(job); stats.queuedBytes += job.bytes.byteLength;
      stats.peakQueuedBytes = Math.max(stats.peakQueuedBytes, stats.queuedBytes);
      ++stats.scheduled; stats.plannedDelayTotalMs += delay;
      job.timer = later(() => {
        release(job);
        if (!active || job.channel.readyState !== "open") { ++stats.canceled; return; }
        const elapsed = Math.max(0, now() - job.start), overrun = Math.max(0, elapsed - delay);
        try { original.call(job.channel, job.bytes); }
        catch (error) { ++stats.errors; stats.lastError = String(error?.message || error); return; }
        ++stats.sent; stats.deliveredDelayTotalMs += elapsed;
        stats.maxDeliveredDelayMs = Math.max(stats.maxDeliveredDelayMs, elapsed);
        stats.timerOverrunTotalMs += overrun;
        stats.maxTimerOverrunMs = Math.max(stats.maxTimerOverrunMs, overrun);
      }, delay);
    }
    Object.defineProperty(wrapped, "__eaglerRtcInputImpairment", { value: true });
    proto.send = wrapped;
    return { stats, uninstall() {
      if (!active) return;
      active = false;
      for (const job of [...pending]) { cancel(job.timer); release(job); ++stats.canceled; }
      if (proto.send === wrapped) proto.send = original;
    }};
  }
  root.installRtcInputImpairment = installRtcInputImpairment;
  if (typeof module !== "undefined") module.exports = { installRtcInputImpairment };
})(globalThis);
