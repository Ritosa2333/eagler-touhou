import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const scratch = path.join(root, 'artifacts/dsh-player-shot-groups');

test('actual pl00a SHT player shot groups create real nonzero 8x2 Scene/VM ownership', () => {
    assert.ok(fs.existsSync(path.join(root, 'src/game/PlayerShotGroups.cpp')));
    const compiler = path.resolve(root, '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    assert.ok(fs.existsSync(compiler), `required compiler is missing: ${compiler}`);
    const sdk = path.resolve(path.dirname(compiler), '../..');
    const python = path.join(sdk, 'python/3.13.3_64bit/python.exe');
    assert.ok(fs.existsSync(python), `required compiler Python is missing: ${python}`);
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
    const output = path.join(scratch, 'player-shot-groups.cjs');
    const build = spawnSync(python, [compiler,
        'tests/native/player-shot-groups.cpp',
        'src/game/PlayerShotGroups.cpp',
        'src/game/PlayerShotBirthRules.cpp',
        'src/animation/AnmLogic.cpp',
        'src/animation/AnmScene.cpp',
        '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
        '-fno-fast-math', '-ffp-contract=off', '-fsanitize=undefined', '-fno-sanitize-recover=all',
        '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-sNODERAWFS=1', '-sALLOW_MEMORY_GROWTH=1',
        '-o', output], {cwd: root, env, encoding: 'utf8', timeout: 180000});
    assert.equal(build.status, 0, build.stdout + build.stderr);
    const catalog = JSON.parse(fs.readFileSync(path.join(root, 'local/catalog.json'), 'utf8'));
    const raw = path.join(path.dirname(path.join(root, 'local', catalog.variants.ja.manifest)), 'raw');
    const sht = path.join(raw, 'pl00a.sht');
    const animation = path.join(raw, 'pl00.anm');
    assert.ok(fs.existsSync(sht) && fs.existsSync(animation));
    const run = spawnSync(process.execPath, [output, sht, animation],
        {cwd: root, encoding: 'utf8', timeout: 30000});
    assert.equal(run.status, 0, run.stdout + run.stderr);
    assert.match(run.stdout, /actual pl00a SHT player shot groups/);
});
