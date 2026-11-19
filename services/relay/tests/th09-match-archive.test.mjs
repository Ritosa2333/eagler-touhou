import assert from 'node:assert/strict';
import test from 'node:test';
import { decodeTh09MatchArchive, encodeTh09MatchArchive } from '../../../common/contracts/th09-match-archive.mjs';

function packet(name, version, size, frame) {
  const value = Buffer.alloc(size);
  value.write(name);
  value.writeUInt32LE(version, 4);
  value.writeUInt32LE(frame, 8);
  value.fill(17, 12, 24);
  return value;
}

function fixture() {
  const start = 17;
  const begin = packet('T9BG', 0x00020002, 102, start);
  begin.set([1, 2, 3, 4, 10, 10, 0, 0, 0, 0, 2, 0], 74);
  const left = packet('T9SP', 0x00020301, 46, start);
  const right = packet('T9SP', 0x00020302, 126, start + 1);
  for (const side of [46, 86]) right.writeFloatLE(-1, side + 28);
  const boundary = packet('T9RD', 0x00020001, 28, start + 1);
  boundary.writeUInt32LE(1, 24);
  return {start, frames:2, characters:[3, 4], winner:1, begin, left, right, boundary};
}

test('compacts repeated headers and restores the exact spectated inputs for either lane', () => {
  const {begin, left, right, boundary, ...match} = fixture();
  const history = [begin, left, boundary, right];
  const encoded = encodeTh09MatchArchive(history, match);
  assert.ok(encoded.length < history.reduce((size, chunk) => size + chunk.length, 0));
  const replay = decodeTh09MatchArchive(encoded);
  assert.deepEqual([...replay.packets()].map(Buffer.from), [left, boundary, right]);
  assert.deepEqual(replay.rounds, [0, 1]);
  assert.equal(replay.finish, match.start + match.frames);
  assert.equal(replay.winner, match.winner);
  assert.equal(encodeTh09MatchArchive([begin, right], match), null);
  assert.equal(encodeTh09MatchArchive(history, {...match, characters:[5, 6]}), null);
  assert.throws(() => decodeTh09MatchArchive(encoded.subarray(0, -1)), /Invalid th09/);
  right.writeFloatLE(Number.NaN, 46);
  assert.throws(() => decodeTh09MatchArchive(encodeTh09MatchArchive(history, match)), /Invalid th09/);
});
