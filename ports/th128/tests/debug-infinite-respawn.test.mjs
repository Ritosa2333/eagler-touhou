import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
test('debug respawn gate uses the real beginRespawn low/legal motivation branch', () => {
  const source = fs.readFileSync(path.join(root, 'src/runtime/WorldExports.cpp'), 'utf8');
  assert.match(source, /th128_world_set_debug_infinite_respawn/);
  assert.match(source, /debugInfiniteRespawnRun&&motivationRaw<0/);
  assert.match(source, /motivationRaw=10000/);
  assert.match(source, /runTainted/);
  assert.match(source, /debugReviveRefills/);
  assert.match(source, /debugLastRefillBefore/);
  assert.match(source, /debugLastRefillAfter/);
  assert.match(source, /candidate\.debugInfiniteRespawn/);
  assert.match(source, /debugInfiniteRespawnRun=stageCarry\.debugInfiniteRespawn/);
  const compiler = path.resolve(root, '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk = path.resolve(path.dirname(compiler), '../..');
  const python = path.join(sdk, 'python/3.13.3_64bit/python.exe');
  const env = {...process.env,
    EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
    EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
    EMSDK_PYTHON: python, EMCC_CORES: '2', BINARYEN_CORES: '2'};
  const output = path.join(root, 'artifacts/tests/debug-infinite-respawn.cjs');
  const build = spawnSync(python, [compiler, 'tests/native/debug-infinite-respawn.cpp',
    'src/game/PlayerHitHost.cpp', 'src/game/PlayerHitRules.cpp', 'src/game/PlayerRules.cpp',
    'src/game/PlayerDeathRules.cpp', 'src/game/PlayerRespawnRules.cpp',
    'src/animation/AnmLogic.cpp', 'src/animation/AnmScene.cpp',
    '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
    '-fno-fast-math', '-ffp-contract=off', '-fsanitize=undefined', '-fno-sanitize-recover=all',
    '-sENVIRONMENT=node', '-sNODERAWFS=1', '-sEXIT_RUNTIME=1', '-sALLOW_MEMORY_GROWTH=1', '-o', output],
    {cwd: root, env, encoding: 'utf8', timeout: 300000});
  assert.equal(build.status, 0, build.stdout + build.stderr);
  const catalog = JSON.parse(fs.readFileSync(path.join(root, 'local/catalog.json'), 'utf8'));
  const raw = path.join(path.dirname(path.join(root, 'local', catalog.variants.ja.manifest)), 'raw');
  const run = spawnSync(process.execPath, [output, path.join(raw, 'pl00.anm')],
    {cwd: root, encoding: 'utf8', timeout: 60000});
  assert.equal(run.status, 0, run.stdout + run.stderr);
  assert.match(run.stdout, /PASS debug respawn gate boundary/);
});
