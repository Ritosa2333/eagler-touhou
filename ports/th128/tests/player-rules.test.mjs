import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import test, {after} from 'node:test';
import {fileURLToPath} from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const bundledPython = path.join(sdk, 'python/3.13.3_64bit/python.exe');
const python = process.env.TH128_PYTHON ?? (fs.existsSync(bundledPython) ? bundledPython : 'python');
const environment = {...process.env,
  EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
  EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
  EMSDK_PYTHON: python, PYTHONUTF8: '1', PYTHONDONTWRITEBYTECODE: '1', EM_FROZEN_CACHE: '1',
  EMCC_CORES: '2', BINARYEN_CORES: '2'};
const reportPath = path.join(root, 'local/research/player-native-fixtures.json');
const reportSha256 = 'e815449988882ca870c78bc93a50facbc6559a9b4f7cd81300de70a0c27d902e';
const outputs = new Map();
let directory;

after(() => {
  if (!directory) return;
  assert.equal(path.dirname(directory), fs.realpathSync(os.tmpdir()));
  for (const output of outputs.values()) {
    if (fs.existsSync(output)) fs.unlinkSync(output);
  }
  fs.rmdirSync(directory);
});

function buildFixture(sanitized = false) {
  if (outputs.has(sanitized)) return outputs.get(sanitized);
  directory ??= fs.mkdtempSync(path.join(fs.realpathSync(os.tmpdir()), 'th128-player-rules-'));
  const output = path.join(directory, sanitized ? 'player-ubsan.cjs' : 'player.cjs');
  outputs.set(sanitized, output);
  const options = sanitized ? ['-fsanitize=undefined', '-fno-sanitize-recover=all'] : [];
  const build = spawnSync(python, [compiler, 'tests/native/player-rules.cpp', 'src/game/PlayerRules.cpp',
    '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fno-fast-math', '-ffp-contract=off',
    '-sSINGLE_FILE=1', '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-sASSERTIONS=1', ...options, '-o', output],
  {cwd: root, env: environment, encoding: 'utf8', timeout: 180000, maxBuffer: 8 * 1024 * 1024});
  assert.ifError(build.error);
  assert.equal(build.status, 0, build.stdout + build.stderr);
  return output;
}

function readReport() {
  const binary = fs.readFileSync(reportPath);
  assert.equal(createHash('sha256').update(binary).digest('hex'), reportSha256);
  const report = JSON.parse(binary.toString('utf8'));
  assert.equal(report.schema, 'th128-player-native-fixtures/1');
  assert.equal(report.exeSha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.shotSha256, 'aed90a45b565510e97cc7d9ab4846d8c93914e57faafe9339e9ab03d47b1511b');
  assert.equal(report.windowsGameProcessLaunched, false);
  assert.equal(report.caseCount, 677);
  assert.deepEqual(report.nativeRawSpeeds, [576, 256, 407, 181]);
  assert.equal(report.shotLayout.relocation.unknownBytesUnchanged, true);
  for (const chunk of report.nativeCode) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, chunk.endExclusive - chunk.start);
    assert.equal(createHash('sha256').update(bytes).digest('hex'), chunk.sha256);
  }
  return report;
}

function vectors(report) {
  const result = [];
  const add = (operation, inputs, expected) => result.push({line: [operation, ...inputs].join(' '), expected});
  for (const sample of report.cases.directionFocus) {
    const context = sample.context;
    add('focus', [sample.holding, Number(context.managerPresent), Number(context.managerActive),
      context.counterF98, context.holdCount43914], [sample.result.direction, Number(sample.result.focused)]);
  }
  for (const sample of report.cases.velocity) {
    add('velocity', [sample.direction, Number(sample.focused)], sample.result);
  }
  for (const sample of report.cases.position) {
    add('position', [...sample.start, ...sample.delta], [...sample.result.raw, ...sample.result.pixels]);
  }
  for (const sample of report.cases.shotStart) {
    add('start', [sample.holding, sample.pressed, sample.current], [Number(sample.result === 'start')]);
  }
  for (const sample of report.cases.shotCycle) {
    add('cycle', [sample.holding, sample.pressed, sample.current], sample.result.counts);
  }
  for (const sample of report.cases.recordSchedule) {
    add('record', [sample.period, sample.phase, sample.count], [Number(sample.result)]);
  }
  return result;
}

test('player proof pins original instruction bytes and the actual shot resource', () => {
  assert.equal(vectors(readReport()).length, 677);
});

for (const sanitized of [false, true]) {
  test(`pure C++ player rules match 677 original-byte vectors${sanitized ? ' under UBSan' : ''}`,
    {skip: !fs.existsSync(compiler)}, () => {
      const cases = vectors(readReport());
      const run = spawnSync(process.execPath, [buildFixture(sanitized)],
        {cwd: root, input: cases.map(sample => sample.line).join('\n') + '\n', encoding: 'utf8',
          timeout: 30000, maxBuffer: 8 * 1024 * 1024});
      assert.ifError(run.error);
      assert.equal(run.status, 0, run.stdout + run.stderr);
      const lines = run.stdout.trim().split(/\r?\n/);
      assert.equal(lines.shift(), 'player fixed edges passed');
      assert.equal(lines.length, cases.length);
      lines.forEach((line, index) => {
        assert.deepEqual(line.split(' ').map(Number), cases[index].expected, cases[index].line);
      });
      assert.equal(run.stderr, '');
    });
}
