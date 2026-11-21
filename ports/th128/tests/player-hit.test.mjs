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
const reportSha256 = '867a50b52a97e509c10f8d38569da6198862286a8d6990712086498dedf15489';

function evidence() {
  const raw = fs.readFileSync(path.join(root, 'local/research/player-hit-fixtures.json'));
  assert.equal(createHash('sha256').update(raw).digest('hex'), reportSha256);
  const report = JSON.parse(raw.toString('utf8'));
  assert.equal(report.schema, 'th128-original-byte-player-hit-fixtures/1');
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
  assert.equal(report.caseCount, 421);
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
    if (['timer-reset', 'counter-reset'].includes(sample.kind)) assert.equal(sample.requiresPriorHitEffectsResolved, true);
    if (sample.kind === 'tail') {
      assert.equal(sample.requiresPriorPlayerAnimationResolved, true);
      assert.equal(sample.completedIndependentTail, true);
      assert.equal(sample.stop.reason, 'returned');
    }
    if (sample.kind === 'normal-init') {
      assert.equal(sample.stateAfter, 1);
      assert.ok(!('hitCounter' in sample.memory));
      assert.deepEqual(Buffer.from(sample.memory.movementFields.after, 'hex'), Buffer.alloc(8));
    }
    if (sample.kind === 'entry') {
      const head = Buffer.from(sample.stackHead, 'hex');
      assert.equal(sample.registers.eax, 0);
      assert.equal(sample.registers.ecx, 0x340000);
      assert.equal(head.readInt32LE(4), 83);
      assert.equal(head.readInt32LE(8), 0);
      assert.equal(head.readInt32LE(12), 0);
      assert.deepEqual(sample.requestWords, [0, 0x43d11b, 0x4670b0, 83, 0, 1]);
    }
    if (sample.kind === 'counter-step') {
      assert.equal(sample.syntheticInactiveShotRecords, 2000);
      assert.ok(sample.instructionsExecuted < 15000);
      if (sample.input.timer[1] > 0) assert.ok(sample.instructionTrace.includes('0x0048fac0'));
    }
  }
  return report;
}

function requests(words) { return words ? [1, ...words] : [0]; }

function vectors(report) {
  return report.cases.map(sample => {
    const input = sample.input;
    let incoming = [sample.kind];
    let expected = [sample.kind];
    if (sample.kind === 'gate') {
      incoming.push(input.stateF78, input.flags438ec, input.counter438dc,
        Number(input.controlPresent), input.controlCounter5a1c);
      expected.push(sample.outcome, sample.helperReturn ?? -1,
        sample.helperReturn === null ? Number.parseInt(sample.stop.address) : 0,
        sample.helperReturn === null ? Number.parseInt(sample.stop.target) : 0);
    } else if (sample.kind === 'entry') {
      incoming.push(input.stateF78, ...input.positionBits);
      expected.push(sample.stateAfter, ...sample.deferredPositionBits, ...requests(sample.requestWords));
    } else if (['timer-reset', 'counter-reset'].includes(sample.kind)) {
      incoming.push(...input.timer);
      if (sample.kind === 'timer-reset') incoming.push(Number(input.soundControlPresent), input.soundControlFlags60);
      expected.push(...sample.timerAfter, ...requests(sample.requestWords),
        sample.kind === 'timer-reset' ? 0x43d2d1 : 0x43d331);
    } else if (sample.kind === 'window') {
      incoming.push(input.stateF78, ...input.stateTimer, ...input.heldTimer, Number(input.controlPresent),
        input.controlMode3c, input.perfectCharge4b4d70, input.keys4db210);
      expected.push(sample.outcome, ...sample.stateTimerAfter, ...sample.heldTimerAfter,
        ...requests(sample.requestWords), sample.stop.reason === 'unsupported_call'
          ? Number.parseInt(sample.stop.address) + 5 : 0x43b9ee);
    } else if (sample.kind === 'normal-init') {
      incoming.push(...input.stateTimer, ...input.focusTimer, ...input.rechargeTimer, input.flags438ec);
      expected.push(sample.stateAfter, sample.flagsAfter, ...sample.stateTimerAfter,
        ...sample.focusTimerAfter, ...sample.rechargeTimerAfter, 0, 0, ...requests(sample.requestWords));
    } else if (sample.kind === 'constructor-counter' || sample.kind === 'counter-step') {
      incoming.push(...input.timer);
      if (sample.kind === 'counter-step') incoming.push(input.clockBits);
      expected.push(...sample.timerAfter);
    } else {
      assert.equal(sample.kind, 'tail');
      incoming.push(input.flags7c, input.counter28, input.value80, Number(input.controlPresent), input.controlMode3c);
      expected.push(sample.flagsAfter, input.counter28, sample.value80After);
    }
    return {id: sample.id, input: incoming.join(' '), expected: expected.join(' ')};
  });
}

test('TH128 player-hit fixture stages preserve unexecuted calls and independent suffix returns', () => {
  const report = evidence();
  assert.equal(Buffer.from(report.constants['0x0043c1bc'].bytes, 'hex').readUInt32LE(16), 0x43b843);
  assert.equal(Buffer.from(report.constants['0x004a6774'].bytes, 'hex').readUInt32LE(), 0x42f00000);
  assert.ok(report.cases.some(sample => sample.kind === 'entry' && sample.stateAfter === 4));
  assert.ok(report.cases.some(sample => sample.kind === 'counter-reset' && sample.timerAfter[1] === 6));
  for (const outcome of [0, 1, 2, 3]) {
    assert.ok(report.cases.some(sample => sample.kind === 'window' && sample.outcome === outcome));
  }
});

for (const sanitized of [false, true]) {
  test(`TH128 player-hit state prefixes replay 421 original-byte cases${sanitized ? ' with UBSan' : ''}`,
    {skip: !fs.existsSync(compiler)}, () => {
      assert.ok(fs.existsSync(path.join(root, 'src/game/PlayerHitRules.hpp')), 'production PlayerHitRules API is missing');
      assert.ok(fs.existsSync(path.join(root, 'src/game/PlayerHitRules.cpp')), 'production PlayerHitRules are missing');
      const header = fs.readFileSync(path.join(root, 'src/game/PlayerHitRules.hpp'), 'utf8');
      assert.match(header, /normalInitPrefix\s*\(/, 'bounded normal init API is missing');
      assert.match(header, /counterStep\s*\(/, 'ordinary counter-step API is missing');
      assert.match(header, /constructorCounterPrefix\s*\(/, 'explicit constructor counter prefix is missing');
      const records = vectors(evidence());
      const directory = path.resolve(root, '../../work/player-hit-tests');
      fs.mkdirSync(directory, {recursive: true});
      const output = path.join(directory, `player-hit${sanitized ? '-ubsan' : ''}.cjs`);
      const flags = sanitized ? ['-fsanitize=undefined', '-fno-sanitize-recover=all'] : [];
      const build = spawnSync(python, [compiler, 'tests/native/player-hit.cpp', 'src/game/PlayerHitRules.cpp',
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
