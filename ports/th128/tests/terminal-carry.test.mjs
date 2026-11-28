import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const scratch = path.join(root, 'artifacts/dsh-terminal-carry');
const compiler = process.env.TH128_EMCC ?? path.resolve(root,
  '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = process.env.TH128_PYTHON ?? path.join(sdk, 'python/3.13.3_64bit/python.exe');

test('second-stage to terminal carry identity only accepts the exact target', () => {
  assert.ok(fs.existsSync(compiler), `emscripten compiler missing: ${compiler}`);
  assert.ok(fs.existsSync(python), `emsdk python missing: ${python}`);
  fs.mkdirSync(scratch, {recursive: true});
  const cache = path.join(scratch, 'emcache');
  if (!fs.existsSync(path.join(cache, 'sanity.txt'))) {
    const shared = path.resolve(root, '../../work/emscripten-cache-4.0.22');
    assert.ok(fs.existsSync(shared), `shared emscripten cache missing: ${shared}`);
    fs.cpSync(shared, cache, {recursive: true,
      filter: source => !path.basename(source).endsWith('.lock')});
  }
  const temp = path.join(scratch, 'emtemp');
  fs.mkdirSync(path.join(temp, 'emscripten_temp'), {recursive: true});
  const output = path.join(scratch, 'terminal-carry.cjs');
  const environment = {...process.env,
    EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
    EM_CACHE: cache.replaceAll('\\', '/'),
    EMCC_TEMP_DIR: temp,
    EMCC_DEBUG: '1',
    EMSDK_PYTHON: python,
    EMCC_CORES: '2',
    BINARYEN_CORES: '2'};
  const build = spawnSync(python, [compiler,
    'tests/native/terminal-carry.cpp', 'src/game/StageSessionIdentity.cpp',
    'src/game/RouteHandoffRules.cpp',
    '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
    '-fno-fast-math', '-ffp-contract=off', '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1',
    '-o', output], {cwd: root, env: environment, encoding: 'utf8', timeout: 240000,
    maxBuffer: 16 * 1024 * 1024});
  assert.ifError(build.error);
  assert.equal(build.status, 0, build.stdout + build.stderr);
  const run = spawnSync(process.execPath, [output], {cwd: root, encoding: 'utf8',
    timeout: 30000, maxBuffer: 4 * 1024 * 1024});
  assert.ifError(run.error);
  assert.equal(run.status, 0, run.stdout + run.stderr);
  assert.equal(run.stdout.trim(), 'terminal carry identity checks passed');
});
