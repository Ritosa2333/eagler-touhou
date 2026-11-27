// Enemy + ECL（0x474530 / 0x474740 / 0x472030 / 0x41b340 / 0x41aa70 / 0x41d2e0 / 0x41bb50 / 0x41dcb0）：
// harness game-run / game-digest 对 tools/oracle/game-oracle.py（Unicorn 直跑原版全游戏）的逐帧敌人与自机状态 FNV-1a 64 对照。
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
const golden = JSON.parse(readFileSync(resolve(here, 'golden/enemy.json'), 'utf8'));

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
  test(`enemy digest matches oracle: ${c.name} (${c.frames} frames)`, needDat, () => {
    const out = run([
      'game-digest', dat, String(c.stage), String(c.shot), String(c.season),
      String(c.frames), c.input, '', '', String(c.difficulty), String(c.replay_slot),
    ]);
    const em = out.match(/enemies=([0-9a-f]{16})/);
    assert.ok(em, out);
    assert.equal(em[1], c.enemy_digest);
    if (c.player_digest) {
      const pm = out.match(/\bdigest=([0-9a-f]{16})/);
      assert.ok(pm, out);
      assert.equal(pm[1], c.player_digest);
    }
    const last = gameRun(c).frames.at(-1).enemies;
    assert.equal(last.count, c.last_count);
    assert.deepEqual(last.enemies.map(e => e.id), c.last_ids);
  });
}

test('enemy: st01 MainFront → MainSub00 生成波次、0x40000 隔帧门控、op 404/405 加速与 0x41bb50 出界销毁', needDat, () => {
  const c = golden.cases[0];
  const frames = gameRun(c, 205).frames;
  // 第 1..61 帧只有 main (id=1)
  assert.equal(frames[0].enemies.count, 1);
  assert.equal(frames[0].enemies.enemies[0].ctx[3], 'main');
  assert.equal(frames[60].enemies.count, 1);
  // 第 62 帧：main 进入 MainFront → MainSub00，生成 id=2,3,4 三只妖精（60,-32 / 120,-20 / 180,-8）
  const f62 = frames[61].enemies;
  assert.equal(f62.count, 4);
  assert.deepEqual(f62.enemies.map(e => [e.id, e.ctx[3], e.pos[0], e.pos[1]]), [
    [1, 'MainSub00', 0, 0],
    [2, 'GirlA01', 60, -32],
    [3, 'GirlA01', 120, -20],
    [4, 'GirlA01', 180, -8],
  ]);
  // 第 62 帧生成时立即跑首帧 0x41d1e0 置 0x40000，第 63 帧管理器只清 0x40000 不移动，第 64 帧才真正积分移动
  assert.deepEqual(frames[62].enemies.enemies.slice(1).map(e => [e.pos[0], e.pos[1]]), [[60, -32], [120, -20], [180, -8]]);
  assert.ok(frames[63].enemies.enemies[1].pos[1] > -32, '第 64 帧开始向下积分移动');
  // 第 93 帧速度仍为 1.0，第 94 帧起 op 405(120, 0, -1e6, 4.0) 线性加速
  assert.equal(f32(frames[92].enemies.enemies[1].motion1[6]), 1.0);
  assert.equal(f32(frames[93].enemies.enemies[1].motion1[6]), Math.fround(1.025));
  // 第 200 帧 id=3 的 x=207.46 仍在界内（207.46 - 16 <= 192），第 201 帧 x=208.61 > 208 出界销毁
  assert.ok(frames[199].enemies.enemies.some(e => e.id === 3));
  assert.ok(!frames[200].enemies.enemies.some(e => e.id === 3));
});

test('enemy: Lunatic 难度掩码与 op 535 按难度选值（GirlA01 → GirlA01_at）', needDat, () => {
  const c = golden.cases[1];
  const frames = gameRun(c, 100).frames;
  // 第 93 帧：Lunatic 下 GirlA01 调用 GirlA01_at（diff=0xcc & 0x08 != 0），op 535 选出 5 次循环并停在 waitI 12
  const e2 = frames[92].enemies.enemies.find(e => e.id === 2);
  assert.equal(e2.ctx[3], 'GirlA01_at');
  assert.equal(e2.ctx[2], 580);
});

test('enemy: BossSpell 路径（replay_slot=0）— MainBossSpell 生成 Boss、op 520 等待、BossCard1 op 511/504/412', needDat, () => {
  const c = golden.cases[2];
  const frames = gameRun(c, 120).frames;
  // 第 13 帧：MainBossSpell 在 t=11 生成 Boss (id=2, op 512(0) 置 0x800000)，自身停在 op 520 (@128)
  const f13 = frames[12].enemies;
  assert.equal(f13.count, 2);
  assert.equal(f13.enemies[0].ctx[3], 'MainBossSpell');
  assert.equal(f13.enemies[0].ctx[2], 128);
  assert.equal(f13.enemies[1].ctx[3], 'Boss');
  assert.ok((f13.enemies[1].flags & 0x800000) !== 0);
  // 第 116 帧：Boss 进入 BossCard1，op 511(1700) 设血量 1700，op 504 置边界标志 0x20000
  const boss = frames[115].enemies.enemies[1];
  assert.equal(boss.ctx[3], 'BossCard1');
  assert.deepEqual(boss.hp.slice(0, 2), [1700, 1700]);
  assert.ok((boss.flags & 0x20000) !== 0);
});

test('enemy combat (0x41c330 / 0x445a30 / 0x445ee0 / 0x41a6d0): 灵梦追踪索敌击杀妖精、文充能穿透与 BossCard1 7 倍减伤', needDat, () => {
  const reimu = golden.cases.find(x => x.name === 'reimu-homing-hit-girl-st1');
  const rFrames = gameRun(reimu, 105).frames;
  // 第 84 帧：追踪弹锁定 id=2 (target=2)，命中后 active 置 2、damage_source 清 0、z=0.1
  const f84 = rFrames[83];
  const hitBul = f84.player.bullets.find(b => b.active === 2 && b.target === 2);
  assert.ok(hitBul, '追踪弹命中 id=2 后置 active=2');
  assert.equal(hitBul.damage_source, 0);
  // 第 96 帧 id=2 仍在场（hp=8），后续帧被击杀移除并消耗季节掉落 RNG
  assert.ok(rFrames[95].enemies.enemies.some(e => e.id === 2));
  assert.ok(!rFrames[100].enemies.enemies.some(e => e.id === 2));

  const aya = golden.cases.find(x => x.name === 'aya-focus-hit-boss-st1');
  const aFrames = gameRun(aya, 136).frames;
  // 第 133 帧 Boss 无敌刚结束前血量 1700；第 134 帧受 31 点伤害，按符卡 7 倍减伤 (11900-31)/7 = 1695
  assert.equal(aFrames[132].enemies.enemies[1].hp[0], 1700);
  assert.equal(aFrames[133].enemies.enemies[1].hp[0], 1695);
  // 文低速充能弹（kind=2，0x4474a0）首次命中后不销毁（active=1）但伤害降为 1
  const chargeBul = aFrames[133].player.bullets.find(b => b.damage === 1 && b.active === 1);
  assert.ok(chargeBul, '0x4474a0 充能弹命中后伤害降为 1 持续穿透');
});


test('enemy: ECL 630（0x40c040）把背景 STD 脚本跳到标签——st6 符卡练习 MainBossSpellSt6 首帧跳到 ins 16 label 5（@2308, t=4736）', needDat, () => {
  const c = {stage: 6, shot: 0, season: 0, input: '', difficulty: 1, replay_slot: 102};
  const frames = gameRun(c, 3).frames;
  // 第 1 帧：STD 仍在开头（@132 是 t=350 的首条相机指令）；第 2 帧起已跳到标签 5 并按其 time 继续推进
  assert.deepEqual(frames[0].stage_vm, {frame: 1, ins: 132, flags: 1});
  assert.deepEqual(frames[1].stage_vm, {frame: 2, ins: 2308, flags: 1});
  assert.deepEqual(frames[2].stage_vm, {frame: 3, ins: 2584, flags: 1});
});
