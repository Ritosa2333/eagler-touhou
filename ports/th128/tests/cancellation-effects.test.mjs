import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const work = path.resolve(root, '../../work/cancellation-effects-test');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const bundledPython = path.join(sdk, 'python/3.13.3_64bit/python.exe');
const python = process.env.TH128_PYTHON ?? (fs.existsSync(bundledPython) ? bundledPython : 'python');
const sanitized = process.env.TH128_CANCEL_UBSAN === '1';
const noExceptions = process.env.TH128_CANCEL_NOEXCEPT === '1';
const output = path.join(work,
  `cancellation-effects${sanitized ? '-ubsan' : ''}${noExceptions ? '-noexceptions' : ''}.cjs`);
const environment = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
  EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
  EMSDK_PYTHON: python, PYTHONUTF8: '1', EMCC_CORES: '2', BINARYEN_CORES: '2'};
const digest = bytes => createHash('sha256').update(bytes).digest('hex');
let built = false;

function build() {
  if (built) return;
  for (const file of ['src/game/CancellationEffects.hpp', 'src/game/CancellationEffects.cpp'])
    assert.ok(fs.existsSync(path.join(root, file)), `${file}: Scene-backed cancellation is not implemented`);
  fs.mkdirSync(work, {recursive: true});
  const result = spawnSync(python, [compiler, 'tests/native/cancellation-effects.cpp',
    'src/game/CancellationEffects.cpp', 'src/animation/AnmLogic.cpp', 'src/animation/AnmScene.cpp',
    '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fno-fast-math', '-ffp-contract=off',
    ...(noExceptions ? ['-fno-exceptions'] : ['-fexceptions', '-sDISABLE_EXCEPTION_CATCHING=0']),
    ...(sanitized ? ['-fsanitize=undefined', '-fno-sanitize-recover=all'] : []),
    '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-sNODERAWFS=1', '-sASSERTIONS=2',
    '-sALLOW_MEMORY_GROWTH=1', '-sINITIAL_MEMORY=67108864', '-o', output],
  {cwd: root, encoding: 'utf8', env: environment, timeout: 180000});
  assert.ifError(result.error);
  assert.equal(result.status, 0, result.stdout + result.stderr);
  built = true;
}

function run(args, input = '') {
  build();
  const result = spawnSync(process.execPath, [output, ...args],
    {cwd: root, encoding: 'utf8', timeout: 60000, input});
  assert.ifError(result.error);
  assert.equal(result.status, 0, result.stdout + result.stderr);
  return result.stdout;
}

test('Scene cancellation rejects unknown behaviors and preserves RNG/poses/ownership within budgets',
  {skip: !fs.existsSync(compiler)}, () => {
    assert.match(run(['--synthetic']), /cancellation effects synthetic regressions passed/);
  });

test('one shared primary tail preserves interleaved root and delayed-child RNG order across all known effects',
  {skip: !fs.existsSync(compiler)}, () => {
    assert.match(run(['--unified-synthetic']), /unified tail synthetic RNG ordering passed/);
  });

test('actual cancellation357 binds356, with a distinct real361/360 comparison and clean retirement',
  {skip: !fs.existsSync(compiler)}, () => {
    const catalog = JSON.parse(fs.readFileSync(path.join(root, 'local/catalog.json'), 'utf8'));
    const manifest = path.resolve(root, 'local', catalog.variants.ja.manifest);
    const source = path.join(path.dirname(manifest), 'raw/bullet.anm');
    assert.ok(source.startsWith(path.join(root, 'local') + path.sep));
    assert.equal(digest(fs.readFileSync(source)),
      '9860822687f33e740f2a5e1fb25a4b05cf27b0fc6d15ed9626fb903986f7784c');
    const drift = JSON.parse(fs.readFileSync(path.join(root, 'local/research/cancellation-drift-fixtures.json'), 'utf8'));
    const input = [String(drift.cases.length), ...drift.cases.map(sample =>
      [...sample.velocityWords, ...sample.targetWords].join(' '))].join('\n') + '\n';
    const stdout = run([source], input);
    assert.match(stdout, /cancellation effects actual357\/356 passed: 40 primary passes; separate361\/360 verified/);
    assert.match(stdout, /guarded native drift prefix goldens passed/);
    assert.match(stdout, /unified actual175\/83\/84\/357 global-tail and33-hit cleanup passed/);
  });

test('Scene cancellation catches callback throws and never publishes a partial tree',
  {skip: !fs.existsSync(compiler) || noExceptions}, () => {
    assert.match(run(['--throws']), /cancellation effects callback-throw regressions passed/);
  });

test('original bytes prove color10 root357 to green356, not the distinct361 to yellow360 path', () => {
  const fixture = path.join(root, 'local/research/cancellation-static.json');
  assert.ok(fs.existsSync(fixture), 'original-byte cancellation evidence is absent');
  const report = JSON.parse(fs.readFileSync(fixture, 'utf8'));
  assert.equal(report.schema, 'th128-original-byte-cancellation-static/1');
  assert.equal(report.executable.sha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256After, report.executable.sha256);
  assert.equal(report.executable.unchanged, true);
  assert.equal(report.nativeCpuTrace, false);
  assert.equal(report.windowsGameProcessLaunched, false);
  const profile = Buffer.from(report.profile.bytes, 'hex');
  assert.equal(digest(profile), report.profile.sha256);
  assert.equal(profile.readInt32LE(0x10 + 10 * 16), 357);
  assert.equal(report.animation.sha256, '9860822687f33e740f2a5e1fb25a4b05cf27b0fc6d15ed9626fb903986f7784c');
  const childOperands = script => report.animation.programs[String(script)].commands
    .filter(command => command.opcode === 88).map(command => Buffer.from(command.raw, 'hex').readInt32LE());
  assert.deepEqual(childOperands(357), [356, 356]);
  assert.deepEqual(childOperands(361), [360, 360]);
  const color = script => report.animation.programs[String(script)].commands
    .find(command => command.opcode === 52).raw;
  assert.equal(color(356), '00000000ff00000000000000');
  assert.equal(color(360), 'c0000000c000000000000000');
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(digest(bytes), chunk.sha256);
  }
  assert.ok(report.code.irqQueue.bytes.includes('6689bef8030000c3'));
  assert.ok(report.code.hitCancellation.bytes.includes('e850460000'));
  assert.ok(report.code.primaryTail && report.code.primaryHead, 'same-primary head/tail registration evidence is absent');
  assert.ok(report.code.primaryTail.bytes.includes('899104978b00'));
  assert.ok(report.code.primaryTail.bytes.includes('899108978b00'));
  assert.ok(report.code.primaryHead.bytes.includes('898a04978b00'));
  assert.ok(report.code.cancellationRootFactory.bytes.includes('e8ebed0500'));
  assert.ok(report.code.op88Handler.bytes.includes('6a00'));
  assert.ok(report.code.op88Handler.bytes.includes('e8cace0000'));
  assert.ok(report.code.attachedChildFactory.bytes.includes('83e004'));
  assert.ok(report.code.attachedChildFactory.bytes.includes('e882faffff'));
  assert.ok(report.code.primaryTickPass.bytes.startsWith('8b8304978b00'));
  assert.ok(report.code.primaryRegistration.bytes.includes('bb1e000000'));
  assert.ok(report.code.primaryRegistration.bytes.includes('c7400890664600'));
});

test('native drift goldens execute original scalar prefix and initializer with declared zero/clock1 context', () => {
  const report = JSON.parse(fs.readFileSync(path.join(root, 'local/research/cancellation-drift-fixtures.json'), 'utf8'));
  assert.equal(report.schema, 'th128-original-byte-cancellation-drift/1');
  assert.equal(report.executable.sha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256After, report.executable.sha256);
  assert.equal(report.executable.unchanged, true);
  assert.equal(report.x86Emulated, true);
  assert.equal(report.nativeCpuTrace, false);
  assert.equal(report.windowsGameProcessLaunched, false);
  assert.equal(report.doubleConstant.value, 10);
  assert.equal(digest(Buffer.from(report.doubleConstant.bytes, 'hex')), report.doubleConstant.sha256);
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(digest(bytes), chunk.sha256);
  }
  assert.deepEqual(report.cases[0].targetWords, [0x41200000, 0xc1a00000, 0x40200000]);
  for (const sample of report.cases) {
    assert.deepEqual(sample.globalStartWords, [0, 0, 0]);
    assert.equal(sample.clockWord, 0x3f800000);
    assert.deepEqual(sample.stepWords, sample.velocityWords);
    assert.equal(sample.prefix.stop.reason, 'boundary');
    assert.equal(sample.prefix.stop.address, '0x00408b4b');
    assert.equal(sample.initializer.stop.reason, 'returned');
    assert.deepEqual(sample.initializer.fromWords, [0, 0, 0]);
    assert.deepEqual(sample.initializer.toWords, sample.targetWords);
    assert.deepEqual(sample.initializer.slopeWords, [0, 0, 0, 0, 0, 0]);
    assert.equal(sample.initializer.duration, 30);
    assert.equal(sample.initializer.mode, 6);
    assert.equal(sample.initializer.unchangedOutsideInterpolator, true);
    for (const execution of [sample.prefix, sample.initializer])
      assert.ok(execution.instructionsExecuted < execution.instructionBudget);
  }
});
