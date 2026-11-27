// Ending 场景 0xf / Extra 0x10：进出与跳过回标题
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {copyFileSync, existsSync, mkdirSync, mkdtempSync, readFileSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {frameDigests, segmentDigest} from '../tools/draw-structure.mjs';

const here = resolve(import.meta.dirname);
const monorepo = resolve(here, '../../..');
const harness = process.env.TH16_HARNESS ?? resolve(here, 'build/harness');
const dat = process.env.TH16_DAT ?? resolve(monorepo, 'games/th16/original/th16.dat');
const needDat = {skip: !existsSync(dat) && 'original th16.dat not present'};

function run(args) {
  return execFileSync(harness, args, {encoding: 'utf8'});
}

test('ending-smoke: skip after 0x78 frames returns to title', needDat, () => {
  const out = run(['ending-smoke', dat, '140', 'noskip=0', '121:0x1']);
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

test('ending: Loading clears after the animation loads and Z advances one line', needDat, () => {
  const state = (frames, ...input) => run(['ending-smoke', dat, String(frames), ...input]).split('\n')[0];
  assert.match(state(1), /\bloading=1\b/);
  assert.match(state(2), /\bloading=0\b/);
  const afterConfirm = state(122, '121:0x1');
  assert.match(afterConfirm, /scene=15 ending_live=1/);
  assert.match(afterConfirm, /\blines=2\b/);
  assert.match(afterConfirm, /current_bgm=bgm\/th16_14\.wav\b/);
  assert.match(afterConfirm, /\bloading=0\b/);
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
  const staffLoading = run(['ending-smoke', dat, '185', 'diff=1', ...inject]).split('\n')[0];
  const staffReady = run(['ending-smoke', dat, '186', 'diff=1', ...inject]).split('\n')[0];
  assert.match(staffLoading, /\bmsg=staff2\.msg\b.*\bloading=1\b/);
  assert.match(staffReady, /\bbgm=bgm\/th16_15\b.*\bloading=0\b/);
});

test('ending-smoke: ExtraEnding goes straight to title', needDat, () => {
  const out = run(['ending-smoke', dat, '5', 'extra=1']);
  const header = Object.fromEntries(out.trim().split('\n')[0].split(' ').map(kv => kv.split('=')).map(([k, v]) => [k, Number(v)]));
  assert.equal(header.ending_live, 0);
  assert.equal(header.title_live, 1);
  assert.equal(header.scene, 4);
});

test('Extra clear: the final countdown, loading frame and name entry match the original draws', needDat, () => {
  const dir = mkdtempSync(resolve(tmpdir(), 'th16-extra-clear-'));
  try {
    const draw = resolve(dir, 'extra-clear.jsonl');
    const out = run(['ending-smoke', dat, '525', 'extra=1', 'diff=4', 'score=800000',
      'clear-at=100', 'draw-from=470', 'draw=' + draw]);
    assert.match(out, /scene=4 ending_live=0 .*\btitle_screen=14\b/);
    assert.match(out, /current_bgm=th128_08\.wav loading=0/);

    // Award a clear with 0x42e150 after 100 frames, then run the original countdown and scene switch.
    const frames = Array.from({length: 56}, (_, i) => 470 + i);
    const captures = readFileSync(draw, 'utf8').trim().split('\n').map(line => JSON.parse(line));
    assert.deepEqual(captures.map(record => record.frame), frames);
    const digests = new Map(captures.map(({frame, ops}) => [frame, frameDigests(ops)]));
    assert.equal(segmentDigest(digests, frames, 'v'), 'ee169a2bc0bb5a7ae913930f24652ade63d693df');
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('ExtraEnding → title_origin 3: name entry 0xe (0x4532f0) → replay save 0xf (0x453c10) → main menu', needDat, () => {
  // 入榜第 1 名：Z 落 'A'，←↑ 到 End（0x5a），Z 结束 → 未续关进 0xf；Z 选槽 1，名字回填后光标在 End，Z 存档，X 回主菜单
  const out = run(['ending-smoke', dat, '70', 'extra=1', 'diff=4', 'score=900000000',
    '20:0x1', '22:0x40', '24:0x10', '26:0x1', '50:0x1', '52:0x1', '54:0x2']);
  const titleEvents = out.split('\n').filter(l => l.startsWith('title ')).map(l => l.slice(6));
  assert.deepEqual(titleEvents, [
    'bgm-name-entry 0', 'bgm-op 2', 'name-entry 0',
    'sfx 7', 'sfx 10', 'sfx 10', 'name-done 0', 'sfx 7', 'screen 15',
    'replay-save 0', 'sfx 7', 'sfx 17', 'replay-saved 1', 'sfx 7',
    'sfx 9', 'bgm-default 1', 'screen 1', 'bgm-op 2',
  ]);
  assert.match(out, /last_name=\[A {7}\] rank0=\[A {7}\]/);
  assert.match(out, /title_screen=1 title_origin=1/);
  assert.match(out, /current_bgm=th16_01\.wav\b/);
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
  assert.deepEqual(titleEvents.slice(0, 6), ['bgm-name-entry 0', 'bgm-op 2', 'name-entry -1', 'sfx 7', 'screen 15', 'replay-save 0']);
  assert.match(out, /current_bgm=th128_08\.wav\b/);
});

// 第一次观看，Normal，逐隔一帧按 Z。结束帧取自日文 1.00a 的 Oracle。
const endingCases = [
  [0, 0, 'e01.msg', 3127],
  [0, 1, 'e02.msg', 143],
  [1, 0, 'e03.msg', 3123],
  [1, 1, 'e04.msg', 157],
  [2, 0, 'e05.msg', 3137],
  [2, 1, 'e06.msg', 145],
  [3, 0, 'e07.msg', 3137],
  [3, 1, 'e08.msg', 145],
];
const endingPulses = length => Array.from({length: Math.ceil(length / 2)}, (_, i) => `${i * 2}:0x1`);

test('ending draw matches the original dialogue, fade and Staff Roll key frames', needDat, () => {
  // tools/oracle/ending-oracle.py --z-pulses, Japanese 1.00a, Normal Reimu.
  const expected = new Map([
    [1, '79b6c286e493e88e097f5a46ffdc970f47e520de'],
    [2, '46d8d35d10019fef9c92797059cd32833aa8f7b4'],
    [3, 'da0561d13f369e6857428a88c0aae57609114157'],
    [10, 'daa39411bd3a5c4dd364089ec833b1f38bc4a66a'],
    [60, '9befaa7f433b906269c833dd65e69e6dc0aef0f0'],
    [155, 'aa9a0cd18dea97c035d7399fabbb1a5b24f5b10a'],
    [185, 'fa32dee3b4f351d86d0da97a3579769c95387f5b'],
    [186, '39d5e156eaa878ae09fd328cc42e62302da542af'],
    [187, '223dad990987b52c53c23495348bc356ada462bb'],
    [188, 'ceef6fad3c58550e80d74010c6404991cd771ce2'],
    [450, '7ba3e60e085e85767d4e43e86903625b534af301'],
  ]);
  const dir = mkdtempSync(resolve(tmpdir(), 'th16-ending-draw-'));
  try {
    const draw = resolve(dir, 'ending.jsonl');
    run(['ending-smoke', dat, '450', `draw=${draw}`, ...endingPulses(450)]);
    const actual = new Map();
    for (const line of readFileSync(draw, 'utf8').trimEnd().split('\n')) {
      const {frame, ops} = JSON.parse(line);
      if (expected.has(frame)) actual.set(frame, frameDigests(ops).v);
    }
    assert.deepEqual(actual, expected);
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

for (const [shot, continues, message, endFrame] of endingCases) {
  test(`ending-script: ${message} follows the original Normal route into score entry`, needDat, () => {
    const state = frames => run(['ending-smoke', dat, String(frames),
      `shot=${shot}`, `continues=${continues}`, ...endingPulses(frames)]);
    const entry = state(1).split('\n')[0];
    assert.ok(entry.includes(` msg=${message} `));
    assert.match(entry, /\bloading=1$/);
    const before = state(endFrame - 1);
    assert.match(before.split('\n')[0], /^scene=15 ending_live=1 .*\bfinished=0\b/);
    assert.equal(before.includes('event staff\n'), continues === 0);
    assert.match(before, continues === 0 ? /current_bgm=bgm\/th16_15\.wav\b/ : /current_bgm=bgm\/th16_14\.wav\b/);
    const completed = state(endFrame).split('\n')[0];
    assert.match(completed, /^scene=15 ending_live=0 .*\bfinished=1\b/);
    const score = state(endFrame + 13).split('\n')[0];
    assert.match(score, /^scene=4 .*\btitle_live=1 title_screen=14\b.*current_bgm=th128_08\.wav\b/);
  });
}

test('ending-script: closing replay save restores title music', needDat, () => {
  const returned = run(['ending-smoke', dat, '3260', ...endingPulses(3140), '3210:0x2']);
  assert.match(returned, /title replay-save 0/);
  assert.match(returned, /title_screen=1\b.*current_bgm=th16_01\.wav\b/);
});
