// Supervisor / Chain / 标题菜单：Boot→Title 时序（0x43b290 请求 Title；0x44abc0 等 0xb4 帧）与 0x44b5f0 状态机。
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {copyFileSync, existsSync, mkdirSync, rmSync} from 'node:fs';
import * as fs from 'node:fs';
import * as os from 'node:os';
import * as path from 'node:path';
import {resolve} from 'node:path';
import {frameDigests} from '../tools/draw-structure.mjs';

const here = resolve(import.meta.dirname);
const monorepo = resolve(here, '../../..');
const harness = process.env.TH16_HARNESS ?? resolve(here, 'build/harness');
const dat = process.env.TH16_DAT ?? resolve(monorepo, 'games/th16/original/th16.dat');
const available = existsSync(dat);
const needDat = {skip: !available && 'original th16.dat not present'};

function run(args) {
  return execFileSync(harness, args, {encoding: 'utf8'});
}

test('chain: self-removal keeps captures alive until return and preserves mutable callback state', () => {
  assert.equal(run(['chain-self-remove']).trim(), 'trace=1F2F alive=1 released=1 count=1');
});

function parse(out) {
  const lines = out.trim().split('\n');
  const header = Object.fromEntries(lines[0].split(' ').map(kv => kv.split('=')).map(([k, v]) => [k, Number(v)]));
  const events = lines.filter(l => l.startsWith('event ')).map(l => l.split(' ').slice(1));
  // quad <s|w> slot=N entry=N x0 y0 x1 y1 x2 y2 x3 y3 color z=… rhw=… fog=… zt= zw= uv=…
  const quads = lines.filter(l => l.startsWith('quad '));
  return {header, events, quads};
}

// 0x44af80 进标题第 10 帧的延迟重播 th16_01（bgm-op / bgm-default）与菜单 SE 无关
const ui = events => events.filter(e => !e[0].startsWith('bgm'));

// 位：0x1 Z，0x2 X，0x10 ↑，0x20 ↓，0x100 Esc，0x80000 Enter
const Z = '0x1', X = '0x2', UP = '0x10', DOWN = '0x20', LEFT = '0x40', RIGHT = '0x80', ESC = '0x100', ENTER = '0x80000';
const MENU_LIVE = 180;       // 启动幕 +0x638 到 0xb4 的那一帧（绘制之后）标题线程登记更新链；该链从下一帧起运行
// 首次进标题（DAT_004a6f1c == 0 → +0x5ce8 bit1）：0x44b5f0 不跳 Timer，从 0 数到 0x83（>0x82）才进 case 2
const MAIN = MENU_LIVE + 1 + 0x83;
// 常驻绘制链：Supervisor 画面合成 10 条（0x43ba40）+ AnmManager 层绘制 39 条（0x46a3a0：33 条普通层 + 6 条切相机的特殊层）
// + AsciiManager 文字 3 条（0x407cb0：优先级 0x51 / 0x35 / 0x42，启动幕装载完成 0x43b290 时启用）
const PERMANENT_DRAW_CHAINS = 53;  // +1：FrameTimer 全局 FPS 绘制链 0x4b（0x4263e0，默认禁用）

test('scene-boot: boot hold is exactly 0xb4 draw-chain frames, then title decor/intro', needDat, () => {
  const before = parse(run(['scene-boot', dat, String(MENU_LIVE)]));
  assert.equal(before.header.title_live, 1);
  assert.equal(before.header.boot_frames, 180);
  assert.equal(before.header.draw_chains, PERMANENT_DRAW_CHAINS, 'boot draw chain removed once the title thread unloads sig.anm');
  // 原版 oracle 启动完成后的更新链表：(4, 0x408070) AsciiManager、(6) 标题、(9 / 0x21) AnmManager、(0x1f) EffectManager
  assert.equal(before.header.update_chains, 5, 'AsciiManager chain (prio 4, 0x408070) + AnmManager UI/world tick chains (prio 9 / 0x21, 0x46e740 / 0x46e710) + EffectManager chain (prio 0x1f, 0x4187d0 at boot) + title update chain (prio 6) registered');
  // 标题更新链的首帧
  const firstTick = parse(run(['scene-boot', dat, String(MENU_LIVE + 1)]));
  assert.equal(firstTick.header.screen, 1, '0x44af80 case 0 → 0x44a560(1) main menu on the first title tick');
  assert.equal(firstTick.header.sub, 1, '0x44b5f0 case 0 falls into case 1 (Intro)');
  assert.ok(firstTick.header.quads >= 8, `title quads expected, got ${firstTick.header.quads}`);
  const notYet = parse(run(['scene-boot', dat, String(MENU_LIVE - 1)]));
  assert.equal(notYet.header.title_live, 0);
  assert.equal(notYet.header.draw_chains, PERMANENT_DRAW_CHAINS + 1);
});

test('scene-boot: input cannot skip the boot hold; DAT_004c1800&0x180 can', needDat, () => {
  const held = parse(run(['scene-boot', dat, '40', `5:${Z},6:${Z},7:${ENTER}`]));
  assert.equal(held.header.title_live, 0);
  assert.equal(held.header.boot_frames, 40);
  const skipped = parse(run(['scene-boot', dat, '20', 'skip']));
  assert.equal(skipped.header.title_live, 1);
  assert.equal(skipped.header.boot_frames, 1);
  assert.equal(skipped.header.sub, 1, 'first title entry plays the full intro (Timer 0 → 0x78 logo)');
});

test('title BGM (0x44af80 LAB_0044b0c6): th16_01 restarts on the 10th frame after title entry', needDat, () => {
  const bgm = f => parse(run(['scene-boot', dat, String(f)])).events.filter(e => e[0].startsWith('bgm'));
  assert.deepEqual(bgm(MENU_LIVE + 10), []);
  assert.deepEqual(bgm(MENU_LIVE + 11), [['bgm-op', '3'], ['bgm-default', '1'], ['bgm-op', '2']]);
});

test('title menu: Main at Timer>0x82, input before that is ignored', needDat, () => {
  const early = parse(run(['scene-boot', dat, String(MAIN + 1), `${MAIN - 2}:${DOWN}`]));
  assert.equal(early.header.sub, 2);
  assert.equal(early.header.cursor, 0, 'DOWN during Intro must not move the cursor');
  const atMain = parse(run(['scene-boot', dat, String(MAIN)]));
  assert.equal(atMain.header.sub, 2);
  const beforeMain = parse(run(['scene-boot', dat, String(MAIN - 1)]));
  assert.equal(beforeMain.header.sub, 1);
});

test('title menu: cursor wraps and skips disabled Extra (no clear data), SFX 10 per move', needDat, () => {
  const t = MAIN + 1;
  const down = parse(run(['scene-boot', dat, String(t + 4), `${t}:${DOWN}`]));
  assert.equal(down.header.cursor, 2, 'item 1 (Extra) is disabled by 0x440c00 when 0x44a850()==0');
  assert.deepEqual(ui(down.events), [['sfx', '10']]);
  const up = parse(run(['scene-boot', dat, String(t + 4), `${t}:${UP}`]));
  assert.equal(up.header.cursor, 9, 'wrap=1 (+0xf4) → 0 − 1 → 9');
  const upTwice = parse(run(['scene-boot', dat, String(t + 6), `${t}:${UP},${t + 2}:${UP},${t + 4}:${UP}`]));
  assert.equal(upTwice.header.cursor, 7);
  const upToExtra = parse(run(['scene-boot', dat, String(t + 4), `${t}:${DOWN},${t + 2}:${UP}`]));
  assert.equal(upToExtra.header.cursor, 0, '2 − 1 = 1 is disabled → 0');
});

test('title menu: a no-continue clear (+0x5184) enables Extra (0x44a850); Extra shot cursor jumps to the unlocked shot (0x44a800)', needDat, () => {
  const t = MAIN + 1;
  const down = parse(run(['scene-boot', dat, String(t + 4), `${t}:${DOWN}`, 'clear=2,1']));
  assert.equal(down.header.cursor, 1, 'Extra selectable once any shot has a no-continue clear on Easy..Lunatic');
  const extraOnly = parse(run(['scene-boot', dat, String(t + 4), `${t}:${DOWN}`, 'clear=2,4']));
  assert.equal(extraOnly.header.cursor, 2, 'an Extra clear alone does not count');
  const diff = t + 2 + AFTER_MAIN;
  const shot = parse(run(['scene-boot', dat, String(diff + 17), `${t}:${DOWN},${t + 2}:${Z},${diff}:${Z}`, 'clear=2,1']));
  assert.equal(shot.header.screen, 6);
  assert.equal(shot.header.difficulty, 4);
  assert.equal(shot.header.cursor, 2, 'last shot 0 is locked → first unlocked shot');
});

test('difficulty clear medal 0x9e+d stays visible only when all four shots cleared d without continues (0x44a8e0)', needDat, () => {
  const t = MAIN + 1;
  const frames = String(t + 150);
  const none = parse(run(['scene-boot', dat, frames, `${t}:${Z}`]));
  const three = parse(run(['scene-boot', dat, frames, `${t}:${Z}`, 'clear=0,1', 'clear=1,1', 'clear=2,1']));
  const four = parse(run(['scene-boot', dat, frames, `${t}:${Z}`, 'clear=0,1', 'clear=1,1', 'clear=2,1', 'clear=3,1']));
  assert.equal(none.header.screen, 5);
  assert.equal(three.header.quads, none.header.quads);
  assert.equal(four.header.quads, none.header.quads + 1, 'Normal medal drawn');
});

test('title menu: held DOWN auto-repeats after 0x19 frames every 8 frames (0x405580)', needDat, () => {
  const t = MAIN + 1;
  const script = Array.from({length: 40}, (_, i) => `${t + i}:${DOWN}`).join(',');
  const out = parse(run(['scene-boot', dat, String(t + 40), script]));
  // 按下沿 1 次 + 第 26 帧、34 帧连发 2 次 = 3 次移动：0→2→3→4
  assert.equal(out.events.filter(e => e[0] === 'sfx' && e[1] === '10').length, 3);
  assert.equal(out.header.cursor, 4);
});

test('title menu: confirm Game Start → sub 4, SFX 7, Difficulty(5) after 0x13 frames with cursor = last difficulty', needDat, () => {
  const t = MAIN + 1;
  const z = parse(run(['scene-boot', dat, String(t + 19), `${t}:${Z}`]));
  assert.equal(z.header.sub, 4);
  assert.equal(z.header.screen, 1, 'Timer must exceed 0x13 first');
  assert.deepEqual(ui(z.events), [['sfx', '7']]);
  const enter = parse(run(['scene-boot', dat, String(t + 21), `${t}:${ENTER}`]));
  assert.equal(enter.header.screen, 5);
  assert.equal(enter.header.cursor, 1, 'DAT_0049f274 initial = 1 (Normal)');
  assert.equal(enter.header.difficulty, 1);
  assert.deepEqual(ui(enter.events), [['sfx', '7'], ['screen', '5']]);
});

test('title menu: cancel (X|Esc) jumps to Quit with SFX 9; cancel on Quit confirms Quit → scene 3 (exit) request', needDat, () => {
  const t = MAIN + 1;
  const esc = parse(run(['scene-boot', dat, String(t + 3), `${t}:${ESC}`]));
  assert.equal(esc.header.cursor, 9);
  assert.equal(esc.header.sub, 2);
  assert.deepEqual(ui(esc.events), [['sfx', '9']]);
  const quit = parse(run(['scene-boot', dat, String(t + 27), `${t}:${ESC},${t + 2}:${X}`]));
  assert.equal(quit.header.scene, 3, '0x44af80 case 2 requests DAT_004c17c4 = 3 when DAT_004c1800 bit13 is clear');
  assert.equal(quit.header.quit, 1, '0x43ce10 case 3 tears down and returns 4');
  assert.equal(quit.header.title_live, 0, 'title object destroyed on scene switch');
  assert.deepEqual(ui(quit.events), [['sfx', '9'], ['sfx', '9'], ['screen', '2'], ['scene', '3']]);
  assert.deepEqual(quit.events.filter(e => e[0] === 'bgm-op').at(-1), ['bgm-op', '3'], '0x43c440 stops the BGM (op 3)');
  // DAT_004c1800 bit13：请求 2，0x43ce10 无 case 2，只把当前场景记为 2，标题对象留着并每帧重发
  const halt = parse(run(['scene-boot', dat, String(t + 27), `${t}:${ESC},${t + 2}:${X}`, 'cfg=0x2000']));
  assert.equal(halt.header.scene, 2);
  assert.equal(halt.header.quit, 0);
  assert.equal(halt.header.title_live, 1);
  assert.ok(ui(halt.events).filter(e => e[0] === 'scene' && e[1] === '2').length > 1, 'case 2 re-requests every frame');
});

test('title menu: per-item target screens (0x44b5f0 case 4)', needDat, () => {
  const t = MAIN + 1;
  const expect = {2: 5, 3: 0x11, 4: 0xb, 5: 0xa, 6: 0xd, 7: 3, 8: 0x10};
  for (const [item, screen] of Object.entries(expect)) {
    const downs = Array.from({length: Number(item) - 1}, (_, i) => `${t + i * 2}:${DOWN}`);
    const confirmAt = t + (Number(item) - 1) * 2 + 2;
    const out = parse(run(['scene-boot', dat, String(confirmAt + 21), [...downs, `${confirmAt}:${Z}`].join(',')]));
    assert.equal(out.header.screen, screen, `item ${item} → screen ${screen}`);
    if (item === '2') assert.equal(out.header.mode, 1, 'Practice sets mode bits 0x10');
    if (item === '3') assert.equal(out.header.mode, 2, 'Spell Practice sets mode bits 0x20');
  }
});

// 画面切换节拍（确定帧 C → 下一画面可接受输入的帧）：
//   主菜单 C+20 set_screen，C+21 case0，C+28 sub2 → C+29 可输入
//   难度 / 机体 C+14 set_screen，C+15 case0，C+22 sub2 → C+23
//   季节（练习）C+10 set_screen(8)，C+11 case0，C+22 sub2 → C+23
//   取消 C → C+6 set_screen，C+7 case0（sub 1）
const AFTER_MAIN = 29, AFTER_DIFF = 23, AFTER_SHOT = 23, AFTER_SEASON_PRACTICE = 23, AFTER_CANCEL = 7;
const DIFF_READY = 7;
test('difficulty screen: ←/→ and ↑/↓ move (no wrap), confirm → Shot(6) with difficulty saved', needDat, () => {
  const t = MAIN + 1;
  const enterDiff = t + 21;                       // 第一帧 screen=5（case 0）
  const nav = enterDiff + DIFF_READY + 1;
  const down = parse(run(['scene-boot', dat, String(nav + 2), `${t}:${Z},${nav}:${DOWN}`]));
  assert.equal(down.header.cursor, 2, 'Normal → Hard');
  const downThrice = parse(run(['scene-boot', dat, String(nav + 6), `${t}:${Z},${nav}:${DOWN},${nav + 2}:${DOWN},${nav + 4}:${DOWN}`]));
  assert.equal(downThrice.header.cursor, 3, 'wrap=0: clamps at Lunatic');
  const upTwice = parse(run(['scene-boot', dat, String(nav + 4), `${t}:${Z},${nav}:${LEFT},${nav + 2}:${UP}`]));
  assert.equal(upTwice.header.cursor, 0, 'LEFT also moves −1; clamps at Easy');
  const confirm = parse(run(['scene-boot', dat, String(nav + 2 + 15), `${t}:${Z},${nav}:${DOWN},${nav + 2}:${Z}`]));
  assert.equal(confirm.header.screen, 6);
  assert.equal(confirm.header.difficulty, 2);
  assert.equal(confirm.header.cursor, 0, 'shot cursor = DAT_004a6f24 (last shot) = 0');
  const sfx = confirm.events.filter(e => e[0] === 'sfx').map(e => e[1]);
  assert.deepEqual(sfx, ['7', '10', '7']);
  const notYet = parse(run(['scene-boot', dat, String(nav + 2 + 14), `${t}:${Z},${nav}:${DOWN},${nav + 2}:${Z}`]));
  assert.equal(notYet.header.screen, 5, 'Timer must exceed 0xd');
});

test('difficulty screen: cancel → back to main menu with difficulty = cursor, main menu restarts at Intro', needDat, () => {
  const t = MAIN + 1;
  const nav = t + 21 + DIFF_READY + 1;
  const back = parse(run(['scene-boot', dat, String(nav + 2 + AFTER_CANCEL), `${t}:${Z},${nav}:${DOWN},${nav + 2}:${X}`]));
  assert.equal(back.header.screen, 1);
  assert.equal(back.header.sub, 0, 'set_screen(1) on cancel+6');
  const out = parse(run(['scene-boot', dat, String(nav + 2 + AFTER_CANCEL + 1), `${t}:${Z},${nav}:${DOWN},${nav + 2}:${X}`]));
  assert.equal(out.header.screen, 1);
  assert.equal(out.header.difficulty, 2);
  assert.equal(out.header.sub, 1, 'main menu re-enters case 0 → Intro');
  assert.equal(out.header.cursor, 0, 'cursor popped to Game Start');
  assert.deepEqual(out.events.at(-1), ['screen', '1']);
});

test('play count 0x42cb60 and play clock 0x42dbc0: pause banks wall time since the stage load', needDat, () => {
  const t = MAIN + 1;
  const diff = t + AFTER_MAIN;
  const shot = diff + AFTER_DIFF;
  const season = shot + 2 + AFTER_SHOT;
  const game = season + 2 + 0x28 + 2;
  const base = [`${t}:${Z}`, `${diff}:${Z}`, `${shot}:${RIGHT}`, `${shot + 2}:${Z}`, `${season}:${RIGHT}`, `${season + 2}:${Z}`];
  const scores = out => Object.fromEntries(out.split('\n').find(l => l.startsWith('scores ')).split(' ').slice(1)
    .map(kv => kv.split('=')).map(([k, v]) => [k, Number(v)]));
  const running = scores(run(['scene-boot', dat, String(game + 330), base.join(','), 'wall']));
  assert.equal(running.play_count, 1, 'entering a non-replay game bumps the Cirno block play count');
  assert.equal(running.play_time, 0, 'nothing is banked until 0x42dbc0 runs');
  // 墙钟 = 帧号/60；0x42cb60 末在装载帧重置起点，Esc 暂停（0x43f0f0）时按 u64(Δ*100) 截断累加约 301 帧
  const paused = scores(run(['scene-boot', dat, String(game + 330), [...base, `${game + 300}:${ESC}`].join(','), 'wall']));
  assert.equal(paused.play_count, 1);
  assert.equal(paused.play_time, 501);
  assert.equal(paused.total_time, 501, 'ST total play time grows by the same ticks');
  // 0x43c3f0：标题曲 0 号位与 0x42dc50 首帧的第 1 关曲目（DAT_004a23a4 +0x28 = 1）都记入音乐室已听过位
  assert.equal(running.bgm, 0x3, 'title track + stage 1 track heard');
});

const demoDir = resolve(monorepo, 'artifacts/th16/dat');
const needDemo = {skip: (!available || !existsSync(resolve(demoDir, 'demo1.rpy'))) && 'demo1.rpy not present'};
// 0x44af80：主菜单空闲 0x708 帧后进 demo1（scene 0xd → 7）
const DEMO_START = 1981;
const sceneChanges = out => out.split('\n').filter(l => l.startsWith('scene-change ')).map(l => l.split(' ').slice(1).join(' '));

test('title demo 0x42d7b0 bit6: plays to timer 0xf3c, then back to the title without marking BGM', needDemo, () => {
  const out = execFileSync(harness, ['scene-boot', dat, String(DEMO_START + 0xf3c + 10), 'scenes'],
    {encoding: 'utf8', env: {...process.env, TH16_REPLAY_DIR: demoDir}});
  // demo1 第 0x661 帧残机 -1：回放中 0x442560 不进续关（ReplayManager +0xc == 1），一直播到 0xf3c
  assert.deepEqual(sceneChanges(out), ['frame=0 1->4', `frame=${DEMO_START} 4->7`, `frame=${DEMO_START + 0xf3d} 7->4`]);
  const scores = Object.fromEntries(out.split('\n').find(l => l.startsWith('scores ')).split(' ').slice(1).map(kv => kv.split('=')));
  assert.equal(scores.bgm, '1', 'demo (bec bit6) skips the 0x42dc50 stage-track mark');
  assert.equal(Number(scores.play_count), 0, 'replay loads do not bump play counts');
});

test('title demo 0x42d7b0 bit6: shot/bomb/Esc/Enter held on the system input returns to the title', needDemo, () => {
  const at = DEMO_START + 200;
  const out = execFileSync(harness, ['scene-boot', dat, String(at + 20), `${at}:${Z}`, 'scenes'],
    {encoding: 'utf8', env: {...process.env, TH16_REPLAY_DIR: demoDir}});
  assert.deepEqual(sceneChanges(out), ['frame=0 1->4', `frame=${DEMO_START} 4->7`, `frame=${at + 1} 7->4`]);
});

test('game start flow: Normal / Cirno / Autumn → scene 7 with stage 1, title destroyed', needDat, () => {
  const t = MAIN + 1;
  const diff = t + AFTER_MAIN;
  const shot = diff + AFTER_DIFF;
  const season = shot + 2 + AFTER_SHOT;           // RIGHT 在 shot，确定在 shot+2
  const script = [`${t}:${Z}`, `${diff}:${Z}`, `${shot}:${RIGHT}`, `${shot + 2}:${Z}`, `${season}:${RIGHT}`, `${season + 2}:${Z}`].join(',');
  const atSeason = parse(run(['scene-boot', dat, String(season), script]));
  assert.equal(atSeason.header.screen, 7);
  assert.equal(atSeason.header.shot, 1, 'Cirno');
  assert.equal(atSeason.header.cursor, 1, 'season cursor starts at MenuCursor::set(shot)');
  const confirmed = parse(run(['scene-boot', dat, String(season + 2 + 12), script]));
  assert.equal(confirmed.header.screen, 7);
  assert.equal(confirmed.header.sub, 3);
  assert.ok(confirmed.events.some(e => e[0] === 'transition'), 'Timer==10 spawns the transition effect');
  assert.ok(confirmed.events.some(e => e[0] === 'bgm-fade'));
  const started = parse(run(['scene-boot', dat, String(season + 2 + 0x28 + 2), script]));
  assert.equal(started.header.scene, 7);
  assert.equal(started.header.title_live, 0);
  assert.equal(started.header.difficulty, 1);
  assert.equal(started.header.shot, 1);
  assert.equal(started.header.season, 2, 'Autumn');
  assert.equal(started.header.stage, 1);
  // 原版 oracle 进游戏第 3 帧的更新链：4/9/10/0xf/0x10/0x11/0x14/0x16/0x18/0x19/0x1a/0x1b/0x1c/0x1d/0x1e/0x1f/0x20/0x21/0x22
  //（19 条）；0x22 = 0x448e40 Replay 回放侧链随 Replay 子系统接入，含 0x22 Replay 回放链占位，共 19
  assert.equal(started.header.update_chains, 19, 'title update chain removed; AsciiManager(4) + AnmManager ticks(9, 0x21) + EffectManager(0x1f, boot) + GameManager(0xf)+GameMenu(10)+StageManager(0x11)+Replay 记录(0x10)+Replay 回放(0x22)+ScorePopup(0x14)+Player(0x16)+Bullet(0x1c)+Item(0x1d)+Laser(0x1b)+Enemy(0x1a)+Bomb(0x18)+Release(0x19)+Gui(0x20)+Spellcard(0x1e) registered (0x408070/0x46e740/0x46e710/0x4187d0/0x42d700/0x43e3f0/0x409db0/0x447fd0/0x448e40/0x449e50/0x440fb0/0x411dd0/0x42f440/0x4314a0/0x41b340/0x40d890/0x427ca0/0x417700)');
  // 0x44ad20：title.anm / title_v.anm VM 全部清掉；ascii.anm 落叶发射器（+0x73c）只收到 interrupt 1，
  // 其脚本 case 1 在 10/14 帧后自删，所以切场后短暂只剩槽 2 的落叶（mode 8）
  assert.ok(started.quads.length > 0, 'leaf particles (ascii.anm mode 8) still falling right after the switch');
  // 加上新游戏在装载期（0x4440e0）就生成的子机 VM：pl02.anm（槽 9，琪露诺脚本 7）
  // 槽 0 = text.anm（画面合成的渲染目标拷贝）、槽 4 = st01wl.anm（关卡背景）不算在内
  // 槽 5 = front.anm HUD 边框、槽 6 = st01logo.anm 关卡标题（都由 0x426d70 在 Gui 首帧生成；标题脚本 1 在层 0x17，
  // 340 帧后自删）：原版 game-oracle --draw 切场后第 1 帧就有 tex[5,0] n=8 与 tex[6,1] 的四边形，故也在允许之列
  // 槽 2 还有 AsciiManager 的 HUD 文字（0x428e70：屏幕四边形 `quad s`，精灵 0）；落叶是世界四边形 `quad w`
  const text = started.quads.filter(l => /^quad s slot=2 entry=0 /.test(l));
  const leaves = started.quads.filter(l => /^quad w slot=2 entry=/.test(l));
  const others = started.quads.filter(l => !/ slot=(0|2|4) entry=/.test(l));
  assert.ok(text.some(l => / 183.5 451.5 /.test(l) && / 60606060 /.test(l)), 'season level "0" at (183.5, 451.5) colour 60606060, as the oracle draw dump shows (group 1, field viewport)');
  assert.ok(text.some(l => / 527.5 41.5 /.test(l)), 'hiscore "100,000" starts at (527.5, 41.5) (right-aligned at 620, font 4)');
  assert.ok(leaves.length > 0, 'leaf particles still present');
  // slot 8 = effect.anm：标题确定后 0x418ba0 过场种类 0（中断 7）的面板 / 覆盖，会跨进 Game 场景前几帧
  assert.ok(others.length > 0 && others.every(l => / slot=(5|6|8|9) entry=/.test(l)), 'ascii leaves + HUD + logo + player option + effect transition remain: ' + others.filter(l => !/ slot=(5|6|8|9) entry=/.test(l)).slice(0, 3).join(' | '));
  assert.ok(others.some(l => / slot=6 entry=1 /.test(l)), 'stage logo title (script 1) drawn from the first game frame, as in the original');
  assert.ok(others.some(l => / slot=5 entry=0 /.test(l)), 'HUD frame (front.anm) drawn from the first game frame, as in the original');
  assert.ok(leaves.every(l => / zw=1 /.test(l)), 'mode 8 writes Z (0x43c530)');
  assert.deepEqual(started.events.at(-1), ['scene', '7']);
  const settled = parse(run(['scene-boot', dat, String(season + 2 + 0x28 + 2 + 16), script]));
  assert.ok(settled.quads.filter(l => !/ slot=(0|4) entry=/.test(l) && !/^quad s slot=2 entry=0 /.test(l)).every(l => / slot=(5|6|8|9) entry=/.test(l)), 'emitter gone; HUD / logo / option / optional effect transition remain');
  assert.ok(settled.quads.length > 0, 'player option VM keeps drawing');
});

test('title → game transition (effect kind 0): 0x42dc50 interrupts DAT_004c12a0 with 1, panels open (effect.anm 7..10) and self-delete', needDat, () => {
  const t = MAIN + 1;
  const diff = t + AFTER_MAIN;
  const shot = diff + AFTER_DIFF;
  const season = shot + 2 + AFTER_SHOT;
  const script = [`${t}:${Z}`, `${diff}:${Z}`, `${shot}:${RIGHT}`, `${shot + 2}:${Z}`, `${season}:${RIGHT}`, `${season + 2}:${Z}`].join(',');
  const gameFrame1 = season + 2 + 0x28 + 2;
  const panels = frame => parse(run(['scene-boot', dat, String(frame), script])).quads.filter(l => /^quad s slot=8 entry=0 /.test(l));
  const overlay = frame => parse(run(['scene-boot', dat, String(frame), script])).quads.filter(l => /^quad s slot=8 entry=12 /.test(l));
  // 中断 7 合拢的面板（脚本 3..6）跨进游戏场景；首帧 0x42dc50 末中断 1 后切到脚本 7..10（op 1 自删于 t=40/45/50/55）
  assert.equal(panels(gameFrame1 + 20).length, 4, 'four panels still animating 20 frames into the game');
  assert.equal(panels(gameFrame1 + 60).length, 0, 'all panels gone 60 frames into the game (never opened before this fix: the switchbg overlay covered the whole screen)');
  // 覆盖脚本 0xc5（eff_switchbg，模式 7 DESTALPHA）本身是 stop 不自删：VM 留着，但目标 alpha 被 0x4073a0 清零后不可见
  assert.equal(overlay(gameFrame1 + 60).length, 1, 'overlay VM stays alive (invisible through DESTALPHA once panels are gone)');
});

test('season panel (title script 145, mode 8): Y-flip through the UI camera, then settles on the mode-0 rectangle', needDat, () => {
  const t = MAIN + 1;
  const diff = t + AFTER_MAIN;
  const shot = diff + AFTER_DIFF;
  const season = shot + 2 + AFTER_SHOT;
  const script = [`${t}:${Z}`, `${diff}:${Z}`, `${shot}:${RIGHT}`, `${shot + 2}:${Z}`].join(',');
  const panel = frame => {
    const out = parse(run(['scene-boot', dat, String(frame), script]));
    // 季节面板 = title.anm（槽 16）贴图 38（weapon01b）
    const line = out.quads.find(l => / slot=16 entry=38 /.test(l));
    assert.ok(line, `script 145 drawn at frame ${frame}`);
    const p = line.split(' ').map(Number);
    const [x0, y0, x1, y3] = [p[4], p[5], p[6], p[11]];
    const z = line.match(/z=([^ ]+)/)[1].split(',').map(Number);
    const rhw = Number(line.match(/rhw=([^ ,]+)/)[1]);
    const zw = Number(line.match(/zw=(\d)/)[1]);
    return {x0, y0, x1, y3, z, rhw, zw};
  };
  // ins_401 y=π 起、20 帧转到 0：首帧几乎侧对相机（左右边翻转、宽度坍缩）
  const edge = panel(season);
  assert.ok(edge.x1 < edge.x0 && Math.abs(edge.x0 - edge.x1) < 20, `edge-on: ${JSON.stringify(edge)}`);
  assert.equal(edge.zw, 1, 'mode 8 writes Z');
  // 中途：透视使左右 rhw/z 不同（0x43c780 相机 eye z = 240/tan(15°)）
  const mid = panel(season + 5);
  assert.ok(mid.x0 < mid.x1 && Math.abs(mid.z[0] - mid.z[1]) > 1e-4, `perspective mid-flip: ${JSON.stringify(mid)}`);
  // 转到 0 后与 mode 0 未对齐前的矩形一致：pos(640,660)×0.5 ± 精灵 619×205（weapon01b）×0.5 /2
  const flat = panel(season + 10);
  assert.ok(Math.abs(flat.x0 - 165.25) < 1e-3 && Math.abs(flat.x1 - 474.75) < 1e-3, `flat x: ${flat.x0}..${flat.x1}`);
  assert.ok(Math.abs(flat.y0 - 278.75) < 1e-3 && Math.abs(flat.y3 - 381.25) < 1e-3, `flat y: ${flat.y0}..${flat.y3}`);
  const d = 240 / Math.tan(Math.PI / 12);
  assert.ok(Math.abs(flat.rhw - 1 / d) < 1e-7, `rhw = 1/${d}`);
  assert.ok(Math.abs(flat.z[0] - (10000 / 9999) * (1 - 1 / d)) < 1e-5, 'viewport depth of the z=0 plane');
  // 脚本随后 ins_302 0 回到屏幕模式：像素对齐 ROUND−0.5
  const settled = panel(season + 20);
  assert.equal(settled.x0, 164.5); assert.equal(settled.x1, 474.5); assert.equal(settled.rhw, 1); assert.equal(settled.zw, 0);
});

test('shot screen: cancel returns to Difficulty with cursor popped', needDat, () => {
  const t = MAIN + 1;
  const diff = t + AFTER_MAIN;
  const shot = diff + 2 + AFTER_DIFF;             // DOWN 在 diff，确定在 diff+2
  const out = parse(run(['scene-boot', dat, String(shot + 2 + AFTER_CANCEL + 1), `${t}:${Z},${diff}:${DOWN},${diff + 2}:${Z},${shot}:${RIGHT},${shot + 2}:${X}`]));
  assert.equal(out.header.screen, 5);
  assert.equal(out.header.cursor, 2, 'difficulty cursor restored (Hard)');
  assert.equal(out.header.shot, 1, 'DAT_004a57a4 keeps the shot that was highlighted');
});

test('practice: season confirm goes to Stage select (8); locked stages refuse with SFX 0x10', needDat, () => {
  const t = MAIN + 1;
  const diff = t + 2 + AFTER_MAIN;                // 主菜单 ↓ 到 Practice（2）再确定
  const shot = diff + AFTER_DIFF;
  const season = shot + AFTER_SHOT;
  const stage = season + AFTER_SEASON_PRACTICE;
  const script = [`${t}:${DOWN}`, `${t + 2}:${Z}`, `${diff}:${Z}`, `${shot}:${Z}`, `${season}:${Z}`, `${stage}:${Z}`].join(',');
  const out = parse(run(['scene-boot', dat, String(stage + 2), script]));
  assert.equal(out.header.mode, 1);
  assert.equal(out.header.screen, 8);
  assert.equal(out.header.cursor, 0, 'DAT_004a2974 = -1 clamps to 0');
  assert.deepEqual(out.events.at(-1), ['sfx', '16'], 'no clear data → stage locked');
});

test('practice: stage unlock reads +0x51a5 (reached flag of cursor+1 for shot×difficulty)', needDat, () => {
  const t = MAIN + 1;
  const diff = t + 2 + AFTER_MAIN;
  const shot = diff + AFTER_DIFF;
  const season = shot + AFTER_SHOT;
  const stage = season + AFTER_SEASON_PRACTICE;
  const script = [`${t}:${DOWN}`, `${t + 2}:${Z}`, `${diff}:${Z}`, `${shot}:${Z}`, `${season}:${Z}`, `${stage}:${Z}`].join(',');
  const probe = parse(run(['scene-boot', dat, String(stage), script]));
  const block = probe.header.shot;
  const d = probe.header.difficulty;
  const other = parse(run(['scene-boot', dat, String(stage + 2), script, `reach=${block},${d},2`]));
  assert.deepEqual(other.events.at(-1), ['sfx', '16'], 'stage 2 reached does not unlock stage 1');
  const out = parse(run(['scene-boot', dat, String(stage + 2), script, `reach=${block},${d},1`]));
  assert.equal(out.header.screen, 8);
  assert.notDeepEqual(out.events.filter(e => e[0] === 'sfx').at(-1), ['sfx', '16']);
  assert.ok(out.events.some(e => e[0] === 'sfx' && e[1] === '7'), 'confirm SFX 7');
  assert.ok(out.events.some(e => e[0] === 'bgm-fade'));
});

test('practice start: DAT_004a5bf8 digit key held on confirm (0x450ef0) sets starting lives = n-1, else 9 (0x42cb60); HUD 0x42c280 survives 9 lives', needDat, () => {
  const t = MAIN + 1;
  const diff = t + 2 + AFTER_MAIN;
  const shot = diff + AFTER_DIFF;
  const season = shot + AFTER_SHOT;
  const stage = season + AFTER_SEASON_PRACTICE;
  const script = [`${t}:${DOWN}`, `${t + 2}:${Z}`, `${diff}:${Z}`, `${shot}:${Z}`, `${season}:${Z}`, `${stage}:${Z}`].join(',');
  const probe = parse(run(['scene-boot', dat, String(stage), script]));
  const reach = `reach=${probe.header.shot},${probe.header.difficulty},1`;
  const gameFrame = stage + 0x28 + 4;
  // 练习关卡确认到 Game 场景首帧 0x42cb60：无数字键 → 9 残机；原版 0x42c280 第 9 格越过 +0x54..+0x70 落到 bomb_vms[0]
  const plain = parse(run(['scene-boot', dat, String(gameFrame), script, reach]));
  assert.equal(plain.header.scene, 7, 'practice stage started');
  assert.equal(plain.header.mode, 1);
  assert.equal(plain.header.practice_lives, 0);
  assert.equal(plain.header.lives, 9);
  // DIK_3（0x04）在确认帧按住 → DAT_004a5bf8 = 3 → 2 残机；前一帧按住不算（0x402130 在确认当帧快照）
  const three = parse(run(['scene-boot', dat, String(gameFrame), script, reach, `dik=${stage}:0x04`]));
  assert.equal(three.header.practice_lives, 3);
  assert.equal(three.header.lives, 2);
  const early = parse(run(['scene-boot', dat, String(gameFrame), script, reach, `dik=${stage - 1}:0x04`]));
  assert.equal(early.header.lives, 9);
  // 多键同时按住取最小数字（原版按 1..9 顺序 goto）；DIK_1 → 0 残机
  const both = parse(run(['scene-boot', dat, String(gameFrame), script, reach, `dik=${stage}:0x0a,${stage}:0x02`]));
  assert.equal(both.header.practice_lives, 1);
  assert.equal(both.header.lives, 0);
});

test('Option screen (0x44c570): cancel to Quit then back to main menu', needDat, () => {
  const t = MAIN + 1;
  const downs = Array.from({length: 6}, (_, i) => `${t + i * 2}:${DOWN}`);   // → Option (7)
  const confirmAt = t + 12 + 2;
  // 主菜单 case4 等 0x13 → Option case0/1 等 timer>6 后才可输入（约 confirmAt+30）
  // 首次取消跳到 Quit，再次取消进 case4，再等 timer>9 回主菜单
  const ready = confirmAt + 30;
  const out = parse(run(['scene-boot', dat, String(ready + 20), [
    ...downs, `${confirmAt}:${Z}`, `${ready}:${X}`, `${ready + 2}:${X}`,
  ].join(',')]));
  assert.equal(out.header.screen, 1);
  assert.ok(out.events.some(e => e[0] === 'option'));
});

test('Key Config (0x44e930): cancel ignored off Quit; ↑ to Quit then cancel → Option', needDat, () => {
  const t = MAIN + 1;
  const downs = Array.from({length: 6}, (_, i) => `${t + i * 2}:${DOWN}`);
  const confirmAt = t + 12 + 2;
  const optReady = confirmAt + 30;
  const kcReady = optReady + 40;
  // Option 上 ↓↓ 到 KeyConfig(2) 确定 → Key Config；光标 0 上取消无效；↑ 绕到 Quit(6) 后取消退出回 Option
  const out = parse(run(['scene-boot', dat, String(kcReady + 40), [
    ...downs, `${confirmAt}:${Z}`,
    `${optReady}:${DOWN}`, `${optReady + 2}:${DOWN}`, `${optReady + 4}:${Z}`,
    `${kcReady}:${X}`, `${kcReady + 4}:${UP}`, `${kcReady + 8}:${X}`,
  ].join(',')]));
  assert.ok(out.events.some(e => e[0] === 'key-config'));
  assert.ok(!out.events.some(e => e[0] === 'key-commit'));
  assert.ok(out.events.some(e => e[0] === 'screen' && e[1] === '3'));
  assert.equal(out.header.screen, 3);
});

test('Key Config bind (0x44f710): pad button → slot, swap, Quit commits to pad table + th16.cfg', needDat, () => {
  const t = MAIN + 1;
  const downs = Array.from({length: 6}, (_, i) => `${t + i * 2}:${DOWN}`);
  const confirmAt = t + 12 + 2;
  const optReady = confirmAt + 30;
  const kcReady = optReady + 40;
  const cfg = path.join(os.tmpdir(), `th16-keycfg-${process.pid}.cfg`);
  try { fs.unlinkSync(cfg); } catch {}
  // 槽 0(Shot) 按下按钮 1（原 Bomb）→ 对调：Shot=1 Bomb=0；↓ 到槽 1 按下按钮 9；↑×2 绕到 Quit(6) 确定 → 写回
  const raw = run(['scene-boot', dat, String(kcReady + 40), `config=${cfg}`, [
    ...downs, `${confirmAt}:${Z}`,
    `${optReady}:${DOWN}`, `${optReady + 2}:${DOWN}`, `${optReady + 4}:${Z}`,
    `P${kcReady}:1`,
    `${kcReady + 2}:${DOWN}`, `P${kcReady + 4}:9`,
    `${kcReady + 6}:${UP}`, `${kcReady + 8}:${UP}`, `${kcReady + 12}:${ENTER}`,
  ].join(',')]);
  const out = parse(raw);
  const binds = out.events.filter(e => e[0] === 'key-bind').map(e => e[1]);
  const buttons = out.events.filter(e => e[0] === 'key-button').map(e => e[1]);
  assert.deepEqual(binds, ['0', '1']);
  assert.deepEqual(buttons, ['1', '9']);
  assert.ok(out.events.some(e => e[0] === 'key-commit'));
  assert.equal(out.header.screen, 3);
  // Quit 把工作表写回 DAT_004a52e4 / 配置块：槽 0→表[0]=1，槽 1→表[1]：对调得 0 再被 9 覆盖
  assert.match(raw, /^pad 1,9,2,5,-1,-1,-1,-1,-1,3$/m);
  // th16.cfg 只在开机（回写）与退出时落盘：scene-boot 未经过 Exit，文件仍是开机回写的默认表
  const image = fs.readFileSync(cfg);
  assert.equal(image.length, 100);
  assert.equal(image.readUInt32LE(0), 0x160002);
  assert.equal(image.readInt16LE(4), 0);
  fs.unlinkSync(cfg);
});

test('pad-input-bits 0x4018e0 default table', () => {
  const out = run(['pad-input-bits']);
  assert.match(out, /b0=1 b1=2 b2=8 b5=100 b3=800 none=0/);
  assert.match(out, /unbound=0/);
});

test('config-file 0x43c050: missing → defaults written; bad → defaults; valid → kept; range → rejected', () => {
  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'th16-cfg-'));
  const missing = path.join(dir, 'th16.cfg');
  const out = run(['config-file', missing]);
  assert.match(out, /valid=0 bgm=100 se=80 opt=0 input=100 win=0 bgm_mode=1 se_on=1 res=5 skip=0 pad=0,1,2,5,-1,-1,-1,-1,-1,3/);
  const written = fs.readFileSync(missing);
  assert.equal(written.length, 100);
  assert.equal(written.toString('hex'), out.trim().split('image=')[1]);
  const custom = Buffer.alloc(100);
  custom.writeUInt32LE(0x160002, 0);
  [7, 1, 2, 5, -1, -1, -1, -1, -1, 3].forEach((v, i) => custom.writeInt16LE(v, 4 + 2 * i));
  custom.writeUInt16LE(600, 0x18); custom.writeUInt16LE(600, 0x1a);
  Buffer.from([1, 1, 1, 5, 0, 2, 55, 30, 1, 2]).copy(custom, 0x1c);
  custom.writeUInt32LE(0x300, 0x28); custom.writeInt32LE(100, 0x2c); custom.writeInt32LE(200, 0x30);
  const customPath = path.join(dir, 'custom.cfg');
  fs.writeFileSync(customPath, custom);
  const kept = run(['config-file', customPath]);
  assert.match(kept, /valid=1 bgm=55 se=30 opt=1 input=300 win=1 bgm_mode=1 se_on=1 res=5 skip=0 pad=7,1,2,5,-1,-1,-1,-1,-1,3/);
  assert.equal(kept.trim().split('image=')[1], custom.toString('hex'));
  const range = Buffer.from(custom); range[0x1f] = 6;
  fs.writeFileSync(path.join(dir, 'range.cfg'), range);
  assert.match(run(['config-file', path.join(dir, 'range.cfg')]), /valid=0 bgm=100 se=80/);
  fs.writeFileSync(path.join(dir, 'short.cfg'), custom.subarray(0, 99));
  assert.match(run(['config-file', path.join(dir, 'short.cfg')]), /valid=0/);
  fs.rmSync(dir, {recursive: true});
});

test('Option screen: ←/→ adjust BGM, Default restores 100/80', needDat, () => {
  const t = MAIN + 1;
  const downs = Array.from({length: 6}, (_, i) => `${t + i * 2}:${DOWN}`);
  const confirmAt = t + 12 + 2;
  const ready = confirmAt + 30;
  // 未读 th16.cfg 时 DAT_004c12c6/c7 为 BSS 初值 0。两次 ← 仍夹在 0。
  const left = parse(run(['scene-boot', dat, String(ready + 4), [
    ...downs, `${confirmAt}:${Z}`, `${ready}:${LEFT}`, `${ready + 2}:${LEFT}`,
  ].join(',')]));
  assert.equal(left.header.screen, 3);
  assert.equal(left.header.bgm, 0);
  assert.equal(left.header.se, 0);
  // ↓↓↓ 到 Default(3) 再确定
  const restored = parse(run(['scene-boot', dat, String(ready + 12), [
    ...downs, `${confirmAt}:${Z}`,
    `${ready}:${LEFT}`, `${ready + 2}:${LEFT}`,
    `${ready + 4}:${DOWN}`, `${ready + 6}:${DOWN}`, `${ready + 8}:${DOWN}`,
    `${ready + 10}:${Z}`,
  ].join(',')]));
  assert.equal(restored.header.bgm, 100);
  assert.equal(restored.header.se, 80);
});

test('Replay screen (0x451750): cancel returns to the main menu', needDat, () => {
  const t = MAIN + 1;
  // Extra 禁用：↓×3 → Replay(4)；主菜单 case4 等 0x13 后进 Replay，再等 timer>6
  const downs = Array.from({length: 3}, (_, i) => `${t + i * 2}:${DOWN}`);
  const confirmAt = t + 6 + 2;
  const ready = confirmAt + 30;
  const out = parse(run(['scene-boot', dat, String(ready + 20), [
    ...downs, `${confirmAt}:${Z}`, `${ready}:${X}`,
  ].join(',')]));
  assert.equal(out.header.screen, 1);
  assert.ok(out.events.some(e => e[0] === 'replay'));
});

test('Replay screen: confirm th16_01.rpy → scene 0xd play', needDat, () => {
  const slots = resolve('/tmp/th16-replay-slots');
  mkdirSync(slots, {recursive: true});
  copyFileSync(resolve(monorepo, 'artifacts/th16/dat/demo1.rpy'), resolve(slots, 'th16_01.rpy'));
  const t = MAIN + 1;
  const downs = Array.from({length: 3}, (_, i) => `${t + i * 2}:${DOWN}`);
  const confirmAt = t + 8;
  const ready = confirmAt + 30;
  const prev = process.env.TH16_REPLAY_DIR;
  process.env.TH16_REPLAY_DIR = slots;
  try {
    const out = parse(run(['scene-boot', dat, String(ready + 60), [
      ...downs, `${confirmAt}:${Z}`, `${ready}:${Z}`, `${ready + 20}:${Z}`,
    ].join(',')]));
    assert.ok(out.events.some(e => e[0] === 'replay-play' && e[1] === '0'));
    assert.ok(out.events.some(e => e[0] === 'scene' && e[1] === '13'));
  } finally {
    if (prev === undefined) delete process.env.TH16_REPLAY_DIR;
    else process.env.TH16_REPLAY_DIR = prev;
  }
});

test('Replay list draw (0x451d50): No. page, User page th16_ud????, stage list after confirm', needDat, () => {
  const slots = resolve('/tmp/th16-replay-list-draw');
  rmSync(slots, {recursive: true, force: true});
  mkdirSync(resolve(slots, 'replay'), {recursive: true});
  copyFileSync(resolve(monorepo, 'artifacts/th16/dat/demo1.rpy'), resolve(slots, 'replay/th16_01.rpy'));
  copyFileSync(resolve(monorepo, 'artifacts/th16/dat/demo2.rpy'), resolve(slots, 'replay/th16_udABCD.rpy'));
  const t = MAIN + 1;
  const downs = Array.from({length: 3}, (_, i) => `${t + i * 2}:${DOWN}`);
  const confirmAt = t + 8;
  const ready = confirmAt + 30;
  const prev = process.env.TH16_REPLAY_DIR;
  process.env.TH16_REPLAY_DIR = slots;
  const ascii = (frames, keys) => run(['scene-boot', dat, String(frames), 'ascii', [...downs, `${confirmAt}:${Z}`, ...keys].join(',')])
    .split('\n').filter(l => l.startsWith('ascii '));
  try {
    const page0 = ascii(ready + 1, []);
    assert.equal(page0[0], 'ascii 32.0 80.0 ffffff00 1 [No.01 ZUN      17/07/31 18:16 Reimu   Summer Hard    St3 0.0%]');
    assert.equal(page0[1], 'ascii 32.0 95.0 ff808080 1 [No.02 -------- --/--/-- --:-- ------- ------ ------- --- ---%]');
    assert.equal(page0.length, 25);

    const page1 = ascii(ready + 4, [`${ready}:${RIGHT}`]);
    assert.match(page1[0], /^ascii 32\.0 80\.0 ffffff00 1 \[ABCD  \S+ +\d\d\/\d\d\/\d\d \d\d:\d\d \S+ +\S+ +\S+ +\S+ +\(null\) 0\.0%\]$/);
    assert.equal(page1[1], 'ascii 32.0 95.0 ff808080 1 [User  -------- --/--/-- --:-- ------- ------ ------- --- ---%]');

    const stages = ascii(ready + 20, [`${ready}:${Z}`]);
    assert.equal(stages[0], 'ascii 32.0 80.0 ffffffff 1 [No.01 ZUN      17/07/31 18:16 Reimu   Summer Hard    St3 0.0%]');
    assert.equal(stages.length, 1 + 7);
    assert.match(stages[1], /^ascii 220\.0 128\.0 ff808080 1 \[Stage 1  ---------\]$/);
    assert.match(stages[3], /^ascii 220\.0 164\.0 ffffff00 1 \[Stage 3  \d{9}\]$/);
  } finally {
    if (prev === undefined) delete process.env.TH16_REPLAY_DIR;
    else process.env.TH16_REPLAY_DIR = prev;
  }
});

test('Manual screen (0x4545a0): cancel HelpScreen returns to main menu', needDat, () => {
  const t = MAIN + 1;
  const downs = Array.from({length: 7}, (_, i) => `${t + i * 2}:${DOWN}`);   // → Manual(8)
  const confirmAt = t + 14 + 2;
  // 进 Manual 约 +20；HelpScreen Menu 需 >0x13；Closing 再 >0x1d
  const ready = confirmAt + 50;
  const out = parse(run(['scene-boot', dat, String(ready + 40), [
    ...downs, `${confirmAt}:${Z}`, `${ready}:${X}`,
  ].join(',')]));
  assert.equal(out.header.screen, 1);
  assert.ok(out.events.some(e => e[0] === 'manual'));
});

test('Music Room (0x4546f0): cancel returns to main and restores default BGM', needDat, () => {
  const t = MAIN + 1;
  const downs = Array.from({length: 5}, (_, i) => `${t + i * 2}:${DOWN}`);   // → Music(6)
  const confirmAt = t + 10 + 2;
  // 主菜单 Timer>0x13 换屏 → case 0 生成 10 帧 → case 1 Timer>4 → case 2 才收取消
  const ready = confirmAt + 0x14 + 10 + 6 + 1;
  const script = [...downs, `${confirmAt}:${Z}`, `${ready}:${X}`].join(',');
  const early = parse(run(['scene-boot', dat, String(ready + 20), [...downs, `${confirmAt}:${Z}`, `${ready - 2}:${X}`].join(',')]));
  assert.equal(early.header.screen, 0xd, 'cancel during case 0/1 is ignored');
  const before = parse(run(['scene-boot', dat, String(ready + 10), script]));
  assert.equal(before.header.screen, 0xd, 'case 3 waits Timer>9');
  const out = parse(run(['scene-boot', dat, String(ready + 11), script]));
  assert.equal(out.header.screen, 1);
  const music = out.events.findIndex(e => e[0] === 'music');
    assert.deepEqual(out.events.slice(music),
    [['music', '17'], ['sfx', '9'], ['bgm-default', '1'], ['bgm-op', '2'], ['screen', '1']]);
});

test('Spell Practice (0x4553d0): cancel returns to the main menu', needDat, () => {
  const t = MAIN + 1;
  const downs = Array.from({length: 2}, (_, i) => `${t + i * 2}:${DOWN}`);   // → Spell Practice(3)
  const confirmAt = t + 4 + 2;
  const ready = confirmAt + 35;
  const out = parse(run(['scene-boot', dat, String(ready + 20), [
    ...downs, `${confirmAt}:${Z}`, `${ready}:${X}`,
  ].join(',')]));
  assert.equal(out.header.screen, 1);
  assert.ok(out.events.some(e => e[0] === 'spell-practice'));
});

test('Spell List (0x455900): from Spell Practice confirm, cancel → screen 0x11', needDat, () => {
  const t = MAIN + 1;
  const downs = Array.from({length: 2}, (_, i) => `${t + i * 2}:${DOWN}`);
  const confirmAt = t + 4 + 2;
  const spReady = confirmAt + 35;
  // 练习 → 列表；列表 case1 等 >10 后取消，再等 case4 >5 → 回练习 0x11
  const listReady = spReady + 40;
  const out = parse(run(['scene-boot', dat, String(listReady + 20), [
    ...downs, `${confirmAt}:${Z}`, `${spReady}:${Z}`, `${listReady}:${X}`,
  ].join(',')]));
  assert.ok(out.events.some(e => e[0] === 'spell-list'));
  assert.equal(out.header.screen, 0x11);
});

test('Spell Shot (0x456a20): list confirm → shot; cancel returns to list 0x12', needDat, () => {
  const t = MAIN + 1;
  const downs = Array.from({length: 2}, (_, i) => `${t + i * 2}:${DOWN}`);
  const confirmAt = t + 4 + 2;
  const spReady = confirmAt + 35;
  const listReady = spReady + 40;
  // 列表确定后 >0xd 进 shot，再等 case1 >10 才可取消
  const shotReady = listReady + 45;
  const out = parse(run(['scene-boot', dat, String(shotReady + 25), 'unlock-spells', [
    ...downs, `${confirmAt}:${Z}`, `${spReady}:${Z}`, `${listReady}:${Z}`,
    `${shotReady}:${X}`,
  ].join(',')]));
  assert.ok(out.events.some(e => e[0] === 'spell-shot'));
  assert.equal(out.header.screen, 0x12);
});

test('Spell Start (0x455d50): shot confirm → 0x14; cancel → 0x13', needDat, () => {
  const t = MAIN + 1;
  const downs = Array.from({length: 2}, (_, i) => `${t + i * 2}:${DOWN}`);
  const confirmAt = t + 4 + 2;
  const spReady = confirmAt + 35;
  const listReady = spReady + 40;
  const shotReady = listReady + 45;
  const startReady = shotReady + 45;
  const out = parse(run(['scene-boot', dat, String(startReady + 25), 'unlock-spells', [
    ...downs, `${confirmAt}:${Z}`, `${spReady}:${Z}`, `${listReady}:${Z}`,
    `${shotReady}:${Z}`, `${startReady}:${X}`,
  ].join(',')]));
  assert.ok(out.events.some(e => e[0] === 'spell-start'));
  assert.equal(out.header.screen, 0x13);
});

test('Spell HUD (0x456d50): locked id -1 still prints the empty SCORE line', needDat, () => {
  // cmp id,-1; jl 只跳过 -2。-1 读符卡数组前 0x9c（默认 0），四条未解锁槽都画空 SCORE，选中色只在机体页。
  const t = MAIN + 1;
  const downs = Array.from({length: 2}, (_, i) => `${t + i * 2}:${DOWN}`);
  const confirmAt = t + 4 + 2;
  const spReady = confirmAt + 35;
  const at = spReady + 46;
  const ascii = run(['scene-boot', dat, String(at), 'ascii', [...downs, `${confirmAt}:${Z}`, `${spReady}:${Z}`].join(',')])
    .split('\n').filter(l => l.startsWith('ascii '));
  assert.deepEqual(ascii, [0, 1, 2, 3].map(i =>
    `ascii 330.0 ${(191 + i * 44).toFixed(1)} ff404040 1 [SCORE        00  ----/----]`));
});

test('Spell List lock (0x456060): confirm locked slot plays sfx 16', needDat, () => {
  const t = MAIN + 1;
  const downs = Array.from({length: 2}, (_, i) => `${t + i * 2}:${DOWN}`);
  const confirmAt = t + 4 + 2;
  const spReady = confirmAt + 35;
  const listReady = spReady + 40;
  const out = parse(run(['scene-boot', dat, String(listReady + 5), [
    ...downs, `${confirmAt}:${Z}`, `${spReady}:${Z}`, `${listReady}:${Z}`,
  ].join(',')]));
  assert.ok(out.events.some(e => e[0] === 'spell-list'));
  assert.ok(out.events.some(e => e[0] === 'sfx' && e[1] === '16'));
  assert.equal(out.header.screen, 0x12, 'stays on list when locked');
});

test('title_origin=3: name-entry screen 0xe (0x4532f0)', needDat, () => {
  const out = parse(run(['scene-boot', dat, String(MENU_LIVE + 20), 'skip', 'origin=3']));
  assert.equal(out.header.screen, 0xe);
  assert.ok(out.events.some(e => e[0] === 'name-entry'));
});

test('Player Data (0x452330): cancel returns to the main menu', needDat, () => {
  const t = MAIN + 1;
  const downs = Array.from({length: 4}, (_, i) => `${t + i * 2}:${DOWN}`);   // → Score/Player Data(5)
  const confirmAt = t + 8 + 2;
  const ready = confirmAt + 30;
  const out = parse(run(['scene-boot', dat, String(ready + 20), [
    ...downs, `${confirmAt}:${Z}`, `${ready}:${X}`,
  ].join(',')]));
  assert.equal(out.header.screen, 1);
  assert.ok(out.events.some(e => e[0] === 'score'));
});

test('Player Data (0x452330 / 0x452c30 / 0x453030): spell pages per difficulty, shot ←→ on +0x24, footer on every page', needDat, () => {
  const t = MAIN + 1;
  const downs = Array.from({length: 4}, (_, i) => `${t + i * 2}:${DOWN}`);
  const ready = t + 8 + 2 + 30;
  const base = [...downs, `${t + 10}:${Z}`];
  const texts = out => out.split('\n').filter(l => l.startsWith('text '));
  const asciiRows = out => out.split('\n').filter(l => l.startsWith('ascii '));
  // 排名页：机体 0（灵梦）、难度 1（Normal）；10 条排名 + 底部 3 行
  const ranks = run(['scene-boot', dat, String(ready + 2), base.join(','), 'ascii']);
  assert.equal(parse(ranks).header.cursor, 0, 'shot cursor +0x24 starts at 0');
  assert.equal(asciiRows(ranks).length, 13);
  // Normal 共 26 张 → 页数 (26+9)/10+1 = 4；第 1 页 10 行 No.2/6/10…（未见过：灰 808080，问号行）
  const page1 = run(['scene-boot', dat, String(ready + 2), [...base, `${ready}:${Z}`].join(','), 'ascii', 'text']);
  const rows1 = texts(page1);
  assert.equal(rows1.length, 10);
  assert.ok(rows1.every(l => / color=808080 /.test(l)));
  assert.match(rows1[0], /\[No\.[^ ]+ [^ ]+    0\/   0\]$/, 'SJIS No.　　２ + 21 × ？ + captures/attempts');
  assert.deepEqual(asciiRows(page1).map(l => l.split(' ')[2]), ['378.0', '396.0', '414.0'], 'rank rows hidden, footer stays');
  // 第 3 页：剩 6 张 + 4 行空白（色 ffffffff 的 " "）；再确定回到第 0 页
  const pages = [...base, `${ready}:${Z}`, `${ready + 2}:${Z}`, `${ready + 4}:${Z}`];
  const rows3 = texts(run(['scene-boot', dat, String(ready + 6), pages.join(','), 'text'])).slice(-10);
  assert.equal(rows3.filter(l => / color=808080 /.test(l)).length, 6);
  assert.equal(rows3.filter(l => / color=ffffffff .*\[ \]$/.test(l)).length, 4);
  const wrapped = parse(run(['scene-boot', dat, String(ready + 8), [...pages, `${ready + 6}:${Z}`].join(',')]));
  assert.deepEqual(wrapped.events.filter(e => e[0] === 'score-page').map(e => e[1]), ['1', '2', '3', '0']);
  // 合计块见过（unlock-spells）→ 名字行 efefef；→ 换机体（sfx 10），符卡页按新机体刷新
  const seen = run(['scene-boot', dat, String(ready + 4), 'unlock-spells', [...base, `${ready}:${Z}`, `${ready + 2}:${RIGHT}`].join(','), 'text']);
  assert.equal(parse(seen).header.cursor, 1);
  assert.equal(texts(seen).length, 20, 'page refreshed after the shot change');
  assert.ok(texts(seen).every(l => / color=efefef /.test(l)));
});

test('Player Data secret (0x452330 / 0x402130 / 0x44a930): Marisa × Extra, typing ILOVEBEER unlocks everything', needDat, () => {
  const t = MAIN + 1;
  const downs = Array.from({length: 4}, (_, i) => `${t + i * 2}:${DOWN}`);
  const ready = t + 8 + 2 + 30;
  const nav = [...downs, `${t + 10}:${Z}`, `${ready}:${RIGHT}`, `${ready + 2}:${RIGHT}`, `${ready + 4}:${RIGHT}`,
    `${ready + 6}:${DOWN}`, `${ready + 8}:${DOWN}`, `${ready + 10}:${DOWN}`];
  const word = [0x17, 0x26, 0x18, 0x2f, 0x12, 0x30, 0x12, 0x12, 0x13];
  const typed = keys => 'dik=' + keys.map((k, i) => `${ready + 12 + i * 2}:${k}`).join(',');
  const done = ready + 12 + word.length * 2;
  const unlocked = run(['scene-boot', dat, String(done + 6), [...nav, `${done + 2}:${Z}`].join(','), typed(word), 'text']);
  const out = parse(unlocked);
  assert.equal(out.header.cursor, 3, 'Marisa');
  assert.deepEqual(out.events.filter(e => e[0] === 'score-unlock'), [['score-unlock', '1']]);
  assert.ok(out.events.some(e => e[0] === 'sfx' && e[1] === '17'));
  // 0x44a930 合计块挑战次数 +1 → Extra 符卡页 10 行都按“见过”画（efefef，本机体 0/0）
  const rows = unlocked.split('\n').filter(l => l.startsWith('text '));
  assert.equal(rows.length, 10);
  assert.ok(rows.every(l => / color=efefef /.test(l) && /    0\/   0\]$/.test(l)));
  // 中途按了别的字母（A）就从头来
  const wrong = [...word.slice(0, 4), 0x1e, ...word.slice(4)];
  const reset = parse(run(['scene-boot', dat, String(done + 6), nav.join(','), typed(wrong)]));
  assert.equal(reset.events.filter(e => e[0] === 'score-unlock').length, 0);
});

test('chain-demo: priority order, Remove, Restart, enable/disable', () => {
  assert.equal(run(['chain-demo']).trim(), 'BCCA|CA|CD r=1,1,1 count=3');
});

test('frame-clock: 60Hz jitter → 1 step, 120Hz → 0/1, 30Hz → 2, long stall → resync 1', () => {
  const sixty = run(['frame-clock', '1.000', '1.0165', '1.0335', '1.0501', '1.0668']).trim();
  assert.equal(sixty, '1,1,1,1,1');
  const onetwenty = run(['frame-clock', '1.000', '1.0083', '1.0167', '1.0250', '1.0333']).trim();
  assert.equal(onetwenty, '1,0,1,0,1');
  const thirty = run(['frame-clock', '1.000', '1.0333', '1.0667']).trim();
  assert.equal(thirty, '1,2,2');
  const stall = run(['frame-clock', '1.000', '1.0167', '5.0']).trim();
  assert.equal(stall, '1,1,1');
  const drift = run(['frame-clock', ...Array.from({length: 61}, (_, i) => String(1 + i * 0.0170))]).trim().split(',').map(Number);
  assert.equal(drift.reduce((a, b) => a + b, 0), 62, '17.0ms frames: one extra catch-up step over 60 frames');
});

test('input-edges: pressed on rising edge, repeat after 0x19 held frames then every 8, released on fall', () => {
  const held = ['0x20', '0x20', '0', ...Array.from({length: 34}, () => '0x20'), '0'];
  const rows = run(['input-edges', ...held]).trim().split('\n').map(l => l.split(' '));
  assert.deepEqual(rows[0], ['20', '0', '0']);
  assert.deepEqual(rows[1], ['0', '0', '0']);
  assert.deepEqual(rows[2], ['0', '0', '20']);
  assert.deepEqual(rows[3], ['20', '0', '0']);
  const repeatFrames = rows.map((r, i) => (r[1] === '20' ? i : -1)).filter(i => i >= 0);
  // 第 3 行重新按下算第 1 帧；第 26 帧（>0x19）首次连发，之后每 8 帧
  assert.deepEqual(repeatFrames, [3 + 25, 3 + 33]);
  assert.deepEqual(rows.at(-1), ['0', '0', '20']);
});

function spellState(out) {
  const line = out.split('\n').find(l => l.startsWith('spell '));
  return Object.fromEntries(line.split(' ').slice(1).map(kv => kv.split('=')));
}

test('Spell Practice return (DAT_004a6f1c = 5): 0x4553d0 / 0x455900 / 0x456a20 restore group / list / entry one page per frame', needDat, () => {
  const at = f => run(['scene-boot', dat, String(f), 'origin=5', 'spell-return=2,1,3', 'unlock-spells']);
  // 0x44af80 case 0 同帧落入 0x11：分组还原后直接进 0x12
  const first = at(MENU_LIVE + 1);
  assert.equal(parse(first).header.screen, 0x12);
  assert.deepEqual([spellState(first).group, spellState(first).return], ['2', '-1,1,3']);
  const list = at(MENU_LIVE + 2);
  assert.equal(parse(list).header.screen, 0x13);
  assert.deepEqual([spellState(list).list, spellState(list).slots], ['1', '22,23,24,25,-2']);
  const entry = parse(at(MENU_LIVE + 3)).header;
  assert.deepEqual([entry.screen, entry.sub, entry.cursor], [0x13, 1, 3]);
});

test('Spell Practice launch (0x455d50 case 3): DAT_004a6f1c = 5, return slots saved, difficulty from DAT_00491700', needDat, () => {
  const out = run(['scene-boot', dat, '270', 'origin=5', 'spell-return=2,1,3', 'unlock-spells', '195:0x1,225:0x1']);
  const h = parse(out).header;
  assert.deepEqual([h.scene, h.stage, h.difficulty], [7, 3, 3]);
  const s = spellState(out);
  assert.deepEqual([s.return, s.origin, s.replay_slot], ['2,1,3', '5', '25']);
});

test('practice return (DAT_004a6f1c = 4): difficulty → shot → season auto-confirm, one screen per frame, into stage select', needDat, () => {
  const screen = f => parse(run(['scene-boot', dat, String(f), 'origin=4'])).header.screen;
  assert.deepEqual([screen(MENU_LIVE + 1), screen(MENU_LIVE + 2), screen(MENU_LIVE + 3)], [6, 7, 8]);
  const out = run(['scene-boot', dat, String(MENU_LIVE + 4), 'origin=4']);
  assert.equal(spellState(out).origin, '1', '0x450ef0 case 0 resets DAT_004a6f1c 4 → 1');
});

test('practice return: Pause Q restores stage 2 and draws the original transition and scores', needDat, () => {
  const dir = fs.mkdtempSync(path.join(os.tmpdir(), 'th16-practice-return-'));
  try {
    const draw = path.join(dir, 'return.jsonl');
    const input = [
      '313:0x20,315:1,344:1,367:1,390:1,413:0x20,415:1',
      '562:0x100,595:0x10000',
    ].join(',');
    const out = run(['scene-boot', dat, '633', input, 'reach=0,1,1', 'reach=0,1,2',
      'scenes', 'draw-from=611', 'draw=' + draw]);
    assert.match(out, /^scene-change frame=456 4->7$/m);
    assert.match(out, /^scene-change frame=608 7->4$/m);
    assert.match(out, /^scene=4 .*\bscreen=8 sub=2 cursor=1\b/m);

    // Original D3D draws at the first return animation and first practice score table.
    const expected = new Map([
      [611, 'db98ae391c44ef6b73c4d1b3d43de3efa6b9e278'],
      [633, '71f086df3929978302142f5ea0825e5f7cb38015'],
    ]);
    const actual = new Map();
    for (const line of fs.readFileSync(draw, 'utf8').trim().split('\n')) {
      const {frame, ops} = JSON.parse(line);
      if (expected.has(frame)) actual.set(frame, frameDigests(ops).v);
    }
    assert.deepEqual(actual, expected);
  } finally {
    fs.rmSync(dir, {recursive: true, force: true});
  }
});
