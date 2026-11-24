import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('actual MainSub03 float globals survive child spawn and reset',()=>{
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
    EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2',PYTHONUTF8:'1'};
  const output=path.join(root,'artifacts/tests/ecl-global-9979.cjs');
  const build=spawnSync(python,[compiler,'tests/native/ecl-global-9979.cpp','src/game/StageWorld.cpp',
    'src/game/EnemyRandomMovement.cpp','src/game/EnemyRules.cpp','src/game/EnemyPhaseRules.cpp',
    'src/game/EnemyDropRules.cpp','src/game/EnemyBulletCallbackRules.cpp','src/game/BulletRules.cpp','src/game/BulletManager.cpp',
    'src/game/ShotEnemyCollisionRules.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp','-Isrc',
    '-std=c++20','-O1','-fno-fast-math','-ffp-contract=off','-fexceptions','-fsanitize=undefined',
    '-fno-sanitize-recover=all','-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1',
    '-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:300000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8',timeout:30000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS actual opcode45 main writes/);
});
