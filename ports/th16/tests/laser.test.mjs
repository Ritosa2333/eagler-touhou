// LaserManager（0x4314a0 / 0x431510 更新 / 0x431720 绘制（见 draw.test.mjs） / 直线 0x431b30 / 照射 0x435050 / 曲线 0x4370a0 / 占位 0x43a860 +
// 消弹 0x431950..0x431b30 / ECL 700..714、545 / 敌机每帧钩子表 DAT_004917b8）：
// harness game-run / game-digest 对 tools/oracle/game-oracle.py --enemies --bullets --items --lasers（Unicorn 直跑原版全游戏）的
// 逐帧激光池 FNV-1a 64 对照，并用快照核对几处可读的原版行为。
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
const golden = JSON.parse(readFileSync(resolve(here, 'golden/laser.json'), 'utf8'));

const kLine = 0, kInfinite = 1, kCurve = 2;

function run(args) {
  return execFileSync(harness, args, {encoding: 'utf8', maxBuffer: 1 << 28});
}

function caseArgs(c, frames = c.frames) {
  // argv[14] = mode。旧金样在 mode 0 下录制（符卡练习位未置，ins_509 仍放道具）；缺省保持 0。
  return [dat, String(c.stage), String(c.shot), String(c.season), String(frames), c.input, '', '',
          String(c.difficulty), String(c.replay_slot), String(c.lives), String(c.refill), String(c.mode ?? 0)];
}

function gameRun(c, frames = c.frames) {
  return JSON.parse(run(['game-run', ...caseArgs(c, frames)])).frames;
}

const f32 = u => new Float32Array(new Uint32Array([u]).buffer)[0];
const byName = name => golden.cases.find(c => c.name === name);

for (const c of golden.cases) {
  test(`laser digest matches oracle: ${c.name} (${c.frames} frames)`, needDat, () => {
    const out = run(['game-digest', ...caseArgs(c)]);
    const pairs = [['lasers', c.laser_digest], ['bullets', c.bullet_digest], ['enemies', c.enemy_digest],
                   ['enemy_vms', c.enemy_vm_digest], ['items', c.item_digest], ['spell', c.spell_digest],
                   ['digest', c.player_digest]];
    if (c.menu_digest) pairs.push(['menu', c.menu_digest]);
    if (c.sound_digest) pairs.push(['sound', c.sound_digest]);
    for (const [key, want] of pairs) {
      const m = out.match(new RegExp(`\\b${key}=([0-9a-f]{16})`));
      assert.ok(m, out);
      assert.equal(m[1], want, key);
    }
    const frames = gameRun(c);
    const last = frames.at(-1);
    const kinds = [...new Set(frames.flatMap(f => f.lasers.lasers.map(l => l.kind)))].sort();
    assert.deepEqual(kinds, c.laser_kinds);
    assert.equal(frames.findIndex(f => f.lasers.count > 0) + 1, c.first_laser_frame ?? 0);
    assert.equal(Math.max(...frames.map(f => f.lasers.count)), c.max_laser_count);
    assert.equal(last.lasers.next_handle, c.last_next_handle);
    for (const [k, v] of Object.entries(c.last_resources)) assert.equal(last.resources[k], v, k);
    assert.equal(last.power, c.last_power);
    assert.equal(last.season_gauge, c.last_season_gauge);
  });
}

test('curve laser: 0x4370a0 生成即 64 个采样节点，0x4377d0 节点全部出界时整条移出（池计数 1 → 16 稳态 → 峰值 32）', needDat, () => {
  const frames = gameRun(byName('st3-normal-card2-curve-900'), 330);
  const first = frames[234];
  assert.equal(first.frame, 235);
  assert.equal(first.lasers.count, 1);
  assert.equal(first.lasers.lasers[0].kind, kCurve);
  assert.equal(first.lasers.lasers[0].nodes.length, 64);
  // 句柄自 0x10000 起递增（0x431b30：next_handle 先 +1 再钳到 ≥ 0x10000）
  assert.equal(first.lasers.lasers[0].id, 0x10001);
  assert.ok(frames.slice(299, 310).every(f => f.lasers.count === 16));
  assert.equal(frames.findIndex(f => f.lasers.count === 32) + 1, 326);
  // 节点全程留在场内 ± 宽度：出界节点只会在整条移出前出现在尾部
  assert.ok(frames.every(f => f.lasers.lasers.every(l => l.state === 2)));
});

test('curve laser: 复活入场扩圆消弹 0x43a2f0——半径 (t×512)/30+64 为单精度除法；第 388 帧 19 节点中 18 个命中只保留 1 个，第 389 帧全命中即标记并当帧移出', needDat, () => {
  const frames = gameRun(byName('st3-normal-card2-curve-move-1800'), 392);
  const find = (f, id) => frames[f - 1].lasers.lasers.find(l => l.id === id);
  // 自机第 381 帧前后复活（状态 0 入场、无敌计时 275 → 274），消弹圆心为死亡坐标 pos_copy 而非当前坐标
  assert.equal(frames[387].player.state, 0);
  assert.equal(frames[387].player.invincible[1], 275);
  assert.equal(find(387, 65564).nodes.length, 64);
  assert.equal(find(388, 65564).nodes.length, 19);
  assert.equal(frames[387].lasers.count, 19);
  assert.equal(frames[388].lasers.count, 18);
  assert.equal(find(389, 65564), undefined);
});

test('line laser: 0x432f40 每帧按 speed 伸长（4 → 20 → 100 → 219），起点不动；激光擦弹得点道具 z 为 0（原版为未初始化栈值）', needDat, () => {
  const c = byName('st5-normal-card1-line-move-1800');
  const frames = gameRun(c, 460);
  const id = frames[395].lasers.lasers[0].id;
  const at = f => frames[f - 1].lasers.lasers.find(l => l.id === id);
  assert.equal(frames[395].frame, 396);
  assert.equal(at(396).kind, kLine);
  assert.equal(f32(at(396).width), 16);
  assert.deepEqual([396, 400, 420, 450].map(f => Math.round(f32(at(f).length) * 1000) / 1000), [4, 20, 100, 219]);
  assert.deepEqual([396, 450].map(f => at(f).pos.slice(0, 2)), [at(396).pos.slice(0, 2), at(396).pos.slice(0, 2)]);
  // 激光擦弹得点道具（type 0x10）z = 0；子弹擦弹沿用弹的 z（0.1 量级），绝不会出现指针量级的脏值
  assert.ok(frames.every(f => f.items.items.every(i => i.type !== 0x10 || Math.abs(f32(i.pos[2])) < 1)));
});

test('infinite laser: 0x435050 预告(3) → 展开(4) → 维持(2) → 收束(5) 四态首现帧 267 / 307 / 327 / 527；op 632 arg 2 钩子 0x4253b0 自第 205 帧起火力 400 → 0、量表 0、子机随 0x4440e0 重建', needDat, () => {
  const frames = gameRun(byName('st6-normal-card9-infinite-drain-move-1800'), 540);
  const firstState = s => frames.findIndex(f => f.lasers.lasers.some(l => l.kind === kInfinite && l.state === s)) + 1;
  assert.deepEqual([3, 4, 2, 5].map(firstState), [267, 307, 327, 527]);
  assert.equal(frames[203].power, 400);
  assert.equal(frames[204].power, 0);
  assert.equal(frames[204].season_gauge, 0);
  assert.equal(frames[203].player.option_count, 4);
  assert.equal(frames[204].player.option_count, 0);
  assert.ok(frames.slice(204).every(f => f.power === 0));
});

test('enemy frame hook: op 632 arg 1（0x4252d0）——F0 半径内 Ex 0x8000 参数 1 的子弹跳到 Ex 第 8 步（第 1367 帧 3 枚，第 1369 帧 6 枚）', needDat, () => {
  const frames = gameRun(byName('st6-normal-card4-bullet-hook-move-1800'), 1372);
  const triggered = f => frames[f - 1].bullets.bullets.filter(b => b.ex_index === 8).length;
  assert.equal(frames.findIndex(f => f.bullets.bullets.some(b => b.ex_index === 8)) + 1, 1367);
  assert.deepEqual([1366, 1367, 1368, 1369].map(triggered), [0, 3, 3, 6]);
});

test('ECL 503: 清 0x20 位后 0x46f300 对全部 VM 槽置显示位（st7 Boss 第 114 帧 4 个子 VM flags_lo 0x4004 → 0x4006）', needDat, () => {
  const frames = gameRun(byName('st7-normal-card7-infinite-move-1800'), 115);
  const boss = f => frames[f - 1].enemies.enemies.find(e => e.id === 4);
  assert.equal(boss(113).flags & 0x20, 0x20);
  assert.equal(boss(114).flags & 0x20, 0);
  assert.deepEqual(boss(113).vms.map(v => v[10]), [0x4004, 0x4004, 0x4004, 0x4004]);
  assert.deepEqual(boss(114).vms.slice(1).map(v => v[10]), [0x4006, 0x4006, 0x4006, 0x4006]);
});
