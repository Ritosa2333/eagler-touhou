import test from 'node:test';
import assert from 'node:assert/strict';
import {assertScreenVertexGridOrEqual, roundToNearestEven} from './display-grid-evidence.mjs';

function vertices(count = 2) {
  const bytes = Buffer.alloc(count * 28);
  for (let i = 0; i < count; i++) {
    const base = i * 28;
    bytes.writeFloatLE(10.5 + i * 8, base);
    bytes.writeFloatLE(20.5 + i * 8, base + 4);
    bytes.writeFloatLE(0.125, base + 8);
    bytes.writeFloatLE(1, base + 12);
    bytes.writeUInt32LE(0xff123456, base + 16);
    bytes.writeFloatLE(0.25, base + 20);
    bytes.writeFloatLE(0.75, base + 24);
  }
  return bytes;
}

test('nearest-even rounds positive and negative half ties', () => {
  assert.equal(roundToNearestEven(0.5), 0);
  assert.equal(roundToNearestEven(1.5), 2);
  assert.equal(roundToNearestEven(2.5), 2);
  assert.equal(roundToNearestEven(-0.5), 0);
  assert.equal(roundToNearestEven(-1.5), -2);
  assert.equal(roundToNearestEven(-2.5), -2);
});

test('a quarter-pixel X movement at the second vertex is allowed by the native grid relation', () => {
  const native = vertices();
  const candidate = Buffer.from(native);
  candidate.writeFloatLE(Math.fround(native.readFloatLE(28) + 0.25), 28);
  assert.deepEqual(assertScreenVertexGridOrEqual(candidate, native), {
    vertices: 2,
    relation: 'exact-or-nearest-even-half-pixel-grid',
  });
});

test('a one-pixel second-vertex error is rejected', () => {
  const native = vertices();
  const candidate = Buffer.from(native);
  candidate.writeFloatLE(native.readFloatLE(28) + 1, 28);
    assert.throws(() => assertScreenVertexGridOrEqual(candidate, native), /vertex 1 x/);
});

test('Z, RHW, color, and UV remain byte-exact at the second vertex', () => {
  for (const [name, offset, mutate] of [
    ['Z', 8, bytes => bytes.writeFloatLE(0.25, 28 + 8)],
    ['RHW', 12, bytes => bytes.writeFloatLE(2, 28 + 12)],
    ['color', 16, bytes => bytes.writeUInt32LE(0xff123457, 28 + 16)],
    ['UV', 20, bytes => bytes.writeFloatLE(0.5, 28 + 20)],
  ]) {
    const native = vertices();
    const candidate = Buffer.from(native);
    mutate(candidate);
    assert.throws(() => assertScreenVertexGridOrEqual(candidate, native), /vertex 1 non-position bytes differ/, name);
  }
});

test('typed views honor a non-zero byteOffset', () => {
  const nativeBacking = Buffer.alloc(9 + 56 + 5);
  const candidateBacking = Buffer.alloc(11 + 56 + 3);
  const native = vertices();
  const candidate = Buffer.from(native);
  candidate.writeFloatLE(candidate.readFloatLE(28) + 0.25, 28);
  native.copy(nativeBacking, 9);
  candidate.copy(candidateBacking, 11);
  const result = assertScreenVertexGridOrEqual(
    new Uint8Array(candidateBacking.buffer, candidateBacking.byteOffset + 11, 56),
    new Uint8Array(nativeBacking.buffer, nativeBacking.byteOffset + 9, 56),
  );
  assert.equal(result.vertices, 2);
});

test('presentation transform resolves positive and negative ties with nearest-even', () => {
  const native = vertices(4);
  const candidate = Buffer.from(native);
  const expectedX = [1.5, 2.5, -1.5, -2.5];
  const actualX = [1.75, 2.75, -1.25, -2.25];
  for (let i = 0; i < expectedX.length; i++) {
    native.writeFloatLE(expectedX[i], i * 28);
    candidate.writeFloatLE(actualX[i], i * 28);
  }
  assert.equal(assertScreenVertexGridOrEqual(candidate, native).vertices, 4);
});
