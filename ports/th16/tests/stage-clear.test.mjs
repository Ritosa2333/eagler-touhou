// 过关流程对拍：第 1 面击破 Boss 后，通常模式走过关奖励展示 → 换到第 2 面（0x42e150 → 场景 0xc），
// 练习模式走练习结算菜单（0x43f500）→ 回标题（场景 4）。九组摘要与 scene_exit / exit_frame 对照
// tools/oracle/game-oracle.py（Unicorn 直跑原版）。
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
const golden = JSON.parse(readFileSync(resolve(here, 'golden/stage-clear.json'), 'utf8'));

for (const c of golden.cases) {
  test(`stage clear digests match oracle: ${c.name}`, needDat, () => {
    const out = execFileSync(harness, [
      'game-digest', dat, String(c.stage), String(c.shot), String(c.season), String(c.frames), c.input, '', '',
      String(c.difficulty), String(c.replay_slot), String(c.lives), String(c.refill), String(c.mode),
    ], {encoding: 'utf8', maxBuffer: 1 << 28});
    const field = (key) => {
      const m = out.match(new RegExp(`\\b${key}=(-?\\d+)`));
      assert.ok(m, out);
      return Number(m[1]);
    };
    assert.equal(field('scene_exit'), c.scene_exit, '离开游戏场景的去向与原版不符');
    assert.equal(field('exit_frame'), c.exit_frame, '离开游戏场景的帧号与原版不符');
    for (const [key, want] of Object.entries(c.digests)) {
      const m = out.match(new RegExp(`\\b${key}=([0-9a-f]{16})`));
      assert.ok(m, out);
      assert.equal(m[1], want, `${key} 摘要与原版不符`);
    }
  });
}
