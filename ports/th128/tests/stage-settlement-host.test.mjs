import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const scratch = path.join(root, 'artifacts/dsh-stage-settlement');
const cache = path.join(scratch, 'emcache');
const temporary = path.join(scratch, 'emcc-tmp');

test('actual front82 stage-clear ownership, score table and required owner chains', () => {
    assert.ok(fs.existsSync(path.join(root, 'src/game/StageSettlementHost.cpp')));
    assert.ok(fs.existsSync(path.join(root, 'tests/native/stage-settlement-host.cpp')));
    const fixture = path.join(root, 'local/research/stage-settlement-fixtures.json');
    const front = path.join(root, 'artifacts/dsh-readonly-inputs/front.anm');
    assert.ok(fs.existsSync(fixture));
    assert.ok(fs.existsSync(front));
    const compiler = path.resolve(root, '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    assert.ok(fs.existsSync(compiler), `required compiler is missing: ${compiler}`);
    const sdk = path.resolve(path.dirname(compiler), '../..');
    const python = path.join(sdk, 'python/3.13.3_64bit/python.exe');
    assert.ok(fs.existsSync(python), `required compiler Python is missing: ${python}`);
    fs.mkdirSync(cache, {recursive: true});
    fs.mkdirSync(temporary, {recursive: true});
    const env = {
        ...process.env,
        EM_CONFIG: path.join(sdk, '.emscripten').replaceAll('\\', '/'),
        EM_CACHE: cache.replaceAll('\\', '/'),
        EMSDK_PYTHON: python,
        EMCC_TEMP_DIR: temporary.replaceAll('\\', '/'),
        EMCC_SCRIPT: compiler.replaceAll('\\', '/'),
        EMCC_CORES: '2',
        BINARYEN_CORES: '2',
    };
    const output = path.join(scratch, 'stage-settlement-host.cjs');
    const argumentsList = [
        'tests/native/stage-settlement-host.cpp',
        'src/game/StageSettlementHost.cpp',
        'src/animation/AnmLogic.cpp',
        'src/animation/AnmScene.cpp',
        '-Isrc', '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions',
        '-fno-fast-math', '-ffp-contract=off', '-fsanitize=undefined', '-fno-sanitize-recover=all',
        '-sENVIRONMENT=node', '-sEXIT_RUNTIME=1', '-sNODERAWFS=1', '-sALLOW_MEMORY_GROWTH=1',
        '-o', output,
    ];
    let build;
    if (process.platform === 'win32') {
        const wrapper = path.join(scratch, 'build-stage-settlement.ps1');
        const quoted = argumentsList.map(value => `'${value.replaceAll("'", "''")}'`).join(',');
        fs.writeFileSync(wrapper,
            `$ErrorActionPreference='Stop'\r\n& $env:EMSDK_PYTHON $env:EMCC_SCRIPT @(${quoted})\r\nexit $LASTEXITCODE\r\n`);
        build = spawnSync('powershell.exe',
            ['-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', wrapper],
            {cwd: root, env, stdio: 'inherit', timeout: 180000});
    } else {
        build = spawnSync(python, [compiler, ...argumentsList],
            {cwd: root, env, stdio: 'inherit', timeout: 180000});
    }
    assert.equal(build.status, 0, build.error?.message);
    const run = spawnSync(process.execPath, [output, front, fixture],
        {cwd: root, stdio: 'inherit', timeout: 30000});
    assert.equal(run.status, 0, run.error?.message);
});
