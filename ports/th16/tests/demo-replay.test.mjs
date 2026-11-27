// 标题演示回放整段对拍：0x43ce10 case 0xd → 0x42d700(1) → 0x447760(1) 段头全局块读回 / 0x448eb0 自机坐标 →
// 0x448130 逐帧取输入，直到 0x42d7b0 演示计时 0xf3c 请求标题；全部管理器摘要与 unicorn 原版一致
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { copyFileSync, existsSync, mkdirSync, mkdtempSync, readFileSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { fileURLToPath } from 'node:url';
import { dirname, join, resolve } from 'node:path';

const root = join(dirname(fileURLToPath(import.meta.url)), '..');
const monorepo = resolve(root, '../..');
const harness = process.env.TH16_HARNESS || join(root, 'tests/build/harness');
const dat = process.env.TH16_DAT ?? resolve(monorepo, 'games/th16/original/th16.dat');
const demosDir = process.env.TH16_REPLAY_DIR ?? resolve(monorepo, 'artifacts/th16/dat');
const golden = JSON.parse(readFileSync(join(root, 'tests/golden/demo-replay.json'), 'utf8'));
const need = { skip: !(existsSync(dat) && existsSync(join(demosDir, 'demo1.rpy'))) && 'th16.dat / demo*.rpy not present' };

for (const [name, g] of Object.entries(golden.demos)) {
  test(`demo-replay: ${name} matches the original through the 0xf3c title request`, need, () => {
    const r = spawnSync(harness, ['game-digest', dat, String(g.stage), String(g.shot), String(g.season), String(g.frames), '0:0'],
      { encoding: 'utf8', env: { ...process.env, TH16_DEMO: join(demosDir, `${name}.rpy`) } });
    assert.equal(r.status, 0, r.stderr || r.stdout);
    const got = Object.fromEntries(r.stdout.trim().split('\n').at(-1).split(' ').map((kv) => kv.split('=')));
    assert.equal(Number(got.scene_exit), g.scene_exit, 'scene request');
    assert.equal(Number(got.exit_frame), g.exit_frame, 'exit frame');
    for (const key of ['digest', 'enemies', 'enemy_vms', 'bullets', 'items', 'spell', 'lasers', 'bombs', 'menu', 'sound']) {
      assert.equal(got[key], g[key], `${name} ${key}`);
    }
  });
}

test('replay-roundtrip: a port recording plays back identically in the port and in the original', { skip: !existsSync(dat) && 'th16.dat not present' }, () => {
  const g = golden.recorded;
  const dir = mkdtempSync(join(tmpdir(), 'th16-rec-'));
  try {
    const args = ['game-digest', dat, String(g.stage), String(g.shot), String(g.season), String(g.frames)];
    const digests = (env, input) => {
      const r = spawnSync(harness, [...args, input], { encoding: 'utf8', env: { ...process.env, ...env } });
      assert.equal(r.status, 0, r.stderr || r.stdout);
      return Object.fromEntries(r.stdout.trim().split('\n').at(-1).split(' ').map((kv) => kv.split('=')));
    };
    const recorded = digests({ TH16_RECORD_DIR: dir }, g.input);
    const rpy = join(dir, 'replay', 'th16_01.rpy');
    assert.ok(existsSync(rpy), 'th16_01.rpy written');
    // 标题 Replay 口径（bit6 不置）与演示口径（golden 由原版 --demo 回放得出；1300 帧内两者玩法相同）
    const replayed = digests({ TH16_REPLAY: rpy }, '0:0');
    const demo = digests({ TH16_DEMO: rpy }, '0:0');
    for (const key of ['digest', 'enemies', 'enemy_vms', 'bullets', 'items', 'spell', 'lasers', 'bombs', 'sound']) {
      assert.equal(recorded[key], g[key], `recording run ${key}`);
      assert.equal(replayed[key], g[key], `port playback ${key}`);
      assert.equal(demo[key], g[key], `port demo playback ${key}`);
    }
    assert.equal(demo.menu, g.menu, 'demo playback menu digest');
  } finally {
    rmSync(dir, { recursive: true, force: true });
  }
});

test('replay-roundtrip: a two-segment recording (stage 1 clear → stage 2) plays back identically in the port and in the original', { skip: !existsSync(dat) && 'th16.dat not present' }, () => {
  // 0x449030 装载时备好新段头（种子），0x42dee0 → 0x448eb0（换关后第 0x1e 帧）才换段；其间的帧仍记 / 读上一段
  const g = golden.recorded_multi;
  const c = JSON.parse(readFileSync(join(root, 'tests/golden/stage-clear.json'), 'utf8')).cases.find((x) => x.name === g.case);
  const dir = mkdtempSync(join(tmpdir(), 'th16-rec-'));
  try {
    const run = (env, input) => {
      const r = spawnSync(harness, ['game-digest', dat, String(c.stage), String(c.shot), String(c.season), String(g.frames), input, '', '',
        String(c.difficulty), String(c.replay_slot), String(g.lives), '1', String(c.mode)],
        { encoding: 'utf8', env: { ...process.env, ...env }, maxBuffer: 1 << 26 });
      assert.equal(r.status, 0, r.stderr || r.stdout);
      return Object.fromEntries(r.stdout.trim().split('\n').at(-1).split(' ').map((kv) => kv.split('=')));
    };
    const recorded = run({ TH16_RECORD_DIR: dir }, c.input);
    const rpy = join(dir, 'replay', 'th16_01.rpy');
    const segs = spawnSync(harness, ['replay-globals', rpy], { encoding: 'utf8' }).stdout.trim().split('\n')
      .map((l) => ({ stage: Number(l.match(/ stage=(\d+)/)[1]), frames: Number(l.match(/ frames=(\d+)/)[1]) }));
    assert.deepEqual(segs, g.segments, 'segment split at the 0x448eb0 point');
    const title = spawnSync(harness, ['scene-boot', dat, '410', '314:0x20,316:0x20,318:0x20,322:1', 'ascii'],
      {encoding: 'utf8', env: {...process.env, TH16_REPLAY_DIR: dir}});
    assert.equal(title.status, 0, title.stderr);
    const savedStage = title.stdout.split('\n').find(line => line.startsWith('ascii ') && line.includes('[No.01 ')) ?? '';
    assert.match(savedStage, / St2\s+0\.0%\]$/, 'the list shows the stage where the replay was saved');
    const replayed = run({ TH16_REPLAY: rpy }, '0:0');
    for (const key of ['digest', 'enemies', 'enemy_vms', 'bullets', 'items', 'spell', 'lasers', 'bombs', 'menu', 'sound']) {
      assert.equal(recorded[key], g[key], `recording run ${key}`);
      assert.equal(replayed[key], g[key], `port playback ${key}`);
    }

    // Starting from the second segment and pressing R in Pause uses the stage
    // stored as the run's start, along with that stage's replay state and input.
    const chooseStage2 = '314:0x20,316:0x20,318:0x20,322:1,373:1,392:0x20,401:1';
    const boot = (frames, input) => {
      const result = spawnSync(harness, ['scene-boot', dat, String(frames), input, 'scenes'],
        { encoding: 'utf8', env: { ...process.env, TH16_REPLAY_DIR: dir } });
      assert.equal(result.status, 0, result.stderr || result.stdout);
      assert.match(result.stdout, /^event replay-play 0$/m);
      return result.stdout;
    };
    const secondStage = boot(545, chooseStage2);
    assert.match(secondStage, /^scene=7 .*\bstage=2\b/m);
    assert.match(secondStage, /^game .*\bstd=st02\.std\b/m);
    const restarted = boot(650, chooseStage2 + ',550:0x100,583:0x200000');
    assert.match(restarted, /^scene=7 .*\bstage=1\b/m);
    assert.match(restarted, /^game .*\bframe=54\b.*\bstd=st01\.std\b/m);
  } finally {
    rmSync(dir, { recursive: true, force: true });
  }
});

test('replay-globals: stage header +0x14 snapshot decodes the 0x4a5790 block', need, () => {
  const r = spawnSync(harness, ['replay-globals', join(demosDir, 'demo1.rpy')], { encoding: 'utf8' });
  assert.equal(r.status, 0, r.stderr || r.stdout);
  assert.equal(r.stdout.trim(),
    '0 stage=3 shot=0+0 season=1 diff=2 score=0 lives=2/0 bombs=3/0 power=400/400/100 gauge=1140/1140 pv=1000000 graze=0 ' +
    'replay_slot=-1 pos=0,51200 focus=0 new_game=1 seed=30966 frames=6777');
});

test('Spell Practice recording shows the selected number and returns to title like the original',
  { skip: !existsSync(dat) && 'th16.dat not present' }, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-spell-replay-'));
  try {
    const run = (args, env) => {
      const result = spawnSync(harness, args, {encoding: 'utf8', env: {...process.env, ...env}});
      assert.equal(result.status, 0, result.stderr || result.stdout);
      return result.stdout;
    };
    // Stage 2, Easy, Reimu/Spring, spell index 12 in the selection screen.
    run(['game-digest', dat, '2', '0', '0', '1600', '0:0x60,85:0', '', '', '0', '12', '', '0', '2'],
      {TH16_RECORD_DIR: dir});
    const recording = join(dir, 'replay', 'th16_01.rpy');
    assert.ok(existsSync(recording));
    const title = run(['scene-boot', dat, '410', '314:0x20,316:0x20,318:0x20,322:1', 'ascii'],
      {TH16_REPLAY_DIR: dir});
    const row = title.split('\n').find(line => line.startsWith('ascii ') && line.includes('[No.01 ')) ?? '';
    assert.match(row, / SpellPr\s+13 0\.0%\]$/);

    // Original --replay reads the terminator at frame 1601, then Z confirms the return.
    const playback = run(['game-digest', dat, '2', '0', '0', '1670', '1648:1,1649:0', '', '', '0', '-1', '', '0', '0'],
      {TH16_REPLAY: recording});
    assert.match(playback, /\bscene_exit=4 exit_frame=1662 digest=68efa624c151cb03\b/);
    assert.match(playback, /\bmenu=87781d6c875ffabb sound=13164b16e24cc3eb\b/);
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('title Replay: a recording saved from Pause returns to the selected slot after playback',
  { skip: !existsSync(dat) && 'th16.dat not present' }, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-replay-menu-'));
  try {
    const replays = join(dir, 'replay');
    mkdirSync(replays);
    const boot = (frames, inputs, ...options) => {
      const result = spawnSync(harness, ['scene-boot', dat, String(frames), inputs, 'scenes', ...options],
        {encoding: 'utf8', env: {...process.env, TH16_REPLAY_DIR: dir}});
      assert.equal(result.status, 0, result.stderr || result.stdout);
      return result.stdout;
    };
    const saveInput = [
      '313:1,360:1,400:1,450:1,550:1,552:1,554:1',
      '700:0x100,731:0x20,736:0x20,745:1,788:0x10,795:1,829:1',
      '852:1,854:1,856:1,858:1,860:1,862:1,864:1,866:1,868:1',
    ].join(',');
    const slot = boot(900, saveInput).split('\n').find(line => line.startsWith('spell ')) ?? '';
    assert.match(slot, /\breplay_slot=-1$/, 'saving leaves the live spell selection intact');
    const saved = boot(1040, saveInput + ',907:2');
    assert.match(saved, /scene-change frame=920 7->4\b/);
    const recording = join(replays, 'th16_01.rpy');
    assert.ok(existsSync(recording), 'Pause saved the replay');
    copyFileSync(recording, join(replays, 'th16_05.rpy'));

    const draw = join(dir, 'returned-title.jsonl');
    const replayed = boot(865, [
      '314:0x20,316:0x20,318:0x20,322:1',
      '356:0x20,358:0x20,360:0x20,362:0x20,373:1,401:1,705:1',
    ].join(','), `draw=${draw}`, 'draw-from=865');
    assert.match(replayed, /event replay-play 4\b/);
    assert.match(replayed, /scene-change frame=434 4->7\b/);
    assert.match(replayed, /scene-change frame=718 7->4\b/);
    assert.match(replayed, /^scene=4 .*\bscreen=11 sub=2 cursor=4\b/m);
    const {ops} = JSON.parse(readFileSync(draw, 'utf8'));
    const gameSprites = ops.filter(op => op.op === 'draw' &&
      (op.texture[0] === 7 || op.texture[0] === 8 || (op.texture[0] >= 10 && op.texture[0] < 16)));
    assert.deepEqual(gameSprites, [], 'title draws no enemies, bullets or effects from the replay');
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});
