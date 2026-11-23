// 全关卡对拍：第 1..7 面机体 × 子季节 × 难度的长程摘要对照（含第 3 面 Easy 灵梦春点射 6000 帧、第 4 面 Hard 魔理沙夏射击+炸弹 4000 帧）。
// tools/oracle/game-oracle.py（Unicorn 直跑原版）。覆盖面向：各面 ECL / 弹幕 / 激光 / 道具 / 炸弹 / 音效调用序。
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
const golden = JSON.parse(readFileSync(resolve(here, 'golden/stage-all.json'), 'utf8'));

for (const c of golden.cases) {
  test(`stage digests match oracle: ${c.name}`, needDat, () => {
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
