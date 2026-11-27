// Replay 0x448e40 快进 Restart / 0x4482f0 FPS 颜色（unicorn + decomp）
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';

const root = join(dirname(fileURLToPath(import.meta.url)), '..');
const harness = process.env.TH16_HARNESS || join(root, 'tests/build/harness');
const golden = JSON.parse(readFileSync(join(root, 'tests/golden/replay-ff.json'), 'utf8'));

function run(args) {
  const r = spawnSync(harness, args, { encoding: 'utf8' });
  assert.equal(r.status, 0, r.stderr || r.stdout);
  return r.stdout.trim();
}

test('replay-ff 0x448e40 matches unicorn for counter/gates', () => {
  for (const c of golden.fast_forward) {
    const out = run([
      'replay-ff', String(c.mode), String(c.counter), `0x${Number(c.held).toString(16)}`,
      String(c.gm), String(c.loading),
    ]);
    assert.equal(Number(out), c.eax, JSON.stringify(c));
  }
});

test('replay FPS reads the sample before advancing at frames 0, 30, and 60', () => {
  const samples = run(['replay-fps-timing']).split('\n').map(Number);
  assert.deepEqual(samples, [60, ...Array(30).fill(59), ...Array(31).fill(58)]);
});

test('replay-fps-color 0x4482f0 thresholds', () => {
  for (const [byte, color] of Object.entries(golden.fps_colors)) {
    assert.equal(Number(run(['replay-fps-color', byte])), color, `fps=${byte}`);
  }
});

test('global-fps-color 0x4263e0 thresholds 30/40', () => {
  assert.equal(Number(run(['global-fps-color', '29'])), 0xff5050ff);
  assert.equal(Number(run(['global-fps-color', '30'])), 0xffa0a0ff);
  assert.equal(Number(run(['global-fps-color', '39.9'])), 0xffa0a0ff);
  assert.equal(Number(run(['global-fps-color', '40'])), 0xffffffff);
});
