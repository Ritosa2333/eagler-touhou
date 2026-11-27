import {test} from 'node:test';
import assert from 'node:assert/strict';
import {execFileSync} from 'node:child_process';
import {existsSync, mkdtempSync, rmSync} from 'node:fs';
import {tmpdir} from 'node:os';
import {join, resolve} from 'node:path';

const root = resolve(import.meta.dirname, '../../..');
const dat = process.env.TH16_DAT ?? resolve(root, 'games/th16/original/th16.dat');
const harness = process.env.TH16_HARNESS ?? resolve(import.meta.dirname, 'build/harness');

test('runtime: nine original manual pages, full ending, stage BGM and embedded title demo', {
  skip: !existsSync(dat) && 'original th16.dat not present',
}, () => {
  const demoDirectory = mkdtempSync(join(tmpdir(), 'th16-runtime-demos-'));
  let out;
  try {
    out = execFileSync(harness, ['runtime-regressions', dat, demoDirectory], {encoding: 'utf8', timeout: 120000});
  } finally {
    rmSync(demoDirectory, {recursive: true, force: true});
  }
  const pages = [...out.matchAll(/^manual-page (\d+) ([0-9a-f]+)$/gm)];
  assert.deepEqual(pages.map(p => Number(p[1])), [1, 2, 3, 4, 5, 6, 7, 8, 9]);
  assert.equal(new Set(pages.map(p => p[2])).size, 9);
  assert.match(out, /manual page load failures kept the directory usable/);
  assert.match(out, /pause manual can be cancelled when its animation bank is missing/);
  assert.match(out, /pause manual directory and first page use horizontal offsets/);
  assert.match(out, /ending-confirm keeps full script; loading dismissed/);
  assert.match(out, /stage-bgm .* -> /);
  const demos = [...out.matchAll(/^embedded-demo played stage (\d+) and returned to title$/gm)];
  assert.deepEqual(demos.map(demo => Number(demo[1])), [3, 1, 2, 3]);
  assert.match(out, /missing idle demo kept the title and accepted game start/);
  assert.match(out, /damaged embedded demo used the disk fallback in the next slot/);
});
