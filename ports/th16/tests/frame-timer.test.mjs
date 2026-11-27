// 0x4262f0 FrameTimer / 0x417bc0 spell time encode
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';

const root = join(dirname(fileURLToPath(import.meta.url)), '..');
const harness = process.env.TH16_HARNESS || join(root, 'tests/build/harness');

function run(args) {
  const r = spawnSync(harness, args, { encoding: 'utf8' });
  assert.equal(r.status, 0, r.stderr || r.stdout);
  return r.stdout.trim();
}

test('spell-time-encode roundtrips through 0x426840 checks', () => {
  for (const sec of ['0', '1.00', '12.34', '60.50', '999.99']) {
    const out = run(['spell-time-encode', sec]);
    assert.match(out, /ok=1/, out);
  }
});

test('0x417bc0 spell time: recorded into stage header slots by +0x88, read back in playback, 0x417530 rejects tampering', () => {
  const out = run(['spell-time-replay', '12.34']).split('\n');
  // 12.34 s + 0.0167 补帧 → 12 s 35；24.68 s + 0.0167 → 24 s 69
  assert.deepEqual(out, [
    'rec[0]=6907868 count=1', 'rec[1]=11509002 count=2',
    'play[0]=6907868 count=1', 'play[1]=11509002 count=2',
    'play-tampered[0]=112006532 count=1', 'play-tampered[1]=112006532 count=2',
  ]);
});

test('frame-timer 0x4262f0: 120 logic frames → ~60fps window', () => {
  const out = run(['frame-timer', '120']);
  const p = Object.fromEntries(out.split(/\s+/).map((kv) => kv.split('=')));
  assert.ok(Number(p.fps) > 55 && Number(p.fps) < 65, out);
  assert.ok(Number(p.total) >= 60, out);
});
