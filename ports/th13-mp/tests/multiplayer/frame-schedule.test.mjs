import {test} from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {fileURLToPath} from 'node:url';

test('packet wake retains a due frame without advancing the input clock', () => {
  const root = fileURLToPath(new URL('../..', import.meta.url));
  const out = resolve(root, '../../../artifacts/th13-mp-frame-schedule');
  mkdirSync(out, {recursive: true});
  const binary = resolve(out, 'frame-schedule-test');
  const build = spawnSync(process.env.CXX || 'g++', ['-std=c++17', '-O2',
    '-I' + resolve(root, '../../common/include'),
    resolve(root, 'tests/multiplayer/frame_schedule_test.cpp'), '-o', binary], {encoding: 'utf8'});
  assert.equal(build.status, 0, build.stderr);
  const run = spawnSync(binary, [], {encoding: 'utf8'});
  assert.equal(run.status, 0, run.stderr);
});
