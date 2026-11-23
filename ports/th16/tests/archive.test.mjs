// THA1 档案与 ANM v8 解析：对照 thtk（thdat -x16 / thanm -l16）对同一 th16.dat 的解包结果。
// 原版资源不入库：缺少 games/th16/original/th16.dat 或 artifacts/th16/dat 时整体跳过。
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {existsSync, readFileSync, readdirSync} from 'node:fs';
import {resolve} from 'node:path';

const here = resolve(import.meta.dirname);
const monorepo = resolve(here, '../../..');
const harness = process.env.TH16_HARNESS ?? resolve(here, 'build/harness');
const dat = process.env.TH16_DAT ?? resolve(monorepo, 'games/th16/original/th16.dat');
const extracted = process.env.TH16_THTK_DIR ?? resolve(monorepo, 'artifacts/th16/dat');
const anmDumps = resolve(monorepo, 'artifacts/th16/dumps/anm');
const available = existsSync(dat) && existsSync(extracted);

function fnv1a64(bytes) {
  let h = 0xcbf29ce484222325n;
  for (const b of bytes) { h ^= BigInt(b); h = (h * 0x100000001b3n) & 0xffffffffffffffffn; }
  return h.toString(16).padStart(16, '0');
}

test('archive-list matches thtk extraction byte-for-byte (fnv1a64 + size)', {skip: !available && 'original th16.dat / thtk extraction not present'}, () => {
  const lines = execFileSync(harness, ['archive-list', dat], {encoding: 'utf8', maxBuffer: 1 << 26}).trim().split('\n');
  const ours = new Map(lines.map(l => { const [name, size, hash] = l.split(' '); return [name, {size: Number(size), hash}]; }));
  const files = readdirSync(extracted).filter(n => !['list.txt', 'extract.log'].includes(n));
  assert.equal(ours.size, files.length, 'entry count equals thtk file count');
  for (const name of files) {
    const bytes = readFileSync(resolve(extracted, name));
    const entry = ours.get(name);
    assert.ok(entry, `entry ${name} present`);
    assert.equal(entry.size, bytes.length, `${name} size`);
    assert.equal(entry.hash, fnv1a64(bytes), `${name} content`);
  }
});

for (const anm of ['title.anm', 'sig.anm', 'ascii.anm', 'text.anm', 'front.anm', 'bullet.anm', 'effect.anm', 'enemy.anm', 'pl00.anm', 'st01wl.anm', 'e01.anm']) {
  test(`${anm}: entries/sprites/scripts agree with thanm -l16`, {skip: !available && 'original th16.dat not present'}, () => {
    const dump = readFileSync(resolve(anmDumps, anm.replace('.anm', '.txt')), 'utf8');
    const expectedEntries = [...dump.matchAll(/^entry entry\d+ \{\n\s+version: (\d+),\n\s+name: "([^"]*)",\n\s+format: (\d+),\n\s+width: (\d+),\n\s+height: (\d+),/gm)]
      .map(m => ({version: +m[1], name: m[2], format: +m[3], width: +m[4], height: +m[5]}));
    const expectedSprites = (dump.match(/^\s+sprite\d+: \{/gm) ?? []).length;
    const expectedScripts = (dump.match(/^script (-?\d+ )?script\d+ \{/gm) ?? []).length;
    const out = execFileSync(harness, ['anm', dat, anm], {encoding: 'utf8', maxBuffer: 1 << 26}).trim().split('\n');
    const entries = out.filter(l => !l.startsWith('  ')).map(l => { const [name, w, h, format, sprites, scripts, thtx] = l.split(' '); return {name, width: +w, height: +h, format: +format, sprites: +sprites, scripts: +scripts, thtx}; });
    assert.equal(entries.length, expectedEntries.length, 'entry count');
    entries.forEach((e, i) => {
      assert.equal(e.name, expectedEntries[i].name, `entry ${i} name`);
      assert.equal(e.width, expectedEntries[i].width, `entry ${i} width`);
      assert.equal(e.height, expectedEntries[i].height, `entry ${i} height`);
      assert.equal(e.format, expectedEntries[i].format, `entry ${i} format`);
    });
    assert.equal(entries.reduce((n, e) => n + e.sprites, 0), expectedSprites, 'sprite total');
    assert.equal(entries.reduce((n, e) => n + e.scripts, 0), expectedScripts, 'script total');
    const scripts = out.filter(l => l.startsWith('  script'));
    for (const s of scripts) assert.match(s, / sentinel=1$/, `every script ends with exactly one sentinel: ${s}`);
  });
}
