// 场景 7：GameManager(0x42d700) / StageManager(0x409db0) / STD(0x40ac30) / 关卡脚本 0x40b3b0 / 背景绘制 0x409f90。
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

function run(args) {
  return execFileSync(harness, args, {encoding: 'utf8'});
}

function kv(line) {
  return Object.fromEntries(line.split(' ').slice(1).map(p => p.split('=')).map(([k, v]) => [k, v]));
}

function parse(out) {
  const lines = out.trim().split('\n');
  const find = prefix => lines.find(l => l.startsWith(prefix + ' '));
  return {
    header: kv('x ' + lines[0]),
    game: find('game') && kv(find('game')),
    stage: find('stage') && kv(find('stage')),
    pass: find('stagepass') && kv(find('stagepass')),
    // 关卡背景的世界四边形：quad w slot=<3+(stage&1)> entry=N x0 y0 … color z=… rhw=… fog=on,f0..f3 zt= zw= uv=…
    sq: lines.filter(l => l.startsWith('quad w slot=4 ')).map(l => l.split(' ')),
  };
}

const nums = s => s.split(',').map(Number);
const close = (a, b, eps, msg) => assert.ok(Math.abs(a - b) <= eps, `${msg}: ${a} vs ${b}`);

// 标题流程：主菜单 Z → 难度 Z → 机体 →/Z → 季节 →/Z（与 supervisor.test.mjs 的 game start flow 相同）
const MAIN = 312, AFTER_MAIN = 29, AFTER_DIFF = 23, AFTER_SHOT = 23;
const t = MAIN + 1, diff = t + AFTER_MAIN, shot = diff + AFTER_DIFF, season = shot + 2 + AFTER_SHOT;
const script = [`${t}:0x1`, `${diff}:0x1`, `${shot}:0x80`, `${shot + 2}:0x1`, `${season}:0x80`, `${season + 2}:0x1`].join(',');
const GAME_FRAME_1 = season + 2 + 0x28 + 2;   // 切到场景 7 后 GameManager 的第一次 update

test('std: every stage STD parses; all quads are type 0; st01 = 1 object × 8 ground tiles', needDat, () => {
  const expected = {1: [1, 1, 8], 2: [2, 2, 24], 3: [8, 4, 28], 4: [3, 3, 24], 5: [13, 2, 16], 6: [38, 14, 59], 7: [14, 2, 16]};
  for (const [n, [faces, objects, instances]] of Object.entries(expected)) {
    const out = run(['std', dat, `st0${n}.std`]);
    const head = kv('x ' + out.split('\n')[0]);
    assert.equal(head.anm, `st0${n}wl.anm`);
    assert.equal(Number(head.faces), faces, `st0${n} faces`);
    assert.equal(Number(head.objects), objects, `st0${n} objects`);
    assert.equal(Number(head.instances), instances, `st0${n} instances`);
    assert.ok(out.split('\n').filter(l => /^\s+quad /.test(l)).every(l => / type=0 /.test(l)), 'only quad type 0 exists in th16');
    // 所有 opcode 都在 0x40b3b0 跳转表内（0..0x14；0xf/0x10 走 default）
    for (const m of out.matchAll(/^ins @\d+ t=-?\d+ op=(\d+)/gm)) assert.ok(Number(m[1]) <= 0x14, 'opcode ' + m[1]);
  }
  const st01 = run(['std', dat, 'st01.std']);
  assert.match(st01, /^object 0 layer=1 pos=0,0,300 size=1024,512,0 quads=1$/m);
  assert.match(st01, /^  quad type=0 script=0 pos=0,0,300 wh=1024,512$/m);
  assert.match(st01, /^instance 0 flags=256 pos=1024,1816,0$/m);
  assert.match(st01, /^ins @0 t=0 op=6 \d+\/0\.06 \d+\/1 0\/0$/m, 'up = (0.06, 1, 0)');
  assert.match(st01, /^ins @1052 t=5620 op=1 1004\/\S+ 4920\/\S+$/m, 'tail jmp back to @1004 / t=4920');
});

// Unicorn oracle（tools/oracle/stage-oracle.py）：7 关 × 6000 帧 StageManager 状态（相机 / 脚本 / 雾 / 面 VM / 层精灵 VM 原始字节）摘要
const stageGolden = JSON.parse(readFileSync(resolve(here, 'golden/stage.json'), 'utf8'));
for (const g of stageGolden.stages) {
  test(`stage ${g.stage}: ${g.faces} face VMs × ${stageGolden.frames} frames match th16.exe StageManager byte-for-byte (0x409e50/0x40b3b0/0x40aed0)`, needDat, () => {
    const out = run(['stage-digest', dat, String(g.stage), String(stageGolden.frames)]).trim();
    const got = kv('x ' + out);
    assert.equal(Number(got.faces), g.faces);
    if (got.digest !== g.digest) {
      // 本机若有 artifacts/th16/oracle/stage/stNN-600.json 完整快照，给出首个差异
      const oraclePath = resolve(monorepo, 'artifacts/th16/oracle/stage', `st0${g.stage}-600.json`);
      let hint = '';
      if (existsSync(oraclePath)) {
        const gold = JSON.parse(readFileSync(oraclePath, 'utf8'));
        const ours = JSON.parse(run(['stage-run', dat, String(g.stage), String(gold.frames.length)]));
        hint = firstStageDifference(ours.frames, gold.frames);
      }
      assert.fail(`digest ${got.digest} !== ${g.digest}${hint ? ': ' + hint : ''}`);
    }
  });
}

function firstStageDifference(ours, gold) {
  for (let i = 0; i < Math.min(ours.length, gold.length); ++i) {
    const a = ours[i], b = gold[i];
    for (const k of ['timer', 'ins', 'rocking', 'stage_frame', 'flags', 'bg', 'fog', 'layer_ids', 'radius_sq']) {
      if (JSON.stringify(a[k]) !== JSON.stringify(b[k])) return `frame ${a.frame} ${k}: ours=${JSON.stringify(a[k])} gold=${JSON.stringify(b[k])}`;
    }
    for (const k of Object.keys(b.cam)) {
      if (JSON.stringify(a.cam[k]) !== JSON.stringify(b.cam[k])) return `frame ${a.frame} cam.${k}: ours=${JSON.stringify(a.cam[k])} gold=${JSON.stringify(b.cam[k])}`;
    }
    for (const [name, list] of [['face', 'faces'], ['layer', 'layers']]) {
      for (let v = 0; v < b[list].length; ++v) {
        if (a[list][v] === b[list][v]) continue;
        for (let off = 0; off < b[list][v].length; off += 8) {
          if (a[list][v].slice(off, off + 8) !== b[list][v].slice(off, off + 8)) return `frame ${a.frame} ${name} ${v} @0x${(off / 2).toString(16)}: ours=${a[list][v].slice(off, off + 8)} gold=${b[list][v].slice(off, off + 8)}`;
        }
      }
    }
  }
  return 'no difference in the first 600 frames (diverges later)';
}

test('game entry: GameManager first frame loads st01.std + st01wl.anm into slot 4 and starts the stage script', needDat, () => {
  const r = parse(run(['scene-boot', dat, String(GAME_FRAME_1), script]));
  assert.equal(r.header.scene, '7');
  assert.equal(r.game.std, 'st01.std');
  assert.equal(r.game.anm, 'st01wl.anm');
  assert.equal(r.game.flags, '0', 'kLoading cleared, no end flags');
  assert.equal(r.game.frame, '1');
  assert.equal(r.game.game_frame, '1', 'DAT_004a579c');
  assert.equal(r.game.stage_frame, '1', 'DAT_004a57a0');
  assert.equal(r.game.objects, '1');
  assert.equal(r.game.instances, '8');
  assert.equal(r.game.faces, '1');
  // 脚本 t=0 段：up/fov/bgColor/fog/fogTime/pos/posTime/facing/facingTime 全部执行，停在 ins 9（t=720）
  assert.equal(r.stage.ins, '9');
  assert.equal(r.stage.time, '1');
  assert.deepEqual(nums(r.stage.up), [0.06, 1, 0]);
  close(Number(r.stage.fov), 0.62831855, 1e-7, 'fov = π/5');
  assert.equal(r.stage.bg, '000000b0', 'ins 13 → DAT_004c1b0c');
  // ins 3 posTime(700, mode 4, (0,312,-250)) 从 (0,-200,-400) 出发：mode 4 = 1-(1-t)²
  const pos = nums(r.stage.pos);
  const k = 1 - (1 - 1 / 700) ** 2;
  close(pos[1], -200 + 512 * k, 1e-3, 'pos.y after 1 frame');
  close(pos[2], -400 + 150 * k, 1e-3, 'pos.z after 1 frame');
  // ins 9 fogTime(720, mode 0, 0x40d0b0, 400, 1400) 从 (0x80d0b0, 300, 600) 出发
  const [fogColor, near, far] = r.stage.fog.split(',');
  close(Number(near), 300 + 100 / 720, 1e-3, 'fog near');
  close(Number(far), 600 + 800 / 720, 1e-3, 'fog far');
  assert.equal(fogColor, '007fd0b0', 'red 0x80→0x40 after one of 720 frames');
});

test('stage script timeline: frame 61 eased camera, frame 801 past ins 9/10 (t=720) with fog settled', needDat, () => {
  const f61 = parse(run(['scene-boot', dat, String(GAME_FRAME_1 + 60), script]));
  assert.equal(f61.stage.frame, '61');
  const k = 1 - (1 - 61 / 700) ** 2;
  close(nums(f61.stage.pos)[1], -200 + 512 * k, 1e-3, 'pos.y mode 4');
  const facing = nums(f61.stage.facing);
  close(facing[0], 30 + 30 * k, 1e-3, 'facing.x mode 4');
  close(facing[1], 180 - 40 * k, 1e-3, 'facing.y mode 4');
  assert.equal(facing[2], 150);

  const f801 = parse(run(['scene-boot', dat, String(GAME_FRAME_1 + 800), script]));
  assert.equal(f801.stage.frame, '801');
  assert.equal(f801.stage.ins, '11', 'waiting on ins @236 (t=1070)');
  // t=720: pos(0,-200,-250) 然后 posTime(350, mode 0, (0,312,-250))，第 81 帧线性
  const pos = nums(f801.stage.pos);
  assert.equal(pos[0], 0);
  close(pos[1], -200 + 512 * 81 / 350, 1e-3, 'pos.y linear');
  assert.equal(pos[2], -250);
  assert.deepEqual(nums(f801.stage.facing), [60, 140, 150], 'facingTime finished at t=700');
  assert.equal(f801.stage.fog, '0040d0b0,400,1400', 'fogTime finished at t=720');
  assert.equal(f801.pass.clear, '000000b0', 'compositor clears the field with DAT_004c1b0c (STD ins 13)');
  assert.equal(f801.pass.fog_clear, '0040d0b0', '0x409f90 clears the field again with the fog colour');
});

test('stage draw (0x409f90/0x40af70): 4 of 8 tiles pass the actual-field 0x40a7d0 frustum test, fogged mode-8 faces with Z test/write, 2×4 tiling', needDat, () => {
  const r = parse(run(['scene-boot', dat, String(GAME_FRAME_1 + 800), script]));
  assert.equal(r.pass.active, '1');
  // 原版第 799–802 帧均画 4 个实例；0x40a7d0 的投影包围盒对实际场地 (128,16) 判定。
  assert.equal(r.pass.quads, '4');
  assert.equal(r.pass.drawn_instances, '4');
  assert.equal(r.pass.culled, '4');
  assert.equal(r.pass.drawn_quads, '4');
  assert.equal(r.sq.length, 4);
  const distinct = new Set(r.sq.map(q => q.slice(4, 12).join(' ')));
  assert.equal(distinct.size, 4, 'each instance gets its own entity_offset (face VM shared)');
  for (const q of r.sq) {
    assert.equal(q[3], 'entry=0', 'st01wl.anm entry 0');
    assert.ok(q.includes('zt=1') && q.includes('zw=1'), 'mode 8: ZENABLE on, Z write on');
    const fog = q.find(p => p.startsWith('fog=')).slice(4).split(',').map(Number);
    assert.equal(fog[0], 1, 'FOGENABLE for mode 8');
    const rhw = q.find(p => p.startsWith('rhw=')).slice(4).split(',').map(Number);
    for (let v = 0; v < 4; ++v) {
      const z = 1 / rhw[v];
      const f = Math.min(1, Math.max(0, (1400 - z) / (1400 - 400)));
      close(fog[1 + v], Math.round(f * 255), 1, 'linear vertex fog from z_eye = 1/rhw');
    }
    assert.ok(q.includes('uv=0,0,2,1'), 'quad 1024 wide over a 512 px sprite → u repeats twice');
  }
  // 近处（大 rhw）瓦片的雾因子小，远处的接近 255
  const fogs = r.sq.map(q => q.find(p => p.startsWith('fog=')).slice(4).split(',').map(Number).slice(1));
  assert.ok(fogs.some(f => f.some(v => v < 40)), 'nearest tiles are barely fogged');
  assert.ok(fogs.some(f => f.some(v => v > 200)), 'far tiles are fogged');
});
