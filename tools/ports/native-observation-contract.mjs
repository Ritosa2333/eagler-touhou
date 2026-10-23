export const NATIVE_OBSERVATION_ABI = Object.freeze({
  version: 1,
  words: 12,
  missing: 0xffffffff,
  missingFloatBits: 0x7fc00000,
  index: Object.freeze({
    version: 0,
    validMask: 1,
    phase: 2,
    inputHeld: 3,
    worldFrame: 4,
    restartFrame: 5,
    pauseState: 6,
    multiplayerFrame: 7,
    playerX: 8,
    playerY: 9,
    applicationFrame: 10,
    localSeat: 11,
  }),
  valid: Object.freeze({
    phase: 0x001,
    input: 0x002,
    position: 0x004,
    worldFrame: 0x008,
    restartFrame: 0x010,
    pauseState: 0x020,
    multiplayerFrame: 0x040,
    applicationFrame: 0x080,
    localSeat: 0x100,
  }),
  phase: Object.freeze({
    title: 0,
    gameplay: 1,
    paused: 2,
    gameOverOrResults: 3,
    ending: 4,
    finishedOrExiting: 5,
    inactive: 6,
    transition: 7,
  }),
});

const {index, valid, phase, missing, missingFloatBits, words: wordCount} = NATIVE_OBSERVATION_ABI;
const allValidBits = Object.values(valid).reduce((all, bit) => all | bit, 0);

function floatFromBits(bits) {
  const bytes = new ArrayBuffer(4);
  const view = new DataView(bytes);
  view.setUint32(0, bits, true);
  return view.getFloat32(0, true);
}

/** Decode the stable 12-word thNN_native_observation_v1 result. */
export function decodeNativeObservation(input) {
  if (!input || input.length !== wordCount) {
    throw new TypeError(`Native observation ABI v1 requires exactly ${wordCount} words`);
  }
  const raw = Array.from(input, value => Number(value) >>> 0);
  if (raw[index.version] !== NATIVE_OBSERVATION_ABI.version) {
    throw new Error(`Unsupported native observation ABI version: ${raw[index.version]}`);
  }
  const mask = raw[index.validMask];
  if ((mask & ~allValidBits) !== 0) throw new Error(`Unknown native observation valid bits: 0x${(mask & ~allValidBits).toString(16)}`);

  const has = bit => (mask & bit) !== 0;
  const requireMissingWhenInvalid = (field, bit, absentValue = missing) => {
    if (has(bit) && raw[index[field]] === absentValue) {
      throw new Error(`Native observation field ${field} is missing despite its valid bit`);
    }
    if (!has(bit) && raw[index[field]] !== absentValue) {
      throw new Error(`Native observation field ${field} has data without its valid bit`);
    }
  };
  for (const [field, bit] of [
    ['phase', valid.phase], ['inputHeld', valid.input], ['worldFrame', valid.worldFrame],
    ['restartFrame', valid.restartFrame], ['pauseState', valid.pauseState],
    ['multiplayerFrame', valid.multiplayerFrame], ['applicationFrame', valid.applicationFrame],
    ['localSeat', valid.localSeat],
  ]) requireMissingWhenInvalid(field, bit);
  if (!has(valid.position) && (raw[index.playerX] !== missingFloatBits || raw[index.playerY] !== missingFloatBits)) {
    throw new Error('Native observation position has data without its valid bit');
  }

  const decoded = {
    version: raw[index.version],
    validMask: mask,
    phase: has(valid.phase) ? raw[index.phase] : null,
    inputHeld: has(valid.input) ? raw[index.inputHeld] : null,
    worldFrame: has(valid.worldFrame) ? raw[index.worldFrame] : null,
    restartFrame: has(valid.restartFrame) ? raw[index.restartFrame] : null,
    pauseState: has(valid.pauseState) ? raw[index.pauseState] : null,
    multiplayerFrame: has(valid.multiplayerFrame) ? raw[index.multiplayerFrame] : null,
    position: null,
    applicationFrame: has(valid.applicationFrame) ? raw[index.applicationFrame] : null,
    localSeat: has(valid.localSeat) ? raw[index.localSeat] : null,
  };
  if (has(valid.phase) && !Object.values(phase).includes(decoded.phase)) {
    throw new Error(`Invalid canonical native phase: ${decoded.phase}`);
  }
  if (has(valid.pauseState) && decoded.pauseState !== 0 && decoded.pauseState !== 1) {
    throw new Error(`Invalid native pause state: ${decoded.pauseState}`);
  }
  if (has(valid.position)) {
    const x = floatFromBits(raw[index.playerX]);
    const y = floatFromBits(raw[index.playerY]);
    if (!Number.isFinite(x) || !Number.isFinite(y)) throw new Error('Native observation position is not finite');
    decoded.position = {x, y};
  }
  return decoded;
}
