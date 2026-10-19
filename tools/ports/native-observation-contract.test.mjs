import test from 'node:test';
import assert from 'node:assert/strict';
import {decodeNativeObservation, NATIVE_OBSERVATION_ABI as abi} from './native-observation-contract.mjs';

function words() {
  const value = new Uint32Array(abi.words).fill(abi.missing);
  value[abi.index.playerX] = value[abi.index.playerY] = abi.missingFloatBits;
  return value;
}

test('decodes independently valid fields and IEEE-754 coordinates', () => {
  const value = words();
  value[abi.index.version] = 1;
  value[abi.index.validMask] = abi.valid.phase | abi.valid.position | abi.valid.pauseState | abi.valid.applicationFrame;
  value[abi.index.phase] = abi.phase.gameplay;
  value[abi.index.pauseState] = 0;
  value[abi.index.playerX] = 0x3f800000;
  value[abi.index.playerY] = 0xc0000000;
  value[abi.index.applicationFrame] = 37;
  assert.deepEqual(decodeNativeObservation(value), {
    version: 1, validMask: value[1], phase: 1, inputHeld: null, worldFrame: null,
    restartFrame: null, pauseState: 0, multiplayerFrame: null,
    position: {x: 1, y: -2}, applicationFrame: 37, localSeat: null,
  });
});

test('preserves transition as a distinct phase while gameplay-only input stays unavailable', () => {
  const value = words();
  value[0] = 1;
  value[1] = abi.valid.phase | abi.valid.pauseState;
  value[abi.index.phase] = abi.phase.transition;
  value[abi.index.pauseState] = 0;
  const decoded = decodeNativeObservation(value);
  assert.equal(decoded.phase, abi.phase.transition);
  assert.equal(decoded.inputHeld, null);
  assert.equal(decoded.pauseState, 0);
});

test('rejects unsupported versions and unknown v1 validity bits', () => {
  const value = words(); value[0] = 2; value[1] = 0;
  assert.throws(() => decodeNativeObservation(value), /Unsupported native observation ABI version/);
  value[0] = 1; value[1] = 0x200;
  assert.throws(() => decodeNativeObservation(value), /Unknown native observation valid bits/);
});

test('rejects a field populated without its valid bit', () => {
  const value = words(); value[0] = 1; value[1] = 0; value[abi.index.worldFrame] = 4;
  assert.throws(() => decodeNativeObservation(value), /without its valid bit/);
});

test('rejects malformed phases, pause states, positions, and incomplete layouts', () => {
  const invalidPhase = words(); invalidPhase[0] = 1; invalidPhase[1] = abi.valid.phase; invalidPhase[2] = 8;
  assert.throws(() => decodeNativeObservation(invalidPhase), /Invalid canonical native phase/);
  const invalidPause = words(); invalidPause[0] = 1; invalidPause[1] = abi.valid.pauseState; invalidPause[6] = 2;
  assert.throws(() => decodeNativeObservation(invalidPause), /Invalid native pause state/);
  const invalidPosition = words(); invalidPosition[0] = 1; invalidPosition[1] = abi.valid.position;
  assert.throws(() => decodeNativeObservation(invalidPosition), /position is not finite/);
  assert.throws(() => decodeNativeObservation(words().subarray(0, 11)), /requires exactly 12 words/);
});
