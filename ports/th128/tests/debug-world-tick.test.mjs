import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
test('actual WorldExports tick infinite-respawn boundary, default GameOver and taint/reset', () => {
  const compiler = path.resolve(root, '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk = path.resolve(path.dirname(compiler), '../..');
  const python = path.join(sdk, 'python/3.13.3_64bit/python.exe');
  assert.ok(fs.existsSync(compiler) && fs.existsSync(python));
  const scratch = path.join(root, 'artifacts/tests/debug-world-tick');
  fs.mkdirSync(scratch, {recursive: true});
  const sources = ['tests/native/debug-world-tick.cpp',
    ...fs.readdirSync(path.join(root, 'src/game')).filter(name => name.endsWith('.cpp')).map(name => `src/game/${name}`),
    'src/animation/AnmLogic.cpp', 'src/animation/AnmScene.cpp', 'src/ecl/EclProgram.cpp', 'src/ecl/EclVm.cpp'];
  const rsp = path.join(scratch, 'sources.rsp');
  fs.writeFileSync(rsp, sources.map(source => `"${source}"`).join('\n') + '\n');
  const env = {...process.env,
    EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
    EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
    EMSDK_PYTHON: python, EMCC_CORES: '2', BINARYEN_CORES: '2'};
  const output = path.join(root, 'artifacts/tests/debug-world-tick.cjs');
  const build = spawnSync(python, [compiler, '@' + rsp,
    '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
    '-fno-fast-math', '-ffp-contract=off', '-fsanitize=undefined', '-fno-sanitize-recover=all',
    '-sENVIRONMENT=node', '-sNODERAWFS=1', '-sEXIT_RUNTIME=1', '-sALLOW_MEMORY_GROWTH=1', '-o', output],
    {cwd: root, env, encoding: 'utf8', timeout: 600000});
  assert.equal(build.status, 0, build.stdout + build.stderr);
  const catalog = JSON.parse(fs.readFileSync(path.join(root, 'local/catalog.json'), 'utf8'));
  const directory = path.dirname(path.join(root, 'local', catalog.variants.ja.manifest));
  const profile = path.join(root, 'local/runtime/bullet-profile-8.bin');
  const run = spawnSync(process.execPath, [output, directory, profile],
    {cwd: root, encoding: 'utf8', timeout: 180000});
  assert.equal(run.status, 0, run.stdout + run.stderr);
  assert.match(run.stdout, /PASS debug world tick/);
});
