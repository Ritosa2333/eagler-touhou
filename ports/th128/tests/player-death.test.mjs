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
const reportSha256 = 'b15a3ded92a89cb5517711d1f69fec11af38e5be89575b9a7491633abab02a39';

function evidence() {
  const raw = fs.readFileSync(path.join(root, 'local/research/player-death-fixtures.json'));
  assert.equal(createHash('sha256').update(raw).digest('hex'), reportSha256);
  const report = JSON.parse(raw.toString('utf8'));
  assert.equal(report.schema, 'th128-original-byte-player-death-fixtures/1');
  assert.equal(report.executable.sha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256After, report.executable.sha256);
  assert.equal(report.executable.unchanged, true);
  for (const key of ['nativeCpuTrace', 'windowsGameProcessLaunched', 'fakeCallReturns', 'completedNativeFunction']) {
    assert.equal(report[key], false);
  }
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(createHash('sha256').update(bytes).digest('hex'), chunk.sha256);
  }
  for (const constant of Object.values(report.constants)) {
    assert.equal(createHash('sha256').update(Buffer.from(constant.bytes, 'hex')).digest('hex'), constant.sha256);
  }
  assert.equal(report.caseCount, 607);
  assert.equal(report.cases.length, report.caseCount);
  assert.equal(new Set(report.cases.map(sample => sample.id)).size, report.caseCount);
  for (const sample of report.cases) {
    assert.equal(sample.completedNativeFunction, false, sample.id);
    assert.ok(sample.instructionsExecuted < sample.instructionBudget, sample.id);
    assert.ok(['returned', 'boundary', 'unsupported_call'].includes(sample.stop.reason), sample.id);
    if (sample.stop.reason === 'unsupported_call') {
      assert.ok(!sample.instructionTrace.includes(sample.stop.address), sample.id);
      if (sample.requestWords) {
        assert.equal(sample.requestWords[1], Number.parseInt(sample.stop.address), sample.id);
        assert.equal(sample.requestWords[2], Number.parseInt(sample.stop.target), sample.id);
      }
    }
    if (['after-awards', 'state-two', 'cleanup-record', 'tail', 'charge-suffix', 'after-field',
      'respawn-position'].includes(sample.kind)) assert.equal(sample.requiresEarlierCallsResolved, true, sample.id);
    if (sample.kind === 'motivation') {
      assert.equal(sample.completedIndependentHelper, true);
      assert.equal(sample.stop.reason, 'returned');
    }
    if (sample.kind === 'tail') {
      assert.equal(sample.completedIndependentTail, true);
      assert.equal(sample.stop.reason, 'returned');
    }
    if (sample.kind === 'respawn-position') assert.ok(sample.instructionTrace.includes('0x00403280'));
  }
  return report;
}

function request(words) { return words ? [1, ...words] : [0]; }

function vectors(report) {
  const suffixes = [0, 1].map(stage => report.cases.find(sample => sample.kind === 'charge-suffix'
    && sample.input.stage === stage).requestWords);
  return report.cases.map(sample => {
    const input = sample.input;
    const incoming = [sample.kind];
    const expected = [sample.kind];
    if (sample.kind === 'commit-call' || sample.kind === 'charge-suffix') {
      incoming.push(input.stage);
      expected.push(...sample.requestWords);
    } else if (sample.kind === 'motivation') {
      incoming.push(input.value4b4d64, input.flag4b4d68);
      expected.push(sample.valueAfter, sample.flagAfter);
    } else if (sample.kind === 'charge') {
      incoming.push(input.perfectCharge4b4d70);
      const planned = sample.soundRequired ? [sample.requestWords, ...suffixes] : [];
      expected.push(sample.valueAfter, planned.length, ...planned.flat(), Number(sample.completedIndependentHelper));
    } else if (sample.kind === 'after-awards') {
      incoming.push(input.selector4b4cfc, input.value4b4d00);
      expected.push(sample.powerAfter, ...request(sample.requestWords), sample.continuation);
    } else if (sample.kind === 'state-two') {
      incoming.push(...input.stateTimer, ...input.protectionTimer);
      expected.push(sample.stateAfter, ...sample.stateTimerAfter, ...sample.protectionTimerAfter, ...sample.requestWords);
    } else if (sample.kind === 'cleanup-record') {
      incoming.push(input.active, input.primary, input.secondary, Number(input.afterPrimary));
      expected.push(sample.activeAfter, sample.requiredHandle, sample.phase, sample.lookupInstruction, sample.continuation);
    } else if (sample.kind === 'tail') {
      incoming.push(input.flags7c, input.counter28, input.value80, Number(input.controlPresent),
        input.mode3c, input.scalar4b4d30);
      expected.push(sample.flagsAfter, sample.value80After, sample.scalar30After, sample.field438f8After);
    } else if (sample.kind === 'respawn-gate') {
      incoming.push(input.stateF78, input.elapsedF84, input.resource4b4d64, Number(input.scenePresent),
        input.sceneMode10, ...input.positionBits);
      expected.push(sample.outcome, sample.stateAfter, Number(sample.outcome === 3));
      if (sample.outcome === 3) expected.push(sample.clockAfterBits);
      expected.push(...request(sample.requestWords), sample.continuation);
    } else if (sample.kind === 'after-field') {
      incoming.push(input.difficulty4b4d04);
      expected.push(...request(sample.requestWords), sample.continuation);
    } else {
      assert.equal(sample.kind, 'respawn-position');
      incoming.push(...input.positionBits, ...input.protectionTimer, ...input.stateTimer);
      expected.push(...sample.positionAfterBits, ...sample.savedPositionBits, ...sample.rawAfter,
        ...sample.protectionTimerAfter, ...sample.stateTimerAfter);
    }
    return {id: sample.id, input: incoming.join(' '), expected: expected.join(' ')};
  });
}

test('TH128 death evidence separates actual helper returns from unexecuted system and ANM calls', () => {
  const report = evidence();
  assert.equal(report.cases.find(sample => sample.id === 'commit-call-0').argument, -10000);
  assert.equal(report.cases.find(sample => sample.id === 'commit-call-1').argument, 2500);
  assert.equal(report.cases.find(sample => sample.id === 'charge--10000').soundRequired, true);
  assert.equal(Buffer.from(report.constants['0x0043c1bc'].bytes, 'hex').readUInt32LE(8), 0x43b862);
});

for (const sanitized of [false, true]) {
  test(`TH128 death prefixes and required request plans replay 607 original-byte cases${sanitized ? ' with UBSan' : ''}`,
    {skip: !fs.existsSync(compiler)}, () => {
      assert.ok(fs.existsSync(path.join(root, 'src/game/PlayerDeathRules.hpp')), 'production PlayerDeathRules API is missing');
      assert.ok(fs.existsSync(path.join(root, 'src/game/PlayerDeathRules.cpp')), 'production PlayerDeathRules are missing');
      const records = vectors(evidence());
      const directory = path.resolve(root, '../../work/player-death-tests');
      fs.mkdirSync(directory, {recursive: true});
      const output = path.join(directory, `player-death${sanitized ? '-ubsan' : ''}.cjs`);
      const flags = sanitized ? ['-fsanitize=undefined', '-fno-sanitize-recover=all'] : [];
      const build = spawnSync(python, [compiler, 'tests/native/player-death.cpp', 'src/game/PlayerDeathRules.cpp',
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
