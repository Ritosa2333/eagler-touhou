// ANM 绘制几何：harness `anm-draw` 把 layer 链上的屏幕四边形（mode 0..3）导出为数值行，
// 与启动幕 sig.anm script0 的稳态期望对照（1280×960 坐标系 ×0.5 → 640×480，pixel snap -0.5）。
// 原版资源不入库：缺少 games/th16/original/th16.dat 时跳过。
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {existsSync} from 'node:fs';
import {resolve} from 'node:path';

const here = resolve(import.meta.dirname);
const monorepo = resolve(here, '../../..');
const harness = process.env.TH16_HARNESS ?? resolve(here, 'build/harness');
const dat = process.env.TH16_DAT ?? resolve(monorepo, 'games/th16/original/th16.dat');
const available = existsSync(dat);

// 行格式：frame quad <s|w> slot=N entry=N x0 y0 x1 y1 x2 y2 x3 y3 color z=… rhw=… fog=… zt= zw= uv=u0,v0,u3,v3
function rows(script, frames, layer = -1) {
  return execFileSync(harness, ['anm-draw', dat, 'sig.anm', String(script), String(frames), String(layer)], {encoding: 'utf8'})
    .trim().split('\n').filter(Boolean).map(line => {
      const p = line.split(' ');
      const field = name => p.find(x => x.startsWith(name + '=')).slice(name.length + 1);
      const uv = field('uv').split(',').map(Number);
      return {
        frame: Number(p[0]), kind: p[2], slot: Number(field('slot')), entry: Number(field('entry')),
        x0: Number(p[5]), y0: Number(p[6]), x1: Number(p[7]), y1: Number(p[8]),
        x2: Number(p[9]), y2: Number(p[10]), x3: Number(p[11]), y3: Number(p[12]),
        u0: uv[0], v0: uv[1], u3: uv[2], v3: uv[3],
        color: p[13],
      };
    });
}

test('sig.anm script0: two layer-26 quads cover 640×480 with THTX UVs and fade to white', {skip: !available && 'original th16.dat not present'}, () => {
  const all = rows(0, 15, -1);
  const steady = all.filter(r => r.frame === 15);
  assert.equal(steady.length, 2);
  // 两块都是屏幕四边形，贴图取 sig.anm（槽 0）的 entry 0 / entry 1
  const byEntry = Object.fromEntries(steady.map(r => [r.entry, r]));
  assert.deepEqual(steady.map(r => r.kind), ['s', 's']);
  assert.deepEqual(steady.map(r => r.slot), [0, 0]);
  // script0（entry 0）：1024×960 @ (0,0) ×0.5 → 512×480，pixel snap -0.5
  assert.deepEqual([byEntry[0].x0, byEntry[0].y0, byEntry[0].x1, byEntry[0].y3], [-0.5, -0.5, 511.5, 479.5]);
  assert.deepEqual([byEntry[0].u0, byEntry[0].v0, byEntry[0].u3, byEntry[0].v3], [0, 0, 1, 0.9375]);
  assert.equal(byEntry[0].color, 'ffffffff');
  // script1（entry 1，script0 的子 VM）：256×960 @ (1024,0) ×0.5 → 128×480 @ x=512
  assert.deepEqual([byEntry[1].x0, byEntry[1].y0, byEntry[1].x1, byEntry[1].y3], [511.5, -0.5, 639.5, 479.5]);
  assert.deepEqual([byEntry[1].u0, byEntry[1].v0, byEntry[1].u3, byEntry[1].v3], [0, 0, 1, 0.9375]);
  assert.equal(byEntry[1].color, 'ffffffff');
  // 淡入：frame 0 非白，frame 15 纯白（ins_408 10 帧）
  const first = all.filter(r => r.frame === 0);
  assert.equal(first.length, 2);
  assert.notEqual(first[0].color, 'ffffffff');
  assert.match(first[0].color, /^ff[0-9a-f]{6}$/);
});
