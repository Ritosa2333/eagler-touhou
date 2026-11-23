// 暂停菜单重开（0x43ce10 case 0xa → 0x42d200 bit0 快速重开路径）对拍：菜单光标重开与 R 键直接重开各一例，
// 九组摘要对照 tools/oracle/game-oracle.py（Unicorn 直跑原版）。
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {existsSync, readFileSync} from 'node:fs';
import {resolve} from 'node:path';

const here = resolve(import.meta.dirname);
const monorepo = resolve(here, '../../..');
const harness = process.env.TH16_HARNESS ?? resolve(here, 'build/harness');
const dat = process.env.TH16_DAT ?? resolve(monorepo, 'games/th16/original/th16.dat');
const needDat = {skip: !existsSync(dat) && 'original th16.dat not present'};
const golden = JSON.parse(readFileSync(resolve(here, 'golden/restart.json'), 'utf8'));

for (const c of golden.cases) {
  test(`restart digests match oracle: ${c.name}`, needDat, () => {
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
