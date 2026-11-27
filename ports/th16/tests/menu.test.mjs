// 游戏内菜单 GameMenu（0x43e350 族：0x43e5f0 每帧 / 0x43f0f0 暂停 / 0x43f350 续关 / 0x43f980 状态机 / 0x43f6a0 恢复）：
// harness game-run / game-digest 对 tools/oracle/game-oracle.py（Unicorn 直跑原版全游戏）的逐帧 GameMenu 快照
// FNV-1a 64 对照，并用快照核对几处可读的原版行为（暂停期间世界冻结、菜单计时、续关资源重置）。
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
const golden = JSON.parse(readFileSync(resolve(here, 'golden/menu.json'), 'utf8'));

const kMenuOpen = 0x10;                                  // GameManager flags bit4
const kClosed = 0, kPause = 1, kContinue = 2;            // GameMenu::Mode
const kPauseIntro = 0, kContinueIntro = 2, kMainMenu = 6, kExecute = 0x10;   // GameMenu::State
const kSfxPause = 14, kSfxConfirm = 7, kSfxMove = 10;

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

const byName = name => golden.cases.find(c => c.name === name);

for (const c of golden.cases) {
  test(`menu digest matches oracle: ${c.name} (${c.frames} frames)`, needDat, () => {
    const out = run(['game-digest', ...caseArgs(c)]);
    for (const [key, want] of [['menu', c.menu_digest], ['bombs', c.bomb_digest], ['lasers', c.laser_digest],
                               ['bullets', c.bullet_digest], ['enemies', c.enemy_digest], ['enemy_vms', c.enemy_vm_digest],
                               ['items', c.item_digest], ['spell', c.spell_digest], ['digest', c.player_digest]]) {
      const m = out.match(new RegExp(`\\b${key}=([0-9a-f]{16})`));
      assert.ok(m, out);
      assert.equal(m[1], want, key);
    }
  });
}

test('暂停：第 200 帧按 Esc → 模式 1、GameManager flags |= 0x10、音效 14；第 10 帧进主菜单；世界帧计数与 visual_rng 冻结', needDat, () => {
  const frames = gameRun(byName('st1-pause-esc-600'));
  const open = frames[200];
  assert.equal(open.menu.menu.mode, kPause);
  assert.equal(open.menu.menu.state, kPauseIntro);
  assert.equal(open.menu.game_flags & kMenuOpen, kMenuOpen);
  assert.deepEqual(open.menu.sfx, [kSfxPause]);
  // 截屏框（text.anm 0x34）与菜单 VM（front.anm 0x9c）都已生成；菜单计时从 1 起
  assert.deepEqual(open.menu.menu.vm, [1, 1]);
  assert.equal(open.menu.menu.timer[1], 1);
  assert.equal(frames[210].menu.menu.state, kMainMenu);
  assert.deepEqual(frames[210].menu.menu.cursor, [0, 0, 5, 0]);
  // 暂停中 GameManager 返回 3：帧计数、visual_rng 调用数与敌机都不再推进
  assert.equal(frames[200].game_frame, frames[270].game_frame);
  assert.deepEqual(frames[200].rng, frames[270].rng);
  // 第 260 帧再按 Esc → 状态 0x10，12 帧后 0x43f6a0 恢复：flags 清位、帧计数继续
  assert.equal(frames[260].menu.menu.state, kExecute);
  assert.equal(frames[272].menu.menu.mode, kClosed);
  assert.equal(frames[272].menu.game_flags & kMenuOpen, 0);
  assert.equal(frames[272].game_frame, frames[200].game_frame + 1);
});

test('暂停菜单导航：下键移动光标并响音效 10（previous 记上一位置）；Esc 直接选回到游戏', needDat, () => {
  const frames = gameRun(byName('st1-pause-nav-600'));
  assert.deepEqual(frames[215].menu.menu.cursor.slice(0, 2), [1, 0]);
  assert.deepEqual(frames[215].menu.sfx, [kSfxMove]);
  assert.deepEqual(frames[230].menu.menu.cursor.slice(0, 2), [2, 1]);
  assert.equal(frames[250].menu.menu.state, kExecute);
  assert.equal(frames[250].menu.menu.cursor[0], 0);
  assert.equal(frames[262].menu.menu.mode, kClosed);
});

test('in-game replay name entry wraps between the first and last rows', needDat, () => {
  const c = byName('st1-pause-nav-600');
  // 1.00a Oracle: pause, select Save Replay, confirm, select the first slot.
  const presses = [[200, 0x100], [215, 0x20], [230, 0x20], [245, 1], [280, 0x10], [285, 1], [320, 1],
                   [334, 0x10], [336, 0x20], [338, 0x40], [340, 0x10], [342, 0x20]];
  const input = presses.flatMap(([frame, bits]) => [`${frame}:${bits}`, `${frame + 1}:0`]).join(',');
  const frames = gameRun({...c, input}, 343);
  for (const [step, cursor] of [[333, 0], [334, 78], [336, 0], [338, 12], [340, 90], [342, 12]]) {
    const menu = frames[step].menu.menu;
    assert.equal(menu.state, 0xc);
    assert.equal(menu.name_cursor[0], cursor, `frame ${step + 1}`);
  }
});

test('暂停菜单确定：射击键在主菜单上选「回到游戏」→ 音效 7、状态 0x10；恢复后自机继续射击', needDat, () => {
  const frames = gameRun(byName('st1-pause-shoot-600'));
  assert.deepEqual(frames[230].menu.sfx, [kSfxConfirm]);
  assert.equal(frames[230].menu.menu.state, kExecute);
  assert.equal(frames[242].menu.menu.mode, kClosed);
  assert.ok(frames.slice(243, 300).some(f => f.menu.sfx.includes(0)), 'shot sfx after resume');
});

test('续关：残机 < 0 的复活等待第 0x1e 帧打开续关画面（模式 2、音效 14）；未入榜 → 主菜单 0xa0；确定后残机 2、分数清零、续关 +1 / 剩余 −1', needDat, () => {
  const frames = gameRun(byName('st1-continue-1500'));
  const open = frames[1040];
  assert.equal(open.menu.menu.mode, kContinue);
  assert.equal(open.menu.menu.state, kContinueIntro);
  assert.deepEqual(open.menu.sfx, [kSfxPause]);
  assert.equal(open.resources.lives, -1);
  // 第 10 帧：分数 4042 低于默认榜底 10000 → no_rank，直接进主菜单；续关菜单 VM 生成
  const menu = frames[1051];
  assert.equal(menu.menu.menu.state, kMainMenu);
  assert.equal(menu.menu.menu.no_rank, 1);
  assert.deepEqual(menu.menu.menu.vm, [1, 1]);
  assert.deepEqual(menu.menu.menu.cursor, [0, 0, 5, 0]);
  assert.deepEqual([menu.menu.continues_used, menu.menu.continues_left], [0, 5]);
  // 第 1100 帧按射击确定 → 状态 0x10；第 0xc 帧执行续关
  assert.deepEqual(frames[1100].menu.sfx, [kSfxConfirm]);
  const resumed = frames[1112];
  assert.equal(resumed.menu.menu.mode, kClosed);
  assert.equal(resumed.menu.game_flags & kMenuOpen, 0);
  assert.deepEqual([resumed.menu.continues_used, resumed.menu.continues_left], [1, 4]);
  assert.equal(resumed.resources.lives, 2);
  assert.ok(resumed.resources.score < 100, `score reset, got ${resumed.resources.score}`);
  assert.equal(resumed.power, 400);
});
