import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const scratch = path.join(root, 'artifacts/dsh-perfect-freeze');

test('actual ice PerfectFreezeHost X gate, stock, protection, freeze/damage/effect services', () => {
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
    const output = path.join(scratch, 'perfect-freeze-host.cjs');
    const build = spawnSync(python, [compiler,
        'tests/native/perfect-freeze-host.cpp',
        'src/game/PerfectFreezeHost.cpp',
        'src/game/FreezeRules.cpp',
        'src/animation/AnmLogic.cpp',
        'src/animation/AnmScene.cpp',
        '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
        '-fno-fast-math', '-ffp-contract=off', '-fsanitize=undefined', '-fno-sanitize-recover=all',
        '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-sNODERAWFS=1', '-sALLOW_MEMORY_GROWTH=1',
        '-o', output], {cwd: root, env, encoding: 'utf8', timeout: 180000});
    assert.equal(build.status, 0, build.stdout + build.stderr);
    const catalog = JSON.parse(fs.readFileSync(path.join(root, 'local/catalog.json'), 'utf8'));
    const raw = path.join(path.dirname(path.join(root, 'local', catalog.variants.ja.manifest)), 'raw');
    const ice = path.join(raw, 'ice.anm');
    assert.ok(fs.existsSync(ice));
    const run = spawnSync(process.execPath, [output, ice], {cwd: root, encoding: 'utf8', timeout: 30000});
    assert.equal(run.status, 0, run.stdout + run.stderr);
    assert.match(run.stdout, /actual ice PerfectFreezeHost/);
});
