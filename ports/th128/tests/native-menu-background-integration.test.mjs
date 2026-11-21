import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import test from 'node:test';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const compiler = path.resolve(root, '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = path.join(sdk, 'python/3.13.3_64bit/python.exe');
const environment = {...process.env,
  EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
  EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
  EMSDK_PYTHON: python, EMCC_CORES: '2', BINARYEN_CORES: '2'};

test('production menu background/header integration and transactional rejection', () => {
  const catalog = JSON.parse(fs.readFileSync(path.join(root, 'local/catalog.json'), 'utf8'));
  const raw = path.join(path.dirname(path.join(root, 'local', catalog.variants.ja.manifest)), 'raw');
  const outputDirectory = path.join(root, 'artifacts/dsh-menu-background-production');
  fs.mkdirSync(outputDirectory, {recursive: true});
  const output = path.join(outputDirectory, 'menu-background-integration.cjs');
  const build = spawnSync(python, [compiler, 'tests/native/menu-background-integration.cpp',
    'src/animation/AnmLogic.cpp', 'src/animation/AnmScene.cpp', 'src/game/FreezeRules.cpp',
    '-Isrc', '-Isrc/runtime', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
    '-fno-fast-math', '-ffp-contract=off', '-fsanitize=undefined', '-fno-sanitize-recover=all',
    '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-sNODERAWFS=1', '-sALLOW_MEMORY_GROWTH=1', '-o', output],
    {cwd: root, env: environment, encoding: 'utf8', timeout: 240000});
  assert.equal(build.status, 0, build.stdout + build.stderr);
  const run = spawnSync(process.execPath, [output, path.join(raw, 'title.anm'), path.join(raw, 'title_v.anm')],
    {cwd: root, encoding: 'utf8', timeout: 120000});
  assert.equal(run.status, 0, run.stdout + run.stderr);
  assert.match(run.stdout, /PASS production menu background retirement and authored headers/);
  assert.match(run.stdout, /PASS production rejection preserves state\/owners and reset clears header timers/);
});
