// Title-neutral orchestration for a presentation lab. The adapter owns every
// game-specific wire layout, state fingerprint and classification policy.
export class PresentationLabControllerCore {
  constructor(runtime, identity, adapter) {
    if (!runtime?.core || !adapter) throw Error('Presentation Lab requires a runtime and adapter');
    this.runtime = runtime;
    this.core = runtime.core;
    this.identity = identity;
    this.adapter = adapter;
    this.history = [];
    this.last = null;
    this.running = false;
    this.keys = new Set();
    for (const name of ['sdl_loop_start', 'sdl_loop_stop', 'sdl_loop_tick', 'sdl_key', 'sdl_keys_clear'])
      if (typeof this.core[name] !== 'function') throw Error('Runtime is missing ' + name);
    adapter.validate(this.core);
    adapter.enable(this.core, true);
  }

  records(reference = null) { return this.adapter.records(this.core, reference); }
  state() { return this.adapter.state(this.core); }
  timing() { return this.adapter.timing(this.core); }
  trace() { return this.adapter.trace(this.core, this.runtime); }
  freeze() { this.core.sdl_loop_stop(); this.running = false; }
  play() { this.adapter.setFault(this.core, false); this.core.sdl_loop_start(); this.running = true; }

  key(code, down) {
    const bytes = new TextEncoder().encode(code + '\0'), pointer = this.core.allocate(bytes.length);
    try {
      new Uint8Array(this.core.memory.buffer, pointer, bytes.length).set(bytes);
      this.core.sdl_key(pointer, down ? 1 : 0);
      if (down) this.keys.add(code); else this.keys.delete(code);
    } finally { this.core.deallocate(pointer); }
  }

  clearKeys() { this.core.sdl_keys_clear(); this.keys.clear(); }

  step(count = 1) {
    this.freeze();
    if (!Number.isInteger(count) || count < 1 || count > 600) throw Error('Step size must be 1..600');
    for (let i = 0; i < count; i++) {
      const result = this.core.sdl_loop_tick(this.runtime.app, 1 / 60, 17);
      if (result) throw Error('Game tick stopped: ' + result);
    }
    return this.records(1);
  }

  press(code, wait = 40) { this.key(code, true); this.step(3); this.key(code, false); this.step(wait); }

  preview(alpha, {world = !this.adapter.worldFrozen(this.core), negativeControl = false} = {}) {
    this.freeze();
    this.adapter.setFault(this.core, negativeControl);
    try {
      const value = Number(alpha);
      if (!Number.isFinite(value) || value < 0 || value > 1) throw Error('Invalid presentation alpha');
      const result = this.adapter.draw(this.core, value, world);
      if (result) throw Error('Presentation draw failed: ' + result);
      return this.records();
    } finally { this.adapter.setFault(this.core, false); }
  }

  sweep({label = '', negativeControl = false, images = false, world = !this.adapter.worldFrozen(this.core)} = {}) {
    this.freeze();
    const previous = this.records(0), current = this.records(1), stateBefore = this.state(), traceBefore = this.trace();
    const samples = [], statesAfter = [], pictures = [];
    for (const alpha of [0, .25, .5, .75, 1, .5, .5, 1]) {
      const frame = this.preview(alpha, {world, negativeControl});
      samples.push({...frame, alpha});
      statesAfter.push(this.state());
      if (images && pictures.length < 5) pictures.push(this.adapter.picture(this.runtime, frame, alpha));
    }
    const report = this.adapter.analyze({previous, current, samples, stateBefore, statesAfter,
      worldFrozen: !world, gate: this.adapter.gate(this.core), build: this.identity, label, negativeControl});
    report.traceBefore = traceBefore;
    report.traceAfter = this.trace();
    report.scene = this.adapter.scene(this.core, this.runtime);
    if (images) report.images = pictures;
    this.last = report;
    this.history.push(this.adapter.compact(report, {maxObjects: 96, images: false}));
    if (this.history.length > 16) this.history.shift();
    return report;
  }

  mark() {
    this.freeze();
    const timing = this.timing(), report = this.sweep({label: 'f8-incident', images: true});
    report.timing = timing;
    return report;
  }

  async scan({ticks = 180, stride = 6, negativeControl = false, onWindow = () => {}, signal} = {}) {
    if (!Number.isInteger(ticks) || ticks < 1 || ticks > 3600 || !Number.isInteger(stride) || stride < 1 || stride > 60 || Math.ceil(ticks / stride) > 120)
      throw Error('Bounded scan: 1..3600 ticks, stride 1..60, at most 120 windows');
    const reports = [];
    let completedTicks = 0;
    for (let n = 0; n < ticks; n += stride) {
      if (signal?.aborted) break;
      this.step(Math.min(stride, ticks - n));
      completedTicks = Math.min(n + stride, ticks);
      const report = this.sweep({label: `scan+${completedTicks}`, negativeControl});
      reports.push(this.adapter.compact(report, {maxObjects: 96, images: false}));
      onWindow(report);
      if (!report.purity) break;
      await new Promise(resolve => setTimeout(resolve, 0));
    }
    return {schema: this.adapter.scanSchema, build: this.identity, reports, completedTicks, windows: reports.length,
      groups: this.adapter.mergeIssues(reports), detailsBound: 96,
      stoppedForPurity: reports.some(report => !report.purity)};
  }

  export() {
    return {schema: this.adapter.sessionSchema, build: this.identity, recent: this.history,
      last: this.last ? this.adapter.compact(this.last) : null};
  }
}
