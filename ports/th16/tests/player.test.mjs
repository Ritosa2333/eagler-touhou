// Player（0x441c60 / 0x440fb0 / 0x441740 / 0x442560 / 0x441cf0 / 0x442380 / 0x4440e0）与 .sht（0x443790）：
// harness game-run / game-digest 对 tools/oracle/game-oracle.py（Unicorn 直跑原版全游戏）的逐帧规范字段 FNV-1a 64 对照。
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
const golden = JSON.parse(readFileSync(resolve(here, 'golden/player.json'), 'utf8'));

function run(args) {
  return execFileSync(harness, args, {encoding: 'utf8', maxBuffer: 1 << 28});
}

function gameRun(c, frames = c.frames) {
  return JSON.parse(run(['game-run', dat, String(c.stage), String(c.shot), String(c.season), String(frames), c.input, c.enemies]));
}

const f32 = u => new Float32Array(new Uint32Array([u]).buffer)[0];

for (const c of golden.cases) {
  test(`player digest matches oracle: ${c.name} (${c.frames} frames)`, needDat, () => {
    const out = run(['game-digest', dat, String(c.stage), String(c.shot), String(c.season), String(c.frames), c.input, c.enemies]);
    const m = out.match(/digest=([0-9a-f]{16})/);
    assert.ok(m, out);
    assert.equal(m[1], c.digest);
    const last = gameRun(c).frames.at(-1).player;
    assert.equal(last.state, c.last.state);
    assert.deepEqual(last.pos_fixed, c.last.pos_fixed);
    assert.equal(last.dir, c.last.dir);
    assert.equal(last.focus, c.last.focus);
    assert.equal(last.option_count, c.last.option_count);
    assert.deepEqual(last.options.slice(0, 4).map(o => [o.active, o.pos[0], o.pos[1]]), c.last.options);
  });
}

test('灵梦向右上移动时，场地右半边的自机弹仍留场', needDat, () => {
  // 0x4457df：640×480 下场地的屏幕坐标是 (128,16) 到 (512,464)。
  const {frames} = JSON.parse(run(['game-run', dat, '1', '0', '0', '35', '0:0x91', '', '', '3']));
  assert.equal(frames[33].player.bullets.length, 13);
  assert.equal(frames[34].player.bullets.length, 16);
});

test('player: creation state (0x440fb0) — 灵梦 .sht 速度 ×128、半径常量、子机初值、首帧复位', needDat, () => {
  const c = golden.cases[0];
  const first = gameRun(c, 1).frames[0];
  const p = first.player;
  assert.equal(p.state, 1, '0x441740 置状态 1');
  assert.deepEqual(p.pos_fixed, [0, 51200], '0x440e40：(0, 400) × 128');
  assert.deepEqual(p.pos.map(f32), [0, 400, 0]);
  assert.deepEqual(p.speeds, [576, 256, 407, 181], 'pl00.sht 4.5/2.0/3.18/1.414 × 128 截断');
  assert.deepEqual(p.radii.map(f32), [1.5, 1.5, 5, 30, 30, 5, 50, 50, 5], 'DAT_00492c98/88/68 ×0.5，z=5');
  assert.deepEqual(p.timers.map(t => t.slice(0, 2)), [[0, 1], [0, 1], [0, 1]], '三计时器复位后推进一帧');
  assert.deepEqual(p.shot_timers.map(t => t.slice(0, 2)), [[-2, -1], [-2, -1]], '射击计时 -1 待机');
  assert.equal(p.option_lag, 30);
  assert.equal(p.option_count, 1, '火力 100 / 每级 100 = 1 子机');
  assert.equal(first.power, 100);
  assert.deepEqual(p.options[0].pos, [0, 47104], '子机 0 直接吸附到 (0, -32) 偏移');
  assert.equal(p.options[0].active, 2);
  assert.equal(p.options[1].active, 0);
  assert.equal(p.options[1].pos[1], -51200, '未激活子机 y 初值 0xffff3800');
  assert.equal(p.sub_count, 0, '季节量表 0 → 无子季节子机');
});

test('player: movement (0x441cf0) — 方向表、低速门限、边界钳制、子机 30% 跟随', needDat, () => {
  // 向左 4.5/帧，第 61 帧起；第 121 帧起低速向右（敌人在场 → 低速有效）
  const c = golden.cases[1];
  const frames = gameRun(c, 130).frames;
  const f61 = frames[60].player, f62 = frames[61].player;
  assert.equal(f61.dir, 3);
  assert.deepEqual(f61.pos_fixed, [-576, 51200]);
  assert.deepEqual(f61.last_move, [-576, 0]);
  assert.deepEqual(f61.options[0].pos, [-172, 47104], '((−576 − 0) × 30) / 100 = −172（截断）');
  assert.deepEqual(f62.options[0].pos, [-466, 47104]);
  const f121 = frames[120].player;
  assert.equal(f121.focus, 1);
  assert.equal(f121.dir, 4);
  assert.deepEqual(f121.last_move, [256, 0], '低速 2.0 × 128');
  assert.notEqual(f121.focus_vm, 0, 'effect.anm 脚本 0x1a 判定点 VM 已生成');
  assert.equal(frames[119].player.focus_vm, 0);
  // 无敌人时低速无效
  const noEnemy = JSON.parse(run(['game-run', dat, '1', '0', '0', '5', '0:0x8', '0:0'])).frames.at(-1).player;
  assert.equal(noEnemy.focus, 0);
  assert.equal(noEnemy.focus_vm, 0);
  // 左边界：-0x5c00 = -184
  const clamp = JSON.parse(run(['game-run', dat, '1', '0', '0', '60', '0:0x40', '0:1'])).frames.at(-1).player;
  assert.deepEqual(clamp.pos_fixed, [-0x5c00, 51200]);
  assert.equal(f32(clamp.pos[0]), -184);
});

test('player: full power (stage 2) — 4 子机 + 0x426160 前插附加 VM；sht 子机位置表', needDat, () => {
  const c = golden.cases.find(x => x.name.startsWith('fullpower'));
  const first = gameRun(c, 1).frames[0];
  assert.equal(first.power, 400);
  assert.equal(first.player.option_count, 4);
  const main = first.player.options.slice(0, 4);
  for (const o of main) { assert.equal(o.active, 2); assert.deepEqual(o.vm, [1, 1], '子机 VM + 满火力前插 VM'); }
  for (const o of first.player.options.slice(4)) assert.equal(o.active, 0, '季节量表 0 → 子季节子机未激活');
  // pl03.sht（文）等级 4 → DAT_004a5e4c 行 2 下标 6..9 的位置 × 128，相对自机 (0, 51200)
  assert.deepEqual(main.map(o => o.pos), [[384, 46080], [-384, 46080], [1152, 46592], [-1152, 46592]]);
});

test('player shooting (0x4455d0 / 0x445470 / 0x444e10 / 0x4456d0) — 琪露诺 + 秋：计时、出弹、伤害源、音效、出界回收', needDat, () => {
  const c = golden.cases.find(x => x.name === 'shoot-cirno-autumn');
  const frames = gameRun(c).frames;
  // 第 20 帧（0 起）按下射击 → 第 21 帧计时 -1 → 0 并出弹：组 1（火力 100）的 6 条全部相位 0
  const f21 = frames[20].player;
  assert.deepEqual(f21.shot_timers.map(t => t.slice(0, 2)), [[0, 1], [0, 1]]);
  assert.deepEqual(f21.bullets.map(b => b.shot_id), [0x100, 0x101, 0x102, 0x103, 0x104, 0x105]);
  assert.deepEqual(f21.bullets.map(b => b.pos.slice(0, 2).map(f32)), [[-10, 392], [10, 392], [0, 368], [0, 368], [0, 368], [0, 368]],
    '主弹 (±10, 400−8) 先减速度再推进一帧；子机弹从子机 (0, 368) 出发');
  assert.deepEqual(f21.bullets.map(b => f32(b.speed)), [24, 24, 8, 8, 8, 8]);
  assert.deepEqual(f21.bullets.map(b => b.damage), [14, 14, 14, 14, 14, 14]);
  assert.deepEqual(f21.bullets.map(b => b.damage_source), [2, 3, 4, 5, 6, 7], '0x444b20 从上次下标 0 的下一个（池项 1）起分配，返回 1 起下标');
  assert.deepEqual(f21.sfx, [0], '组内只有第一条 sfx 0，其余 -1');
  assert.equal(f21.damage_sources.length, 6);
  const ds = f21.damage_sources[0];
  assert.deepEqual(ds.size.map(f32), [48, 16]);
  assert.deepEqual(ds.tail, [0, 10000000, 1, 0, 0, 0, 1], '+0x78..+0x90：0 / 10000000 / 1 / 0 / 弹下标 / 0 / 类型 1');
  assert.equal(ds.timer[1], 9999999, '寿命 9999999；伤害源回退循环在射击之前，本帧不减');
  assert.equal(frames[21].player.damage_sources[0].timer[1], 9999998, '次帧起每帧回退 1');
  // 主弹周期 3（第 24/27/30…帧），子机弹 0x102/0x103 周期 8（第 29/37 帧）；0x104/0x105 周期 30（第 21/51/81 帧）
  const spawned = id => frames.slice(0, 40).map((fr, i) => fr.player.bullets.some(b => b.shot_id === id && b.timer[1] === 1) ? i + 1 : 0).filter(Boolean);
  assert.deepEqual(spawned(0x100), [21, 24, 27, 30, 33, 36, 39]);
  assert.deepEqual(spawned(0x102), [21, 29, 37]);
  assert.deepEqual(spawned(0x104), [21]);
  assert.ok(frames[50].player.bullets.some(b => b.shot_id === 0x104 && b.timer[1] === 1));
  assert.deepEqual(frames[23].player.sfx, [0]);
  assert.deepEqual(frames[24].player.sfx, []);
  assert.deepEqual(frames[28].player.sfx, [], '子机弹 sfx -1：不入队');
  // 主射击计时 0..14 环绕（0x4455d0 到 15 归零），第二计时继续增长
  assert.deepEqual(frames[34].player.shot_timers.map(t => t[1]), [0, 15]);
  assert.deepEqual(frames[35].player.shot_timers.map(t => t[1]), [1, 16]);
  // 直线弹 24/帧 向上，四角越过屏幕 y=16 的场地上缘后于第 40 帧回收。
  assert.ok(frames[38].player.bullets.some(b => b.index === 0));
  assert.ok(!frames[39].player.bullets.some(b => b.index === 0));
  assert.ok(!frames[39].player.damage_sources.some(d => d.index === 2), '伤害源随弹失活');
  const last = frames.at(-1).player;
  assert.equal(last.bullets.length, c.shooting.bullets);
  assert.equal(last.damage_sources.length, c.shooting.damage_sources);
  assert.equal(last.last_damage_index, c.shooting.last_damage_index);
});

test('player shooting — 低速组 5..9（+1+最大等级）、文 t4 弹回调 4/5（伤害源类型 2、+0.5/帧加速）', needDat, () => {
  const focus = golden.cases.find(x => x.name === 'shoot-focus-cirno-autumn');
  const ff = gameRun(focus, 72).frames;
  assert.equal(ff[20].player.focus, 1);
  assert.deepEqual(ff[20].player.bullets.map(b => b.shot_id), [0x600, 0x601, 0x602, 0x603, 0x604, 0x605], '低速：组 1 + 1 + 4 = 6');
  assert.deepEqual(ff[71].player.bullets.filter(b => b.timer[1] === 1).map(b => b.shot_id), [0x100, 0x101], '松开低速后回到组 1');
  const aya = golden.cases.find(x => x.name === 'shoot-aya-autumn');
  const af = gameRun(aya, 64).frames;
  const a21 = af[20].player;
  assert.deepEqual(a21.bullets.map(b => [b.shot_id, f32(b.speed)]), [[0x100, 24], [0x101, 24], [0x102, 0.5], [0x103, 0.5]], 't4 弹初速 0，回调 5 本帧 +0.5');
  assert.deepEqual(af[21].player.bullets.slice(2).map(b => f32(b.speed)), [1, 1]);
  assert.deepEqual(a21.bullets.slice(2).map(b => b.size.map(f32)), [[48, 8], [48, 8]]);
  const a61 = af[60].player;
  const charge = a61.bullets.find(b => b.shot_id === 0x602);
  assert.ok(charge, '低速组 6 的 t4 弹（周期 5）');
  const cds = a61.damage_sources.find(d => d.index === charge.damage_source);
  assert.equal(cds.tail[6], 2, '生成回调 4：伤害源 +0x90 = 2');
});
