// 过关流程对拍：第 1 面击破 Boss 后，通常模式走过关奖励展示 → 换到第 2 面（0x42e150 → 场景 0xc），
// 练习模式走练习结算菜单（0x43f500）→ 回标题（场景 4）。九组摘要与 scene_exit / exit_frame 对照
// tools/oracle/game-oracle.py（Unicorn 直跑原版）。
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
const golden = JSON.parse(readFileSync(resolve(here, 'golden/stage-clear.json'), 'utf8'));
const stagePlayback = new Map([
  ['st1-easy-reimu-spring-clear-to-st2', [
    'bgm-stop 1 1 -',
    'bgm-start 9 1 th16_02.wav',
    'bgm-start 6506 1 th16_03.wav',
    'bgm-stop 11724 2 th16_03.wav',
    'bgm-stop 11874 2 th16_03.wav',
    'bgm-start 11880 2 th16_04.wav',
  ]],
  ['st2-normal-cirno-summer-clear-to-st3-24000', [
    'bgm-stop 1 2 -',
    'bgm-start 9 2 th16_04.wav',
    'bgm-start 7682 2 th16_05.wav',
    'bgm-stop 16524 3 th16_05.wav',
    'bgm-stop 16674 3 th16_05.wav',
    'bgm-start 16680 3 th16_06.wav',
  ]],
]);

for (const c of golden.cases) {
  test(`stage clear digests match oracle: ${c.name}`, needDat, () => {
    const playback = stagePlayback.get(c.name);
    const out = execFileSync(harness, [
      'game-digest', dat, String(c.stage), String(c.shot), String(c.season), String(c.frames), c.input, '', '',
      String(c.difficulty), String(c.replay_slot), String(c.lives), String(c.refill), String(c.mode),
    ], {
      encoding: 'utf8',
      maxBuffer: 1 << 28,
      env: playback ? {...process.env, TH16_TRACE_BGM: '1'} : process.env,
    });
    if (playback) {
      // 原版在过关后的装载帧发出停曲命令，下一帧音频后端消费该命令。
      assert.deepEqual(out.split('\n').filter(line => line.startsWith('bgm-')), playback);
    }
    const field = (key) => {
      const m = out.match(new RegExp(`\\b${key}=(-?\\d+)`));
      assert.ok(m, out);
      return Number(m[1]);
    };
    assert.equal(field('scene_exit'), c.scene_exit, '离开游戏场景的去向与原版不符');
    assert.equal(field('exit_frame'), c.exit_frame, '离开游戏场景的帧号与原版不符');
    for (const [key, want] of Object.entries(c.digests)) {
      const m = out.match(new RegExp(`\\b${key}=([0-9a-f]{16})`));
      assert.ok(m, out);
      assert.equal(m[1], want, `${key} 摘要与原版不符`);
    }
  });
}

// 原版队列记录停曲和播放请求。这里断言音频后端实际消费请求的帧。
const laterStagePlayback = [
  [3, 39828, [
    'bgm-stop 1 3 -',
    'bgm-start 9 3 th16_06.wav',
    'bgm-start 15718 3 th16_07.wav',
    'bgm-stop 39672 4 th16_07.wav',
    'bgm-stop 39822 4 th16_07.wav',
    'bgm-start 39828 4 th16_08.wav',
  ]],
  [4, 42381, [
    'bgm-stop 1 4 -',
    'bgm-start 9 4 th16_08.wav',
    'bgm-start 16878 4 th16_09.wav',
    'bgm-stop 42225 5 th16_09.wav',
    'bgm-stop 42375 5 th16_09.wav',
    'bgm-start 42381 5 th16_10.wav',
  ]],
  [5, 50124, [
    'bgm-stop 1 5 -',
    'bgm-start 9 5 th16_10.wav',
    'bgm-start 15889 5 th16_11.wav',
    'bgm-stop 49968 6 th16_11.wav',
    'bgm-stop 50118 6 th16_11.wav',
    'bgm-start 50124 6 th16_13.wav',
  ]],
];

for (const [stage, nextTrackFrame, playback] of laterStagePlayback) {
  test(`stage ${stage} switches to the original stage ${stage + 1} music`, needDat, () => {
    const out = execFileSync(harness, [
      'game-digest', dat, String(stage), '0', '0', String(nextTrackFrame + 1), '0:0', '', '', '0', '-1', '8', '1', '0',
    ], {encoding: 'utf8', env: {...process.env, TH16_TRACE_BGM: '1'}});
    assert.deepEqual(out.split('\n').filter(line => line.startsWith('bgm-')), playback);
  });
}

test('stage 6 Reimu spring tap fire switches to the boss music and enters Ending on original frame 39121', needDat, () => {
  const input = '@' + resolve(here, 'fixtures/st6-reimu-tap.input');
  const out = execFileSync(harness, [
    'game-digest', dat, '6', '0', '0', '40290', input, '', '', '0', '-1', '8', '1', '0',
  ], {encoding: 'utf8', env: {...process.env, TH16_TRACE_BGM: '1'}});

  // 1.00a queues slot 0 (th16_13) on frame 1 and slot 1 (th16_12) on frame 4793.
  assert.deepEqual(out.split('\n').filter(line => line.startsWith('bgm-start ')), [
    'bgm-start 9 6 th16_13.wav',
    'bgm-start 4798 6 th16_12.wav',
  ]);
  assert.match(out, /\bscene_exit=15 exit_frame=39121\b/);
});
