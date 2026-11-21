import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const bundledPython = path.join(sdk, 'python/3.13.3_64bit/python.exe');
const python = process.env.TH128_PYTHON ?? (fs.existsSync(bundledPython) ? bundledPython : 'python');
const environment = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
  EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
  EMSDK_PYTHON: python, PYTHONUTF8: '1', EMCC_CORES: '2', BINARYEN_CORES: '2'};
const reportSha256 = '959afe7bf10aef9e0f46d348340d92a9d52dba3e7d13dc3efff131b7461ac997';

function floatBits(value) {
  const bytes = Buffer.alloc(4);
  bytes.writeFloatLE(value);
  return bytes.readUInt32LE();
}

function evidence() {
  const raw = fs.readFileSync(path.join(root, 'local/research/bullet-lifecycle-fixtures.json'));
  assert.equal(createHash('sha256').update(raw).digest('hex'), reportSha256);
  const report = JSON.parse(raw.toString('utf8'));
  assert.equal(report.schema, 'th128-original-byte-bullet-lifecycle-fixtures/1');
  assert.equal(report.executable.sha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256After, report.executable.sha256);
  assert.equal(report.executable.unchanged, true);
  assert.equal(report.nativeCpuTrace, false);
  assert.equal(report.windowsGameProcessLaunched, false);
  assert.equal(report.caseCount, 35);
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(createHash('sha256').update(bytes).digest('hex'), chunk.sha256);
  }
  for (const constant of Object.values(report.constants)) {
    assert.equal(createHash('sha256').update(Buffer.from(constant.bytes, 'hex')).digest('hex'), constant.sha256);
  }
  for (const sample of report.cases) {
    assert.equal(sample.stop.reason, 'boundary', sample.id);
    assert.ok(sample.instructionsExecuted < sample.instructionBudget, sample.id);
    if (sample.completedEmission !== undefined) assert.equal(sample.completedEmission, false);
    if (sample.completedTick !== undefined) assert.equal(sample.completedTick, false);
  }
  return report;
}

function vectors(report) {
  return report.cases.map(sample => {
    if (sample.id.startsWith('aim-')) {
      return {input: ['aim', ...sample.input.position.map(floatBits), 0,
        ...sample.input.playerPosition.map(floatBits), 0].join(' '), expected: `aim ${sample.aimBits}`};
    }
    if (sample.id.startsWith('spawn-')) {
      const emitter = Buffer.from(sample.input.emitter, 'hex');
      const extension = Buffer.from(sample.state.extensionBytes, 'hex');
      const state = sample.state;
      return {input: ['spawn', emitter.readUInt32LE(0x36c), emitter.readInt32LE(0x374),
        emitter.readInt32LE(0x38), emitter.readInt32LE(0x48), emitter.readUInt32LE(0x28),
        emitter.readUInt32LE(0x2c), emitter.readInt32LE(0x3c), emitter.readInt32LE(0x4c),
        sample.input.postAnimationFlags, ...sample.input.positionBits,
        ...sample.input.velocityBits].join(' '), expected: ['spawn', state.motionState,
        state.animationInterrupt, state.extensionIndex, state.flags, state.activeExtensions,
        state.emitterFlags, state.transformSound, state.profileValue, ...state.sizeBits,
        ...state.positionBits, ...state.velocityBits, extension.readUInt32LE(0), extension.readUInt32LE(4),
        extension.readInt32LE(0x10), extension.readInt32LE(0x14), extension.readInt32LE(0x20),
        extension.readInt32LE(0x24)].join(' ')};
    }
    return {input: ['motion', sample.input.motionState, sample.input.timeScaleBits,
      ...sample.input.positionBits, ...sample.input.velocityBits].join(' '),
    expected: ['motion', ...sample.positionBits].join(' ')};
  });
}

test('TH128 bullet lifecycle evidence contains original calls and bounded partial results', () => {
  const report = evidence();
  assert.ok(report.cases.some(sample => sample.instructionTrace.includes('0x004928ab')));
  assert.ok(report.cases.some(sample => sample.instructionTrace.includes('0x00409968')));
  assert.ok(report.cases.every(sample => !sample.instructionTrace.includes('0x004099e1')));
});

for (const sanitized of [false, true]) {
  test(`TH128 bullet lifecycle prefixes match 35 original-byte vectors${sanitized ? ' with UBSan' : ''}`,
    {skip: !fs.existsSync(compiler)}, () => {
      assert.match(fs.readFileSync(path.join(root, 'src/game/BulletRules.hpp'), 'utf8'),
        /targetAnglePrefix/, 'portable lifecycle prefixes are not implemented');
      const records = vectors(evidence());
      const directory = path.join(root, 'artifacts/tests');
      fs.mkdirSync(directory, {recursive: true});
      const output = path.join(directory, `bullet-lifecycle${sanitized ? '-ubsan' : ''}.cjs`);
      const flags = sanitized ? ['-fsanitize=undefined', '-fno-sanitize-recover=all'] : [];
      const build = spawnSync(python, [compiler, 'tests/native/bullet-lifecycle.cpp', 'src/game/BulletRules.cpp',
        '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fno-fast-math', '-ffp-contract=off', ...flags,
        '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-o', output],
      {cwd: root, encoding: 'utf8', env: environment, timeout: 180000});
      assert.ifError(build.error);
      assert.equal(build.status, 0, build.stdout + build.stderr);
      const run = spawnSync(process.execPath, [output], {cwd: root, encoding: 'utf8', timeout: 30000,
        input: records.map(record => record.input).join('\n') + '\n'});
      assert.ifError(run.error);
      assert.equal(run.status, 0, run.stdout + run.stderr);
      assert.deepEqual(run.stdout.trim().split(/\r?\n/), records.map(record => record.expected));
    });
}
