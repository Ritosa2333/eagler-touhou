import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {resolve, dirname} from 'node:path';
import {fileURLToPath} from 'node:url';

const port = resolve(dirname(fileURLToPath(import.meta.url)), '..');

test('录像解码与输入流单测（XOR/LZ77/全链路/真机关键帧，g++ 套件）', {timeout: 180000}, () => {
  const run = spawnSync('bash', [resolve(port, 'tests/replay/run.sh')], {encoding: 'utf8'});
  if (run.status !== 0) {
    console.log(run.stdout);
    console.error(run.stderr);
  }
  assert.equal(run.status, 0, 'tests/replay/run.sh 应以 0 退出');
  const outlines = run.stdout.split('\n').filter(l => l.startsWith('ok '));
  assert.ok(outlines.length >= 8, `应至少通过 8 条用例，实际 ${outlines.length}`);
});
