import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const compiler = path.resolve(root, '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = path.join(sdk, 'python/3.13.3_64bit/python.exe');
const environment = {...process.env, EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
  EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
  EMSDK_PYTHON: python, PYTHONUTF8: '1', EMCC_CORES: '2', BINARYEN_CORES: '2'};

test('actual first-stage 501 constructs a bullet through the required host and stops before an unimplemented update', () => {
  assert.match(fs.readFileSync(path.join(root, 'src/game/StageWorld.hpp'), 'utf8'), /configureBullets/,
    'actual first emission is not integrated into StageWorld');
  const directory = path.join(root, 'artifacts/tests');
  fs.mkdirSync(directory, {recursive: true});
  const output = path.join(directory, 'world-emission.cjs');
  const build = spawnSync(python, [compiler, 'tests/native/world-emission.cpp', 'src/game/StageWorld.cpp',
    'src/game/EnemyRules.cpp', 'src/game/EnemyRandomMovement.cpp', 'src/game/EnemyPhaseRules.cpp', 'src/game/EnemyDropRules.cpp', 'src/game/BulletRules.cpp', 'src/game/BulletManager.cpp', 'src/game/ShotEnemyCollisionRules.cpp',
    'src/ecl/EclProgram.cpp', 'src/ecl/EclVm.cpp', '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra',
    '-fno-fast-math', '-ffp-contract=off', '-fexceptions', '-sENVIRONMENT=node', '-sNODERAWFS=1',
    '-sEXIT_RUNTIME=1', '-sALLOW_MEMORY_GROWTH=1', '-o', output],
  {cwd: root, encoding: 'utf8', env: environment, timeout: 180000});
  assert.equal(build.status, 0, build.stdout + build.stderr);
  const local = path.join(root, 'local');
  const catalog = JSON.parse(fs.readFileSync(path.join(local, 'catalog.json'), 'utf8'));
  const manifestPath = path.join(local, catalog.variants.ja.manifest);
  const manifest = JSON.parse(fs.readFileSync(manifestPath, 'utf8'));
  const resources = manifest.ecl.map(item => path.join(path.dirname(manifestPath), 'raw', item.name));
  const run = spawnSync(process.execPath, [output, ...resources, path.join(local, 'runtime/bullet-profile-8.bin')],
    {cwd: root, encoding: 'utf8', timeout: 30000});
  assert.equal(run.status, 0, run.stdout + run.stderr);
  assert.match(run.stdout, /ACTUAL first emission script=43 type=8 color=10/);
  console.log(run.stdout);
});
