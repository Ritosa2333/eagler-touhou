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
  EMSDK_PYTHON: python, PYTHONUTF8: '1', EMCC_CORES: '2', BINARYEN_CORES: '2'};
const reportSha256 = 'a034fcc2db9b9ecf07f32d26ed24daaa20c2d241c79b0ad44d19defd1a86cc9b';
const outcomes = {'miss': 0, 'suppressed': 1, 'hit-return': 2, 'graze-return': 3, 'damage-call-required': 4};
const routes = {'none': 0, 'rectangle': 1, 'circle': 2};
const intents = {'no-helper': 0, 'helper-returned': 1, 'damage-call-required': 2,
  'hit-effects-required': 3, 'graze-call-required': 4};
const geometries = {'miss': 0, 'potential-hit': 1, 'potential-graze': 2};

function evidence() {
  const raw = fs.readFileSync(path.join(root, 'local/research/bullet-collision-fixtures.json'));
  assert.equal(createHash('sha256').update(raw).digest('hex'), reportSha256);
  const report = JSON.parse(raw.toString('utf8'));
  assert.equal(report.schema, 'th128-original-byte-bullet-collision-fixtures/1');
  assert.equal(report.executable.sha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256After, report.executable.sha256);
  assert.equal(report.executable.unchanged, true);
  for (const key of ['nativeCpuTrace', 'windowsGameProcessLaunched', 'fakeCallReturns', 'completedNativeFrame']) {
    assert.equal(report[key], false);
  }
  assert.equal(report.caseCount, 270);
  assert.equal(report.cases.length, report.caseCount);
  assert.equal(new Set(report.cases.map(sample => sample.id)).size, report.caseCount);
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(createHash('sha256').update(bytes).digest('hex'), chunk.sha256);
  }
  for (const constant of Object.values(report.constants)) {
    assert.equal(createHash('sha256').update(Buffer.from(constant.bytes, 'hex')).digest('hex'), constant.sha256);
  }
  for (const sample of report.cases) {
    assert.equal(sample.completedTick, false, sample.id);
    assert.ok(sample.instructionsExecuted < sample.instructionBudget, sample.id);
    assert.ok(['returned', 'boundary', 'unsupported_call'].includes(sample.stop.reason), sample.id);
    if (sample.stop.reason === 'unsupported_call') {
      assert.ok(['0x0043d0c0', '0x0043e090', '0x0040d2a0'].includes(sample.stop.target), sample.id);
      assert.ok(!sample.instructionTrace.includes(sample.stop.address), sample.id);
    }
    if (sample.kind !== 'update-prefix' && sample.stop.reason !== 'returned') {
      assert.equal(sample.helperReturn, null, sample.id);
    }
    if (sample.stop.reason === 'returned') {
      assert.equal(sample.stackDelta, sample.kind === 'rectangle' ? 4 : 8, sample.id);
    }
    for (const measurement of Object.values(sample.measurements ?? {})) {
      assert.equal(measurement.stop.reason, 'boundary', sample.id);
      assert.ok(measurement.instructionsExecuted < measurement.instructionBudget, sample.id);
    }
    if (sample.kind === 'update-prefix') assert.equal(sample.flagsAfter, sample.input.bulletFlags, sample.id);
  }
  const circleSources = report.cases.filter(sample => sample.kind === 'circle' || sample.route === 'circle');
  assert.equal(report.geometryCaseCount, circleSources.length);
  assert.equal(report.geometryCases.length, circleSources.length);
  assert.deepEqual(report.geometryCases.map(sample => sample.sourceCaseId), circleSources.map(sample => sample.id));
  for (const [index, sample] of report.geometryCases.entries()) {
    assert.equal(sample.hitGatesRead, false, sample.id);
    assert.equal(sample.completedTick, false, sample.id);
    assert.ok(sample.instructionsExecuted < sample.instructionBudget, sample.id);
    assert.ok(sample.instructionTrace.every(location => Number.parseInt(location) < 0x43c608), sample.id);
    assert.deepEqual(Object.keys(sample.memory).sort(), ['center', 'playerXY', 'profilePointer',
      'profileRadius', 'playerPointer', '0x004a6910', '0x004a6a00'].sort(), sample.id);
    assert.ok(Object.values(sample.memory).every(region => !region.writable), sample.id);
    const source = circleSources[index];
    const derived = source.instructionTrace.includes('0x0043c608') ? 'potential-hit'
      : source.instructionTrace.includes('0x0043c5fd') ? 'potential-graze' : 'miss';
    assert.equal(sample.geometry, derived, sample.id);
    if (sample.geometry === 'potential-hit') {
      assert.deepEqual(sample.stop, {reason: 'boundary', address: '0x0043c608'}, sample.id);
      assert.equal(sample.helperReturn, null, sample.id);
    } else {
      assert.equal(sample.stop.reason, 'returned', sample.id);
      assert.equal(sample.helperReturn, sample.geometry === 'potential-graze' ? 2 : 0, sample.id);
    }
  }
  return report;
}

function baseInput(sample) {
  const input = sample.input;
  return [sample.kind, ...input.centerBits, ...input.sizeBits, ...input.playerPositionBits,
    ...input.boundsBits, input.playerRadiusBits, input.stateF78, input.flags438ec, input.counter438dc,
    Number(input.controlPresent), input.controlCounter5a1c];
}

function decision(sample, name) {
  const requiresDamage = name === 'damage-call-required';
  return [outcomes[name], sample.helperReturn ?? -1,
    requiresDamage ? Number.parseInt(sample.stop.address) : 0,
    requiresDamage ? Number.parseInt(sample.stop.target) : 0];
}

function vectors(report) {
  const complete = report.cases.map(sample => {
    const input = baseInput(sample);
    let expected = [sample.kind];
    if (sample.kind === 'rectangle') {
      expected.push(...decision(sample, sample.decision), ...sample.hitBoundsBits,
        Number(sample.grazeBoundsBits !== undefined), ...(sample.grazeBoundsBits ?? []));
    } else if (sample.kind === 'circle') {
      expected.push(...decision(sample, sample.decision), sample.distanceSquaredBits,
        Number(sample.grazeSizeBits !== undefined));
      if (sample.grazeSizeBits !== undefined) expected.push(sample.dividedGrazeSizeBits, sample.grazeSizeBits);
    } else {
      input.push(sample.input.site === 'half-step' ? 0 : 1, sample.input.bulletFlags,
        sample.input.age1188, sample.input.scale1180Bits);
      expected.push(routes[sample.route], intents[sample.intent], Number(sample.route !== 'none'));
      if (sample.route !== 'none') expected.push(...decision(sample, sample.helperDecision));
      expected.push(sample.stop.reason === 'unsupported_call' ? Number.parseInt(sample.stop.address) : 0,
        sample.stop.reason === 'unsupported_call' ? Number.parseInt(sample.stop.target) : 0,
        Number(sample.effectiveSizeBits !== undefined), ...(sample.effectiveSizeBits ?? []),
        sample.intent === 'hit-effects-required' ? sample.motionStateAfter : -1,
        sample.intent === 'graze-call-required' ? 4 : 0);
    }
    return {id: sample.id, input: input.join(' '), expected: expected.join(' ')};
  });
  const geometryOnly = report.geometryCases.map(sample => ({id: sample.id,
    input: ['circle-geometry', ...sample.input.centerBits, sample.input.hitboxSizeBits,
      ...sample.input.playerPositionBits, sample.input.playerRadiusBits].join(' '),
    expected: `circle-geometry ${geometries[sample.geometry]}`}));
  return complete.concat(geometryOnly);
}

test('TH128 collision fixtures prove helper and actual update bytes without invented call returns', () => {
  const report = evidence();
  assert.equal(report.type8Evidence.stop.reason, 'boundary');
  assert.equal(report.type8Evidence.flagsAfter & 0x10, 0x10);
  assert.equal(report.type8Evidence.hitboxSizeBytes, '9a991940');
  assert.equal(report.type8Evidence.completedEmission, false);
  assert.ok(report.cases.some(sample => sample.instructionTrace.includes('0x004087b4')));
  assert.ok(report.cases.some(sample => sample.instructionTrace.includes('0x0040880f')));
  assert.ok(report.cases.some(sample => sample.instructionTrace.includes('0x00408a44')));
  assert.ok(report.cases.some(sample => sample.instructionTrace.includes('0x00408a9f')));
  for (const code of [0, 1, 2]) assert.ok(report.cases.some(sample => sample.helperReturn === code));
  const retained = report.cases.filter(sample => sample.id.startsWith('circle-retained-x87-'));
  assert.deepEqual(retained.map(sample => sample.distanceSquaredBits), [0x4f800000, 0x4f800000]);
  assert.deepEqual(retained.map(sample => sample.helperReturn), [2, 1]);
  const precomputed = report.cases.filter(sample => sample.id.startsWith('rect-precomputed-bounds-'));
  const actualXY = report.cases.filter(sample => sample.id.startsWith('circle-actual-position-'));
  assert.deepEqual(precomputed.map(sample => sample.helperReturn), [1, 0]);
  assert.deepEqual(actualXY.map(sample => sample.helperReturn), [0, 1]);
});

for (const sanitized of [false, true]) {
  test(`TH128 pure collision decisions and gate-free geometry replay original-byte cases${sanitized ? ' with UBSan' : ''}`,
    {skip: !fs.existsSync(compiler)}, () => {
      assert.ok(fs.existsSync(path.join(root, 'src/game/BulletCollisionRules.hpp')),
        'production pure collision API is not implemented');
      assert.ok(fs.existsSync(path.join(root, 'src/game/BulletCollisionRules.cpp')),
        'production pure collision rules are not implemented');
      assert.match(fs.readFileSync(path.join(root, 'src/game/BulletCollisionRules.hpp'), 'utf8'),
        /circleGeometry\s*\(/, 'gate-free geometry API is not implemented');
      const records = vectors(evidence());
      const edges=JSON.parse(fs.readFileSync(path.join(root,'local/research/bullet-collision-edge-fixtures.json')));
      for(const [index,sample] of edges.cases.entries())records.push({id:`tiny-coordinate-${index}`,
        input:`circle-geometry ${sample.input.join(' ')}`,expected:`circle-geometry ${sample.geometry}`});
      const directory = path.resolve(root, '../../work/bullet-collision-tests');
      fs.mkdirSync(directory, {recursive: true});
      const output = path.join(directory, `bullet-collision${sanitized ? '-ubsan' : ''}.cjs`);
      const flags = sanitized ? ['-fsanitize=undefined', '-fno-sanitize-recover=all'] : [];
      const build = spawnSync(python, [compiler, 'tests/native/bullet-collision.cpp',
        'src/game/BulletCollisionRules.cpp', '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror',
        '-fno-fast-math', '-ffp-contract=off', '-fno-exceptions', ...flags,
        '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-o', output],
      {cwd: root, encoding: 'utf8', env: environment, timeout: 180000});
      assert.ifError(build.error);
      assert.equal(build.status, 0, build.stdout + build.stderr);
      const run = spawnSync(process.execPath, [output], {cwd: root, encoding: 'utf8', timeout: 30000,
        input: records.map(record => record.input).join('\n') + '\n'});
      assert.ifError(run.error);
      assert.equal(run.status, 0, run.stdout + run.stderr);
      const lines = run.stdout.trim().split(/\r?\n/);
      assert.equal(lines.length, records.length);
      for (const [index, record] of records.entries()) assert.equal(lines[index], record.expected, record.id);
    });
}
