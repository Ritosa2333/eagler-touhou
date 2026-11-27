// Bomb / 季节解放（0x40d890 族：0x40dda0 可用判定 / 0x40db20 发动 / 0x40dd00 每帧 / 0x40e040 解放收尾；
// 灵梦 0x410d10 / 琪露诺 0x40f070 / 文 0x40e780 / 魔理沙 0x40f930；解放 春 0x411460 / 夏 0x40f590 / 秋 0x40ec70 /
// 冬 0x410150 / 土用 0x40e0f0）与魔理沙激光（.sht 类型 2：0x446200 / 0x446260 / 0x446870）：
// harness game-run / game-digest 对 tools/oracle/game-oracle.py --bombs（Unicorn 直跑原版全游戏）的逐帧 Bomb 对象
// FNV-1a 64 对照，并用快照核对几处可读的原版行为。
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
const golden = JSON.parse(readFileSync(resolve(here, 'golden/bomb.json'), 'utf8'));

const kEnter = 0, kNormal = 1, kRespawnWait = 2, kDying = 4;   // Player::State
const kOrbit = 2, kStraight = 0;                   // MotionState::mode
const kReleaseFrame = 854;                        // 原版第 853 帧积满一级量表

function run(args) {
  return execFileSync(harness, args, {encoding: 'utf8', maxBuffer: 1 << 28});
}

function caseArgs(c, frames = c.frames) {
  return [dat, String(c.stage), String(c.shot), String(c.season), String(frames), c.input, '', '',
          String(c.difficulty), String(c.replay_slot), String(c.lives), String(c.refill)];
}

function gameRun(c, frames = c.frames) {
  return JSON.parse(run(['game-run', ...caseArgs(c, frames)])).frames;
}

const f32 = u => new Float32Array(new Uint32Array([u]).buffer)[0];
const byName = name => golden.cases.find(c => c.name === name);
const activeFrames = (frames, key) => frames.filter(f => f.bombs[key]?.active === 1).map(f => f.frame);

for (const c of golden.cases) {
  test(`bomb digest matches oracle: ${c.name} (${c.frames} frames)`, needDat, () => {
    const out = run(['game-digest', ...caseArgs(c)]);
    for (const [key, want] of [['bombs', c.bomb_digest], ['lasers', c.laser_digest], ['bullets', c.bullet_digest],
                               ['enemies', c.enemy_digest], ['enemy_vms', c.enemy_vm_digest], ['items', c.item_digest],
                               ['spell', c.spell_digest], ['digest', c.player_digest]]) {
      const m = out.match(new RegExp(`\\b${key}=([0-9a-f]{16})`));
      assert.ok(m, out);
      assert.equal(m[1], want, key);
    }
    const frames = gameRun(c);
    const last = frames.at(-1);
    assert.equal(frames[0].bombs.main.class, c.main_class);
    assert.equal(frames[0].bombs.sub.class, c.sub_class);
    for (const [key, want] of [['main', c.main_active_frames], ['sub', c.sub_active_frames]]) {
      const active = activeFrames(frames, key);
      assert.deepEqual(active.length ? [active[0], active.at(-1), active.length] : [0, 0, 0], want, key);
    }
    assert.equal(Math.max(0, ...frames.map(f => f.bombs.sub.level)), c.sub_level);
    for (const [k, v] of Object.entries(c.last_resources)) assert.equal(last.resources[k], v, k);
    assert.equal(last.season_gauge, c.last_season_gauge);
  });
}

test('灵梦 夢想封印：第 120 帧按 X → 残弹 3 → 2，八枚阴阳玉先绕自机（mode 2）再索敌直飞（mode 0），250 帧后结束', needDat, () => {
  const frames = gameRun(byName('st1-reimu-bomb-400'));
  assert.equal(frames[119].resources.bombs, 3);
  assert.equal(frames[120].resources.bombs, 2);
  const first = frames[120].bombs.main;
  assert.equal(first.active, 1);
  assert.equal(first.orbs.length, 8);
  assert.ok(first.orbs.every(o => o.mode === kOrbit && o.active === 1));
  // 0x4109d0：八枚按 45° 等分，半径从 0 起（$pos 相对自机）
  const launched = frames.find(f => f.bombs.main.orbs.some(o => o.mode === kStraight));
  assert.ok(launched && launched.frame > 121 && launched.frame < 250, `launch frame ${launched?.frame}`);
  assert.ok(launched.bombs.main.orbs.filter(o => o.mode === kStraight).every(o => o.target !== 0 || f32(o.speed) > 0));
  // 结束后阴阳玉数组释放（0x410bb0），对象回到未发动
  assert.equal(frames[370].bombs.main.active, 1);
  assert.equal(frames[371].bombs.main.active, 0);
  assert.equal(frames[371].bombs.main.orbs.length, 0);
});

test('决死猶予：第 892 帧被弹进入状态 4，第 895 帧按 X 发动炸弹 → 0x444070 回到状态 1，不进入复活等待（三机体同）', needDat, () => {
  for (const name of ['st1-reimu-deathbomb-1000', 'st1-aya-deathbomb-1000', 'st1-marisa-deathbomb-1000']) {
    const frames = gameRun(byName(name));
    assert.deepEqual(frames.slice(891, 895).map(f => f.player.state), [kDying, kDying, kDying, kDying], name);
    assert.equal(frames[895].player.state, kNormal, name);
    assert.equal(frames[895].bombs.main.active, 1, name);
    assert.ok(frames.slice(895).every(f => f.player.state !== kRespawnWait), name);
    // 决死成功也要扣残弹，且符卡外发动不记「符卡中用过」
    assert.equal(frames[894].resources.bombs - frames[895].resources.bombs, 1, name);
    assert.equal(frames[895].bombs.main.in_spell, 0, name);
  }
});

test('复活入场结束的当帧可按 X 发动炸弹', needDat, () => {
  const c = byName('st1-reimu-deathbomb-1000');
  // 第一次被弹时不决死。在结束复活入场的第 991 帧点按炸弹。
  const frames = gameRun({...c, input: '0:0x11,990:0x13,991:0x11'});
  const entering = frames[989];
  const resumed = frames[990];
  assert.equal(entering.player.state, kEnter);
  assert.equal(entering.bombs.main.active, 0);
  assert.equal(resumed.player.state, kNormal);
  assert.equal(resumed.bombs.main.active, 1);
  assert.equal(resumed.resources.bombs, entering.resources.bombs - 1);
});

test('季节解放：一级量表积满后按 C 发动，结束后冷却 45 帧并显示奖励 180 帧', needDat, () => {
  const c = byName('st1-release-autumn-1100');
  const frames = gameRun(c);
  const before = frames[kReleaseFrame - 2];
  const startFrame = frames[kReleaseFrame - 1];
  assert.equal(before.season_gauge, 100);
  assert.equal(before.bombs.sub.active, 0);
  const start = startFrame.bombs.sub;
  assert.equal(start.active, 1);
  assert.equal(start.level, 1);
  assert.equal(start.class, 'AyaSub');
  // 秋：量表清零、自机移动倍率 1.5、光环贴着自机
  assert.equal(startFrame.season_gauge, 0);
  const moving = frames[kReleaseFrame + 9];
  assert.equal(f32(moving.player.speed_mult), 1.5);
  assert.deepEqual(moving.bombs.sub.pos, moving.player.pos);
  const active = activeFrames(frames, 'sub');
  const end = active.at(-1);                       // 最后一个活动帧（持续 70 帧：等级 1 查表）
  assert.equal(active.length, 71);
  assert.equal(frames[end].bombs.sub.timer[1] >>> 0, 0xffffffd3);   // -45
  assert.equal(f32(frames[end + 1].player.speed_mult), 1.0);
  assert.equal(frames[end + 44].bombs.sub.timer[1] >>> 0, 0xffffffff);
  assert.equal(frames[end + 45].bombs.sub.timer[1], 0);
  assert.equal(frames[end].bombs.sub.popup[1], 180);
  assert.equal(frames[end].bombs.sub.popup_level, 1);
});

test('季节解放：五个子季节各自的类与持续帧数（等级 1：春 40 / 夏 60 / 秋 70 / 冬 60 / 土用 70）', needDat, () => {
  const expect = {spring: ['ReimuSub', 40], summer: ['CirnoSub', 60], autumn: ['AyaSub', 70], winter: ['MarisaSub', 60], doyou: ['AllSub', 70]};
  for (const [season, [cls, duration]] of Object.entries(expect)) {
    const c = byName(`st1-release-${season}-1100`);
    assert.equal(c.sub_class, cls, season);
    assert.equal(c.sub_active_frames[2], duration + 1, season);   // 活动帧含发动当帧
    assert.equal(c.sub_active_frames[0], kReleaseFrame, season);
  }
});

test('魔理沙激光：类型 2 弹贴子机、长度每帧 +18 至 512，命中敌机时截到表面；炸弹（kFlagNoShot）期间全部收束（active 2）', needDat, () => {
  const frames = gameRun(byName('st1-marisa-bomb-500'));
  // pl01.sht：主弹 hit_h 18、激光 hit_h 16（长度放在 hit_w）
  const lasers = f => f.player.bullets.filter(b => b.size[1] === 0x41800000);
  const growth = [];
  for (let i = 19; i < 120; ++i) {
    const l = lasers(frames[i]).find(b => (b.shot_id >>> 8) === 1);
    if (l) growth.push(f32(l.size[0]));
  }
  assert.ok(growth.length > 20);
  assert.deepEqual(growth.slice(0, 5), [18, 36, 54, 72, 90]);
  assert.ok(growth.some(v => v >= 512), '≥ 512 后不再增长');
  assert.ok(growth.some(v => v % 18 !== 0), '命中截断');
  // 第 120 帧按 X：0x40f930 置 kFlagNoShot，持续条件失效 → 激光 active 2 并随 ANM 回收
  assert.ok(lasers(frames[125]).every(b => b.active === 2));
  assert.equal(lasers(frames[200]).length, 0);
});
