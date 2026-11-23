// BulletManager（0x411dd0 / 0x412cb0 / 0x411e70 / 0x413860 / 0x4124b0 / 0x416840 + ECL 600 族 0x42085b..0x4226d0）：
// harness game-run / game-digest 对 tools/oracle/game-oracle.py --enemies --bullets（Unicorn 直跑原版全游戏）的
// 逐帧敌弹链表 / 敌人 / 自机状态 FNV-1a 64 对照。
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
const golden = JSON.parse(readFileSync(resolve(here, 'golden/bullet.json'), 'utf8'));

function run(args) {
  return execFileSync(harness, args, {encoding: 'utf8', maxBuffer: 1 << 28});
}

function gameRun(c, frames = c.frames) {
  return JSON.parse(run([
    'game-run', dat, String(c.stage), String(c.shot), String(c.season),
    String(frames), c.input, '', '', String(c.difficulty), String(c.replay_slot),
  ]));
}

const f32 = u => new Float32Array(new Uint32Array([u]).buffer)[0];

for (const c of golden.cases) {
  test(`bullet digest matches oracle: ${c.name} (${c.frames} frames)`, needDat, () => {
    const out = run([
      'game-digest', dat, String(c.stage), String(c.shot), String(c.season),
      String(c.frames), c.input, '', '', String(c.difficulty), String(c.replay_slot),
    ]);
    const bm = out.match(/bullets=([0-9a-f]{16})/);
    assert.ok(bm, out);
    assert.equal(bm[1], c.bullet_digest);
    const em = out.match(/enemies=([0-9a-f]{16})/);
    assert.ok(em, out);
    assert.equal(em[1], c.enemy_digest);
    if (c.player_digest) {
      const pm = out.match(/\bdigest=([0-9a-f]{16})/);
      assert.ok(pm, out);
      assert.equal(pm[1], c.player_digest);
    }
    const last = gameRun(c).frames.at(-1).bullets;
    assert.equal(last.count, c.last_count);
    assert.equal(last.bullets.length, c.last_count);
  });
}

test('bullet: Lunatic GirlA01_at 首发（0x412cb0 雾化生成态 2、0x10 圆判定位）→ 擦弹 0x4 位 → 被弹溶解态 3（0x4124b0 / 0x443f10）', needDat, () => {
  const c = golden.cases[0];
  const frames = gameRun(c, 205).frames;
  // 第 92 帧无弹；第 93 帧三发 type 4 / color 6（Ex 步 0 为雾化生成 → 状态 2），flags = 活动|可判定|圆判定
  assert.equal(frames[91].bullets.count, 0);
  const f93 = frames[92].bullets;
  assert.equal(f93.count, 3);
  assert.deepEqual(f93.bullets.map(b => [b.index, b.type, b.color, b.state, b.flags, b.ex_index]),
                   [[1997, 4, 6, 2, 0x13, 1], [1998, 4, 6, 2, 0x13, 1], [1999, 4, 6, 2, 0x13, 1]]);
  assert.equal(f32(f93.bullets[0].speed), 4.0);
  // 自机精灵号经 0x417140 重映射（bullet.anm 槽 7），三发同精灵
  assert.ok(f93.bullets.every(b => b.sprite_id === 326));
  // 第 195 帧 1998/1999 进入擦弹框：flags |= 0x4，自机侧登记音效 0x2a
  const f195 = frames[194];
  assert.deepEqual(f195.bullets.bullets.filter(b => (b.flags & 4) !== 0).map(b => b.index), [1998, 1999]);
  assert.ok(f195.player.sfx.includes(0x2a));
  // 第 203 帧 1999 命中自机：弹进入溶解态 3（0x4174d0 中断 1 + 消弹特效），自机状态 4（决死窗口）、音效 2
  const f203 = frames[202];
  assert.deepEqual(f203.bullets.bullets.filter(b => b.state === 3).map(b => b.index), [1999]);
  assert.equal(f203.player.state, 4);
  assert.ok(f203.player.sfx.includes(2));
  assert.deepEqual(f203.player.invincible.slice(0, 2), [5, 6]);
});

test('bullet: BossSpell BossCard1_at 异步线程参数（0x471db0 调用帧）与 0x473bc0 线程次序——首发 18 弹、绝对角 ±38°/±114°/±190°', needDat, () => {
  const c = golden.cases[2];
  const frames = gameRun(c, 208).frames;
  // 第 206 帧（主线程时刻 51）生成线程本帧不跑；第 207 帧 BossCard1_at(1.3264502, -0.12217305) 发 6 way × 3 stack
  assert.equal(frames[205].bullets.count, 0);
  const f207 = frames[206].bullets;
  assert.equal(f207.count, 18);
  const angles = [...new Set(f207.bullets.map(b => Math.round(f32(b.angle) * 1e4) / 1e4))].sort((a, b) => a - b);
  assert.deepEqual(angles, [-2.7227, -1.7453, -1.3963, -0.4189, 0.9076, 2.234]);
  const speeds = [...new Set(f207.bullets.map(b => f32(b.speed)))].sort((a, b) => a - b);
  assert.deepEqual(speeds, [2.5, 2.75, 3]);
});
