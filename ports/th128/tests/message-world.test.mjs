import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';import {fileURLToPath} from 'node:url';import {spawnSync} from 'node:child_process';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('original418/419 use the live message gate and preserve the first Boss release before dialogue ends',()=>{
 const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py'),sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
 const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
 const output=path.join(root,'artifacts/tests/message-world.cjs');
 const sources=['src/game/StageWorld.cpp','src/game/MessageHost.cpp','src/game/EnemyRules.cpp','src/game/EnemyPhaseRules.cpp','src/game/EnemyDropRules.cpp','src/game/EnemyRandomMovement.cpp','src/game/ShotEnemyCollisionRules.cpp','src/game/BulletRules.cpp','src/game/BulletManager.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp'];
 const build=spawnSync(python,[compiler,'tests/native/message-world.cpp',...sources,'-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});assert.equal(build.status,0,build.stdout+build.stderr);
 const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'))),directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
 const run=spawnSync(process.execPath,[output,path.join(directory,'raw/st_a1.msg')],{cwd:root,encoding:'utf8',timeout:30000});assert.equal(run.status,0,run.stdout+run.stderr);
});
