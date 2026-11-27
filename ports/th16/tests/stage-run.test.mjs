// 整关长程对照：harness game-run / game-digest 与 tools/oracle/game-oracle.py（Unicorn 直跑原版）逐帧比对。
// 覆盖本轮修复的几处原版行为：0x41aa70 默认击破特效（effect.anm 40 族）与 score_bonus_spell=DAT_004a5798、
// 0x41a6d0 damage_accum 在无敌分支也累加、0x423050 子敌机继承 I0..I3/F0..F3/F4..F7、
// 0x409e50 相机副本 DAT_004c168c（ANM 变量 10016..10021）与 DAT_004c1790 entity_offset_delta、
// 0x407a50 AnmVm 尾段重置、0x4178a0 Spellcard 初始标志；以及 Gui 对话（0x429ff0 / 0x429b20 / 0x42a1d0）、
// ECL 518 / 519 / 554 与长按快进（DAT_004a5244 累计按住帧数）；以及过关换关全链路
// （0x42e150 → 0x43ce10 case 0xc → 0x42d200 / 0x42cb60 → 0x42dc50 / 0x40c0d0 / 0x40c210 → 0x42dee0 → 二面）。
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
const golden = JSON.parse(readFileSync(resolve(here, 'golden/stage-run.json'), 'utf8'));

function run(args, env = {}) {
  return execFileSync(harness, args, {encoding: 'utf8', maxBuffer: 1 << 30, env: {...process.env, ...env}});
}

function argsFor(command, c) {
  return [
    command, dat, String(c.stage), String(c.shot), String(c.season), String(c.frames), c.input, '', '',
    String(c.difficulty), String(c.replay_slot), String(c.lives), String(c.refill ?? 0),
  ];
}

for (const c of golden.cases) {
  test(`stage run digests match oracle: ${c.name}`, needDat, () => {
    const out = run(argsFor('game-digest', c));
    for (const key of ['digest', 'enemies', 'enemy_vms', 'bullets', 'items', 'spell']) {
      const m = out.match(new RegExp(`\\b${key}=([0-9a-f]{16})`));
      assert.ok(m, out);
      assert.equal(m[1], c[key], `${key} 摘要与原版不符`);
    }
  });

  test(`stage run checkpoints match oracle: ${c.name}`, needDat, () => {
    // frames_from：长程用例只取尾段快照（TH16_FRAMES_FROM），避免 GB 级 JSON
    const from = c.frames_from ?? 1;
    const frames = JSON.parse(run(argsFor('game-run', c), from > 1 ? {TH16_FRAMES_FROM: String(from)} : {})).frames;
    assert.equal(frames.length, c.frames - from + 1);
    for (const [frame, want] of Object.entries(c.checkpoints)) {
      const f = frames[Number(frame) - from];
      assert.equal(f.frame, Number(frame));
      if (want.stage !== undefined) assert.equal(f.stage, want.stage, `frame ${frame} 关卡号 DAT_004a5790`);
      if (want.game_frame !== undefined) assert.equal(f.game_frame, want.game_frame, `frame ${frame} DAT_004a579c`);
      if (want.stage_frame !== undefined) assert.equal(f.stage_frame, want.stage_frame, `frame ${frame} DAT_004a57a0`);
      if (want.input) assert.deepEqual(f.input, want.input, `frame ${frame} 游戏输入 held / pressed`);
      if (want.rng) assert.deepEqual(f.rng, want.rng, `frame ${frame} 视觉 RNG`);
      if (want.game_rng) assert.deepEqual(f.game_rng, want.game_rng, `frame ${frame} 游戏 RNG`);
      if (want.enemy_count !== undefined) assert.equal(f.enemies.count, want.enemy_count, `frame ${frame} 敌机数`);
      if (want.item_count !== undefined) assert.equal(f.items.items.length, want.item_count, `frame ${frame} 道具数`);
      if (want.bullet_count !== undefined) assert.equal(f.bullets.count, want.bullet_count, `frame ${frame} 子弹数`);
      if (want.dialogue !== undefined) assert.equal(f.dialogue, want.dialogue, `frame ${frame} 对话中`);
      if (want.msg_pc !== undefined) assert.equal(f.msg?.pc, want.msg_pc, `frame ${frame} 对话脚本偏移`);
      if (want.spell_flags !== undefined) assert.equal(f.spell.flags, want.spell_flags, `frame ${frame} 符卡标志`);
      if (want.season_gauge !== undefined) assert.equal(f.season_gauge, want.season_gauge, `frame ${frame} 季节量表`);
      if (want.power !== undefined) assert.equal(f.power, want.power, `frame ${frame} 火力`);
      if (want.lives !== undefined) assert.equal(f.resources.lives, want.lives, `frame ${frame} 残机`);
      if (want.score !== undefined) assert.equal(f.resources.score, want.score, `frame ${frame} 得分`);
      if (want.graze !== undefined) assert.equal(f.resources.graze, want.graze, `frame ${frame} 擦弹`);
      if (want.spell_id !== undefined) assert.equal(f.spell.spell_id, want.spell_id, `frame ${frame} spell_id`);
      if (want.enemy_bonus) assert.deepEqual(f.spell.enemy_bonus, want.enemy_bonus, `frame ${frame} 击破奖励`);
    }
    assert.deepEqual(frames.at(-1).enemies.enemies.map(e => e.id), c.last_ids);
    if (!c.phantom01) return;
    // 子敌机继承父机寄存器（0x423050 → 0x41aa70 +0x20..+0x4c）：Phantom01 的朝向来自父机 F 寄存器而非 0
    const ph = frames[c.phantom01.frame - 1].enemies.enemies.find(e => e.id === c.phantom01.id);
    assert.ok(ph, 'Phantom01 在场');
    assert.equal(ph.ctx[3], 'Phantom01');
    assert.deepEqual(ph.pos, c.phantom01.pos);
    assert.equal(ph.motion1[7], c.phantom01.angle_bits);
  });
}
