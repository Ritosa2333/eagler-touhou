// Game Over 流程对拍：残机 0 被弹 → 续关画面（0x43f350）。Extra 无续关直接回标题；第 3 面下移回标题；
// 第 3 面续关两次，第二次入榜（0x43f7e0 → 0x43e250）走名字输入后再续关。九组摘要与 scene_exit / exit_frame
// 对照 tools/oracle/game-oracle.py（Unicorn 直跑原版）。
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
const golden = JSON.parse(readFileSync(resolve(here, 'golden/game-over.json'), 'utf8'));

for (const c of golden.cases) {
  test(`game over digests match oracle: ${c.name}`, needDat, () => {
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
