// Ending 场景 0xf / Extra 0x10：进出与跳过回标题
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {copyFileSync, existsSync, mkdirSync, rmSync} from 'node:fs';
import {resolve} from 'node:path';

const here = resolve(import.meta.dirname);
const monorepo = resolve(here, '../../..');
const harness = process.env.TH16_HARNESS ?? resolve(here, 'build/harness');
const dat = process.env.TH16_DAT ?? resolve(monorepo, 'games/th16/original/th16.dat');
const needDat = {skip: !existsSync(dat) && 'original th16.dat not present'};

function run(args) {
  return execFileSync(harness, args, {encoding: 'utf8'});
}

test('ending-smoke: skip after 0x78 frames returns to title', needDat, () => {
  const out = run(['ending-smoke', dat, '140', '121:0x1']);
  const headerLine = out.trim().split('\n')[0];
  const header = Object.fromEntries(headerLine.split(' ').map(kv => kv.split('=')).map(([k, v]) => {
    const n = Number(v);
    return [k, Number.isFinite(n) && !String(v).includes('/') ? n : v];
  }));
  assert.equal(header.ending_live, 0);
  assert.equal(header.finished, 1);
  assert.equal(header.title_live, 1);
  assert.equal(header.scene, 4);
  assert.equal(header.ending_index, 0, 'shot0 + continues0 → e01');
  assert.ok(out.includes('event begin'));
  assert.ok(out.includes('event msg-loaded'));
  assert.ok(out.includes('event vms'));
  assert.ok(out.includes('event anm-loaded'), 'op7 loads e01.anm');
  assert.ok(out.includes('event bgm'), 'op10 plays ending BGM');
  assert.ok(out.includes('event color'), 'op9 sets text color');
  assert.ok(out.includes('event text'), 'op3 decrypts ending line');
  assert.ok(out.includes('event end'));
  assert.match(headerLine, /bgm=bgm\/th16_14/);
});

test('ending-script: first page reaches wait without confirm', needDat, () => {
  const out = run(['ending-smoke', dat, '5']);
  assert.ok(out.includes('event anm-loaded'));
  assert.ok(out.includes('event anm-script'));
  assert.ok(out.includes('event bgm'));
  assert.ok(out.includes('event text'));
  assert.equal(out.includes('event end'), false, 'still waiting on op5');
  assert.match(out, /ending_live=1/);
  assert.match(out, /paints=[1-9]/, 'op3 queues 0x46d990 paints');
});

test('ending-script: mash confirm through dialogue reaches staff', needDat, () => {
  // 剧本时刻要走到 @152 才到 op12；noskip 关闭 >0x78 烟雾短路。
  // injected_input 是 held：奇偶交替才能每两帧产生一次 pressed 边沿（首次结局 flags bit0 禁长按快进）。
  const inject = Array.from({length: 320}, (_, i) => i % 2 === 0 ? `${i}:0x1` : null).filter(Boolean);
  const out = run(['ending-smoke', dat, '320', 'noskip=1', 'diff=1', ...inject]);
  assert.ok(out.includes('event staff'), 'op12 loads staffN.msg');
  assert.ok(out.includes('event fade-out'), 'op0xe ScreenEffect kind 5 before staff');
  assert.match(out, /msg=staff2\.msg/);
  assert.ok(out.includes('event anm-loaded'));
});

test('ending-smoke: ExtraEnding goes straight to title', needDat, () => {
  const out = run(['ending-smoke', dat, '5', 'extra=1']);
  const header = Object.fromEntries(out.trim().split('\n')[0].split(' ').map(kv => kv.split('=')).map(([k, v]) => [k, Number(v)]));
  assert.equal(header.ending_live, 0);
  assert.equal(header.title_live, 1);
  assert.equal(header.scene, 4);
});

test('ExtraEnding → title_origin 3: name entry 0xe (0x4532f0) → replay save 0xf (0x453c10) → main menu', needDat, () => {
  // 入榜第 1 名：Z 落 'A'，←↑ 到 End（0x5a），Z 结束 → 未续关进 0xf；Z 选槽 1，名字回填后光标在 End，Z 存档，X 回主菜单
  const out = run(['ending-smoke', dat, '70', 'extra=1', 'diff=4', 'score=900000000',
    '20:0x1', '22:0x40', '24:0x10', '26:0x1', '50:0x1', '52:0x1', '54:0x2']);
  const titleEvents = out.split('\n').filter(l => l.startsWith('title ')).map(l => l.slice(6));
  assert.deepEqual(titleEvents, [
    'bgm-name-entry 0', 'name-entry 0',
    'sfx 7', 'sfx 10', 'sfx 10', 'name-done 0', 'sfx 7', 'screen 15',
    'replay-save 0', 'sfx 7', 'sfx 17', 'replay-saved 1', 'sfx 7',
    'sfx 9', 'bgm-default 1', 'screen 1',
  ]);
  assert.match(out, /last_name=\[A {7}\] rank0=\[A {7}\]/);
  assert.match(out, /title_screen=1 title_origin=1/);
});

test('name entry draw (0x4538b0) and replay-save draw (0x4541b0): Ascii rows / colours / slide', needDat, () => {
  const slots = '/tmp/th16-name-entry-slots';
  rmSync(slots, {recursive: true, force: true});
  mkdirSync(slots, {recursive: true});
  copyFileSync(resolve(monorepo, 'artifacts/th16/dat/demo1.rpy'), resolve(slots, 'th16_01.rpy'));
  const base = ['extra=1', 'diff=4', 'score=900000000', `replay_dir=${slots}`, 'ascii', '20:0x1', '22:0x40'];
  const rows = n => run(['ending-smoke', dat, String(n), ...base, '24:0x10', '26:0x1', '50:0x1'])
    .split('\n').filter(l => l.startsWith('ascii '));

  const entry = run(['ending-smoke', dat, '23', ...base]).split('\n').filter(l => l.startsWith('ascii '));
  assert.equal(entry[0], 'ascii 22.0 160.0 ffffffff 1 [ 1            9000000000  ----/--/-- --:-- Season  Stage -  ---%]');
  assert.equal(entry[1].slice(0, 30), 'ascii 22.0 178.0 ff808040 1 [ ');
  assert.equal(entry[10], 'ascii 58.0 160.0 ffffffff 1 [A       ]');
  assert.equal(entry[11], 'ascii 67.0 160.0 ffffff00 1 [_]');
  assert.equal(entry[12], 'ascii 212.0 360.0 ff808080 1 [A]');
  assert.equal(entry[24], 'ascii 428.0 360.0 ffffff00 1 [M]', '← from column 0 wraps to column 12');
  assert.equal(entry.length, 10 + 2 + 0x5b);

  const list = rows(49);
  assert.equal(list[0], 'ascii 58.0 80.0 ffffff00 1 [No.01 ZUN      17/07/31 18:16 Reimu   Hard    St3 0.0%]');
  assert.equal(list[1], 'ascii 58.0 95.0 ff808080 1 [No.02 -------- --/--/-- --:-- ------- ------- --- ---%]');
  assert.equal(list.length, 25);

  const slide = rows(52);
  assert.match(slide[0], /^ascii 58\.0 112\.0 ffffffff 0 \[No\.01 {10}\d\d\/\d\d\/\d\d \d\d:\d\d Reimu {3}Easy {4}All 0\.0%\]$/);
  assert.equal(slide.length, 1, 'name input appears only from timer 10');
  const named = rows(64);
  assert.equal(named[1], 'ascii 112.0 240.0 ffffffff 0 [A       ]');
  assert.equal(named[2], 'ascii 121.0 240.0 ffffff00 0 [_]');
  assert.equal(named.length, 1 + 2 + 0x5b);
});

test('ExtraEnding with no rank: confirm skips straight to replay save', needDat, () => {
  const out = run(['ending-smoke', dat, '40', 'extra=1', 'diff=4', 'score=0', '20:0x1']);
  const titleEvents = out.split('\n').filter(l => l.startsWith('title ')).map(l => l.slice(6));
  assert.deepEqual(titleEvents.slice(0, 5), ['bgm-name-entry 0', 'name-entry -1', 'sfx 7', 'screen 15', 'replay-save 0']);
});

test('ending-script: mash through staff returns to title', needDat, () => {
  // staff 时刻到 ~2940；边沿确认跳过等待后约 3140 帧剧本结束 → 场景 4
  const inject = Array.from({length: 3140}, (_, i) => i % 2 === 0 ? `${i}:0x1` : null).filter(Boolean);
  const out = run(['ending-smoke', dat, '3140', 'noskip=1', 'diff=1', ...inject]);
  assert.ok(out.includes('event staff'));
  assert.ok(out.includes('event end'));
  assert.match(out, /finished=1/);
  assert.match(out, /scene=4/);
  assert.match(out, /title_live=1/);
  assert.match(out, /msg=staff2\.msg/);
});
