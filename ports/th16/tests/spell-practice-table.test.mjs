// 0x4560b0 难度重排 + 符卡练习最高分 0x970
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { test } from 'node:test';
import { resolve } from 'node:path';

const harness = process.env.TH16_HARNESS ?? resolve('tests/build/harness');
const run = (...args) => spawnSync(harness, args, { encoding: 'utf8' }).stdout.trim();

test('spell-table: group2 slot0 remaps by difficulty 2/3', () => {
  assert.equal(run('spell-table').split('\n')[0], 'g2s0 -2 -2 20 21 -2');
});

test('spell-table: locked slots are -1', () => {
  assert.equal(run('spell-table', 'lock'), 'g2s0 -2 -2 -1 -1 -2');
});

test('spell-table: high score floors to /10*10 and only rises', () => {
  const lines = run('spell-table').trim().split('\n');
  assert.equal(lines[1], 'high 999990 caps 2/5');
});
