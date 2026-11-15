import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('owning526 host executes authored ECL and geometry work without skipping missing-host boundaries',()=>{
    assert.match(fs.readFileSync(path.join(root,'src/game/StageWorld.hpp'),'utf8'),/setGeometryHost/,'owning526 world lifecycle is not installed');
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    const output=path.join(root,'artifacts/tests/geometry526-world.cjs');
    const build=spawnSync(python,[compiler,'tests/native/geometry526-world-integration.cpp','src/game/StageWorld.cpp','src/game/EnemyRandomMovement.cpp',
        'src/game/EnemyRules.cpp','src/game/EnemyPhaseRules.cpp','src/game/EnemyDropRules.cpp','src/game/BulletRules.cpp',
        'src/game/BulletManager.cpp','src/game/ShotEnemyCollisionRules.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp',
        'src/game/Geometry526.cpp','src/game/Geometry526Host.cpp','src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp',
        '-Isrc','-std=c++20','-O1','-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
        '-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],
        {cwd:root,env,encoding:'utf8',timeout:240000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json')));
    const resource=path.join(root,'local',path.dirname(catalog.variants.ja.manifest),'raw/text.anm');
    const run=spawnSync(process.execPath,[output,resource],{cwd:root,encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);assert.match(run.stdout,/owning526 ECL/);
});
