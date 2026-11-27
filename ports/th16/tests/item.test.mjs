// ItemManager（0x42f440 / 0x4184a0 / 0x42f4e0 / 0x430960 / 0x430c90 / 0x430d90 + 拾取结算 0x430100..0x4304a0 / 0x43df70..0x43dfb0）：
// harness game-run / game-digest 对 tools/oracle/game-oracle.py --enemies --bullets --items（Unicorn 直跑原版全游戏）的
// 逐帧道具池 + 资源全局（得分 / 残机 / Bomb / 得点值 / 收点统计）FNV-1a 64 对照。
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {existsSync, mkdtempSync, readFileSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join, resolve} from 'node:path';

const here = resolve(import.meta.dirname);
const monorepo = resolve(here, '../../..');
const harness = process.env.TH16_HARNESS ?? resolve(here, 'build/harness');
const dat = process.env.TH16_DAT ?? resolve(monorepo, 'games/th16/original/th16.dat');
const needDat = {skip: !existsSync(dat) && 'original th16.dat not present'};
const golden = JSON.parse(readFileSync(resolve(here, 'golden/item.json'), 'utf8'));

function run(args) {
  return execFileSync(harness, args, {encoding: 'utf8', maxBuffer: 1 << 28});
}

function gameRun(c, frames = c.frames) {
  return JSON.parse(run([
    'game-run', dat, String(c.stage), String(c.shot), String(c.season),
    String(frames), c.input, '', '', String(c.difficulty), String(c.replay_slot),
  ]));
}

test('item: the first draw clears the original seasonal drop indicator field', needDat, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-item-draw-'));
  try {
    const input = '0:0x11,140:0x41,170:0x49,220:0x89,400:0x809,401:0x09';
    const {frames} = JSON.parse(run([
      'game-draw', dat, '4', '3', '3', '477', input, '', '', '2', '-1', '8', '1', '0',
      join(dir, 'draw.jsonl'), '478',
    ]));
    // Original 0x430bb2 seeds Item+0xc58 and 0x4308c6 clears it after drawing.
    const drops = frame => frames[frame - 1].items.items.map(i => [i.index, i.state, i.type2]);
    assert.deepEqual(drops(471), [[4695, 6, 16]]);
    assert.deepEqual(drops(472), [[4695, 3, 0]]);
    assert.deepEqual(drops(475), [[4694, 6, 16], [4695, 3, 0]]);
    assert.deepEqual(drops(477), [[4694, 3, 0], [4695, 3, 0]]);
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

const f32 = u => new Float32Array(new Uint32Array([u]).buffer)[0];
const near = (u, v, eps = 1e-3) => Math.abs(f32(u) - v) <= eps;

for (const c of golden.cases) {
  test(`item digest matches oracle: ${c.name} (${c.frames} frames)`, needDat, () => {
    const out = run([
      'game-digest', dat, String(c.stage), String(c.shot), String(c.season),
      String(c.frames), c.input, '', '', String(c.difficulty), String(c.replay_slot),
    ]);
    for (const [key, want] of [['items', c.item_digest], ['bullets', c.bullet_digest], ['enemies', c.enemy_digest],
                               ['enemy_vms', c.enemy_vm_digest], ['digest', c.player_digest]]) {
      const m = out.match(new RegExp(`\\b${key}=([0-9a-f]{16})`));
      assert.ok(m, out);
      assert.equal(m[1], want, key);
    }
    const last = gameRun(c).frames.at(-1);
    assert.equal(last.items.active, c.last_active);
    assert.equal(last.items.items.length, c.last_active);
    assert.equal(last.items.spawned, c.last_spawned);
    for (const [k, v] of Object.entries(c.last_resources)) assert.equal(last.resources[k], v, k);
    assert.equal(last.season_gauge, c.last_season_gauge);
    assert.equal(last.power, c.last_power);
  });
}

test('item: BossItemCard 掉落——0x430960 普通池 / 得点池头插分配次序、0x430c90 季节道具脚本 0x81+季节、得点池延迟态 6 计数', needDat, () => {
  const c = golden.cases[0];
  const frames = gameRun(c, 120).frames;
  // 第 115 帧无道具；第 116 帧 0x41b340 掉落：普通池最高位先出（599 火力 → 598 Bomb 碎片 0xf→6），得点池 4695 → 4694
  assert.equal(frames[114].items.active, 0);
  const f116 = frames[115].items;
  assert.equal(f116.active, 2);
  assert.equal(f116.spawned, 4);
  assert.deepEqual(f116.items.map(i => [i.index, i.state, i.type, i.counter]),
                   [[598, 1, 6, 0], [599, 1, 1, 0], [4694, 3, 16, -1], [4695, 6, 16, 1]]);
  // 普通道具：DAT_004917f8 脚本 117（Bomb 碎片）/ 112（火力），精灵 510 / 500；季节道具脚本 0x81 + 季节 0 = 129，精灵 519
  assert.deepEqual(f116.items.map(i => i.vm.slice(0, 2)), [[117, 510], [112, 500], [129, 519], [0, 0]]);
  // 掉落点 Boss 坐标 (0, 125.8)，季节道具 0x430c90 出现于 y=128（0x4304a0 上抛起点）
  assert.ok(near(f116.items[0].pos[1], 125.8));
  assert.ok(near(f116.items[2].pos[1], 128.0));
  assert.ok(near(f116.items[2].speed, 0.7729, 1e-4));
  // 资源全局：Bomb 碎片道具计数 +1（0xf 转 6 时 DAT_004a587c++），得点值 Normal 初值 1000000（10000×100）
  assert.deepEqual(frames[115].resources.collect, [0, 0, 0, 1]);
  assert.equal(frames[115].resources.point_value, 1000000);
  assert.equal(frames[115].resources.lives, 2);
  assert.equal(frames[115].resources.bombs, 3);
});

test('item: 季节道具拾取 0x430620——第 303 帧量表 +1 / 得分 +1，收点圈态 5 先于拾取（第 296 帧）', needDat, () => {
  const c = golden.cases[0];
  const frames = gameRun(c, 320).frames;
  assert.equal(frames[301].season_gauge, 0);
  assert.equal(frames[302].season_gauge, 1);
  assert.equal(frames[302].resources.score, 1);
  assert.equal(frames[309].season_gauge, 2);
  assert.equal(frames[309].resources.score, 2);
  const firstRing = frames.findIndex(f => f.items.items.some(i => i.state === 5));
  assert.equal(firstRing + 1, 296);
  // 收点圈吸引速度 = 基准 5（sht+0x08）/ 3
  const ring = frames[firstRing].items.items.find(i => i.state === 5);
  assert.ok(near(ring.attract_speed, 5 / 3, 1e-5));
});

test('item: 持续射击场景——首发掉落第 96 帧（op 1001 季节道具 ×4 得点池计数 0..3）、得点道具 0x430100 结算与火力 0x4303a0 拾取', needDat, () => {
  const c = golden.cases[1];
  const frames = gameRun(c, 700).frames;
  const f96 = frames[95].items;
  assert.deepEqual(f96.items.map(i => [i.index, i.state, i.type, i.counter]),
                   [[599, 1, 1, 0], [4692, 6, 16, 0], [4693, 3, 16, -1], [4694, 6, 16, 2], [4695, 6, 16, 1]]);
  assert.ok(near(f96.items[1].speed, 1.6834, 1e-4));
  // 第 695 帧最后一次得分变化；得点道具 6 枚、火力 110、量表 41
  const last = frames.at(-1);
  assert.equal(last.resources.score, 2110);
  assert.equal(last.resources.point_items, 6);
  assert.equal(last.power, 110);
  assert.equal(last.season_gauge, 41);
  assert.equal(frames[694].resources.score, 2110);
  assert.equal(frames[693].resources.score, 2109);
  // 道具状态覆盖：1 下落 / 3 季节减速 / 5 收点圈 / 6 延迟出现
  const states = new Set(frames.flatMap(f => f.items.items.map(i => i.state)));
  assert.deepEqual([...states].sort(), [1, 3, 5, 6]);
});

test('item: 收点线吸引态 4（0x42fbc1）——首个吸引帧 89、吸引速度自基准 5 起当帧即 +0.2（快照 5.2）、每帧 +0.2 至上限 12；ECL 507 仅登记掉落表（GirlB01 生成帧不掉落）', needDat, () => {
  const c = golden.cases[2];
  const frames = gameRun(c, 900).frames;
  const first = frames.findIndex(f => f.items.items.some(i => i.state === 4));
  assert.equal(first + 1, 89);
  const track = [];
  for (let f = first; f < first + 6; ++f) {
    const it = frames[f].items.items.find(i => i.state === 4);
    if (!it) break;
    track.push(Math.round(f32(it.attract_speed) * 10) / 10);
  }
  assert.deepEqual(track, [5.2, 5.4, 5.6, 5.8, 6, 6.2]);
  assert.ok(frames.every(f => f.items.items.every(i => f32(i.attract_speed) <= 12)));
  // 第 712 帧 GirlB01 ×2 生成（ins_507(1,2)/ins_507(2,2) 只写掉落表）：道具数不变
  assert.equal(frames[710].items.spawned, frames[711].items.spawned);
  // 第 853 帧季节量表达到 100 → 季节子机 0 激活；激活当帧 x/y 取主子机 0 偏移（0, -4096）→ (0, 0)，次帧吸附到 (0, 8192)
  assert.equal(frames[851].player.sub_count, 0);
  assert.equal(frames[852].player.sub_count, 1);
  assert.deepEqual(frames[852].player.options[4].pos, [0, 0]);
  assert.deepEqual(frames[853].player.options[4].pos, [0, 8192]);
  const states = new Set(frames.flatMap(f => f.items.items.map(i => i.state)));
  assert.deepEqual([...states].sort(), [1, 3, 4, 6]);
});

test('player: 被弹 → 决死猶予 8 帧 → 0x443cd0 死亡结算 → 第 3 帧散出 7 枚火力道具 → 第 0x1e 帧 0x4449b0 复活圆伤害源并回到 (0, 480) 入场', needDat, () => {
  const c = golden.cases[2];
  const frames = gameRun(c, 940).frames;
  // 第 892 帧被弹（状态 4、无敌 6 帧），决死猶予计时 > 7 后第 901 帧状态 2：残机 2→1、无敌 0xb4-1、Bomb 回 3
  const hitFrame = frames.findIndex(f => f.player.state === 4);
  assert.equal(hitFrame + 1, 892);
  assert.equal(frames[900].player.state, 2);
  assert.equal(frames[899].resources.lives, 2);
  assert.equal(frames[900].resources.lives, 1);
  assert.equal(frames[900].player.invincible[1], 179);
  assert.equal(frames[900].resources.bombs, 3);
  // 第 904 帧（状态 2 计时 3）：火力 112 → 100（-每级/2 = -12），7 枚火力道具从自机位置以速度 3 扇形散出
  assert.equal(frames[902].power, 112);
  assert.equal(frames[903].power, 100);
  assert.equal(frames[903].items.spawned - frames[902].items.spawned, 7);
  // 原版此场景的普通道具池空闲头为 599，七枚依次分配至 593。
  const scattered = frames[903].items.items.filter(i => i.index >= 593 && i.index <= 599);
  assert.equal(scattered.length, 7);
  assert.ok(scattered.every(i => i.type === 1 && i.state === 1));
  // 第 931 帧复活：状态 0、y = 480 起入场、无敌 0x118-1；复活圆伤害源使得分连续增长
  assert.equal(frames[930].player.state, 0);
  assert.equal(f32(frames[930].player.pos[1]), 480);
  assert.equal(frames[930].player.invincible[1], 279);
  assert.ok(frames[934].resources.score > frames[930].resources.score);
});
