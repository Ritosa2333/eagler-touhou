import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const work = path.resolve(root, '../../work/player-shot-birth-test');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');
const digest = value => createHash('sha256').update(value).digest('hex');
const bits = value => { const bytes = Buffer.alloc(4); bytes.writeFloatLE(value); return bytes.readUInt32LE(); };
const callbackTables = [0x4b1cd8, 0x4b1a14, 0x4d2e2c, 0x4b1cf8];

function evidence() {
  const raw = fs.readFileSync(path.join(root, 'local/research/player-shot-birth-fixtures.json'));
  assert.equal(digest(raw), 'b55bfa34b8d5fd7abfb8a0c7d4d56e42bbdb346ba380c5b9b38779f550cf155e');
  const report = JSON.parse(raw.toString('utf8'));
  assert.equal(report.schema, 'th128-original-byte-player-shot-birth-fixtures/1');
  assert.equal(report.executable.sha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256After, report.executable.sha256);
  assert.equal(report.executable.unchanged, true);
  assert.equal(report.nativeCpuTrace, false);
  assert.equal(report.windowsGameProcessLaunched, false);
  assert.equal(report.resource.sha256, 'aed90a45b565510e97cc7d9ab4846d8c93914e57faafe9339e9ab03d47b1511b');
  assert.equal(report.resource.groupCount, 16);
  assert.equal(report.resource.recordCount, 114);
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(digest(bytes), chunk.sha256);
  }
  for (const constant of Object.values(report.constants))
    assert.equal(digest(Buffer.from(constant.bytes, 'hex')), constant.sha256);
  assert.equal(digest(fs.readFileSync(path.join(root, 'tools/research/player_shot_birth_probe.py'))), report.generatorSha256);
  for (const [name, hash] of Object.entries(report.engineSha256))
    assert.equal(digest(fs.readFileSync(path.join(root, 'tools/research', name))), hash);
  return report;
}

test('production player-shot API requires a real SHT bank and explicit motion/callback plans', () => {
  for (const file of ['src/game/PlayerShotBirthRules.hpp', 'src/game/PlayerShotBirthRules.cpp'])
    assert.ok(fs.existsSync(path.join(root, file)), `${file}: bounded player-shot birth is missing`);
});

test('actual first-shot field stores and native motion prefixes stop before unknown allocator returns', () => {
  const report = evidence();
  assert.equal(report.birthCases.length, 119);
  assert.equal(report.motionCases.length, 114);
  for (const sample of report.birthCases) {
    assert.ok(sample.instructionsExecuted < sample.instructionBudget, sample.id);
    if (sample.occupiedRun === 256) {
      assert.equal(sample.stop.reason, 'returned');
      assert.equal(sample.registers.eax, 0);
      continue;
    }
    assert.equal(sample.stop.reason, 'unsupported_call');
    assert.equal(sample.stop.target, '0x0046a700');
    assert.equal(sample.completedBirth, false);
    const record = Buffer.from(sample.rawRecord, 'hex');
    assert.equal(sample.next.damage, record.readInt16LE(2));
    assert.deepEqual(sample.next.sizeBits, [record.readUInt32LE(12), record.readUInt32LE(16)]);
    assert.equal(sample.next.speedBits, record.readUInt32LE(24));
  }
  for (const sample of report.motionCases) {
    assert.equal(sample.stop.target, '0x00468010');
    assert.ok(sample.instructionTrace.includes('0x0046c1fd'));
    assert.equal(sample.completedBirth, false);
  }
  const actual = report.resource.groups[0].records[0];
  assert.deepEqual(actual.callbackIds, [0, 0, 0, 0]);
  assert.ok(report.callbackTables.every(table => table.state.includes('not live runtime')));
  for (const difficulty of [0, 1, 2, 3]) {
    const sample = report.groupCases.find(sample => sample.frozenAreaBits === bits(0) && sample.difficulty === difficulty);
    assert.equal(sample.group, difficulty === 0 ? 2 : 0);
  }
});

test('owning SHT bank, first-shot state and required plans match native goldens in WASM and UBSan',
  {skip: !fs.existsSync(compiler)}, () => {
    const report = evidence();
    const catalog = JSON.parse(fs.readFileSync(path.join(root, 'local/catalog.json'), 'utf8'));
    const manifest = path.resolve(root, 'local', catalog.variants.ja.manifest);
    const source = path.join(path.dirname(manifest), 'raw/pl00a.sht');
    assert.ok(source.startsWith(path.join(root, 'local') + path.sep));
    assert.equal(digest(fs.readFileSync(source)), report.resource.sha256);
    const input = [], expected = [];
    for (const sample of report.groupCases) {
      input.push(`G ${sample.frozenAreaBits} ${sample.difficulty}`);
      expected.push(String(sample.group));
    }
    for (const sample of report.schedulerCases) {
      input.push(`D ${sample.frozenAreaBits} ${sample.difficulty} ${sample.shotCount}`);
      expected.push(String(sample.firstRecordOffset ?? -1));
    }
    report.resource.groups.forEach((group, groupIndex) => group.records.forEach((record, recordIndex) => {
      input.push(`R ${groupIndex} ${recordIndex}`);
      expected.push(`${record.offset} ${group.unknownWord} ${record.bytes}`);
    }));
    for (const sample of [...report.birthCases, ...report.motionCases, ...report.recycledCases]) {
      input.push([sample.recycledInput ? (sample.motionExecuted ? 'N' : 'C') : (sample.motionExecuted ? 'M' : 'B'), sample.group, sample.record, sample.occupiedRun,
        sample.timerFlags, sample.shotCount, ...sample.positionBits,
        ...(sample.recycledInput ? [sample.slotBefore] : []),
        ...(sample.motionExecuted ? [...sample.next.velocityBits, sample.next.angleBits] : [])].join(' '));
      if (sample.occupiedRun === 256) {
        expected.push('2 256 256');
        continue;
      }
      const image = Buffer.from(sample.slotAfter, 'hex');
      image.writeUInt32LE(sample.recordOffset, 0x74);
      const record = Buffer.from(sample.rawRecord, 'hex');
      const callbacks = callbackTables.flatMap((table, index) => [0x24 + 4 * index, table, record.readUInt32LE(0x24 + 4 * index)]);
      expected.push([sample.motionExecuted ? 1 : 0, sample.slot, sample.examined, image.toString('hex'),
        record.readInt16LE(30) + 5, 23, 0x10, record.readInt16LE(34), record.readUInt32LE(20), record.readUInt32LE(24),
        0x43efd4, 0x468010, 0x43f011, 0x4593b0, 0x43f01c, 0x466d60, 0x130, 0x4d33f8, 0x8000,
        0x46a700, 0x46a700, 0x46ae00, 0x4b6f58, bits(1),
        ...callbacks].join(' '));
    }
    fs.mkdirSync(work, {recursive: true});
    const cache = path.join(work, 'emscripten-cache');
    if (!fs.existsSync(cache))
      fs.cpSync(path.resolve(root, '../../work/emscripten-cache-4.0.22'), cache,
        {recursive: true, filter: source => !path.basename(source).endsWith('.lock')});
    const temporary = path.join(work, 'temp');
    fs.mkdirSync(temporary, {recursive: true});
    const environment = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
      EM_CACHE: cache.replaceAll('\\', '/'), EMSDK_PYTHON: python, PYTHONUTF8: '1', EMCC_CORES: '2', BINARYEN_CORES: '2',
      TEMP: temporary, TMP: temporary, TMPDIR: temporary};
    for (const sanitized of [false, true]) {
      const output = path.join(work, `player-shot-birth${sanitized ? '-ubsan' : ''}.cjs`);
      const build = spawnSync(python, [compiler, 'tests/native/player-shot-birth.cpp', 'src/game/PlayerShotBirthRules.cpp',
        '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fno-exceptions', '-fno-fast-math', '-ffp-contract=off',
        ...(sanitized ? ['-fsanitize=undefined', '-fno-sanitize-recover=all'] : []),
        '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-sNODERAWFS=1', '-o', output],
      {cwd: root, env: environment, encoding: 'utf8', timeout: 180000});
      assert.ifError(build.error);
      assert.equal(build.status, 0, build.stdout + build.stderr);
      const run = spawnSync(process.execPath, [output, source], {cwd: root, input: input.join('\n') + '\n', encoding: 'utf8', timeout: 30000});
      assert.ifError(run.error);
      assert.equal(run.status, 0, run.stderr);
      const actual = run.stdout.trim().split(/\r?\n/);
      assert.equal(actual.length, expected.length);
      actual.forEach((line, index) => assert.equal(line, expected[index], `native shot vector ${index}`));
    }
  });
