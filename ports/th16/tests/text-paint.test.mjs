// 0x46d990 / 0x46dab0 逻辑侧：flags_lo bit0 + 字号表 DAT_00491b68
import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { existsSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const root = join(dirname(fileURLToPath(import.meta.url)), '..');
const harness = process.env.TH16_HARNESS || join(root, 'tests/build/harness');
const dat = process.env.TH16_DAT ?? resolve(root, '../../games/th16/original/th16.dat');
const needDat = {skip: !existsSync(dat) && 'original th16.dat not present'};

test('text-paint: 0x46d990 sets flags_lo bit0; font heights match DAT_00491b68', () => {
  const r = spawnSync(harness, ['text-paint-flag'], { encoding: 'utf8' });
  assert.equal(r.status, 0, r.stderr || r.stdout);
  assert.match(r.stdout, /flag=1 queued=0 height0=17 height8=14/);
});

test('text-raster: 0x459240 TextOut order, DIB alpha flip / soften / bleed, Meiryo row offset', () => {
  const r = spawnSync(harness, ['text-raster'], { encoding: 'utf8' });
  assert.equal(r.status, 0, r.stderr || r.stdout);
  const out = r.stdout;
  // param_9 == 0：阴影 8 向（相对 x+2 / y=2），主色最后
  const whole = out.split('case spaced')[0].match(/out .*/g);
  assert.deepEqual(whole.map(l => l.match(/x=(-?\d+) y=(\d+) color=(\w+)/).slice(1).join(',')), [
    '13,1,102030', '11,1,102030', '13,3,102030', '11,3,102030',
    '14,2,102030', '10,2,102030', '12,0,102030', '12,4,102030', '12,2,c08040']);
  // 0x458c80 两次翻 α：画到的像素 α=f；0x458af0：(4 邻×2 + 对角)/14；COLORREF 按 4 位截取
  assert.match(out, /px12_2=f48c px13_2=8000 px13_1=f321 px15_2=2000 rect=16x8 r12_2=f48c/);
  // param_9 != 0：起点 x-2，四角阴影 + (+2,2) 主色，步长 param_9，奇数长度末对带 NUL
  const spaced = out.split('case spaced')[1].split('case edge')[0].match(/out .*/g);
  assert.equal(spaced.length, 15);
  assert.match(spaced[0], /x=12 y=4 .*\[82a0\]/);
  assert.match(spaced[4], /x=10 y=2 color=c08040 .*\[82a0\]/);
  assert.match(spaced[9], /x=50 y=2 .*\[82a2\]/);
  assert.match(spaced[14], /x=90 y=2 .*\[7800\]/);
  // 第 0 列不柔化 → 0x458730 取邻色 (15>>1)=7
  assert.match(out, /px0_2=0777 px1_2=ffff px2_2=2000 px1_1=2000 px0_1=0000/);
  // DAT_004a6f2c：字体非 2/6 时源行偏 6
  assert.match(out, /font0 r0_6=0000/);
  assert.match(out, /font2 r2_6=ffff/);
});

test('text-paint: Spell Practice queues 0x4560b0 rows and 0x417f00 spell name with 0x459240 arguments', needDat, () => {
  const r = spawnSync(harness, ['scene-boot', dat, '1020', 'unlock-spells', 'text',
    '380:0x20,393:0x20,406:0x1,467:0x1,540:0x1,600:0x1,660:0x1,720:0x1'], { encoding: 'latin1' });
  assert.equal(r.status, 0, r.stderr || r.stdout);
  const rows = r.stdout.split('\n').filter(l => l.startsWith('text '));
  // 0x46d990(…, 0, 0)：x = param_6×2 = 0、整串 TextOut、flags_hi bit12 清 → 带阴影
  assert.ok(rows.some(l => l.startsWith('text font=0 height=17 x=0 spacing=0 shadow=1 color=808080 outline=000000 rect=0,0,1024,38 [ Nothing ... ]')));
  assert.ok(rows.some(l => l.startsWith('text font=0 height=17 x=0 spacing=0 shadow=1 color=efefef outline=000000 rect=0,38,1024,76 [ No.  2  ')));
  // 0x417f00 → 0x46dab0(…, font 0, spacing 0)：x = (int)(sprite.width − ((17×2−1)×strlen >> 1))
  const name = rows.find(l => l.includes('rect=0,184,640,224'));
  assert.ok(name, rows.join('\n'));
  const m = name.match(/x=(\d+) spacing=0 shadow=1 color=ffffff outline=000000 rect=\S+ \[(.*)\]$/);
  assert.ok(m, name);
  assert.equal(Number(m[1]), 640 - ((33 * m[2].length) >> 1));
});

test('text-paint: Music Room 0x4546f0 rows, comments and the unheard-track notice (PTR_DAT_004936b8, 0x8080ff)', needDat, () => {
  // 标题 181 起、第 191 帧 0x44af80 重播 th16_01 置 0 号已听过；主菜单（312 起）↓×5 → Music Room
  const t = 313, ready = t + 12 + 0x14 + 17;
  const script = [0, 2, 4, 6, 8].map(i => `${t + i}:0x20`)
    .concat([`${t + 12}:0x1`, `${ready}:0x20`, `${ready + 2}:0x1`, `${ready + 30}:0x1`]).join(',');
  const r = spawnSync(harness, ['scene-boot', dat, String(ready + 60), script, 'text']);
  assert.equal(r.status, 0, String(r.stderr));
  const dec = new TextDecoder('shift_jis');
  const lines = dec.decode(r.stdout).split('\n');
  const rows = lines.filter(l => l.startsWith('text ')).map(l => l.match(/color=(\w+) .*rect=(\S+) \[(.*)\]$/).slice(1));
  // DAT_004935e0：未听过曲目印 No.%2d + 全角 ？×11；title.anm 0xbc+i 每行 38 高
  assert.deepEqual(rows[0], ['ffffff', '0,0,1024,38', 'No. 1  桜舞い散る天空']);
  assert.deepEqual(rows[1], ['ffffff', '0,38,1024,76', 'No. 2  ？？？？？？？？？？？']);
  assert.deepEqual(rows[16], ['ffffff', '0,608,1024,646', 'No.17  ？？？？？？？？？？？']);
  assert.deepEqual(rows[17], ['ffffff', '0,0,1024,38', '♪桜舞い散る天空']);
  const notice = rows.filter(([c]) => c === '8080ff').map(([, , s]) => s);
  assert.equal(notice.length, 8);
  assert.equal(notice[1], '　　＊＊選択した曲はまだゲーム中で再生されていません＊＊');
  assert.equal(notice[7], '　　　再生したくない方は、カーソルを移動してください。');
  // 第二次确认：0x43c370 载入并播放；注释 VM 改刷该曲注释
  assert.deepEqual(rows.at(-8), ['ffffff', '0,0,1024,38', '♪希望の星は青霄に昇る']);
  const events = lines.filter(l => l.startsWith('event ')).map(l => l.slice(6));
  const from = events.indexOf('music-notice 1');
  assert.ok(from > 0, events.join('\n'));
  assert.deepEqual(events.slice(from - 1), ['bgm-op 3', 'music-notice 1', 'music-play 1', 'bgm-op 2']);
});
