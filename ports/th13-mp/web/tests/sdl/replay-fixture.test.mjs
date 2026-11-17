import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {resolve, dirname} from 'node:path';
import {fileURLToPath} from 'node:url';
import {replayFixture} from './replay-fixture.mjs';

const workspace = resolve(dirname(fileURLToPath(import.meta.url)), '../../../../../..');
test('browser EOF fixture decodes through the independent Python replay reader', () => {
  const result = spawnSync('python3', ['-c', `
import sys
from oracle.replayfmt import ReplayFile
r = ReplayFile().decode(sys.stdin.buffer.read())
s = r.stages[0]
assert s.frame_count == 2
assert s.frame(0) == (0, 0, 0)
assert s.frame(1) == (65535, 65535, 65535)
assert r.practice_stage == -1
assert r.rank == 1
print('decoded: neutral frame + EOF')
`], {cwd: workspace, input: replayFixture(), encoding: 'utf8'});
  assert.equal(result.status, 0, result.stderr);
  assert.match(result.stdout, /neutral frame \+ EOF/);
});
