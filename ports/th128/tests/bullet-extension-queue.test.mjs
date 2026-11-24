import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('original extension queue waits, overlaps on control and chains completion before one final movement',()=>{
    const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/bullet-extension-queue-fixtures.json')));
    assert.equal(report.apiReturnsAssigned,false);
    assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/bullet_extension_queue_probe.py'))).digest('hex'),report.generatorSha256);
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    const output=path.join(root,'artifacts/tests/bullet-extension-queue.cjs');
    const build=spawnSync(python,[compiler,'tests/native/bullet-extension-queue.cpp','src/game/BulletUpdateHost.cpp','src/game/BulletManager.cpp',
        'src/game/BulletExtension16.cpp','src/game/BulletExtension4.cpp','src/game/BulletExtension1.cpp','src/game/BulletRules.cpp','src/game/BulletTickRules.cpp','src/game/BulletVisuals.cpp',
        'src/game/BulletCollisionRules.cpp','src/game/GrazeRules.cpp','src/animation/AnmLogic.cpp',
        '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
        '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'))),directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
    const run=spawnSync(process.execPath,[output,path.join(root,'local/runtime/bullet-profile-8.bin'),path.join(directory,'raw/bullet.anm')],
        {cwd:root,input:report.cases.map(sample=>sample.input.join(' ')).join('\n')+'\n',encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);
    assert.deepEqual(run.stdout.trim().split(/\r?\n/),report.cases.map(sample=>sample.expected.join(' ')));
});
