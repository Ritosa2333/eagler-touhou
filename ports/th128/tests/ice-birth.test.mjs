import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const work = path.resolve(root, '../../work/ice-birth-test');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');
const digest = bytes => createHash('sha256').update(bytes).digest('hex');
const bits = value => { const bytes = Buffer.alloc(4); bytes.writeFloatLE(value); return bytes.readUInt32LE(); };
const record = state => [state.occupied, ...state.positionBits, state.valueBits, ...state.lifetime,
  ...state.delay, state.state48, state.mode, state.animation50, state.animation54, state.maskSelector];

function evidence() {
  const raw = fs.readFileSync(path.join(root, 'local/research/ice-birth-fixtures.json'));
  assert.equal(digest(raw), 'eca430530642bc9bc3d73458b09b6f27bde0986b25ad298fbb6a87bfb2d3abdd');
  const report = JSON.parse(raw.toString('utf8'));
  assert.equal(report.schema, 'th128-original-byte-ice-birth-fixtures/1');
  assert.equal(report.executable.sha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256After, report.executable.sha256);
  assert.equal(report.executable.unchanged, true);
  assert.equal(report.nativeCpuTrace, false);
  assert.equal(report.windowsGameProcessLaunched, false);
  assert.equal(report.caseCount, 52);
  assert.equal(report.animation.nativeResourceSlot, 36);
  assert.equal(report.animation.activationLayer, 23);
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(digest(bytes), chunk.sha256);
  }
  for (const constant of Object.values(report.constants))
    assert.equal(digest(Buffer.from(constant.bytes, 'hex')), constant.sha256);
  assert.equal(digest(fs.readFileSync(path.join(root, 'tools/research/ice_birth_probe.py'))), report.generatorSha256);
  for (const [name, expected] of Object.entries(report.engineSha256))
    assert.equal(digest(fs.readFileSync(path.join(root, 'tools/research', name))), expected);
  return report;
}

test('production ice birth API provides real state and mandatory list/effect plans', () => {
  for (const file of ['src/game/IceBirthRules.hpp', 'src/game/IceBirthRules.cpp'])
    assert.ok(fs.existsSync(path.join(root, file)), `${file}: bounded ice birth is not implemented`);
});

test('actual builders return only after both real list insertions; prefixes remain incomplete', () => {
  const report = evidence();
  for (const sample of report.cases) {
    assert.ok(sample.instructionsExecuted < sample.instructionBudget, sample.id);
    const selection = sample.selection;
    assert.equal(selection.examined, Math.min(sample.input.occupiedRun + 1, 4000));
    assert.equal(selection.slot, (sample.input.cursor + selection.examined - 1) % 4000);
    if (sample.completedBuilder) {
      assert.equal(sample.stop.reason, 'returned');
      assert.equal(sample.stackDelta, 20);
      const object = 0x300000 + 0x10c + selection.slot * 0x68;
      assert.equal(sample.registers.eax, object);
      assert.equal(sample.listState.bucketFirst, object);
      assert.equal(sample.listState.globalFirst, object + 0x5c);
      const image = Buffer.from(sample.objectAfter, 'hex');
      assert.equal(image.readUInt32LE(0), object);
      assert.equal(image.readUInt32LE(4), sample.input.nonempty ? 0x700000 : 0);
      assert.equal(image.readUInt32LE(8), 0x300000 + 0x65a1c + sample.observedBucket * 12);
      assert.equal(image.readUInt32LE(0x5c), object);
      assert.equal(image.readUInt32LE(0x60), sample.input.nonempty ? 0x710000 : 0);
      assert.equal(image.readUInt32LE(0x64), 0x300000 + 0x65a10);
      if (sample.input.nonempty) {
        assert.equal(sample.listState.oldBucketPrevious, object);
        assert.equal(sample.listState.oldGlobalPrevious, object + 0x5c);
      }
      assert.ok(sample.instructionTrace.includes('0x004106dd'));
    } else {
      assert.equal(sample.stop.reason, 'boundary');
    }
  }
  assert.equal(report.cases.find(sample => sample.id === 'full-pool-selection').selection.overwritesOccupied, true);
  const tinyValid = report.cases.find(sample => sample.id === 'review-negative-tiny-valid-bucket');
  const tinyInvalid = report.cases.find(sample => sample.id === 'review-negative-tiny-invalid-bucket');
  assert.equal(tinyValid.observedBucket, 38);
  assert.equal(tinyValid.portableAccepted, true);
  assert.equal(tinyInvalid.observedBucket, -10);
  assert.equal(tinyInvalid.completedBuilder, false);
  assert.deepEqual(tinyInvalid.stop, {reason: 'boundary', address: '0x0041069c'});
  assert.equal(tinyInvalid.portableAccepted, false);
  const [effect, sound] = report.requiredCallPrefixes;
  const loader = Buffer.from(report.code['0x40e825:0x40e83f'].bytes, 'hex');
  assert.equal(loader.readUInt32LE(1), 0x4a2200);
  assert.equal(loader.readUInt32LE(6), 36);
  assert.equal(Buffer.from(report.constants['0x004a2200'].bytes, 'hex').toString(), 'ice.anm\0');
  assert.equal(effect.stop.target, '0x00407ec0');
  assert.equal(effect.registers.eax, 23);
  assert.equal(effect.script, 33);
  assert.equal(sound.stop.target, '0x004584a0');
  assert.equal(sound.registers.eax, 47);
  const activationBytes = Buffer.from(report.code['0x410208:0x4102e1'].bytes, 'hex');
  assert.equal(activationBytes.subarray(0x410283 - 0x410208, 0x410286 - 0x410208).toString('hex'), '894e50');
  assert.equal(activationBytes.readUInt32LE(0x410288 - 0x410208), 0x6b218);
  assert.equal(activationBytes.readUInt32LE(0x41028c - 0x410208), 1);
  const wrapper = report.code['0x407f60:0x407fa4'];
  const wrapperBytes = Buffer.from(wrapper.bytes, 'hex');
  assert.equal(0x407f70 + 5 + wrapperBytes.readInt32LE(0x407f71 - 0x407f60), 0x466d60);
  const registration = Buffer.from(report.code['0x466d60:0x466ddc'].bytes, 'hex');
  assert.equal(registration.readUInt32LE(0x466d78 - 0x466d60), 0x8b9704);
  assert.equal(registration.readUInt32LE(0x466da8 - 0x466d60), 0x8b9708);
  for (const sample of report.activationCases) {
    assert.equal(sample.stop.reason, 'unsupported_call');
    assert.equal(sample.stop.target, '0x00410590');
    assert.equal(sample.builderArguments.duration, Math.trunc(sample.nativeFrozenPower / 100));
    assert.equal(sample.builderArguments.valueBits, bits(50));
    assert.equal(sample.completedActivation, false);
  }
});

test('portable first-ice state and required plans match original-byte goldens in WASM and UBSan',
  {skip: !fs.existsSync(compiler)}, () => {
    assert.ok(fs.existsSync(path.join(root, 'src/game/IceBirthRules.hpp')), 'ice birth API is not implemented');
    const report = evidence();
    const catalog = JSON.parse(fs.readFileSync(path.join(root, 'local/catalog.json'), 'utf8'));
    const manifest = path.resolve(root, 'local', catalog.variants.ja.manifest);
    const source = path.join(path.dirname(manifest), 'raw/ice.anm');
    assert.ok(source.startsWith(path.join(root, 'local') + path.sep));
    assert.equal(digest(fs.readFileSync(source)), report.animation.sha256);
    const inputs = [], expected = [];
    for (const sample of report.cases) {
      const value = sample.input, selection = sample.selection;
      if (value.stage === 'selection') {
        inputs.push(['S', value.cursor, value.occupiedRun].join(' '));
        expected.push([selection.slot, selection.nextCursor, selection.examined, Number(selection.overwritesOccupied)].join(' '));
      } else {
        inputs.push([sample.portableAccepted ? 'B' : 'U', value.cursor, value.occupiedRun, ...value.positionBits, bits(value.value),
          value.duration, value.mode, ...record(sample.before)].join(' '));
        expected.push(sample.portableAccepted ? [selection.slot, selection.nextCursor, selection.examined, Number(selection.overwritesOccupied),
          ...record(sample.next), 0, 0x65a1c + sample.observedBucket * 12, 0, 0x65a10, 0x5c].join(' ') :
          [selection.slot, selection.nextCursor, selection.examined, Number(selection.overwritesOccupied), 'unsupported'].join(' '));
      }
    }
    for (const sample of report.activationCases) {
      inputs.push(['A', sample.nativeFrozenPower, ...sample.positionBits].join(' '));
      expected.push([bits(50), sample.builderArguments.duration, 1, sample.nativeFrozenPower, 0x4102c9, ...sample.positionBits,
        0, 0x410276, 0x407ec0, 23, 36, 33, 0x65ddc, -1, 0x50, 0x6b218,
        1, 0x41029b, 0x4584a0, -1, -1, -1, 0, 47, 0, 0,
        ...sample.positionBits, ...sample.positionBits].join(' '));
    }
    fs.mkdirSync(work, {recursive: true});
    const cache = path.join(work, 'emscripten-cache');
    if (!fs.existsSync(cache))
      fs.cpSync(path.resolve(root, '../../work/emscripten-cache-4.0.22'), cache,
        {recursive: true, filter: source => !path.basename(source).endsWith('.lock')});
    const environment = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
      EM_CACHE: cache.replaceAll('\\', '/'), EMSDK_PYTHON: python, PYTHONUTF8: '1', EMCC_CORES: '2', BINARYEN_CORES: '2'};
    for (const sanitized of [false, true]) {
      const output = path.join(work, `ice-birth${sanitized ? '-ubsan' : ''}.cjs`);
      const build = spawnSync(python, [compiler, 'tests/native/ice-birth.cpp', 'src/game/IceBirthRules.cpp',
        'src/animation/AnmLogic.cpp', '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror',
        '-fno-exceptions', '-fno-fast-math', '-ffp-contract=off',
        ...(sanitized ? ['-fsanitize=undefined', '-fno-sanitize-recover=all'] : []),
        '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-sNODERAWFS=1', '-o', output],
      {cwd: root, env: environment, encoding: 'utf8', timeout: 180000});
      assert.ifError(build.error);
      assert.equal(build.status, 0, build.stdout + build.stderr);
      const run = spawnSync(process.execPath, [output, source],
        {cwd: root, input: inputs.join('\n') + '\n', encoding: 'utf8', timeout: 30000});
      assert.ifError(run.error);
      assert.equal(run.status, 0, run.stdout + run.stderr);
      const actual = run.stdout.trim().split(/\r?\n/).map(line => line.trim());
      assert.equal(actual.length, expected.length);
      actual.forEach((line, index) => assert.equal(line, expected[index], `original ice vector ${index}`));
    }
  });
