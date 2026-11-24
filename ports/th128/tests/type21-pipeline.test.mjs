import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
test('type21 actual profile drives Manager, script75 visual, update host, collision/freeze/cancellation', () => {
  const compiler = path.resolve(root, '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk = path.resolve(path.dirname(compiler), '../..');
  const python = path.join(sdk, 'python/3.13.3_64bit/python.exe');
  const scratch = path.join(root, 'artifacts/tests/type21-pipeline');
  fs.mkdirSync(scratch, {recursive: true});
  const sources = ['tests/native/type21-pipeline.cpp',
    ...fs.readdirSync(path.join(root, 'src/game')).filter(name => name.endsWith('.cpp')).map(name => `src/game/${name}`),
    'src/animation/AnmLogic.cpp', 'src/animation/AnmScene.cpp'];
  const rsp = path.join(scratch, 'sources.rsp');
  fs.writeFileSync(rsp, sources.map(source => `"${source}"`).join('\n') + '\n');
  const env = {...process.env,
    EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
    EM_CACHE: path.resolve(root, '../../work/emscripten-cache-4.0.22').replaceAll('\\', '/'),
    EMSDK_PYTHON: python, EMCC_CORES: '2', BINARYEN_CORES: '2'};
  const output = path.join(root, 'artifacts/tests/type21-pipeline.cjs');
  const build = spawnSync(python, [compiler, '@' + rsp,
    '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
    '-fno-fast-math', '-ffp-contract=off', '-fsanitize=undefined', '-fno-sanitize-recover=all',
    '-sENVIRONMENT=node', '-sNODERAWFS=1', '-sEXIT_RUNTIME=1', '-sALLOW_MEMORY_GROWTH=1', '-o', output],
    {cwd: root, env, encoding: 'utf8', timeout: 600000});
  assert.equal(build.status, 0, build.stdout + build.stderr);
  const base = path.join(root, 'local/generations/8c71c316a4b24e68827f9fc560242050');
  const profiles = ['ja', 'zh'].map(variant => JSON.parse(fs.readFileSync(
    path.join(base, variant, 'animations/bullet-type21-profile.json'), 'utf8')));
  for (const profile of profiles) {
    const bytes = Buffer.from(profile.hex, 'hex');
    assert.equal(bytes.length, 288);
    assert.equal(profile.sha256, '785afc6af751f17007ce0c8bc620f4fa0082e1320cfa36ba7769bef6ff87ccd5');
  }
  assert.deepEqual(profiles[0].hex, profiles[1].hex);
  const profilePath = path.join(scratch, 'type21-profile.bin');
  fs.writeFileSync(profilePath, Buffer.from(profiles[0].hex, 'hex'));
  const run = spawnSync(process.execPath, [output, profilePath, path.join(base, 'zh/raw/bullet.anm')],
    {cwd: root, encoding: 'utf8', timeout: 120000});
  assert.equal(run.status, 0, run.stdout + run.stderr);
  assert.match(run.stdout, /PASS type21 full Manager\/Visual\/Update\/collision\/freeze\/cancellation pipeline/);
});
