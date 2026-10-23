import assert from 'node:assert/strict';

function bytesOf(value, label) {
  assert(ArrayBuffer.isView(value), `${label} must be an ArrayBuffer view`);
  return new Uint8Array(value.buffer, value.byteOffset, value.byteLength);
}

/** Round finite values to the nearest integer, resolving half-way cases to even. */
export function roundToNearestEven(value) {
  if (!Number.isFinite(value)) throw new RangeError('roundToNearestEven requires a finite value');
  const lower = Math.floor(value);
  const fraction = value - lower;
  if (fraction < 0.5) return lower;
  if (fraction > 0.5) return lower + 1;
  return lower % 2 === 0 ? lower : lower + 1;
}

function snapPresentationCoordinate(candidate) {
  return Math.fround(roundToNearestEven(Math.fround(candidate + 0.5)) - 0.5);
}

/** Compare screen-space float32 XY with exact nearest-even raster-grid fallback. */
export function assertScreenVertexGridOrEqual(candidateValue, nativeValue, {stride = 28, label = 'screen vertices'} = {}) {
  const candidate = bytesOf(candidateValue, 'candidate vertices');
  const native = bytesOf(nativeValue, 'native vertices');
  assert(Number.isInteger(stride) && stride >= 8, `${label}: vertex stride must include float32 XY`);
  assert(candidate.byteLength > 0, `${label}: no drawable vertices`);
  assert.equal(candidate.byteLength % stride, 0, `${label}: candidate byte length must use ${stride}-byte vertices`);
  assert.equal(native.byteLength, candidate.byteLength, `${label}: vertex byte lengths differ`);
  const candidateView = new DataView(candidate.buffer, candidate.byteOffset, candidate.byteLength);
  const nativeView = new DataView(native.buffer, native.byteOffset, native.byteLength);
  for (let base = 0; base < candidate.byteLength; base += stride) {
    const vertex = base / stride;
    for (const [axis, offset] of [['x', 0], ['y', 4]]) {
      const candidateBytes = candidate.subarray(base + offset, base + offset + 4);
      const nativeBytes = native.subarray(base + offset, base + offset + 4);
      if (Buffer.from(candidateBytes).equals(Buffer.from(nativeBytes))) continue;
      const actual = candidateView.getFloat32(base + offset, true);
      const expected = nativeView.getFloat32(base + offset, true);
      const snapped = snapPresentationCoordinate(actual);
      assert(Object.is(snapped, expected),
        `${label}: vertex ${vertex} ${axis} candidate ${actual} snaps to ${snapped}, native is ${expected}`);
    }
    assert.deepEqual(candidate.subarray(base + 8, base + stride), native.subarray(base + 8, base + stride),
      `${label}: vertex ${vertex} non-position bytes differ`);
  }
  return {vertices: candidate.byteLength / stride, relation: 'exact-or-nearest-even-half-pixel-grid'};
}
