import assert from 'node:assert/strict';
import test from 'node:test';
import crypto from 'node:crypto';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const scratch = path.join(root, 'artifacts/dsh-route-choice');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');
const front = path.join(root, 'artifacts/dsh-readonly-inputs/front.anm');
const fixture = JSON.parse(fs.readFileSync(path.join(root, 'local/research/route-choice-fixtures.json'), 'utf8'));
const sha256 = value => crypto.createHash('sha256').update(value).digest('hex');

test('actual front169 route choice Scene, input, wait, IRQ and lifecycle', () => {
  assert.ok(fs.existsSync(compiler), `emscripten compiler missing: ${compiler}`);
  assert.ok(fs.existsSync(python), `emsdk python missing: ${python}`);
  assert.ok(fs.existsSync(front), `read-only front.anm missing: ${front}`);
  assert.equal(fixture.schema, 'th128-route-choice-fixtures/1');
  assert.equal(fixture.executable.sha256,
    'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(fixture.front.sha256, sha256(fs.readFileSync(front)));
  assert.deepEqual(fixture.spawnCommands.map(item => item.raw),
    ['a6000000', 'a7000000', 'a8000000']);
  assert.deepEqual(fixture.choiceJumpTable.targets,
    ['0x004232d0', '0x00423557', '0x00423557', '0x00423648']);
  for (const index of ['166', '167', '168', '169'])
    assert.ok(fixture.scripts[index].commands.length > 0, `fixture script ${index}`);
  assert.deepEqual(fixture.stageSprites, {
    '1': {167: 0x70, 168: 0x71},
    '6': {167: 0x6f, 168: 0x71},
    '11': {167: 0x6f, 168: 0x70},
  });
  const emulated = fixture.emulations;
  assert.equal(emulated.intro.fields.state.after, 1);
  assert.equal(emulated.intro.fields.choice.after, 2);
  assert.equal(emulated.intro.fields.base.after, 3);
  assert.equal(emulated.selectUp.fields.choice.after, 0);
  assert.equal(emulated.selectUp.registerEbx, 7);
  assert.equal(emulated.selectDown.fields.choice.after, 1);
  assert.equal(emulated.selectDown.registerEbx, 8);
  assert.equal(emulated.selectBoth.fields.choice.after, 1);
  assert.equal(emulated.selectBoth.registerEbx, 8);
  assert.equal(emulated.selectNone.fields.choice.after, 2);
  assert.equal(emulated.selectLocked.fields.choice.after, 0);
  assert.equal(emulated.selectRepeat.stop.address, '0x0042366d');
  assert.equal(emulated.selectRepeat.registerEbx, 0);
  assert.equal(emulated.confirmUp.registerEbx, 9);
  assert.equal(emulated.confirmDown.registerEbx, 10);
  assert.equal(emulated.confirmLocked.registerEbx, 10);
  assert.equal(emulated.selectCommit.fields.frame.after, 0);
  assert.equal(emulated.confirmCommit.fields.state.after, 3);
  assert.equal(emulated.confirmCommit.fields.frame.after, 0);
  assert.equal(emulated.cleanupDecision.registerEbx, 1);
  assert.equal(emulated.cleanupLocked.stop.address, '0x0042366d');
  assert.equal(emulated.cleanupCommit.fields.handle.after, 0);
  assert.deepEqual(emulated.msg30AnmCall.argumentsAtCall.slice(1), [0xa9, 0x17, 0]);
  assert.equal(emulated.msg30Fields[0].fields.choice.after, 0);
  assert.equal(emulated.msg30Fields[2].fields.choice.after, -6);
  assert.equal(fixture.nativeCpuTrace, false);
  assert.equal(fixture.windowsGameProcessLaunched, false);

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
  const output = path.join(scratch, 'route-choice-host.cjs');
  const environment = {...process.env,
    EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
    EM_CACHE: cache.replaceAll('\\', '/'),
    EMCC_TEMP_DIR: temp,
    EMCC_DEBUG: '1',
    EMSDK_PYTHON: python,
    EMCC_CORES: '2',
    BINARYEN_CORES: '2'};
  const build = spawnSync(python, [compiler,
    'tests/native/route-choice-host.cpp', 'src/game/RouteChoiceHost.cpp',
    'src/animation/AnmLogic.cpp', 'src/animation/AnmScene.cpp',
    '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
    '-fno-fast-math', '-ffp-contract=off', '-fsanitize=undefined', '-fno-sanitize-recover=all',
    '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-sNODERAWFS=1', '-sALLOW_MEMORY_GROWTH=1',
    '-o', output], {cwd: root, env: environment, encoding: 'utf8', timeout: 240000,
    maxBuffer: 16 * 1024 * 1024});
  assert.ifError(build.error);
  assert.equal(build.status, 0, build.stdout + build.stderr);
  const run = spawnSync(process.execPath, [output, front],
    {cwd: root, encoding: 'utf8', timeout: 60000, maxBuffer: 4 * 1024 * 1024});
  assert.ifError(run.error);
  assert.equal(run.status, 0, run.stdout + run.stderr);
  assert.match(run.stdout, /actual front169\/166\/167\/168 route choice lifecycle passed/);
});
