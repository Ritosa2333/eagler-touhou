import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
test('Perfect world owner prepares real ice/pl00 protection and phase120 ownership', () => {
  const source = fs.readFileSync(path.join(root, 'src/runtime/WorldExports.cpp'), 'utf8');
  assert.match(source, /perfectFreezeNormalEmptyLasers\\\":false/);
  assert.match(source, /assignPerfectProtection200/);
  assert.match(source, /settlePerfectFreezeArea/);
  assert.match(source, /perfectHost->tick\(false/);
  assert.match(source, /perfectFreeze\\\":/);
  assert.match(source, /lastDenied/);
  assert.match(source, /perfectComplete/);

  const compiler = path.resolve(root, '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk = path.resolve(path.dirname(compiler), '../..');
  const python = path.join(sdk, 'python/3.13.3_64bit/python.exe');
  assert.ok(fs.existsSync(compiler) && fs.existsSync(python));
  const scratch = path.join(root, 'artifacts/tests/perfect-world-owner');
  fs.mkdirSync(path.join(scratch, 'emcache'), {recursive: true});
  fs.mkdirSync(path.join(scratch, 'emcc-tmp'), {recursive: true});
  const env = {...process.env,
    EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
    EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
    EMSDK_PYTHON: python, EMCC_CORES: '2', BINARYEN_CORES: '2'};
  const output = path.join(root, 'artifacts/tests/perfect-world-owner.cjs');
  const sources = ['src/game/PerfectFreezeHost.cpp', 'src/game/FreezeRules.cpp',
    'src/animation/AnmLogic.cpp', 'src/animation/AnmScene.cpp',
    'src/game/IceHost.cpp', 'src/game/IceBirthRules.cpp', 'src/game/IceMaskRules.cpp', 'src/game/IceContact.cpp',
    'src/game/BulletManager.cpp', 'src/game/BulletVisuals.cpp', 'src/game/BulletRules.cpp',
    'src/game/FreezeAccounting.cpp', 'src/game/PlayerHitHost.cpp', 'src/game/PlayerHitRules.cpp',
    'src/game/PlayerRules.cpp', 'src/game/PlayerDeathRules.cpp', 'src/game/PlayerRespawnRules.cpp'];
  const build = spawnSync(python, [compiler, 'tests/native/perfect-world-owner.cpp', ...sources,
    '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
    '-fno-fast-math', '-ffp-contract=off', '-fsanitize=undefined', '-fno-sanitize-recover=all',
    '-sENVIRONMENT=node', '-sNODERAWFS=1', '-sEXIT_RUNTIME=1', '-sALLOW_MEMORY_GROWTH=1', '-o', output],
    {cwd: root, env, encoding: 'utf8', timeout: 300000});
  assert.equal(build.status, 0, build.stdout + build.stderr);

  const catalog = JSON.parse(fs.readFileSync(path.join(root, 'local/catalog.json'), 'utf8'));
  const directory = path.dirname(path.join(root, 'local', catalog.variants.ja.manifest));
  const masks = JSON.parse(fs.readFileSync(path.join(directory, 'animations/ice-masks.json')));
  const maskFile = path.join(scratch, 'masks.bin');
  fs.writeFileSync(maskFile, Buffer.concat(masks.profiles.map(profile => Buffer.from(profile.hex, 'hex'))));
  const type25 = JSON.parse(fs.readFileSync(path.join(directory, 'animations/bullet-type25-profile.json')));
  const type13 = JSON.parse(fs.readFileSync(path.join(directory, 'animations/bullet-type13-profile.json')));
  const type25File = path.join(scratch, 'type25.bin');
  const type13File = path.join(scratch, 'type13.bin');
  fs.writeFileSync(type25File, Buffer.from(type25.hex, 'hex'));
  fs.writeFileSync(type13File, Buffer.from(type13.hex, 'hex'));
  const run = spawnSync(process.execPath, [output,
    path.join(directory, 'raw/ice.anm'), path.join(directory, 'raw/bullet.anm'), maskFile,
    type25File, type13File, path.join(directory, 'raw/pl00.anm')],
    {cwd: root, encoding: 'utf8', timeout: 120000});
  assert.equal(run.status, 0, run.stdout + run.stderr);
  assert.match(run.stdout, /PASS perfect world owner/);
});
