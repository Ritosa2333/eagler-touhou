import assert from 'node:assert/strict';
import test from 'node:test';
import { gunzipSync } from 'node:zlib';
import { decodeTh09MatchArchive, encodeTh09MatchArchive } from '../../../common/contracts/th09-match-archive.mjs';
import { createTh09MatchArchiveQueue } from '../src/th09-match-archive.mjs';

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

test('archives a confirmed result once after a late input tail arrives', async () => {
  const {begin, left, right, boundary, ...match} = fixture();
  const run = {spectatorTimeline:true, spectatorNextFrame:match.start+1, spectatorHistory:[begin, left, boundary]};
  const report = {...match, duelId:'duel', roundId:'1:'+match.start, players:['left','right']};
  const saved = [];
  const archive = createTh09MatchArchiveQueue({enqueue: value => saved.push(value)});
  archive(run, report);
  assert.equal(saved.length, 0);
  run.spectatorHistory.push(right);
  ++run.spectatorNextFrame;
  archive(run);
  archive(run);
  const end = Date.now() + 3000;
  while (!saved.length && Date.now() < end) await new Promise(resolve => setTimeout(resolve, 20));
  assert.equal(saved.length, 1);
  assert.equal(saved[0].roundId, report.roundId);
  const replay = decodeTh09MatchArchive(gunzipSync(Buffer.from(saved[0].replay, 'base64')));
  assert.deepEqual([...replay.packets()].map(Buffer.from), [left, boundary, right]);
});
