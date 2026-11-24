import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const compiler = path.resolve(root, '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = path.join(sdk, 'python/3.13.3_64bit/python.exe');
const environment = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
  EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
  EMSDK_PYTHON: python, PYTHONUTF8: '1', EMCC_CORES: '2', BINARYEN_CORES: '2'};

test('bounded production bullet manager allocates real births and preserves explicit unsupported paths', () => {
  assert.ok(fs.existsSync(path.join(root, 'src/game/BulletManager.cpp')), 'production bullet manager is missing');
  const profile = path.join(root, 'local/runtime/bullet-profile-8.bin');
  const raw = fs.readFileSync(profile);
  assert.equal(raw.length, 288);
  const provenance = JSON.parse(fs.readFileSync(path.join(root, 'local/runtime/bullet-profile-8.json'), 'utf8'));
  assert.equal(provenance.sha256, createHash('sha256').update(raw).digest('hex'));
  assert.equal(provenance.exeSha256, 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  const directory = path.join(root, 'artifacts/tests');
  fs.mkdirSync(directory, {recursive: true});
  for (const sanitized of [false, true]) {
    const output = path.join(directory, `bullet-manager${sanitized ? '-ubsan' : ''}.cjs`);
    const flags = sanitized ? ['-fsanitize=undefined', '-fno-sanitize-recover=all'] : [];
    const build = spawnSync(python, [compiler, 'tests/native/bullet-manager.cpp', 'src/game/BulletManager.cpp',
      'src/game/BulletRules.cpp', '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror',
      '-fno-fast-math', '-ffp-contract=off', ...flags, '-sENVIRONMENT=node', '-sNODERAWFS=1',
      '-sEXIT_RUNTIME=1', '-sALLOW_MEMORY_GROWTH=1', '-o', output],
    {cwd: root, encoding: 'utf8', env: environment, timeout: 180000});
    assert.equal(build.status, 0, build.stdout + build.stderr);
    const run = spawnSync(process.execPath, [output, profile], {cwd: root, encoding: 'utf8', timeout: 30000});
    assert.equal(run.status, 0, run.stdout + run.stderr);
  }
});
