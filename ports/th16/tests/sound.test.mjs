// 音效请求队列对拍：0x45e150 / 0x45e1f0 / 0x45e2a0 登记、0x45e330 尾段消费（同声部合并、声像取平均、12 槽 / 60 声像上限）、
// 0x45e8d0 重播（音量曲线）、0x42e4d0 暂停 / 0x440bd0 恢复——逐条 IDirectSoundBuffer 调用与 unicorn 原版一致
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';

const root = join(dirname(fileURLToPath(import.meta.url)), '..');
const harness = process.env.TH16_HARNESS || join(root, 'tests/build/harness');
const golden = JSON.parse(readFileSync(join(root, 'tests/golden/sound.json'), 'utf8'));

for (const c of golden.cases) {
  test(`sound queue matches the original: ${c.name}`, () => {
    const r = spawnSync(harness, ['sound-script', c.ops, ...c.args], { encoding: 'utf8' });
    assert.equal(r.status, 0, r.stderr || r.stdout);
    assert.deepEqual(r.stdout.trim().split('\n'), c.lines);
  });
}
