// 标题演示回放整段对拍：0x43ce10 case 0xd → 0x42d700(1) → 0x447760(1) 段头全局块读回 / 0x448eb0 自机坐标 →
// 0x448130 逐帧取输入，直到 0x42d7b0 演示计时 0xf3c 请求标题；全部管理器摘要与 unicorn 原版一致
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { existsSync, mkdtempSync, readFileSync, rmSync } from 'node:fs';
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
    const replayed = run({ TH16_REPLAY: rpy }, '0:0');
    for (const key of ['digest', 'enemies', 'enemy_vms', 'bullets', 'items', 'spell', 'lasers', 'bombs', 'menu', 'sound']) {
      assert.equal(recorded[key], g[key], `recording run ${key}`);
      assert.equal(replayed[key], g[key], `port playback ${key}`);
    }
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
