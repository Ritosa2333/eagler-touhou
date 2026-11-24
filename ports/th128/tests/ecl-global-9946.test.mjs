import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const scratch = path.join(root, 'artifacts/dsh-ecl-global-9946');

test('actual st_a1_2 MainSub03 -9946 reads active enemy node count', () => {
    const compiler = path.resolve(root, '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk = path.resolve(path.dirname(compiler), '../..');
    const python = path.join(sdk, 'python/3.13.3_64bit/python.exe');
    assert.ok(fs.existsSync(compiler) && fs.existsSync(python));
    fs.mkdirSync(path.join(scratch, 'emcache'), {recursive: true});
    fs.mkdirSync(path.join(scratch, 'emcc-tmp'), {recursive: true});
    const env = {
        ...process.env,
        EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
        EM_CACHE: path.join(scratch, 'emcache').replaceAll('\\', '/'),
        EMCC_TEMP_DIR: path.join(scratch, 'emcc-tmp').replaceAll('\\', '/'),
        EMSDK_PYTHON: python,
        EMCC_CORES: '2',
        BINARYEN_CORES: '2',
    };
    const output = path.join(scratch, 'ecl-global-9946.cjs');
    const build = spawnSync(python, [compiler,
        'tests/native/ecl-global-9946.cpp',
        'src/game/StageWorld.cpp',
        'src/game/EnemyRandomMovement.cpp',
        'src/game/EnemyRules.cpp',
        'src/game/EnemyPhaseRules.cpp',
        'src/game/EnemyDropRules.cpp',
        'src/game/BulletRules.cpp',
        'src/game/BulletManager.cpp',
        'src/game/ShotEnemyCollisionRules.cpp',
        'src/ecl/EclProgram.cpp',
        'src/ecl/EclVm.cpp',
        '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
        '-fno-fast-math', '-ffp-contract=off', '-sENVIRONMENT=node', '-sNODERAWFS=1',
        '-sEXIT_RUNTIME=1', '-sALLOW_MEMORY_GROWTH=1', '-o', output],
        {cwd: root, env, encoding: 'utf8', timeout: 240000});
    assert.equal(build.status, 0, build.stdout + build.stderr);
    const run = spawnSync(process.execPath, [output], {cwd: root, encoding: 'utf8', timeout: 30000});
    assert.equal(run.status, 0, run.stdout + run.stderr);
    assert.match(run.stdout, /actual st_a1_2 MainSub03 offset152 -9946 count boundary passed/);
});
