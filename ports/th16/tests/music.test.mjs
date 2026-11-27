// 乐曲命令队列对拍：0x45ed00 入队（0x1f 上限）、0x45e330 前段每次推进队头一步（命令 1 登记曲名槽 / 2 播放七步 / 3 停止 /
// 4 关闭 / 5 淡出 / 6 暂停 / 7 恢复 / 8 音量 / 9 换循环曲目）、0x45da30 按名查 thbgm.fmt——对串流播放器的调用序与 unicorn 原版一致
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { existsSync, readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join, resolve } from 'node:path';

const root = join(dirname(fileURLToPath(import.meta.url)), '..');
const harness = process.env.TH16_HARNESS || join(root, 'tests/build/harness');
const dat = process.env.TH16_DAT ?? resolve(root, '../../games/th16/original/th16.dat');
const golden = JSON.parse(readFileSync(join(root, 'tests/golden/music.json'), 'utf8'));
const need = { skip: !existsSync(dat) && 'th16.dat not present' };

for (const c of golden.cases) {
  test(`music queue matches the original: ${c.name}`, need, () => {
    const r = spawnSync(harness, ['music-script', dat, c.ops, ...c.args], { encoding: 'utf8' });
    assert.equal(r.status, 0, r.stderr || r.stdout);
    assert.deepEqual(r.stdout.trim().split('\n'), c.lines);
  });
}
