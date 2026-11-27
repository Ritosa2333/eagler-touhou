// 标题主菜单空闲 0x708 帧触发 demo（0x44af80）
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';

const root = join(dirname(fileURLToPath(import.meta.url)), '..');
const harness = process.env.TH16_HARNESS || join(root, 'tests/build/harness');

test('title-demo-idle: 0x708 idle cycles demo index %3 and sets flags bit6', () => {
  const r = spawnSync(harness, ['title-demo-idle'], { encoding: 'utf8' });
  assert.equal(r.status, 0, r.stderr || r.stdout);
  const line = r.stdout.trim();
  assert.match(line, /^demos=0,1,2,0 flags=/);
  const flags = Number.parseInt(line.split('flags=')[1], 16);
  assert.equal(flags & 0x40, 0x40);
});
