// TH19 v1.10c, 0x4026c0. This models one timer invocation, not when the
// gameplay scheduler invokes it. frame/previous are raw uint32 storage.
const lower = Math.fround(0.99), upper = Math.fround(1.01);
const uint32 = n => Number.isInteger(n) && n >= 0 && n <= 0xffffffff;
export function nativeTimerStep({ frame, previous, fraction, selector }, speed = null) {
  if (!uint32(frame) || !uint32(previous) || !uint32(selector) ||
      !Number.isFinite(fraction) || (speed !== null && !Number.isFinite(speed))) {
    throw Error('Invalid native timer state');
  }
  const scale = speed === null ? null : Math.fround(speed);
  const unit = scale === null || (scale > lower && scale < upper);
  const nextFraction = Math.fround(Math.fround(fraction) + (unit ? 1 : scale));
  // CVTTSS2SI returns integer-indefinite 0x80000000 for NaN/overflow.
  const truncated = Math.trunc(nextFraction);
  const nextFrame = unit ? (frame + 1) >>> 0 :
    !Number.isFinite(truncated) || truncated < -2147483648 || truncated > 2147483647
      ? 0x80000000 : truncated >>> 0;
  return { previous: frame, frame: nextFrame, fraction: nextFraction, selector: 0 };
}
