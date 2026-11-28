import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const scratch = path.join(root, 'artifacts/dsh-stage-control');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');
const fixture = JSON.parse(fs.readFileSync(path.join(root, 'local/research/stage-control-fixtures.json'), 'utf8'));

test('original 411c70 stage request and 558 lifecycle', () => {
  assert.equal(fixture.schema, 'th128-stage-control-fixtures/1');
  assert.equal(fixture.executable.sha256,
    'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(fixture.stage.address, '0x004d2e68');
  assert.equal(fixture.stage.size, 2500);
  assert.equal(fixture.stage.flags, '0x004d33f8');
  assert.equal(fixture.stage.flagsOffset, 0x590);
  assert.deepEqual(fixture.initialization.constructor.arguments, [0x4d2e68, 0, 2500]);
  assert.equal(fixture.initialization.initialFlags, 0x4240);
  assert.deepEqual(fixture.initialization.cases.map(item => item.flags),
    [0x4240, 0xC240, 0x6240]);
  assert.deepEqual(fixture.initialization.clear.map(item => item.flags),
    [0x4240, 0x8240]);
  assert.deepEqual(fixture.requests.map(item => item.pending), [12, 2, 12]);
  for (const item of fixture.requests) assert.equal(item.stop.reason, 'returned');
  for (const key of ['0x411c70:0x411c8e', '0x436210:0x436239', '0x4358e0:0x4359e1',
    '0x435a71:0x435aa2', '0x4334b0:0x43350f'])
    assert.equal(fixture.slices[key].sha256.length, 64);

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
  const output = path.join(scratch, 'stage-control.cjs');
  const environment = {...process.env,
    EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
    EM_CACHE: cache.replaceAll('\\', '/'),
    EMCC_TEMP_DIR: temp,
    EMCC_DEBUG: '1',
    EMSDK_PYTHON: python,
    EMCC_CORES: '2',
    BINARYEN_CORES: '2'};
  const build = spawnSync(python, [compiler,
    'tests/native/stage-control.cpp', 'src/game/StageControl.cpp',
    '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
    '-fno-fast-math', '-ffp-contract=off', '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1',
    '-o', output], {cwd: root, env: environment, encoding: 'utf8', timeout: 240000,
    maxBuffer: 16 * 1024 * 1024});
  assert.ifError(build.error);
  assert.equal(build.status, 0, build.stdout + build.stderr);

  const input = [
    'INIT 0', 'REQUEST 12', 'CONSUME 1 7', 'REQUEST 12', 'CONSUME 1 7',
    'REQUEST 2', 'CONSUME 0 0', 'INIT 8192', 'REQUEST 12', 'CONSUME 1 7',
    'RELEASE', 'REQUEST 12', 'CONSUME 0 0', 'SYNC 8192', 'INIT 0',
    'REQUEST 4', 'CONSUME 1 0', 'REQUEST 17', 'CONSUME 0 0',
    'REQUEST 18', 'REQUEST -1', 'REQUEST 0', 'CONSUME 0 0',
  ];
  const expected = [
    'init flags=16960 current=0 pending=0 session=0 gate=0',
    'request status=written requested=12 pending=12 collapsed=0',
    'consume status=applied previous=0 current=7 pending=7 session=0 action=recreate destroy=0 arg=7',
    'request status=written requested=12 pending=12 collapsed=0',
    'consume status=applied previous=7 current=7 pending=7 session=0 action=recreate destroy=1 arg=7',
    'request status=written requested=2 pending=2 collapsed=0',
    'consume status=applied previous=7 current=2 pending=2 session=0 action=none destroy=0 arg=0',
    'init flags=25152 current=0 pending=0 session=0 gate=1',
    'request status=written requested=12 pending=2 collapsed=1',
    'consume status=applied previous=0 current=2 pending=2 session=0 action=none destroy=0 arg=0',
    'release changed=1 flags=16960 gate=0',
    'request status=written requested=12 pending=12 collapsed=0',
    'consume status=missing-session previous=2 current=2 pending=12 session=0 action=none destroy=0 arg=0',
    'sync flags=8192 gate=1',
    'init flags=16960 current=0 pending=0 session=0 gate=0',
    'request status=written requested=4 pending=4 collapsed=0',
    'consume status=unsupported previous=0 current=0 pending=4 session=0 action=none destroy=0 arg=0',
    'request status=written requested=17 pending=17 collapsed=0',
    'consume status=unsupported previous=0 current=0 pending=17 session=0 action=none destroy=0 arg=0',
    'request status=rejected requested=18 pending=0 collapsed=0',
    'request status=rejected requested=-1 pending=0 collapsed=0',
    'request status=written requested=0 pending=0 collapsed=0',
    'consume status=no-change previous=0 current=0 pending=0 session=0 action=none destroy=0 arg=0',
  ];
  const run = spawnSync(process.execPath, [output], {cwd: root, input: input.join('\n') + '\n',
    encoding: 'utf8', timeout: 30000, maxBuffer: 4 * 1024 * 1024});
  assert.ifError(run.error);
  assert.equal(run.status, 0, run.stdout + run.stderr);
  assert.deepEqual(run.stdout.trim().split(/\r?\n/), expected);
});
