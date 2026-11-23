// ANM v8 VM 行为还原：harness `anm-run` 的逐帧 VM 快照（0x568 字节纯数据区 + layer 绘制链）
// 与 Unicorn 直接执行原版 th16.exe（tools/oracle/anm-oracle.py）得到的快照按脚本逐一对照。
// golden/anm-vm.json 只存每脚本的 sha256 摘要（tools/oracle/anm-golden.mjs 生成）；
// 若 artifacts/th16/oracle/<name>.json 完整快照在本机存在，失败时附带首个差异字段，便于定位。
// 原版资源不入库：缺少 games/th16/original/th16.dat 时整体跳过。
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {existsSync, readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {digestRun, SNAPSHOT_BYTES} from '../tools/oracle/anm-golden.mjs';

const here = resolve(import.meta.dirname);
const monorepo = resolve(here, '../../..');
const harness = process.env.TH16_HARNESS ?? resolve(here, 'build/harness');
const dat = process.env.TH16_DAT ?? resolve(monorepo, 'games/th16/original/th16.dat');
const oracleDir = resolve(monorepo, 'artifacts/th16/oracle');
const golden = JSON.parse(readFileSync(resolve(here, 'golden/anm-vm.json'), 'utf8'));
const available = existsSync(dat);

// AnmVm.hpp 的字段偏移表（0x568 之前）；只用于把字节差异翻译成字段名
const fields = [[0, 'interrupt_return_time', 0x14], [0x14, 'interrupt_return_offset', 4], [0x18, 'layer', 4], [0x1c, 'file_slot', 4], [0x20, 'sprite_index', 4],
  [0x24, 'script_index', 4], [0x28, 'ins_offset', 4], [0x2c, 'pos', 12], [0x38, 'rotation', 12], [0x44, 'angular_velocity', 12], [0x50, 'scale', 8], [0x58, 'scale2', 8],
  [0x60, 'scale_growth', 8], [0x68, 'uv_scale', 8], [0x70, 'sprite_size', 8], [0x78, 'uv_scroll', 8], [0x80, 'anchor_offset', 8], [0x88, 'unknown_088', 4],
  [0x8c, 'pos_interp', 0x58], [0xe4, 'color_interp', 0x58], [0x13c, 'alpha_interp', 0x30], [0x16c, 'rotation_interp', 0x58], [0x1c4, 'angle_interp', 0x30],
  [0x1f4, 'scale_interp', 0x44], [0x238, 'scale2_interp', 0x44], [0x27c, 'uv_scale_interp', 0x44], [0x2c0, 'color2_interp', 0x58], [0x318, 'alpha2_interp', 0x30],
  [0x348, 'u_scroll_interp', 0x30], [0x378, 'v_scroll_interp', 0x30], [0x3a8, 'uv', 0x20], [0x3c8, 'uv_scroll_velocity', 8], [0x3d0, 'texture_matrix', 0x40],
  [0x410, 'texture_matrix_copy', 0x40], [0x450, 'sprite_matrix', 0x40], [0x490, 'pending_interrupt', 4], [0x494, 'sprite_set_time', 4], [0x498, 'unknown_498', 4],
  [0x49c, 'template_script', 2], [0x49e, 'unknown_49e', 2], [0x4a0, 'int_vars', 16], [0x4b0, 'float_vars', 16], [0x4c0, 'float_vars2', 12], [0x4cc, 'int_vars2', 8],
  [0x4d4, 'rand_scale', 4], [0x4d8, 'rand_angle_scale', 4], [0x4dc, 'rand_int_max', 4], [0x4e0, 'pos2', 12], [0x4ec, 'unknown_4ec', 0x30], [0x51c, 'creation_flags', 4],
  [0x520, 'color', 4], [0x524, 'color2', 4], [0x528, 'unknown_528', 8], [0x530, 'flags_lo', 4], [0x534, 'flags_hi', 4], [0x538, 'id', 4], [0x53c, 'pool_index', 4],
  [0x540, 'time', 0x14], [0x554, 'age', 0x14]];

function firstDifference(ours, gold) {
  for (let i = 0; i < Math.max(ours.frames.length, gold.frames.length); ++i) {
    const fo = ours.frames[i], fg = gold.frames[i];
    if (!fo || !fg) return `frame ${i}: frame count differs`;
    const idsO = fo.vms.map(v => [v.id, v.script]), idsG = fg.vms.map(v => [v.id, v.script]);
    if (JSON.stringify(idsO) !== JSON.stringify(idsG)) return `frame ${i}: world list ours=${JSON.stringify(idsO)} gold=${JSON.stringify(idsG)}`;
    if (JSON.stringify(fo.layers) !== JSON.stringify(fg.layers)) return `frame ${i}: layer chains differ`;
    for (let k = 0; k < fo.vms.length; ++k) {
      const bo = fo.vms[k].bytes.slice(0, SNAPSHOT_BYTES * 2), bg = fg.vms[k].bytes.slice(0, SNAPSHOT_BYTES * 2);
      if (bo === bg) continue;
      for (const [off, name, size] of fields) {
        const a = bo.slice(off * 2, (off + size) * 2), b = bg.slice(off * 2, (off + size) * 2);
        if (a !== b) return `frame ${i} vm ${fo.vms[k].id} (script ${fo.vms[k].script}) ${name}@0x${off.toString(16)}: ours=${a} gold=${b}`;
      }
    }
  }
  return 'no byte difference found (digest mismatch in canonical form)';
}

for (const c of golden.cases) {
  test(`${c.anm}: ${c.scripts.length} scripts × ${golden.frames} frames match th16.exe ANM VM byte-for-byte`, {skip: !available && 'original th16.dat not present'}, async () => {
    const spec = c.scripts.map(s => s.script).join(',');
    const ours = JSON.parse(execFileSync(harness, ['anm-run', dat, c.anm, spec, String(golden.frames), String(golden.layer)], {encoding: 'utf8', maxBuffer: 1 << 30}));
    assert.equal(ours.runs.length, c.scripts.length);
    const oraclePath = resolve(oracleDir, c.anm.replace('.anm', '.json'));
    const oracle = existsSync(oraclePath) ? JSON.parse(readFileSync(oraclePath, 'utf8')) : null;
    const failures = [];
    for (let i = 0; i < c.scripts.length; ++i) {
      const run = ours.runs[i], expected = c.scripts[i];
      assert.equal(run.script, expected.script);
      if (run.spawnedId !== expected.spawnedId) failures.push(`script ${run.script}: spawned id ${run.spawnedId} ≠ ${expected.spawnedId}`);
      else if (run.frames.at(-1).vms.length !== expected.vmsAtEnd) failures.push(`script ${run.script}: ${run.frames.at(-1).vms.length} VMs at end ≠ ${expected.vmsAtEnd}`);
      else if (await digestRun(run) !== expected.digest) {
        const gold = oracle?.runs.find(r => r.script === run.script);
        failures.push(`script ${run.script}: ${gold ? firstDifference(run, gold) : 'digest differs (full oracle snapshot not present for field diff)'}`);
      }
    }
    assert.deepEqual(failures, [], failures.slice(0, 10).join('\n'));
  });
}
