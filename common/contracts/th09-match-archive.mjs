export const TH09_MATCH_ARCHIVE_LIMIT = 16 * 1024 * 1024;
export const TH09_MATCH_ARCHIVE_COMPRESSED_LIMIT = 1024 * 1024;

const headerSize = 8;
const archiveMagic = 0x41523954; // T9RA
const beginMagic = 0x47423954; // T9BG
const inputMagic = 0x50533954; // T9SP
const roundMagic = 0x44523954; // T9RD
const timingMagic = 0x4d543954; // T9TM
const invalid = () => { throw new Error('Invalid th09 match replay'); };
const view = bytes => new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
const sameBuild = (a, b) => a.subarray(12, 24).every((byte, i) => byte === b[12 + i]);
const between = (value, min, max) => value >= min && value <= max;

// Mirrors ChargeSample::valid for T9SP/2. A bad snapshot must not become
// a published file that the native spectator decoder would reject.
function validCharge(data, offset) {
  const charge = data.getFloat32(offset, true);
  const released = data.getFloat32(offset + 28, true);
  const events = data.getUint32(offset + 32, true);
  if (!between(charge, 0, 400) || !(released === -1 || between(released, 0, 400)) ||
      events > 7 || ((events & 2) !== 0) !== (released >= 0) || data.getUint32(offset + 36, true) > 1) return false;
  for (const timer of [offset + 4, offset + 16]) {
    if (!between(data.getInt32(timer, true), -999999, 100000000) ||
        !between(data.getFloat32(timer + 4, true), -1, 100000000) ||
        !between(data.getInt32(timer + 8, true), -1, 100000000)) return false;
  }
  return true;
}

// Only the self-contained spectator initializer can start a standalone file.
export function isTh09MatchInitializer(bytes) {
  if (bytes.length !== 102 || view(bytes).getUint32(0, true) !== beginMagic ||
      view(bytes).getUint32(4, true) !== 0x00020002) return false;
  const [fresh, difficulty, p1, p2, h1, h2, a1, a2, f1, f2, selector, extra] = bytes.subarray(74, 86);
  return fresh <= 1 && difficulty <= 3 && p1 < 16 && p2 < 16 &&
    h1 >= 1 && h1 <= 10 && h2 >= 1 && h2 <= 10 &&
    a1 <= 1 && a2 <= 1 && f1 <= 1 && f2 <= 1 && selector <= 17 && extra <= 9;
}

export function encodeTh09MatchArchive(history, {start, frames, winner, characters}) {
  if (frames < 1 || frames > 216000 || (winner !== 0 && winner !== 1)) return null;
  const end = start + frames;
  const first = history.findIndex(packet => isTh09MatchInitializer(packet) && view(packet).getUint32(8, true) === start);
  if (first < 0) return null;
  const begin = history[first];
  if (characters?.some((character, side) => character !== begin[76 + side])) return null;
  let next = start;
  let size = headerSize + begin.length;
  let last = first + 1;
  for (; last < history.length; ++last) {
    const packet = history[last];
    if (packet.length < 24) return null;
    const data = view(packet);
    const magic = data.getUint32(0, true);
    if (packet.length === 40 && magic === timingMagic) continue;
    const type = data.getUint32(4, true);
    if (next === end && magic !== roundMagic) break;
    if (data.getUint32(8, true) !== next || !sameBuild(begin, packet)) return null;
    if (magic === inputMagic && (packet.length === 46 && type === 0x00020301 || packet.length === 126 && type === 0x00020302)) ++next;
    else if (magic !== roundMagic || packet.length !== 28 || type !== 0x00020001) return null;
    size += packet.length - 23;
    if (size > TH09_MATCH_ARCHIVE_LIMIT) return null;
  }
  if (next !== end) return null;
  const result = new Uint8Array(size);
  view(result).setUint32(0, archiveMagic, true);
  result[4] = 1;
  result[5] = winner;
  result.set(begin, headerSize);
  let offset = headerSize + begin.length;
  for (let i = first + 1; i < last; ++i) {
    const packet = history[i];
    if (packet.length === 40) continue;
    result[offset++] = packet.length === 28 ? 3 : packet.length === 126 ? 2 : 1;
    result.set(packet.subarray(24), offset);
    offset += packet.length - 24;
  }
  return result;
}

export function decodeTh09MatchArchive(bytes) {
  if (bytes.byteLength < headerSize + 102 + 23 || bytes.byteLength > TH09_MATCH_ARCHIVE_LIMIT) invalid();
  const data = view(bytes);
  if (data.getUint32(0, true) !== archiveMagic || bytes[4] !== 1 || bytes[5] > 1 || bytes[6] || bytes[7]) invalid();
  const begin = bytes.subarray(headerSize, headerSize + 102);
  if (!isTh09MatchInitializer(begin)) invalid();
  const start = view(begin).getUint32(8, true);
  const rounds = [0];
  let finish = start;
  let inputCount = 0;
  let lastRound = 0;
  for (const {kind, offset} of records()) {
    if (kind === 3) {
      const round = data.getUint32(offset, true);
      if (round !== lastRound + 1 || round >= 10 || finish === start || rounds.at(-1) === finish - start) invalid();
      rounds.push(finish - start);
      lastRound = round;
    } else {
      for (const side of [2, 13]) {
        const mode = bytes[offset + side];
        const limit = mode >= 2 ? 4096 : 16;
        const x = data.getFloat32(offset + side + 1, true), y = data.getFloat32(offset + side + 5, true);
        if (mode > 3 || !(Math.abs(x) <= limit && Math.abs(y) <= limit)) invalid();
      }
      if (kind === 2 && (!validCharge(data, offset + 22) || !validCharge(data, offset + 62))) invalid();
      ++finish;
      ++inputCount;
    }
  }
  if (!inputCount || inputCount > 216000 || finish > 0xffffffff) invalid();
  return {begin, start, finish, rounds: rounds.filter(frame => frame < inputCount), winner: bytes[5],
    characters: [begin[76], begin[77]], difficulty: begin[75], packets};

  function* records() {
    for (let offset = headerSize + begin.length; offset < bytes.length;) {
      const kind = bytes[offset++];
      const size = kind === 1 ? 22 : kind === 2 ? 102 : kind === 3 ? 4 : 0;
      if (!size || offset + size > bytes.length) invalid();
      yield {kind, offset, size};
      offset += size;
    }
  }

  function* packets() {
    let frame = start;
    for (const {kind, offset, size} of records()) {
      const packet = new Uint8Array(24 + size);
      const header = view(packet);
      header.setUint32(0, kind === 3 ? roundMagic : inputMagic, true);
      header.setUint32(4, kind === 3 ? 0x00020001 : 0x00020300 + kind, true);
      header.setUint32(8, frame, true);
      packet.set(begin.subarray(12, 24), 12);
      packet.set(bytes.subarray(offset, offset + size), 24);
      if (kind !== 3) ++frame;
      yield packet;
    }
  }
}
