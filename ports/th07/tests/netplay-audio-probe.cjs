/* Test-only SDL output observation. Preserve the actual audio callback, sample
 * only 64 output values per block, and never substitute audio or touch state. */
(function(root) {
  function createNetplayAudioProbe() {
    const attached = new WeakSet();
    let active = false, firstTime = null, lastTime = null;
    const stats = {runningObservations:0, suspendedObservations:0, blocks:0, nonzeroBlocks:0,
                   sampledValues:0, maxAmplitude:0};
    return {
      observe(sdl, measured) {
        active = measured;
        const context = sdl?.audioContext, node = sdl?.audio_playback?.scriptProcessorNode;
        if (measured && context) {
          ++stats[context.state === 'running' ? 'runningObservations' : 'suspendedObservations'];
          if (firstTime === null) firstTime = context.currentTime;
          lastTime = context.currentTime;
        }
        if (!node || attached.has(node) || typeof node.onaudioprocess !== 'function') return;
        const original = node.onaudioprocess;
        node.onaudioprocess = function(event) {
          const result = original.call(this, event);
          if (active && event.outputBuffer?.numberOfChannels) {
            const samples = event.outputBuffer.getChannelData(0);
            let peak = 0;
            for (let index = 0, stride = Math.max(1,Math.ceil(samples.length/64)); index < samples.length; index += stride) {
              peak = Math.max(peak, Math.abs(samples[index]));
              ++stats.sampledValues;
            }
            ++stats.blocks;
            if (peak > 1e-6) ++stats.nonzeroBlocks;
            stats.maxAmplitude = Math.max(stats.maxAmplitude, peak);
          }
          return result;
        };
        attached.add(node);
      },
      snapshot() { return {...stats, progressedSeconds: firstTime === null ? 0 : lastTime-firstTime}; },
    };
  }
  root.createNetplayAudioProbe = createNetplayAudioProbe;
  if (typeof window !== 'undefined' && window.parent !== window)
    root.__th07AudioProbe = createNetplayAudioProbe();
})(globalThis);
