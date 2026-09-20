import {
  TICK_STATUS,
  validateObservationAdapter,
  validateReferencePair,
  validateRuntimeDriver,
  validateTickReceipt,
} from './contracts.mjs';

// Title-neutral orchestration. All runtime ABI and game semantics belong to
// the driver/observer pair supplied by the consumer.
export class PresentationLabControllerCore {
  constructor({driver, observer, identity = {}}) {
    this.driver = driver;
    this.observer = validateObservationAdapter(observer);
    this.capabilities = validateRuntimeDriver(driver);
    this.identity = {...identity, game: this.capabilities.game,
      adapterVersion: this.capabilities.adapterVersion,
      nativeAbi: this.capabilities.nativeAbi};
    this.history = [];
    this.last = null;
    this.running = false;
    this.keys = new Set();
    this.freezeToken = null;
    this.observer.enable(true);
  }

  references() { return validateReferencePair(this.observer.readReferences()); }
  records(reference = null) {
    if (reference === null) return this.observer.readObservation();
    const pair = this.references();
    if (reference === 0) return pair.previous;
    if (reference === 1) return pair.current;
    throw Error('Presentation Lab reference must be 0 or 1');
  }
  state() { return this.observer.readStateEvidence(); }
  timing() { return this.observer.readTiming(); }
  trace() { return this.observer.readTrace(); }

  freeze() {
    if (!this.freezeToken) this.freezeToken = this.driver.freeze();
    this.running = false;
    return this.freezeToken;
  }

  play() {
    this.observer.setNegativeControl(false);
    const token = this.freeze();
    this.driver.resume(token);
    this.freezeToken = null;
    this.running = true;
  }

  key(code, down) {
    this.driver.setInput({code, down: !!down});
    if (down) this.keys.add(code); else this.keys.delete(code);
  }
  clearKeys() { this.driver.clearInput(); this.keys.clear(); }

  step(count = 1) {
    this.freeze();
    if (!Number.isInteger(count) || count < 1 || count > 600) throw Error('Step size must be 1..600');
    for (let i = 0; i < count; i++) {
      const receipt = validateTickReceipt(this.driver.advanceOneTick());
      if (receipt.status !== TICK_STATUS.ADVANCED)
        throw Error(`Game tick did not advance: ${receipt.status}${receipt.detail ? ` (${receipt.detail})` : ''}`);
    }
    return this.records(1);
  }

  press(code, wait = 40) { this.key(code, true); this.step(3); this.key(code, false); this.step(wait); }

  preview(alpha, {world = !this.observer.worldFrozen(), negativeControl = false} = {}) {
    this.freeze();
    this.observer.setNegativeControl(negativeControl);
    try {
      const value = Number(alpha);
      if (!Number.isFinite(value) || value < 0 || value > 1) throw Error('Invalid presentation alpha');
      const receipt = this.driver.drawOnly({alpha: value, world: !!world});
      if (!receipt || receipt.status !== 'drawn')
        throw Error(`Presentation draw failed: ${receipt?.status || 'invalid receipt'}`);
      return this.records();
    } finally { this.observer.setNegativeControl(false); }
  }

  sweep({label = '', negativeControl = false, images = false, world = !this.observer.worldFrozen()} = {}) {
    this.freeze();
    const {previous, current} = this.references();
    const stateBefore = this.state(), traceBefore = this.trace();
    const samples = [], statesAfter = [], pictures = [];
    for (const alpha of [0, .25, .5, .75, 1, .5, .5, 1]) {
      const frame = this.preview(alpha, {world, negativeControl});
      samples.push({...frame, alpha});
      statesAfter.push(this.state());
      if (images && pictures.length < 5) pictures.push(this.observer.captureImage(frame, alpha));
    }
    const report = this.observer.analyze({previous, current, samples, stateBefore, statesAfter,
      worldFrozen: !world, gate: this.observer.readGate(), build: this.identity, label, negativeControl});
    report.traceBefore = traceBefore;
    report.traceAfter = this.trace();
    report.scene = this.observer.readScene();
    if (images) report.images = pictures;
    this.last = report;
    this.history.push(this.observer.compact(report, {maxObjects: 96, images: false}));
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
      reports.push(this.observer.compact(report, {maxObjects: 96, images: false}));
      onWindow(report);
      if (report.purity === false) break;
      await new Promise(resolve => setTimeout(resolve, 0));
    }
    return {schema: this.observer.scanSchema, build: this.identity, reports, completedTicks, windows: reports.length,
      groups: this.observer.mergeIssues(reports), detailsBound: 96,
      stoppedForPurity: reports.some(report => report.purity === false)};
  }

  export() {
    return {schema: this.observer.sessionSchema, build: this.identity, recent: this.history,
      last: this.last ? this.observer.compact(this.last) : null};
  }

  close() {
    this.clearKeys();
    this.observer.enable(false);
    return this.driver.close();
  }
}
