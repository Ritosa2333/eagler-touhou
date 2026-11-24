import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';import {fileURLToPath} from 'node:url';import {spawnSync} from 'node:child_process';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('world437/423 use an owning active spell host, expanded HP, original title bytes and real completion',()=>{
 const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py'),sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
 const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
 const sources=['src/game/StageWorld.cpp','src/game/EnemyRules.cpp','src/game/EnemyPhaseRules.cpp','src/game/EnemyDropRules.cpp','src/game/EnemyRandomMovement.cpp','src/game/ShotEnemyCollisionRules.cpp','src/game/BulletRules.cpp','src/game/BulletManager.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp'];
 const output=path.join(root,'artifacts/tests/spell-world.cjs');const build=spawnSync(python,[compiler,'tests/native/spell-world.cpp',...sources,'-Isrc','-std=c++20','-O1','-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});assert.equal(build.status,0,build.stdout+build.stderr);
 const run=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8'});assert.equal(run.status,0,run.stdout+run.stderr);
});
