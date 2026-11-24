import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const work = path.resolve(root, '../../work/bullet-visuals-test');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const bundledPython = path.join(sdk, 'python/3.13.3_64bit/python.exe');
const python = process.env.TH128_PYTHON ?? (fs.existsSync(bundledPython) ? bundledPython : 'python');
const environment = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
  EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
  EMSDK_PYTHON: python, PYTHONUTF8: '1', EMCC_CORES: '2', BINARYEN_CORES: '2'};
const digest = bytes => createHash('sha256').update(bytes).digest('hex');

function evidence() {
  const bytes = fs.readFileSync(path.join(root, 'local/research/bullet-visual-fixtures.json'));
  assert.equal(digest(bytes), 'fe5e11ed7c2719d0bc8a76a8b1c1982cd757037c5cd284bd27b19d257eb043ad');
  const report = JSON.parse(bytes.toString('utf8'));
  assert.equal(report.schema, 'th128-original-byte-bullet-visual-fixtures/1');
  assert.equal(report.executable.sha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256After, report.executable.sha256);
  assert.equal(report.executable.unchanged, true);
  assert.equal(report.nativeCpuTrace, false);
  assert.equal(report.windowsGameProcessLaunched, false);
  assert.equal(report.profile.script, 43);
  assert.equal(report.profile.secondaryScript, 0);
  assert.equal(report.caseCount, 64);
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(digest(bytes), chunk.sha256);
  }
  for (const constant of Object.values(report.constants))
    assert.equal(digest(Buffer.from(constant.bytes, 'hex')), constant.sha256);
  return report;
}

test('BulletVisuals reusable two-VM API is implemented', () => {
  for (const file of ['src/game/BulletVisuals.hpp', 'src/game/BulletVisuals.cpp'])
    assert.ok(fs.existsSync(path.join(root, file)), `${file}: BulletVisuals is not implemented`);
});

test('type8 sprite selections come from the actual four-slot TH128 callback', () => {
  const report = evidence();
  const profile = Buffer.from(report.constants['0x004b2dd0'].bytes, 'hex');
  for (const sample of report.cases) {
    assert.equal(sample.stop.reason, 'returned');
    assert.ok(sample.instructionsExecuted < sample.instructionBudget);
    assert.equal(sample.entry, '0x0040ccb0');
    assert.equal(sample.input.type, 8);
    assert.equal(sample.result, sample.input.remap[sample.input.sprite]);
    assert.equal(sample.result, profile.readInt32LE(4 + sample.input.color * 16 + sample.input.sprite * 4));
    assert.ok(sample.instructionTrace.includes('0x0040ccdf'));
  }
  assert.deepEqual(report.cases.find(sample => sample.input.color === 10).input.remap, [138, 328, 346, 357]);
  assert.equal(report.unknownCall.stop.reason, 'unsupported_call');
  assert.equal(report.unknownCall.stop.target, '0x00402ea0');
});

test('actual bullet43 startup, IRQ8, control-int handoff and two-VM lifecycle in WASM',
  {skip: !fs.existsSync(compiler)}, () => {
    assert.ok(fs.existsSync(path.join(root, 'src/game/BulletVisuals.hpp')), 'BulletVisuals is not implemented');
    const report = evidence();
    const catalog = JSON.parse(fs.readFileSync(path.join(root, 'local/catalog.json'), 'utf8'));
    const manifest = path.resolve(root, 'local', catalog.variants.ja.manifest);
    const source = path.join(path.dirname(manifest), 'raw/bullet.anm');
    assert.ok(source.startsWith(path.join(root, 'local') + path.sep));
    assert.equal(digest(fs.readFileSync(source)), report.animation.sha256);
    fs.mkdirSync(work, {recursive: true});
    const output = path.join(work, 'bullet-visuals.cjs');
    const build = spawnSync(python, [compiler, 'tests/native/bullet-visuals.cpp',
      'src/game/BulletVisuals.cpp', 'src/animation/AnmLogic.cpp', '-Isrc', '-std=c++20', '-O1',
      '-Wall', '-Wextra', '-Werror', '-fno-fast-math', '-ffp-contract=off',
      '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-sNODERAWFS=1', '-o', output],
    {cwd: root, encoding: 'utf8', env: environment, timeout: 180000});
    assert.ifError(build.error);
    assert.equal(build.status, 0, build.stdout + build.stderr);
    const inputs = Array.from({length: 16}, (_, color) =>
      report.cases.find(sample => sample.input.color === color).input.remap.join(' ')).join('\n');
    const run = spawnSync(process.execPath, [output, source], {cwd: root, encoding: 'utf8',
      timeout: 30000, input: inputs + '\n'});
    assert.ifError(run.error);
    assert.equal(run.status, 0, run.stdout + run.stderr);
    assert.match(run.stdout, /bullet visuals actual script43 regressions passed: 16 colors/);
  });
