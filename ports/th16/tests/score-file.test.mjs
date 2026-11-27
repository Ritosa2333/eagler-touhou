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

test('score cap advances the same extend counters as original 0x43e080', () => {
  // Arguments are difficulty, score, extend index, lives and points. The
  // expected result records score, extend index, lives and extend count.
  const scenarios = [
    [[1, 999999998, 10, 6, 10], [999999999, 10, 6, 0]],
    [[1, 999999999, 10, 6, 10], [999999999, 214, 8, 2]],
    [[1, 999999999, 10, 8, 13000000], [999999999, 214, 8, 0]],
    [[1, 999999999, 214, 8, 653532160], [999999999, 214, 8, 0]],
    [[1, 999999999, 214, 8, 653532170], [999999999, 238, 8, 0]],
    [[1, 999999999, 10, 8, 2147483647], [999999999, 238, 8, 0]],
    [[4, 999999999, 6, 6, 10], [999999999, 7, 7, 1]],
    [[4, 999999999, 12, 8, 1120148480], [999999999, 12, 8, 0]],
    [[4, 999999999, 12, 8, 1120148490], [999999999, 13, 8, 0]],
    [[4, 999999999, 6, 8, 2147483647], [999999999, 13, 8, 0]],
  ];
  for (const [args, expected] of scenarios) {
    const result = execFileSync(harness, ['score-add', ...args.map(String)], {encoding: 'utf8', timeout: 5000});
    assert.deepEqual(result.trim().split(/\s+/).map(Number), expected, args.join(' '));
  }
});

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

test('Spell Practice capture updates the high score, capture and attempt read back from scores', needDat, () => {
  const dir = mkdtempSync(join(tmpdir(), 'th16-spell-score-'));
  try {
    const path = join(dir, 'scoreth16.dat');
    const chooseSpell = '314:0x20,316:0x20,320:1,355:1,395:1';
    const shoot = Array.from({length: 1640 - 527 + 1}, (_, i) => `${527 + i}:9`).join(',');
    const run = (frames, input, ...options) =>
      execFileSync(harness, ['scene-boot', dat, String(frames), input, `score=${path}`, ...options],
        {encoding: 'utf8'});

    // Easy, Reimu/Spring, first spell. Q leaves the Spell Practice results.
    const saved = run(1733, `${chooseSpell},440:1,485:1,${shoot},1670:0x10000`, 'unlock-spells', 'scenes');
    assert.match(saved, /^scene-change frame=1683 7->4$/m);
    assert.ok(existsSync(path));

    const reopened = run(450, chooseSpell, 'ascii');
    const score = reopened.split('\n').find(line => line.startsWith('ascii ') && line.includes('[SCORE')) ?? '';
    // Original game score record contains 36440 units, printed with a trailing zero.
    assert.match(score, /\[SCORE\s+364400\s+1\/\s+1\]$/);
  } finally {
    rmSync(dir, {recursive: true, force: true});
  }
});
