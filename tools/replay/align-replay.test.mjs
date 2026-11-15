import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { alignReplay, convertWebToJunowen } from './align-replay.mjs';
import { encodeJunowen } from '../../ports/th19/tools/replay/junowen-format.mjs';
import { encodeTh19Replay, th19StreamFrame } from '../../ports/th19/web/sdl-runtime/th19-recording.mjs';

const config = { protocol: 1, build: 'a'.repeat(64), seed: 1, difficulty: 1, characters: [0, 1], delay: 0, rollback: true, rules: [0, 1, 1, 4, 4, 1, 1, 3, 5] };
function webReplay(keys, rightKeys = []) {
  const input = keys => ({ keys, direct: 0, x: 0, y: 0 });
  const payload = Buffer.concat(keys.map((left, frame) =>
    th19StreamFrame(frame, input(left), input(rightKeys[frame] ?? 0), { played: true, hash: frame })));
  return encodeTh19Replay({ kind: 'versus', game: 'th19', build: config.build, seed: config.seed, config, frames: keys.length, startedAt: 1, names: ['p1', 'p2'], partial: false }, payload);
}
const header = { randSeed1: 1, randSeed2: 2, difficulty: 1, matchup: 0, common: 0, p1Settings: 0, p2Settings: 0, p1Character: 0, p1Card: 0, p2Character: 1, p2Card: 0 };

test('aligns web held keys to native key bits frame by frame', () => {
  const web = webReplay([1, 1024, 1025]);
  const native = encodeJunowen({ header, inputs: [[1, 0], [256, 0], [257, 0]] });
  const report = alignReplay(web, native);
  assert.equal(report.aligned, true);
  assert.deepEqual(report.comparedSeats, [0, 1]);
  assert.equal(report.compared, 6);
});

test('reports unmapped web bits alongside mapped key divergences', () => {
  const web = webReplay([256, 1025], [512, 0]);
  const native = encodeJunowen({ header, inputs: [[0, 0], [1, 0]] });
  const report = alignReplay(web, native);
  assert.equal(report.aligned, false);
  assert.equal(report.compared, 4);
  assert.ok(report.blockers.includes('web-unsupported-key-bits'));
  assert.deepEqual(report.mismatches, [
    { frame: 0, seat: 0, webKeys: 256, expectedNative: null, nativeKeys: 0, unsupportedWebKeyBits: 256 },
    { frame: 0, seat: 1, webKeys: 512, expectedNative: null, nativeKeys: 0, unsupportedWebKeyBits: 512 },
    { frame: 1, seat: 0, webKeys: 1025, expectedNative: 257, nativeKeys: 1 },
  ]);
});

test('counts omitted mismatches even when no details are requested', () => {
  const web = webReplay([1, 512], [4, 0]);
  const native = encodeJunowen({ header, inputs: [[0, 0], [0, 0]] });
  for (const maxMismatches of [0, 1, 3, 4]) {
    const report = alignReplay(web, native, { maxMismatches });
    assert.equal(report.aligned, false);
    assert.equal(report.mismatches.length, Math.min(maxMismatches, 3));
    assert.equal(report.truncatedMismatchCount, 3 - report.mismatches.length);
  }
  assert.throws(() => alignReplay(web, native, { maxMismatches: -1 }), RangeError);
});

test('human-vs-CPU compares only P1 and reports a frame-count difference', () => {
  const web = webReplay([1, 2], [4, 8]);
  const native = inputs => encodeJunowen({ header: { ...header, matchup: 1 }, inputs });
  const report = alignReplay(web, native([[1], [2]]));
  assert.equal(report.aligned, true);
  assert.deepEqual(report.comparedSeats, [0]);
  assert.equal(report.compared, 2);

  const shorter = alignReplay(web, native([[1]]), { maxMismatches: 0 });
  assert.equal(shorter.aligned, false);
  assert.equal(shorter.webFrames, 2);
  assert.equal(shorter.nativeFrames, 1);
  assert.equal(shorter.truncatedMismatchCount, 0);
});

test('youkai-vs-youkai compares both seats', () => {
  const web = webReplay([1], [2]);
  const native = encodeJunowen({ header: { ...header, matchup: 3 }, inputs: [[1, 4]] });
  const report = alignReplay(web, native);
  assert.deepEqual(report.comparedSeats, [0, 1]);
  assert.equal(report.aligned, false);
  assert.equal(report.mismatches[0].seat, 1);
});

test('rejects CPU-vs-CPU before interpreting either seat layout', () => {
  const web = webReplay([1, 2]);
  const native = encodeJunowen({ header: { ...header, matchup: 1 }, inputs: [[1], [2]] });
  native[9] = 2; // The CPU-vs-CPU recorder writes the same one-seat payload.
  for (const bytes of [native, native.subarray(0, native.length - 2)])
    assert.throws(() => alignReplay(web, bytes), /Ambiguous Ju.N.Owen CPU-vs-CPU layout/);
});

test('CLI writes mismatch reports and protects existing files', t => {
  const dir = mkdtempSync(join(tmpdir(), 'align-replay-'));
  t.after(() => rmSync(dir, { recursive: true, force: true }));
  const webPath = join(dir, 'web.rpy');
  const nativePath = join(dir, 'native.rpy');
  const reportPath = join(dir, 'report.json');
  const web = webReplay([256]);
  writeFileSync(webPath, web);
  writeFileSync(nativePath, encodeJunowen({ header, inputs: [[0, 0]] }));
  const script = fileURLToPath(new URL('./align-replay.mjs', import.meta.url));
  const run = path => spawnSync(process.execPath, [
    script, '--web', webPath, '--junowen', nativePath, '--report', path,
  ], { encoding: 'utf8' });

  const result = run(reportPath);
  assert.equal(result.status, 2, result.stderr);
  const report = JSON.parse(readFileSync(reportPath, 'utf8'));
  assert.equal(report.mismatches[0].unsupportedWebKeyBits, 256);
  assert.equal(run(webPath).status, 1);
  assert.deepEqual(readFileSync(webPath), Buffer.from(web));

  const convertible = webReplay([1]);
  writeFileSync(webPath, convertible);
  const conversion = spawnSync(process.execPath, [
    script, '--convert', '--web', webPath, '--out', webPath,
  ], { encoding: 'utf8' });
  assert.equal(conversion.status, 1);
  assert.deepEqual(readFileSync(webPath), Buffer.from(convertible));
});

test('reports frame and key divergence without starting a player', () => {
  const report = alignReplay(webReplay([1]), encodeJunowen({ header, inputs: [[2, 0]] }));
  assert.equal(report.aligned, false);
  assert.deepEqual(report.mismatches[0], { frame: 0, seat: 0, webKeys: 1, expectedNative: 1, nativeKeys: 2 });
});

test('converts web replay to Ju.N.Owen 17-byte file layout', () => {
  const web = webReplay([1, 1024, 1025]);
  const converted = convertWebToJunowen(web);
  assert.equal(converted.bytes.length, 17 + 3 * 4);
  const report = alignReplay(web, converted.bytes);
  assert.equal(report.aligned, true);
  assert.equal(converted.header.randSeed1, 1);
  assert.equal(converted.seed2Fallback, true);
});

test('rejects touch displacement that native replay cannot encode', () => {
  const payload = th19StreamFrame(0, { keys: 1, direct: 1, x: 4, y: 5 }, { keys: 0, direct: 0, x: 0, y: 0 }, { played: true, hash: 1 });
  const web = encodeTh19Replay({ kind: 'versus', game: 'th19', build: config.build, seed: config.seed, config, frames: 1, startedAt: 1, names: ['p1', 'p2'], partial: false }, payload);
  assert.throws(() => convertWebToJunowen(web), /touch movement/);
});
