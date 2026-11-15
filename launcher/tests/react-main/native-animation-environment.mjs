/** Manual browser timeline. Production Motion and lifecycle code run unchanged. */
export function installNativeTimeline(view, records) {
  view.HTMLElement.prototype.getAnimations = function () {return records.filter(item => item.node === this && item.playState !== 'idle');};
  view.HTMLElement.prototype.animate = function (keyframes, options) {
    let resolve, reject;
    const finished = new Promise((yes, no) => {resolve = yes; reject = no;});
    const frames = Array.isArray(keyframes)
      ? Object.fromEntries(Object.keys(keyframes[0]).map(name => [name, keyframes.map(frame => frame[name])])) : keyframes;
    const values = Object.fromEntries(Object.entries(frames).map(([name, items]) => [name, items[0]]));
    const node = this;
    const record = {node, element: node, keyframes, options, values, finished, settled: false, cancelled: false,
      currentTime: 0, startTime: 0, playbackRate: 1, playState: 'running', onfinish: null,
      effect: {getComputedTiming: () => ({duration: options.duration})},
      play() {this.playState = 'running';}, pause() {this.playState = 'paused';},
      commitStyles() {for (const [name, value] of Object.entries(values)) node.style[name] = value;},
      finish() {
        if (this.settled) return;
        this.settled = true; this.playState = 'finished'; this.currentTime = options.duration; resolve();
        queueMicrotask(() => this.onfinish?.());
      },
      cancel() {
        this.cancelled = true; this.playState = 'idle';
        if (!this.settled) {this.settled = true; reject(new view.DOMException('Cancelled', 'AbortError'));}
      },
    };
    records.push(record); return record;
  };
}
