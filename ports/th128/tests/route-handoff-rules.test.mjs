import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const work = path.resolve(root, '../../work/dsh-route-handoff-rules');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');
const intMin = -2147483648;
const intMax = 2147483647;
const basis = {
  startZero: '4273ac;4273df;4273ee;4273ff;427403;42740f',
  startNonzero: '4273ac;4273ee;4273f4;4273da;427403;42740f',
  gateParity: '4273c2;4273d5;4273da;4273ff;427403;42740f',
  second: '4273ff;427403;42740f',
  terminal: '424695/4245d4 settlement owner; 4273a0 terminal excluded',
  invalid: 'scope guard; unsupported id',
};
const cases = [
  {input: [1, 0, 0, 0], expected: 'plan 2 0 0 0x4b1a80', basis: basis.startZero},
  {input: [2, 0, 0, 0], expected: 'plan 3 0 0 0x4b1aa8', basis: basis.second},
  {input: [1, 0, 0, 1], expected: 'plan 4 1 1 0x4b1ad0', basis: basis.startNonzero},
  {input: [4, 1, 0, 0], expected: 'plan 5 1 0 0x4b1af8', basis: basis.second},
  {input: [6, 0, 0, 0], expected: 'plan 7 0 0 0x4b1b48', basis: basis.startZero},
  {input: [7, 0, 0, 0], expected: 'plan 8 0 0 0x4b1b70', basis: basis.second},
  {input: [6, 0, 0, -1], expected: 'plan 9 1 1 0x4b1b98', basis: basis.startNonzero},
  {input: [9, 1, 0, 0], expected: 'plan 10 1 0 0x4b1bc0', basis: basis.second},
  {input: [11, 0, 0, 0], expected: 'plan 12 0 0 0x4b1c10', basis: basis.startZero},
  {input: [12, 0, 0, 0], expected: 'plan 13 0 0 0x4b1c38', basis: basis.second},
  {input: [11, 0, 0, 1], expected: 'plan 14 1 1 0x4b1c60', basis: basis.startNonzero},
  {input: [14, 1, 0, 0], expected: 'plan 15 1 0 0x4b1c88', basis: basis.second},
  {input: [1, 0, 1, 0], expected: 'plan 2 0 0 0x4b1a80', basis: basis.gateParity},
  {input: [1, 1, 1, 0], expected: 'plan 4 1 0 0x4b1ad0', basis: basis.gateParity},
  {input: [6, 2, 7, 0], expected: 'plan 7 2 0 0x4b1b48', basis: basis.gateParity},
  {input: [6, -1, 1, 0], expected: 'plan 9 -1 0 0x4b1b98', basis: basis.gateParity},
  {input: [11, -3, 1, 0], expected: 'plan 14 -3 0 0x4b1c60', basis: basis.gateParity},
  {input: [11, intMax, 1, 0], expected: 'plan 14 2147483647 0 0x4b1c60', basis: basis.gateParity},
  {input: [1, intMin, 1, 0], expected: 'plan 2 -2147483648 0 0x4b1a80', basis: basis.gateParity},
  {input: [1, 0, 0, -7], expected: 'plan 4 1 1 0x4b1ad0', basis: basis.startNonzero},
  {input: [1, -1, 0, -1], expected: 'plan 4 0 1 0x4b1ad0', basis: basis.startNonzero},
  {input: [1, intMax, 0, 1], expected: 'plan 4 -2147483648 1 0x4b1ad0', basis: basis.startNonzero},
  {input: [6, intMax, 0, 0], expected: 'plan 7 2147483647 0 0x4b1b48', basis: basis.startZero},
  {input: [11, intMin, 0, 1], expected: 'plan 14 -2147483647 1 0x4b1c60', basis: basis.startNonzero},
  {input: [2, intMax, 5, 9], expected: 'plan 3 2147483647 0 0x4b1aa8', basis: basis.second},
  {input: [14, intMin, -1, -1], expected: 'plan 15 -2147483648 0 0x4b1c88', basis: basis.second},
  {input: [3, 0, 0, 0], expected: 'none', basis: basis.terminal},
  {input: [5, 0, 0, 0], expected: 'none', basis: basis.terminal},
  {input: [8, 0, 0, 0], expected: 'none', basis: basis.terminal},
  {input: [10, 0, 0, 0], expected: 'none', basis: basis.terminal},
  {input: [13, 0, 0, 0], expected: 'none', basis: basis.terminal},
  {input: [15, 0, 0, 0], expected: 'none', basis: basis.terminal},
  {input: [16, 0, 0, 0], expected: 'none', basis: basis.terminal},
  {input: [0, 0, 0, 0], expected: 'none', basis: basis.invalid},
  {input: [-1, 0, 0, 0], expected: 'none', basis: basis.invalid},
  {input: [17, 0, 0, 0], expected: 'none', basis: basis.invalid},
  {input: [99, 0, 0, 0], expected: 'none', basis: basis.invalid},
];

test('route handoff scalar plan follows the original 4273a0 branches', () => {
  assert.ok(fs.existsSync(compiler), `emscripten compiler missing: ${compiler}`);
  const input = cases.map(item => `PLAN ${item.input.join(' ')}`);
  const expected = cases.map(item => item.expected);
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
  const output = path.join(work, 'route-handoff-rules.cjs');
  const build = spawnSync(python, [compiler, 'tests/native/route-handoff-rules.cpp',
    'src/game/RouteHandoffRules.cpp', '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror',
    '-fexceptions', '-fno-fast-math', '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-o', output],
  {cwd: root, env: environment, encoding: 'utf8', timeout: 180000});
  assert.ifError(build.error);
  assert.equal(build.status, 0, build.stdout + build.stderr);
  const run = spawnSync(process.execPath, [output],
    {cwd: root, input: `${input.join('\n')}\n`, encoding: 'utf8', timeout: 30000, maxBuffer: 4 * 1024 * 1024});
  assert.ifError(run.error);
  assert.equal(run.status, 0, run.stdout + run.stderr);
  const actual = run.stdout.trim().split(/\r?\n/);
  assert.equal(actual.length, expected.length, actual.join('\n'));
  expected.forEach((want, index) => {
    assert.equal(actual[index], want,
      `${cases[index].basis} input ${cases[index].input.join(' ')}`);
  });
});
