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
const reportSha256 = '7411892ba21f9f35bb88d53717e08beda3b3887ec602641d49eaf3b8b4cd3009';
const exeSha256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4';

function evidence() {
  const raw = fs.readFileSync(path.join(root, 'local/research/bullet-fixtures.json'));
  assert.equal(createHash('sha256').update(raw).digest('hex'), reportSha256);
  const report = JSON.parse(raw.toString('utf8'));
  assert.equal(report.schema, 'th128-original-byte-bullet-fixtures/1');
  assert.equal(report.executable.sha256, exeSha256);
  assert.equal(report.executable.sha256After, exeSha256);
  assert.equal(report.executable.unchanged, true);
  assert.equal(report.nativeCpuTrace, false);
  assert.equal(report.windowsGameProcessLaunched, false);
  assert.equal(report.caseCount, 49);
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(createHash('sha256').update(bytes).digest('hex'), chunk.sha256);
  }
  for (const constant of Object.values(report.constants)) {
    assert.equal(createHash('sha256').update(Buffer.from(constant.bytes, 'hex')).digest('hex'), constant.sha256);
  }
  for (const sample of report.cases) {
    assert.ok(['boundary', 'unsupported_call'].includes(sample.stop.reason), sample.id);
    assert.ok(sample.instructionsExecuted < sample.instructionBudget, sample.id);
    if (sample.geometry) assert.equal(sample.completedEmission, false);
  }
  return report;
}

function floatBits(value) {
  const bytes = Buffer.alloc(4);
  bytes.writeFloatLE(value);
  return bytes.readUInt32LE();
}

function vectors(report) {
  const records = [];
  for (const sample of report.cases) {
    if (sample.id.startsWith('choose-')) {
      const command = Buffer.from(sample.memory.command.before, 'hex');
      const choices = Array.from({length: 4}, (_, index) => command.readInt32LE(20 + index * 4));
      const valid = sample.difficulty >= 0 && sample.difficulty <= 4;
      records.push({input: ['choose', sample.difficulty, ...choices].join(' '),
        expected: ['choose', valid ? 1 : 0, valid ? sample.locals['8'] : 0].join(' ')});
    }
    if (sample.id.startsWith('ranked-')) {
      const speedCommand = Buffer.from(sample.speedStep.memory.command.before, 'hex');
      const countCommand = Buffer.from(sample.memory.command.before, 'hex');
      const speedArguments = Array.from({length: 8}, (_, index) => speedCommand.readUInt32LE(20 + index * 4));
      const countArguments = Array.from({length: 8}, (_, index) => countCommand.readInt32LE(20 + index * 4));
      records.push({input: ['ranked', sample.difficulty, ...speedArguments, ...countArguments].join(' '),
        expected: ['ranked', floatBits(sample.emitter.speed), floatBits(sample.emitter.endSpeed),
          sample.emitter.count, sample.emitter.layers].join(' ')});
    }
    if (sample.id === 'extension-distinct-parameters' || sample.id.startsWith('first-')) {
      const extension = sample.emitter.extension0;
      const values = [extension.control, extension.type, extension.parameterA, extension.parameterB,
        floatBits(extension.parameterR), floatBits(extension.parameterS)];
      records.push({input: ['extension', ...values].join(' '), expected: ['extension', ...values].join(' ')});
    }
    if (sample.geometry) {
      const emitter = sample.input.emitter;
      const values = [emitter.angle, emitter.spacing, emitter.speed, emitter.endSpeed, sample.input.aimAngle].map(floatBits);
      records.push({input: ['geometry', emitter.count, emitter.layers, emitter.aimMode,
        sample.input.bulletIndex, sample.input.layerIndex, ...values].join(' '),
      expected: ['geometry', ...['rawAngle', 'angle', 'speed', 'velocityX', 'velocityY']
        .map(name => sample.geometryBits[name])].join(' ')});
    }
  }
  return records;
}

test('bullet original-byte fixtures retain bounded, hash-pinned provenance', () => {
  const report = evidence();
  const expected = {435: '0x0041a55f', 500: '0x0041a67f', 501: '0x0041a7bd',
    502: '0x0041a8c3', 504: '0x0041aa39', 507: '0x0041af99', 509: '0x0041b009',
    521: '0x0041aafb', 522: '0x0041ab92'};
  assert.deepEqual(Object.fromEntries(Object.entries(report.dispatch).map(([opcode, entry]) => [opcode, entry.handler])), expected);
  for (const mapping of report.staticDispatchEvidence.mapping) {
    assert.equal(Buffer.from(mapping.targetBytes, 'hex').readUInt32LE(), Number.parseInt(expected[mapping.opcode]));
  }
});

test('TH128 bullet configuration and pre-ETEX geometry match native-byte vectors', {skip: !fs.existsSync(compiler)}, () => {
  const records = vectors(evidence());
  const directory = path.join(root, 'artifacts/tests');
  fs.mkdirSync(directory, {recursive: true});
  const output = path.join(directory, 'bullet-rules.cjs');
  const build = spawnSync(python, [compiler, 'tests/native/bullet-rules.cpp', 'src/game/BulletRules.cpp',
    '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fno-fast-math', '-ffp-contract=off',
    '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-o', output],
  {cwd: root, encoding: 'utf8', env: environment, timeout: 180000});
  assert.ifError(build.error);
  assert.equal(build.status, 0, build.stdout + build.stderr);
  const run = spawnSync(process.execPath, [output], {cwd: root, encoding: 'utf8', timeout: 30000,
    input: records.map(record => record.input).join('\n') + '\n'});
  assert.ifError(run.error);
  assert.equal(run.status, 0, run.stdout + run.stderr);
  assert.match(run.stdout, /bullet scalar vectors passed/);
  assert.deepEqual(run.stdout.trim().split(/\r?\n/).slice(1), records.map(record => record.expected));
});
