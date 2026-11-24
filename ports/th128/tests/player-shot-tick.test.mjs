import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const work = path.resolve(root, '../../work/player-shot-tick-test');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');
const digest = value => createHash('sha256').update(value).digest('hex');

function evidence() {
  const raw = fs.readFileSync(path.join(root, 'local/research/player-shot-tick-fixtures.json'));
  assert.equal(digest(raw), 'e9caf088669545df700333156536be424e0b03b206afeaa049692e041cb69727');
  const report = JSON.parse(raw.toString('utf8'));
  assert.equal(report.schema, 'th128-original-byte-player-shot-tick-fixtures/1');
  assert.equal(report.executable.sha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256After, report.executable.sha256);
  assert.equal(report.executable.unchanged, true);
  assert.equal(report.nativeCpuTrace, false);
  assert.equal(report.windowsGameProcessLaunched, false);
  assert.equal(report.resource.sha256, 'aed90a45b565510e97cc7d9ab4846d8c93914e57faafe9339e9ab03d47b1511b');
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(digest(bytes), chunk.sha256);
  }
  for (const constant of Object.values(report.constants))
    assert.equal(digest(Buffer.from(constant.bytes, 'hex')), constant.sha256);
  assert.equal(digest(fs.readFileSync(path.join(root, 'tools/research/player_shot_tick_probe.py'))), report.generatorSha256);
  for (const [name, hash] of Object.entries(report.engineSha256))
    assert.equal(digest(fs.readFileSync(path.join(root, 'tools/research', name))), hash);
  return report;
}

test('production ordinary-shot tick exposes independent state and required-call plans', () => {
  for (const file of ['src/game/PlayerShotTickRules.hpp', 'src/game/PlayerShotTickRules.cpp'])
    assert.ok(fs.existsSync(path.join(root, file)), `${file}: player-shot tick rules are missing`);
});

test('production mode1 API does not substitute axis-aligned corners', () => {
  const header = fs.readFileSync(path.join(root, 'src/game/PlayerShotTickRules.hpp'), 'utf8');
  assert.ok(header.includes('rotatedShotAnimationBounds'), 'real mode1 corner API is missing');
});

test('ordinary native pass really returns on misses; callbacks and motion remain mandatory', () => {
  const report = evidence();
  assert.equal(report.caseCount, 59);
  for (const sample of report.returnedPasses) {
    assert.equal(sample.stop.reason, 'returned');
    assert.equal(sample.stackDelta, 8);
    assert.ok(sample.traceSummary.visits['0x00472905']);
  }
  const [callback, motion] = report.requiredPrefixes;
  assert.deepEqual(callback.stop, {reason: 'unsupported_call', address: '0x0043f7ae', target: 'eax'});
  assert.equal(callback.callbackTarget, 0x720000);
  assert.equal(motion.stop.target, '0x0046c1f0');
  for (const sample of report.integratorCases) {
    assert.equal(sample.stop.reason, 'returned');
    assert.ok(sample.traceSummary.visits['0x0048fe30']);
  }
  for (const sample of [...report.integratorCases, ...report.axisBoundsCases, ...report.movementCases]) {
    assert.equal(sample.cpuEnvironment.mxcsr, 0x1f80);
    assert.ok(sample.instructionsExecuted < sample.instructionBudget);
  }
});

test('bounded ordinary-shot state and required metadata match original-byte WASM and UBSan goldens',
  {skip: !fs.existsSync(compiler)}, () => {
    const report = evidence();
    const input = [], expected = [];
    const velocity = image => {
      const bytes = Buffer.from(image, 'hex');
      return [0x20, 0x24, 0x28].map(offset => bytes.readUInt32LE(offset));
    };
    for (const sample of report.movementCases) {
      input.push(['M', sample.before, ...velocity(sample.next)].join(' '));
      expected.push(sample.next);
    }
    for (const sample of report.returnedPasses) {
      input.push(['R', sample.before, ...velocity(sample.next)].join(' '));
      expected.push(sample.next);
    }
    for (const sample of report.integratorCases) {
      input.push(['I', sample.before, ...velocity(sample.before)].join(' '));
      expected.push(sample.next);
    }
    for (const sample of report.ageCases) {
      input.push(`A ${sample.before}`);
      expected.push(sample.next);
    }
    for (const sample of report.boundsCases) {
      input.push(['B', sample.age, ...sample.cornerBits.flat()].join(' '));
      expected.push(String(Number(sample.survivor)));
    }
    for (const sample of report.axisBoundsCases) {
      input.push(['X', ...sample.positionBits, ...sample.spriteBits, ...sample.scaleBits, sample.flags].join(' '));
      expected.push(sample.cornerBits.flat().join(' '));
    }
    const edges=JSON.parse(fs.readFileSync(path.join(root,'local/research/shot-bounds-edge-fixtures.json')));
    for (const sample of [...report.rotatedBoundsCases,...edges.cases]) {
      assert.equal(sample.stop.reason, 'returned');
      assert.equal(sample.basisEvidence.stop.reason, 'returned');
      input.push(['Y', ...sample.positionBits, ...sample.spriteBits, ...sample.scaleBits, sample.flags,
        ...sample.basisEvidence.cosineSineBits].join(' '));
      expected.push(sample.cornerBits.flat().join(' '));
    }
    for (const sample of report.requiredPrefixes) {
      input.push(['P', sample.before, sample.callbackTarget].join(' '));
      expected.push([Number.parseInt(sample.stop.address), sample.callbackTarget || 0x46c1f0, sample.next].join(' '));
    }
    for (const sample of report.syncCases) {
      input.push(`S ${sample.before}`);
      expected.push(sample.animationPositionBits.join(' '));
    }
    fs.mkdirSync(work, {recursive: true});
    const temporary = path.join(work, 'temp');
    fs.mkdirSync(temporary, {recursive: true});
    const cache = path.join(work, 'emscripten-cache');
    if (!fs.existsSync(cache))
      fs.cpSync(path.resolve(root, '../../work/emscripten-cache-4.0.22'), cache,
        {recursive: true, filter: source => !path.basename(source).endsWith('.lock')});
    const environment = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
      EM_CACHE: cache.replaceAll('\\', '/'), EMSDK_PYTHON: python, PYTHONUTF8: '1',
      EMCC_CORES: '2', BINARYEN_CORES: '2', TEMP: temporary, TMP: temporary, TMPDIR: temporary};
    for (const sanitized of [false, true]) {
      const output = path.join(work, `player-shot-tick${sanitized ? '-ubsan' : ''}.cjs`);
      const build = spawnSync(python, [compiler, 'tests/native/player-shot-tick.cpp', 'src/game/PlayerShotTickRules.cpp',
        '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fno-exceptions', '-fno-fast-math',
        '-ffp-contract=off', ...(sanitized ? ['-fsanitize=undefined', '-fno-sanitize-recover=all'] : []),
        '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-o', output],
      {cwd: root, env: environment, encoding: 'utf8', timeout: 180000});
      assert.ifError(build.error);
      assert.equal(build.status, 0, build.stdout + build.stderr);
      const run = spawnSync(process.execPath, [output], {cwd: root, input: input.join('\n') + '\n',
        encoding: 'utf8', timeout: 30000, maxBuffer: 4 * 1024 * 1024});
      assert.ifError(run.error);
      assert.equal(run.status, 0, run.stderr);
      const actual = run.stdout.trim().split(/\r?\n/);
      assert.equal(actual.length, expected.length);
      actual.forEach((line, index) => assert.equal(line, expected[index], `native tick vector ${index}`));
    }
  });
