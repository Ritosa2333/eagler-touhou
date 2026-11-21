import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const compiler = path.resolve(root, '../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk = path.resolve(path.dirname(compiler), '../..');
const python = path.join(sdk, 'python/3.13.3_64bit/python.exe');
const scratch = path.join(root, 'artifacts/dsh-type13-bullets');

test('type13 original profile, remap, birth, ANM, freeze and typed tick', () => {
    assert.ok(fs.existsSync(compiler) && fs.existsSync(python));
    const fixture = JSON.parse(fs.readFileSync(path.join(root, 'local/research/type13-bullet-fixtures.json'), 'utf8'));
    assert.equal(fixture.schema, 'th128-original-type13-bullet/1');
    assert.equal(fixture.profile.script, 48);
    assert.equal(fixture.profile.secondaryScript, 0);
    assert.equal(fixture.profile.hitboxSize, 2.4000000953674316);
    assert.equal(fixture.profile.auxiliarySize, 16);
    assert.equal(fixture.profile.appearance, 4);
    assert.equal(fixture.profile.selector, 6);
    assert.equal(fixture.profile.freezePolicy, 0);
    assert.deepEqual(fixture.profile.spriteRemapColor6, [246, 326, 342, 349]);
    assert.equal(fixture.profile.cancellationScriptColor6, 349);
    assert.equal(fixture.profile.sha256, createHash('sha256').update(Buffer.from(fixture.profile.hex, 'hex')).digest('hex'));
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
    const output = path.join(scratch, 'type13-bullets.cjs');
    const sources = ['src/game/BulletManager.cpp', 'src/game/BulletRules.cpp', 'src/game/BulletTickRules.cpp',
        'src/game/BulletVisuals.cpp', 'src/game/BulletUpdateHost.cpp', 'src/game/BulletCollisionRules.cpp',
        'src/game/GrazeRules.cpp', 'src/game/IceContact.cpp', 'src/game/BulletExtension1.cpp',
        'src/game/BulletExtension4.cpp', 'src/game/BulletExtension16.cpp', 'src/animation/AnmLogic.cpp'];
    const build = spawnSync(python, [compiler, 'tests/native/type13-bullets.cpp', ...sources, '-Isrc',
        '-std=c++20', '-O1', '-Wall', '-Wextra', '-Werror', '-fexceptions', '-fno-fast-math',
        '-ffp-contract=off', '-fsanitize=undefined', '-fno-sanitize-recover=all',
        '-sENVIRONMENT=node', '-sNODERAWFS=1', '-sEXIT_RUNTIME=1', '-sALLOW_MEMORY_GROWTH=1', '-o', output],
        {cwd: root, env, encoding: 'utf8', timeout: 240000});
    assert.equal(build.status, 0, build.stdout + build.stderr);
    const profilePath = path.join(scratch, 'type13-profile.bin');
    fs.writeFileSync(profilePath, Buffer.from(fixture.profile.hex, 'hex'));
    const animation = path.join(root, 'artifacts/dsh-readonly-inputs/bullet.anm');
    assert.equal(createHash('sha256').update(fs.readFileSync(animation)).digest('hex'),
        '9860822687f33e740f2a5e1fb25a4b05cf27b0fc6d15ed9626fb903986f7784c');
    const run = spawnSync(process.execPath, [output, profilePath, animation], {cwd: root, encoding: 'utf8', timeout: 120000});
    assert.equal(run.status, 0, run.stdout + run.stderr);
    assert.match(run.stdout, /PASS type13 profile, birth, remap, single ANM, freeze and tick/);
});
