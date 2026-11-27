// ScreenEffect kind 4 / 6 / 7 更新链对拍（unicorn：0x45cbd0 / 0x45c9e0）
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';

const root = join(dirname(fileURLToPath(import.meta.url)), '..');
const harness = process.env.TH16_HARNESS || join(root, 'tests/build/harness');
const golden = JSON.parse(readFileSync(join(root, 'tests/golden/screen-fade-kind467.json'), 'utf8'));

function run(args) {
  const r = spawnSync(harness, args, { encoding: 'utf8' });
  assert.equal(r.status, 0, r.stderr || r.stdout);
  return r.stdout.trim().split('\n').filter(Boolean).map((line) => line.split(' ').map(Number));
}

test('screen-fade kind 4 (0x45cbd0) duration=8 hold=1 matches unicorn', () => {
  assert.deepEqual(run(['screen-fade', '4', '8', '0', '20', '1']), golden.kind4_d8_hold1);
});

test('screen-fade kind 6 (0x45c9e0) duration=10 flag=0 matches unicorn', () => {
  assert.deepEqual(run(['screen-fade', '6', '10', '0', '15', '0']), golden.kind6_d10_flag0);
});

test('screen-fade kind 6 (0x45c9e0) duration=10 flag=1 matches unicorn', () => {
  assert.deepEqual(run(['screen-fade', '6', '10', '0', '15', '1']), golden.kind6_d10_flag1);
});
