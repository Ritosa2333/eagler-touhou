import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
test('type20 color4 original cancellation owns script377/child376, unknown379 stays strict', () => {
  const compiler = path.resolve(root, '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk = path.resolve(path.dirname(compiler), '../..');
  const python = path.join(sdk, 'python/3.13.3_64bit/python.exe');
  const env = {...process.env,
    EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
    EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
    EMSDK_PYTHON: python, EMCC_CORES: '2', BINARYEN_CORES: '2'};
  const output = path.join(root, 'artifacts/tests/route-b-cancellation.cjs');
  const build = spawnSync(python, [compiler, 'tests/native/route-b-cancellation.cpp',
    'src/game/CancellationEffects.cpp', 'src/animation/AnmLogic.cpp', 'src/animation/AnmScene.cpp',
    '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
    '-fno-fast-math', '-ffp-contract=off', '-fsanitize=undefined', '-fno-sanitize-recover=all',
    '-sENVIRONMENT=node', '-sNODERAWFS=1', '-sEXIT_RUNTIME=1', '-sALLOW_MEMORY_GROWTH=1', '-o', output],
    {cwd: root, env, encoding: 'utf8', timeout: 300000});
  assert.equal(build.status, 0, build.stdout + build.stderr);
  const bullet = path.join(root, 'local/generations/8c71c316a4b24e68827f9fc560242050/zh/raw/bullet.anm');
  const profile = path.join(root, 'local/runtime/bullet-profile-20.bin');
  const run = spawnSync(process.execPath, [output, bullet, profile],
    {cwd: root, encoding: 'utf8', timeout: 60000});
  assert.equal(run.status, 0, run.stdout + run.stderr);
  assert.match(run.stdout, /PASS type20 color4 cancellation 377\/376/);
});
