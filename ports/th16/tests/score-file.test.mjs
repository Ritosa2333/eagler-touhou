// scoreth16.dat 读写（0x4497e0 → 0x449880 / 0x449a00）：tools/oracle/score-golden.py 以 Unicorn 直跑原版得到的
// 镜像 / 写出字节 sha1（tests/golden/score-file.json）对拍 harness score-tool；另验证读入存档后标题按其解锁 Extra。
import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {createHash} from 'node:crypto';
import {existsSync, mkdtempSync, readFileSync, rmSync, writeFileSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join, resolve} from 'node:path';

const here = resolve(import.meta.dirname);
const monorepo = resolve(here, '../../..');
const harness = process.env.TH16_HARNESS ?? resolve(here, 'build/harness');
const dat = process.env.TH16_DAT ?? resolve(monorepo, 'games/th16/original/th16.dat');
const needDat = {skip: !existsSync(dat) && 'original th16.dat not present'};
const golden = JSON.parse(readFileSync(resolve(here, 'golden/score-file.json'), 'utf8'));
const sha1 = bytes => createHash('sha1').update(bytes).digest('hex');

function runCases(dir) {
  const saved = {};
  const results = {};
  for (const c of golden.cases) {
    let load = '-';
    if (c.load) {
      const [src, variant] = c.load.split(':');
      const bytes = Buffer.from(saved[src]);
      if (variant === 'badver') bytes[8] = 3;
      load = join(dir, `${c.name}.in`);
      writeFileSync(load, bytes);
    }
    const image = join(dir, `${c.name}.image`), out = join(dir, `${c.name}.saved`), after = join(dir, `${c.name}.after`);
    const log = execFileSync(harness, ['score-tool', load, image, out, after, ...c.pokes], {encoding: 'utf8'});
    saved[c.name] = readFileSync(out);
    results[c.name] = {log, image: sha1(readFileSync(image)), saved: sha1(saved[c.name]), size: saved[c.name].length,
                       after: sha1(readFileSync(after))};
  }
  return {results, saved};
}

test('score file image / save bytes match the original 0x4497e0 / 0x449a00 byte for byte', () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-score-'));
  try {
    const {results} = runCases(dir);
    for (const c of golden.cases) {
      const r = results[c.name];
      assert.match(r.log, new RegExp(`rng_seed=${c.rng_seed}\\b`), `${c.name}: 0x449720 RNG steps`);
      assert.equal(r.image, c.image, `${c.name}: image after 0x4497e0`);
      assert.equal(r.size, c.saved_size, `${c.name}: saved size`);
      assert.equal(r.saved, c.saved, `${c.name}: 0x449a00 bytes (LZSS 0x457b20 + 0x402330)`);
      assert.equal(r.after, c.image_after_save, `${c.name}: checksums / indices written back`);
    }
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});

test('title reads scoreth16.dat at boot: a stored no-continue clear enables Extra (0x44a850)', needDat, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-score-'));
  try {
    const {saved} = runCases(dir);
    const path = join(dir, 'scoreth16.dat');
    writeFileSync(path, saved.poked);
    const t = 180 + 1 + 0x83 + 1;
    const header = out => Object.fromEntries(out.split('\n')[0].split(' ').map(kv => kv.split('=')).map(([k, v]) => [k, Number(v)]));
    const fresh = header(execFileSync(harness, ['scene-boot', dat, String(t + 4), `${t}:0x20`], {encoding: 'utf8'}));
    assert.equal(fresh.cursor, 2, 'no file → Extra disabled');
    const loaded = header(execFileSync(harness, ['scene-boot', dat, String(t + 4), `${t}:0x20`, `score=${path}`], {encoding: 'utf8'}));
    assert.equal(loaded.cursor, 1, 'file with a no-continue clear → Extra selectable');
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});
