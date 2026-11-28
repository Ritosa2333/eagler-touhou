import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const scratch = path.join(root, 'artifacts/dsh-msg22-control');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');
const fixture = JSON.parse(fs.readFileSync(path.join(root, 'local/research/msg22-control-fixtures.json'), 'utf8'));

test('MSG22 434a20 queue evidence and native fade request', () => {
  assert.equal(fixture.schema, 'th128-msg22-control-fixtures/1');
  assert.equal(fixture.executable.sha256,
    'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(fixture.strings.queueName, '');
  assert.match(fixture.strings.fadeLog, /Sound : Fade Out Stage %d/);
  assert.equal(fixture.strings.bgmFormat, '../../bgm/thbgm.fmt');
  assert.equal(fixture.strings.bgmData, 'thbgm.dat');
  assert.equal(fixture.constants.framesPerSecond, 60);
  assert.equal(fixture.constants.volumeDivisor, 100);
  assert.equal(fixture.queue.capacity, 31);
  assert.equal(fixture.queue.stride, 0x10c);
  assert.deepEqual(fixture.operationTable.targets,
    ['0x004586b5', '0x004586f9', '0x00458893', '0x00458835',
     '0x004588b5', '0x004588ef', '0x0045890d', '0x00458698']);
  const args = [2, 8, 4, 11, 2, 2, -2];
  for (const [index, item] of fixture.cases.entries()) {
    assert.equal(item.stop.reason, 'returned');
    assert.equal(item.queue.operation, 5);
    assert.equal(item.queue.name, '');
    assert.equal(item.queue.field8, 0);
    assert.equal(item.queue.arg, args[index]);
  }
  assert.equal(fixture.slices['0x434a20:0x434a70'].sha256.length, 64);
  assert.equal(fixture.slices['0x4588b5:0x4588ef'].sha256.length, 64);

  assert.ok(fs.existsSync(compiler), `emscripten compiler missing: ${compiler}`);
  assert.ok(fs.existsSync(python), `emsdk python missing: ${python}`);
  fs.mkdirSync(scratch, {recursive: true});
  const cache = path.join(scratch, 'emcache');
  if (!fs.existsSync(path.join(cache, 'sanity.txt'))) {
    const shared = path.resolve(root, '../../work/emscripten-cache-4.0.22');
    assert.ok(fs.existsSync(shared), `shared emscripten cache missing: ${shared}`);
    fs.cpSync(shared, cache, {recursive: true,
      filter: source => !path.basename(source).endsWith('.lock')});
  }
  const temp = path.join(scratch, 'emtemp');
  fs.mkdirSync(path.join(temp, 'emscripten_temp'), {recursive: true});
  const output = path.join(scratch, 'message-time-control.cjs');
  const environment = {...process.env,
    EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
    EM_CACHE: cache.replaceAll('\\', '/'),
    EMCC_TEMP_DIR: temp,
    EMCC_DEBUG: '1',
    EMSDK_PYTHON: python,
    EMCC_CORES: '2',
    BINARYEN_CORES: '2'};
  const build = spawnSync(python, [compiler,
    'tests/native/message-time-control.cpp', 'src/game/MessageTimeControl.cpp',
    '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
    '-fno-fast-math', '-ffp-contract=off', '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1',
    '-o', output], {cwd: root, env: environment, encoding: 'utf8', timeout: 240000,
    maxBuffer: 16 * 1024 * 1024});
  assert.ifError(build.error);
  assert.equal(build.status, 0, build.stdout + build.stderr);

  const input = [
    'PLAN 1 1', 'PLAN 3 1', 'PLAN 5 1', 'PLAN 8 1', 'PLAN 10 1', 'PLAN 13 1',
    'PLAN 15 1', 'PLAN 16 1', 'PLAN 0 1', 'PLAN 2 0.5', 'PLAN 3 0.5', 'PLAN 2 0.7',
    'PLAN 2 0', 'PLAN 2 2', 'PLAN 2 -1', 'PLAN 24 1', 'PLAN -1 1',
    'PLAN 1 nan', 'PLAN 1 inf', 'PLAN 1 1e-9', 'PLAN 1 1e-7', 'PLAN 1 -1e-7',
    'STEP 480 480', 'STEP 1 480', 'STEP 2 480', 'STEP 2 2', 'STEP 1 2',
    'STEP 10 0', 'STEP 481 480',
  ];
  const expectedLines = [
    'fade op=5 base=8 scaled=8 frames=480 seconds=8 start=0 end=-5000 mode=1 immediate=0',
    'fade op=5 base=2 scaled=2 frames=120 seconds=2 start=0 end=-5000 mode=1 immediate=0',
    'fade op=5 base=2 scaled=2 frames=120 seconds=2 start=0 end=-5000 mode=1 immediate=0',
    'fade op=5 base=2 scaled=2 frames=120 seconds=2 start=0 end=-5000 mode=1 immediate=0',
    'fade op=5 base=2 scaled=2 frames=120 seconds=2 start=0 end=-5000 mode=1 immediate=0',
    'fade op=5 base=2 scaled=2 frames=120 seconds=2 start=0 end=-5000 mode=1 immediate=0',
    'fade op=5 base=2 scaled=2 frames=120 seconds=2 start=0 end=-5000 mode=1 immediate=0',
    'fade op=5 base=8 scaled=8 frames=480 seconds=8 start=0 end=-5000 mode=1 immediate=0',
    'fade op=5 base=8 scaled=8 frames=480 seconds=8 start=0 end=-5000 mode=1 immediate=0',
    'fade op=5 base=8 scaled=16 frames=960 seconds=16 start=0 end=-5000 mode=1 immediate=0',
    'fade op=5 base=2 scaled=4 frames=240 seconds=4 start=0 end=-5000 mode=1 immediate=0',
    'fade op=5 base=8 scaled=11 frames=660 seconds=11 start=0 end=-5000 mode=1 immediate=0',
    'fade op=5 base=8 scaled=8 frames=480 seconds=8 start=0 end=-5000 mode=1 immediate=0',
    'fade op=5 base=8 scaled=8 frames=480 seconds=8 start=0 end=-5000 mode=1 immediate=0',
    'fade op=5 base=8 scaled=-8 frames=-480 seconds=-8 start=0 end=-5000 mode=1 immediate=1',
    'unsupported invalid-stage',
    'unsupported invalid-stage',
    'unsupported invalid-clock',
    'unsupported invalid-clock',
    'unsupported conversion-overflow',
    'unsupported duration-overflow',
    'unsupported duration-overflow',
    'step active=1 remaining=479 volume=-11',
    'step active=0 remaining=0 volume=-5000',
    'step active=1 remaining=1 volume=-4990',
    'step active=1 remaining=1 volume=-2500',
    'step active=0 remaining=0 volume=-5000',
    'step active=0 remaining=0 volume=-5000',
    'step active=1 remaining=480 volume=0',
  ];
  const run = spawnSync(process.execPath, [output], {cwd: root, input: input.join('\n') + '\n',
    encoding: 'utf8', timeout: 30000, maxBuffer: 4 * 1024 * 1024});
  assert.ifError(run.error);
  assert.equal(run.status, 0, run.stdout + run.stderr);
  assert.deepEqual(run.stdout.trim().split(/\r?\n/), expectedLines);
});
