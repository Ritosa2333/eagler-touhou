import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const scratch = path.join(root, 'artifacts/dsh-enemy-bullet-callback-rules');

test('529 selector2 typed pure rules match native distance/angle/variable domain', () => {
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
    const output = path.join(scratch, 'enemy-bullet-callback-rules.cjs');
    const module = path.join(scratch, 'EnemyBulletCallbackRules.cpp');
    const build = spawnSync(python, [compiler, 'tests/native/enemy-bullet-callback-rules.cpp',
        module, `-I${scratch}`, '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra',
        '-Werror', '-fexceptions', '-fno-fast-math', '-ffp-contract=off', '-fsanitize=undefined',
        '-fno-sanitize-recover=all', '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1',
        '-sALLOW_MEMORY_GROWTH=1', '-o', output], {cwd: root, env, encoding: 'utf8', timeout: 240000});
    assert.equal(build.status, 0, build.stdout + build.stderr);
    const run = spawnSync(process.execPath, [output], {cwd: root, encoding: 'utf8', timeout: 30000});
    assert.equal(run.status, 0, run.stdout + run.stderr);
    assert.match(run.stdout, /PASS enemy bullet callback selector2\/0 typed rules/);
});
