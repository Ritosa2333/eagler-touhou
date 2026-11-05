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
    const spikeFrame = options.spikeFrame ?? 450, spikeMs = options.spikeMs ?? 0;
    const spikeDelayMs = options.spikeDelayMs ?? 800;
    const includeControlInputs = options.includeControlInputs !== false;
    const maxBytes = options.maxQueuedBytes ?? 1048576;
    if (![latency, jitter].every(v => Number.isFinite(v) && v >= 0 && v <= 10000) ||
        typeof inputLabel !== "string" || !inputLabel ||
        typeof controlLabel !== "string" || !controlLabel || inputLabel === controlLabel ||
        !Number.isSafeInteger(dropEvery) || dropEvery < 0 ||
        !Number.isSafeInteger(maxBytes) || maxBytes <= 0 ||
        !Number.isInteger(blackoutFrame) || blackoutFrame < 0 ||
        !Number.isFinite(blackoutMs) || blackoutMs < 0 || blackoutMs > 10000 ||
        !Number.isInteger(spikeFrame) || spikeFrame < 0 ||
        ![spikeMs, spikeDelayMs].every(v => Number.isFinite(v) && v >= 0 && v <= 10000) ||
        !Number.isInteger(options.seed ?? 7135) || (options.seed ?? 7135) <= 0 ||
        (options.seed ?? 7135) > 0xffffffff)
      throw new Error("invalid RTC impairment settings");
    if (!DataChannel?.prototype || typeof DataChannel.prototype.send !== "function")
      throw new Error("RTCDataChannel unavailable: impairment was not installed");
    const proto = DataChannel.prototype, original = proto.send;
    if (original.__eaglerRtcInputImpairment) throw new Error("RTC impairment already installed");
    let seed = (options.seed ?? 7135) >>> 0, calibrationSeed = seed, active = true;
    let blackoutUntil = null;
    const pending = new Set();
    const stats = { scope: "application-send-delay-not-wire-latency", oneWayMs: latency,
      jitterMs: jitter, matched: 0, scheduled: 0, sent: 0, dropped: 0, canceled: 0,
      controlInputs: 0, blackoutDropped: 0,
      spikeStartMs: null, spikeEndMs: null, spikeDelayed: 0, spikeControlDelayed: 0,
      spikeLastDeliveryMs: null,
      overflow: 0, errors: 0, queuedBytes: 0, peakQueuedBytes: 0,
      plannedDelayTotalMs: 0, deliveredDelayTotalMs: 0, maxDeliveredDelayMs: 0,
      timerOverrunTotalMs: 0, maxTimerOverrunMs: 0 };
    Object.assign(stats, { calibrationMatched: 0, calibrationProbes: 0, calibrationEchoes: 0,
      calibrationScheduled: 0, calibrationSent: 0, calibrationCanceled: 0,
      calibrationControlBypassed: 0, calibrationControlPolicy: "bypass",
      calibrationPlannedDelayTotalMs: 0, calibrationDeliveredDelayTotalMs: 0,
      calibrationMaxDeliveredDelayMs: 0, calibrationTimerOverrunTotalMs: 0,
      calibrationMaxTimerOverrunMs: 0 });
    function random(calibration) {
      // Calibration must not consume the gameplay jitter sequence in an A/B run.
      let value = calibration ? calibrationSeed : seed;
      value ^= value << 13; value ^= value >>> 17; value ^= value << 5; value >>>= 0;
      if (calibration) calibrationSeed = value; else seed = value;
      return value / 4294967296;
    }
    function inputPacket(bytes) {
      // ProtocolConfig owns the alphanumeric title byte in E?NP. Require the
      // actual v4 input header, not byte 5 alone (ADS uses it for the sender).
      return bytes.length >= 44 && bytes[0] === 69 &&
        ((bytes[1] >= 48 && bytes[1] <= 57) || (bytes[1] >= 65 && bytes[1] <= 90)) &&
        bytes[2] === 78 && bytes[3] === 80 && bytes[4] === 4 && bytes[5] === 1 &&
        bytes[7] >= 2 && bytes[7] <= 3 && bytes[6] < bytes[7] &&
        bytes[42] <= 32 && bytes.length === 44 + bytes[42] * 12;
    }
    function adonisKind(bytes) {
      // ADS/4: kind at 4, sender at 5, requested D at 24, sequence at 28.
      // Validate the envelope only; native peers still own its session/ABI.
      if (bytes.length !== 64 || bytes[0] !== 65 || bytes[1] !== 68 || bytes[2] !== 83 ||
          bytes[3] !== 4 || bytes[4] < 1 || bytes[4] > 11 ||
          bytes[56] < 2 || bytes[56] > 3 || bytes[5] >= bytes[56] ||
          (bytes[6] !== 1 && bytes[6] !== 2) ||
          (bytes[6] === 1 ? bytes[7] !== 0 : bytes[7] < 1 || bytes[7] > 2)) return 0;
      const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
      const kind = bytes[4], probe = kind === 2 || kind === 3;
      const requested = view.getUint32(24, true), sequence = view.getUint32(28, true);
      if ((!view.getUint32(8, true) && !view.getUint32(12, true)) || !view.getUint32(16, true) ||
          (requested !== 0xffffffff && requested > 9) || view.getUint32(60, true) > 3 ||
          (probe ? sequence < 1 || sequence > 129 || bytes[57] >= bytes[56] || bytes[57] === bytes[5]
                 : sequence !== 0 || bytes[57] !== 255)) return 0;
      if (kind <= 3 && (view.getUint32(32, true) || view.getUint32(36, true) || view.getUint32(40, true))) return 0;
      for (let at = kind === 4 ? 56 : 44; at < 60; ++at)
        if (at !== 56 && at !== 57 && bytes[at]) return 0;
      return kind;
    }
    function release(job) { pending.delete(job); stats.queuedBytes -= job.bytes.byteLength; }
    function wrapped(data) {
      const bytes = data instanceof ArrayBuffer ? new Uint8Array(data) :
        ArrayBuffer.isView(data) ? new Uint8Array(data.buffer, data.byteOffset, data.byteLength) : null;
      if (!active || !bytes || (this.label !== inputLabel && this.label !== controlLabel) ||
          this.readyState !== "open") return original.call(this, data);
      const kind = adonisKind(bytes), calibration = kind === 2 || kind === 3;
      // This remains an input-lane experiment: ADS HELLO/negotiation controls
      // bypass delay, just like gameplay HELLO. Probe AND echo get one-way
      // latency on every seat, without gameplay frame spikes, blackout or loss.
      if (kind && !calibration) { ++stats.calibrationControlBypassed; return original.call(this, data); }
      const reliableInput = includeControlInputs && this.label === controlLabel;
      if (!calibration && ((!reliableInput && this.label !== inputLabel) || !inputPacket(bytes)))
        return original.call(this, data);
      let spiking = false;
      if (calibration) {
        ++stats.calibrationMatched;
        if (kind === 2) ++stats.calibrationProbes; else ++stats.calibrationEchoes;
      } else {
        ++stats.matched;
        const frame = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength).getUint32(24, true);
        if (spikeMs && stats.spikeStartMs === null && frame >= spikeFrame) {
          stats.spikeStartMs = now(); stats.spikeEndMs = now() + spikeMs;
        }
        spiking = stats.spikeEndMs !== null && now() < stats.spikeEndMs;
        if (reliableInput) ++stats.controlInputs;
        else {
          if (blackoutMs && blackoutUntil === null && frame >= blackoutFrame) blackoutUntil = now() + blackoutMs;
          if (blackoutUntil !== null && now() < blackoutUntil) { ++stats.dropped; ++stats.blackoutDropped; return; }
          if (dropEvery && stats.matched % dropEvery === 0) { ++stats.dropped; return; }
        }
      }
      if (pending.size >= 1024 || stats.queuedBytes + bytes.byteLength > maxBytes) {
        ++stats.overflow; throw new Error("RTC impairment queue capacity exceeded");
      }
      // Both input lanes see the spike. Unlike fast-lane blackout, reliable
      // repair cannot bypass it. Old delayed packets may arrive after recovery.
      const delay = Math.max(0, latency + (random(calibration) * 2 - 1) * jitter) + (spiking ? spikeDelayMs : 0);
      if (spiking) { ++stats.spikeDelayed; if (reliableInput) ++stats.spikeControlDelayed; }
      const job = { bytes: bytes.slice(), channel: this, start: now(), timer: null, calibration };
      pending.add(job); stats.queuedBytes += job.bytes.byteLength;
      stats.peakQueuedBytes = Math.max(stats.peakQueuedBytes, stats.queuedBytes);
      if (calibration) { ++stats.calibrationScheduled; stats.calibrationPlannedDelayTotalMs += delay; }
      else { ++stats.scheduled; stats.plannedDelayTotalMs += delay; }
      job.timer = later(() => {
        release(job);
        if (!active || job.channel.readyState !== "open") {
          if (calibration) ++stats.calibrationCanceled; else ++stats.canceled;
          return;
        }
        const elapsed = Math.max(0, now() - job.start), overrun = Math.max(0, elapsed - delay);
        try { original.call(job.channel, job.bytes); }
        catch (error) { ++stats.errors; stats.lastError = String(error?.message || error); return; }
        if (calibration) {
          ++stats.calibrationSent; stats.calibrationDeliveredDelayTotalMs += elapsed;
          stats.calibrationMaxDeliveredDelayMs = Math.max(stats.calibrationMaxDeliveredDelayMs, elapsed);
          stats.calibrationTimerOverrunTotalMs += overrun;
          stats.calibrationMaxTimerOverrunMs = Math.max(stats.calibrationMaxTimerOverrunMs, overrun);
        } else {
          ++stats.sent; stats.deliveredDelayTotalMs += elapsed;
          if (spiking) stats.spikeLastDeliveryMs = now();
          stats.maxDeliveredDelayMs = Math.max(stats.maxDeliveredDelayMs, elapsed);
          stats.timerOverrunTotalMs += overrun;
          stats.maxTimerOverrunMs = Math.max(stats.maxTimerOverrunMs, overrun);
        }
      }, delay);
    }
    Object.defineProperty(wrapped, "__eaglerRtcInputImpairment", { value: true });
    proto.send = wrapped;
    return { stats, uninstall() {
      if (!active) return;
      active = false;
      for (const job of [...pending]) {
        cancel(job.timer); release(job);
        if (job.calibration) ++stats.calibrationCanceled; else ++stats.canceled;
      }
      if (proto.send === wrapped) proto.send = original;
    }};
  }
  root.installRtcInputImpairment = installRtcInputImpairment;
  if (typeof module !== "undefined") module.exports = { installRtcInputImpairment };
})(globalThis);
