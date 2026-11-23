// 关卡覆盖矩阵：每面 4 难度 × 4 机体 × 4 子季节，600 帧摘要对照
// tools/oracle/game-oracle.py（Unicorn 直跑原版）。黄金按面分文件：golden/stage-matrix-stN.json。
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {existsSync, readdirSync, readFileSync} from 'node:fs';
import {resolve} from 'node:path';

const here = resolve(import.meta.dirname);
const monorepo = resolve(here, '../../..');
const harness = process.env.TH16_HARNESS ?? resolve(here, 'build/harness');
const dat = process.env.TH16_DAT ?? resolve(monorepo, 'games/th16/original/th16.dat');
const needDat = {skip: !existsSync(dat) && 'original th16.dat not present'};
const goldenDir = resolve(here, 'golden');
const files = readdirSync(goldenDir).filter(name => /^stage-matrix-st\d+\.json$/.test(name)).sort();

for (const file of files) {
  const golden = JSON.parse(readFileSync(resolve(goldenDir, file), 'utf8'));
  for (const c of golden.cases) {
    test(`stage matrix digests match oracle: ${c.name}`, needDat, () => {
      const out = execFileSync(harness, [
        'game-digest', dat, String(c.stage), String(c.shot), String(c.season), String(c.frames), c.input, '', '',
        String(c.difficulty), String(c.replay_slot), String(c.lives), String(c.refill),
      ], {encoding: 'utf8', maxBuffer: 1 << 28});
      for (const [key, want] of Object.entries(c.digests)) {
        const m = out.match(new RegExp(`\\b${key}=([0-9a-f]{16})`));
        assert.ok(m, out);
        assert.equal(m[1], want, `${key} 摘要与原版不符`);
      }
    });
  }
}
