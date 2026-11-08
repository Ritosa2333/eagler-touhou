import test from 'node:test';
import assert from 'node:assert/strict';
import { alignReplay, convertWebToJunowen } from './align-replay.mjs';
import { encodeJunowen } from '../../ports/th19/tools/replay/junowen-format.mjs';
import { encodeTh19Replay, th19StreamBegin, th19StreamFrame, th19StreamEnd } from '../../ports/th19/web/sdl-runtime/th19-recording.mjs';

const config = { protocol: 1, build: 'a'.repeat(64), seed: 1, difficulty: 1, characters: [0, 1], delay: 0, rollback: true, rules: [0, 1, 1, 4, 4, 1, 1, 3, 5] };
function webReplay(keys) {
  const packets = [th19StreamBegin(config)];
  for (let frame = 0; frame < keys.length; frame++) packets.push(th19StreamFrame(frame, { keys: keys[frame], direct: 0, x: 0, y: 0 }, { keys: 0, direct: 0, x: 0, y: 0 }, { played: true, hash: frame }));
  packets.push(th19StreamEnd(keys.length, 0));
  const payload = packets.slice(1, -1).reduce((b, p) => { const n = new Uint8Array(b.length + p.length); n.set(b); n.set(p, b.length); return n; }, new Uint8Array());
  return encodeTh19Replay({ kind: 'versus', game: 'th19', build: config.build, seed: config.seed, config, frames: keys.length, startedAt: 1, names: ['p1', 'p2'], partial: false }, payload);
}
const header = { randSeed1: 1, randSeed2: 2, difficulty: 1, matchup: 0, common: 0, p1Settings: 0, p2Settings: 0, p1Character: 0, p1Card: 0, p2Character: 1, p2Card: 0 };

test('aligns web held keys to native key bits frame by frame', () => {
  const web = webReplay([1, 1024, 1025]);
  const native = encodeJunowen({ header, inputs: [[1, 0], [256, 0], [257, 0]] });
  const report = alignReplay(web, native);
  assert.equal(report.aligned, true);
  assert.equal(report.compared, 6);
});

test('reports frame and key divergence without starting a player', () => {
  const report = alignReplay(webReplay([1]), encodeJunowen({ header, inputs: [[2, 0]] }));
  assert.equal(report.aligned, false);
  assert.deepEqual(report.mismatches[0], { frame: 0, seat: 0, webKeys: 1, expectedNative: 1, nativeKeys: 2 });
});

test('converts web replay to Ju.N.Owen 17-byte file layout', async () => {
  const web = webReplay([1, 1024, 1025]);
  const converted = convertWebToJunowen(web);
  assert.equal(converted.bytes.length, 17 + 3 * 4);
  const report = alignReplay(web, converted.bytes);
  assert.equal(report.aligned, true);
  assert.equal(converted.header.randSeed1, 1);
  assert.equal(converted.seed2Fallback, true);
});

test('rejects touch displacement that native replay cannot encode', () => {
  const packets = [th19StreamBegin(config), th19StreamFrame(0, { keys: 1, direct: 1, x: 4, y: 5 }, { keys: 0, direct: 0, x: 0, y: 0 }, { played: true, hash: 1 })];
  const payload = packets[1].slice();
  const web = encodeTh19Replay({ kind: 'versus', game: 'th19', build: config.build, seed: config.seed, config, frames: 1, startedAt: 1, names: ['p1', 'p2'], partial: false }, payload);
  assert.throws(() => convertWebToJunowen(web), /touch movement/);
});
