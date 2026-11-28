import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {createHash} from 'node:crypto';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const bundledPython = path.join(sdk, 'python/3.13.3_64bit/python.exe');
const python = process.env.TH128_PYTHON ?? (fs.existsSync(bundledPython) ? bundledPython : 'python');
const environment = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
  EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
  EMSDK_PYTHON: python, PYTHONUTF8: '1', PYTHONDONTWRITEBYTECODE: '1', EMCC_CORES: '2', BINARYEN_CORES: '2'};
const reportSha256 = 'c80b778830b6b74a940eb45b8f5ef6894225b9b0b92515a376b722839ec5dffd';

function evidence() {
  const raw = fs.readFileSync(path.join(root, 'local/research/graze-fixtures.json'));
  assert.equal(createHash('sha256').update(raw).digest('hex'), reportSha256);
  const report = JSON.parse(raw.toString('utf8'));
  assert.equal(report.schema, 'th128-original-byte-graze-fixtures/1');
  assert.equal(report.executable.sha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256After, report.executable.sha256);
  assert.equal(report.executable.unchanged, true);
  for (const key of ['windowsGameProcessLaunched', 'nativeCpuTrace', 'fakeCallReturns', 'completedNativeFunction']) {
    assert.equal(report[key], false);
  }
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(createHash('sha256').update(bytes).digest('hex'), chunk.sha256);
  }
  const all = ['planCases', 'effectCases', 'positionalSoundCases', 'callerCases', 'postCallFlagCases']
    .flatMap(group => report[group]);
  assert.equal(all.length, report.caseCount);
  assert.equal(all.length, 238);
  assert.equal(new Set(all.map(sample => sample.id)).size, all.length);
  for (const sample of all) {
    assert.equal(sample.completedNativeFunction, false, sample.id);
    assert.ok(sample.instructionsExecuted < sample.instructionBudget, sample.id);
    assert.ok(['boundary', 'unsupported_call'].includes(sample.stop.reason), sample.id);
    if (sample.stop.reason === 'unsupported_call') {
      assert.ok(!sample.instructionTrace.includes(sample.stop.address), sample.id);
      assert.ok(['0x00458410', '0x00407ec0', '0x004584a0', '0x0043e090'].includes(sample.stop.target));
    }
  }
  for (const sample of report.planCases) {
    assert.equal(sample.scalarWritesComplete, true);
    assert.equal(sample.stop.reason, 'unsupported_call');
    assert.equal(sample.requestSequenceKind, 'conditional-independent-suffix-proofs');
    assert.deepEqual(sample.plannedCallWords[0], sample.firstRequestWords, sample.id);
    assert.equal(sample.plannedCallWords.length, sample.requestProofs.length);
    const kinds = sample.plannedCallWords.map(words => words[0]);
    assert.deepEqual(kinds, sample.firstThresholdSound === null ? [1, 2] : [0, 1, 2], sample.id);
    for (const [index, proof] of sample.requestProofs.entries()) {
      if (proof.source === 'own-prefix-first-stop') continue;
      const source = [...report.effectCases, ...report.positionalSoundCases].find(item => item.id === proof.source);
      assert.ok(source, sample.id);
      assert.deepEqual(sample.plannedCallWords[index], source.requestWords, sample.id);
      assert.equal(source.requiresEarlierCallsResolved, true, sample.id);
    }
  }
  for (const sample of report.callerCases) assert.equal(sample.flagsAfter, sample.input.bulletFlags);
  for (const sample of [...report.effectCases, ...report.positionalSoundCases, ...report.postCallFlagCases]) {
    assert.equal(sample.requiresEarlierCallsResolved, true, sample.id);
  }
  return report;
}

function vectors(report) {
  const records = report.planCases.map(sample => ({id: sample.id,
    input: ['plan', sample.input.grazeCount, sample.input.powerRaw, ...sample.input.positionBits].join(' '),
    expected: ['plan', sample.updatedGlobals.grazeCount, sample.updatedGlobals.powerRaw,
      sample.plannedCallWords.length, ...sample.plannedCallWords.flat()].join(' ')}));
  for (const sample of report.callerCases) records.push({id: sample.id,
    input: ['caller', sample.input.site === 'half-step' ? 0 : 1,
      sample.input.helperReturn, sample.input.bulletFlags].join(' '),
    expected: ['caller', Number(sample.grazeCallRequired), Number.parseInt(sample.callSite ?? '0'),
      Number.parseInt(sample.callTarget ?? '0'), sample.pendingFlagsSetAfterSuccess,
      Number.parseInt(sample.continuation)].join(' ')});
  for (const sample of report.postCallFlagCases) records.push({id: sample.id,
    input: ['post', sample.input.site === 'half-step' ? 0 : 1, 2, sample.input.bulletFlags].join(' '),
    expected: `post ${sample.flagsAfter}`});
  for (const sample of report.effectCases) records.push({id: sample.id,
    input: ['serialize-effect', ...sample.input.positionBits].join(' '),
    expected: ' ' + sample.requestWords.join(' ')});
  for (const sample of report.positionalSoundCases) records.push({id: sample.id,
    input: ['serialize-sound', sample.input.positionBits[0]].join(' '),
    expected: ' ' + sample.requestWords.join(' ')});
  return records;
}

test('TH128 graze fixtures preserve first unported calls and independent suffix provenance', () => {
  const report = evidence();
  assert.ok(report.planCases.some(sample => sample.firstThresholdSound === 73));
  assert.ok(report.planCases.some(sample => sample.firstThresholdSound === 74));
  for (const site of ['half-step', 'full-step']) {
    assert.ok(report.callerCases.some(sample => sample.input.site === site
      && sample.input.helperReturn === 2 && sample.input.bulletFlags === 0x17
      && sample.stop.reason === 'boundary' && !sample.grazeCallRequired));
  }
});

for (const sanitized of [false, true]) {
  test(`TH128 graze scalars, caller gates, and request serialization match 238 byte cases${sanitized ? ' with UBSan' : ''}`,
    {skip: !fs.existsSync(compiler)}, () => {
      assert.ok(fs.existsSync(path.join(root, 'src/game/GrazeRules.hpp')), 'production GrazeRules API is missing');
      assert.ok(fs.existsSync(path.join(root, 'src/game/GrazeRules.cpp')), 'production GrazeRules are missing');
      const records = vectors(evidence());
      const directory = path.resolve(root, '../../work/graze-rules-tests');
      fs.mkdirSync(directory, {recursive: true});
      const output = path.join(directory, `graze-rules${sanitized ? '-ubsan' : ''}.cjs`);
      const flags = sanitized ? ['-fsanitize=undefined', '-fno-sanitize-recover=all'] : [];
      const build = spawnSync(python, [compiler, 'tests/native/graze-rules.cpp', 'src/game/GrazeRules.cpp',
        '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fno-exceptions',
        '-fno-fast-math', '-ffp-contract=off', ...flags, '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-o', output],
      {cwd: root, encoding: 'utf8', env: environment, timeout: 180000});
      assert.ifError(build.error);
      assert.equal(build.status, 0, build.stdout + build.stderr);
      const run = spawnSync(process.execPath, [output], {cwd: root, encoding: 'utf8', timeout: 30000,
        input: records.map(record => record.input).join('\n') + '\n'});
      assert.ifError(run.error);
      assert.equal(run.status, 0, run.stdout + run.stderr);
      const lines = run.stdout.trimEnd().split(/\r?\n/);
      assert.equal(lines.length, records.length);
      for (const [index, record] of records.entries()) assert.equal(lines[index], record.expected, record.id);
    });
}
