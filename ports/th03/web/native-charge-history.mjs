// Charge samples are owned by the player who produced them. Once published,
// neither a rollback nor a retransmission may change an accepted sample.
export function validChargeSample(value) {
  return Array.isArray(value) && value.length === 4 &&
    value.every(Number.isSafeInteger) &&
    value[0] >= 0 && value[0] <= 4080 &&
    value[1] >= 0 && value[1] <= 255 &&
    value[2] >= -1 && value[2] <= 4080 &&
    value[3] >= 0 && value[3] <= 7 &&
    ((value[3] & 1) !== 0) === (value[2] >= 0) &&
    ((value[3] & 4) !== 0 || (value[0] === 0 && value[1] === 0 && value[2] === -1 && value[3] === 0));
}

const same = (a, b) => a.length === b.length && a.every((value, i) => value === b[i]);

export class NativeChargeHistory {
  constructor(window = 12) {
    if (!Number.isSafeInteger(window) || window < 1 || window > 256) throw Error('Invalid charge history window');
    this.window = window;
    this.confirmed = 0;
    this.actual = new Map();
    this.used = new Map();
  }
  accept(frame, sample, currentFrame) {
    if (!Number.isSafeInteger(frame) || frame < 0 || !Number.isSafeInteger(currentFrame) ||
        currentFrame < this.confirmed || frame > currentFrame + this.window || !validChargeSample(sample)) {
      throw Error('Invalid charge sample');
    }
    if (frame < this.confirmed) return false;
    const previous = this.actual.get(frame);
    if (previous && !same(previous, sample)) throw Error('Conflicting charge sample');
    if (!previous) this.actual.set(frame, Object.freeze([...sample]));
    const applied = this.used.get(frame);
    return !!applied && !same(applied, sample);
  }
  markUsed(frame, sample) {
    if (!Number.isSafeInteger(frame) || frame < this.confirmed || !validChargeSample(sample)) throw Error('Invalid applied charge sample');
    this.used.set(frame, Object.freeze([...sample]));
  }
  get(frame) { return this.actual.get(frame); }
  rewind(frame) {
    if (!Number.isSafeInteger(frame) || frame < this.confirmed) throw Error('Invalid charge rewind');
    for (const key of this.used.keys()) if (key >= frame) this.used.delete(key);
  }
  confirm(frame) {
    if (frame !== this.confirmed || !this.actual.has(frame)) throw Error('Charge sample is not confirmed');
    this.actual.delete(frame); this.used.delete(frame); this.confirmed++;
  }
  skip(frame) {
    if (frame !== this.confirmed) throw Error('Invalid charge history position');
    this.actual.delete(frame); this.used.delete(frame); this.confirmed++;
  }
  clear() { this.actual.clear(); this.used.clear(); }
}
