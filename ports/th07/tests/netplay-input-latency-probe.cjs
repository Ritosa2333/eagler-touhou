/* Diagnostic only: timestamps do not enter FrameInput, snapshots or Replay.
 * Measures synthetic Launcher event -> first rendered simulation containing it,
 * NOT touchscreen scanout / compositor / photons or an input-to-photon claim.
 */
(function (root) {
  function installNetplayInputLatencyProbe(clock = () => performance.timeOrigin + performance.now()) {
    const limit = 12000, pending = [], captured = new Map(), simulated = [];
    const report = { events: 0, samples: [], overflow: 0, invalidClock: 0 };
    return {
      report,
      receive(message) {
        if (message.type !== 'move' || !Number.isFinite(message.testIssuedAt)) return;
        const received = clock();
        if (message.testIssuedAt > received + 1) { ++report.invalidClock; return; }
        ++report.events;
        if (pending.length >= limit) { ++report.overflow; return; }
        pending.push({issued: message.testIssuedAt, received});
      },
      capture(captureFrame, scheduledFrame, hasMotion) {
        if (!hasMotion || !pending.length) return;
        if (captured.has(scheduledFrame)) throw new Error('duplicate latency capture');
        const now = clock();
        captured.set(scheduledFrame, {captureFrame, scheduledFrame, captured: now, events: pending.splice(0)});
      },
      simulate(frame) {
        const entry = captured.get(frame);
        if (!entry) return;
        captured.delete(frame);
        entry.simulated = clock();
        simulated.push(entry);
      },
      present(nextFrame, bullets) {
        const now = clock();
        while (simulated.length && simulated[0].scheduledFrame < nextFrame) {
          const entry = simulated.shift();
          for (const event of entry.events) {
            if (report.samples.length >= limit) { ++report.overflow; continue; }
            report.samples.push({
              frame: entry.scheduledFrame, delayFrames: entry.scheduledFrame - entry.captureFrame,
              deliveryMs: event.received - event.issued,
              samplingMs: entry.captured - event.received,
              scheduleMs: entry.simulated - entry.captured,
              presentationMs: now - entry.simulated,
              eventToPresentMs: now - event.issued, bullets,
            });
          }
        }
      },
      finish() {
        return {...report, pendingEvents: pending.length,
                capturedFrames: captured.size, simulatedFrames: simulated.length};
      },
    };
  }
  root.installNetplayInputLatencyProbe = installNetplayInputLatencyProbe;
  if (typeof window !== 'undefined' && window.parent !== window)
    root.__th07InputLatencyProbe = installNetplayInputLatencyProbe();
})(globalThis);
