import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const work = path.resolve(root, '../../work/enemy-visuals-test');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const bundledPython = path.join(sdk, 'python/3.13.3_64bit/python.exe');
const python = process.env.TH128_PYTHON ?? (fs.existsSync(bundledPython) ? bundledPython : 'python');
const sanitized = process.env.TH128_ENEMY_UBSAN === '1';
const noExceptions = process.env.TH128_ENEMY_NOEXCEPT === '1';
const output = path.join(work, `enemy-visuals${sanitized ? '-ubsan' : ''}${noExceptions ? '-noexceptions' : ''}.cjs`);
const environment = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
  EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
  EMSDK_PYTHON: python, PYTHONUTF8: '1', EMCC_CORES: '2', BINARYEN_CORES: '2'};
const digest = bytes => createHash('sha256').update(bytes).digest('hex');
let built = false;

function build() {
  if (built) return;
  for (const file of ['src/game/EnemyVisuals.hpp', 'src/game/EnemyVisuals.cpp'])
    assert.ok(fs.existsSync(path.join(root, file)), `${file}: enemy ANM sidecar is not implemented`);
  fs.mkdirSync(work, {recursive: true});
  const result = spawnSync(python, [compiler, 'tests/native/enemy-visuals.cpp',
    'src/game/EnemyVisuals.cpp', 'src/animation/AnmLogic.cpp', '-Isrc', '-std=c++20', '-O1',
    '-Wall', '-Wextra', '-Werror', '-fno-fast-math', '-ffp-contract=off',
    ...(noExceptions ? ['-fno-exceptions'] : ['-fexceptions', '-sDISABLE_EXCEPTION_CATCHING=0']),
    ...(sanitized ? ['-fsanitize=undefined', '-fno-sanitize-recover=all'] : []),
    '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-sNODERAWFS=1', '-sASSERTIONS=2',
    '-sALLOW_MEMORY_GROWTH=1', '-sINITIAL_MEMORY=67108864', '-o', output],
  {cwd: root, encoding: 'utf8', env: environment, timeout: 180000});
  assert.ifError(result.error);
  assert.equal(result.status, 0, result.stdout + result.stderr);
  built = true;
}

function run(args) {
  build();
  const result = spawnSync(process.execPath, [output, ...args],
    {cwd: root, encoding: 'utf8', timeout: 60000});
  assert.ifError(result.error);
  assert.equal(result.status, 0, result.stdout + result.stderr);
  return result.stdout;
}

test('enemy ANM sidecar validates synthetic roots, failures, lifetimes and budgets',
  {skip: !fs.existsSync(compiler)}, () => {
    assert.match(run(['--synthetic']), /enemy visuals synthetic regressions passed/);
  });

test('actual bank1 enemy.anm executes script0 startup, five-tick change and steady loop',
  {skip: !fs.existsSync(compiler)}, () => {
    const catalog = JSON.parse(fs.readFileSync(path.join(root, 'local/catalog.json'), 'utf8'));
    const manifest = path.resolve(root, 'local', catalog.variants.ja.manifest);
    const raw = path.join(path.dirname(manifest), 'raw');
    assert.ok(raw.startsWith(path.join(root, 'local') + path.sep));
    const sources = ['enemy.anm', 'enemy_ll.anm', 'boss01.anm', 'bullet.anm'].map(name => path.join(raw, name));
    assert.equal(digest(fs.readFileSync(sources[0])),
      '09896a5f0644cd84a173637275b098339f2ed5893e3107a6dac014387a2ff8dc');
    assert.match(run(sources), /enemy visuals actual banks passed: 78 scripts, 1024 scheduled ticks/);
  });

test('enemy ANM RNG callback throws fail closed without publishing partial roots',
  {skip: !fs.existsSync(compiler) || noExceptions}, () => {
    assert.match(run(['--rng-throws']), /enemy visuals callback-throw regressions passed/);
  });

test('original-byte static evidence distinguishes TH128 ANM priority30 and first-include bank1', () => {
  const report = JSON.parse(fs.readFileSync(path.join(root, 'local/research/enemy-visual-static.json'), 'utf8'));
  assert.equal(report.schema, 'th128-original-byte-enemy-visuals-static/1');
  assert.equal(report.executable.sha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256After, report.executable.sha256);
  assert.equal(report.executable.unchanged, true);
  assert.equal(report.nativeCpuTrace, false);
  assert.equal(report.windowsGameProcessLaunched, false);
  for (const chunk of Object.values(report.code)) {
    const bytes = Buffer.from(chunk.bytes, 'hex');
    assert.equal(bytes.length, Number.parseInt(chunk.endExclusive) - Number.parseInt(chunk.start));
    assert.equal(digest(bytes), chunk.sha256);
    for (const instruction of chunk.instructions) {
      const relative = Number.parseInt(instruction.address) - Number.parseInt(chunk.start);
      const original = Buffer.from(instruction.bytes, 'hex');
      assert.deepEqual(bytes.subarray(relative, relative + original.length), original);
    }
  }
  const instructions = report.code.anmCallbackRegistration.instructions;
  const at = address => instructions.find(instruction => instruction.address === address);
  assert.equal(at('0x004638f6').bytes, 'bb1e000000');
  assert.equal(at('0x004638fd').bytes, 'c7400890664600');
  assert.equal(at('0x00463907').operands, '0x4681d0');
  assert.equal(at('0x0046393c').bytes, 'bb08000000');
  assert.equal(at('0x00463943').bytes, 'c74008c0664600');
  assert.equal(at('0x00463953').operands, '0x4681d0');
  assert.equal(report.code.bank0Store.instructions[0].bytes, '898f80000000');
  assert.equal(report.code.firstIncludeBase.instructions[0].bytes, 'c745fce0904e00');
  assert.equal(report.code.firstIncludeStore.instructions[0].bytes, '89bc10a46fb1ff');
  assert.equal(report.banks[0].scriptCount, 78);
  assert.deepEqual(report.banks.map(bank => bank.sprite0), [[32, 32], [64, 64], [96, 96]]);
  assert.equal(report.banks[0].script0[1].raw, '00000000');
  assert.equal(report.banks[0].script0[2].time, 5);
  assert.equal(report.banks[0].script0[2].raw, '01000000');
});
