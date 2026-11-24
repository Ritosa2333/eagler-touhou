import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('actual312 owning world requires a genuine player snapshot and updates next-pass motion with original RNG',()=>{
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  const output=path.join(root,'artifacts/tests/enemy312-world.cjs');
  const build=spawnSync(python,[compiler,'tests/native/enemy312-world.cpp','src/game/StageWorld.cpp','src/game/EnemyRules.cpp',
    'src/game/EnemyRandomMovement.cpp','src/game/EnemyPhaseRules.cpp','src/game/EnemyDropRules.cpp','src/game/BulletRules.cpp','src/game/BulletManager.cpp',
    'src/game/ShotEnemyCollisionRules.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp','-Isrc','-std=c++20','-O1','-fexceptions','-fno-fast-math','-ffp-contract=off',
    '-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8',timeout:30000});
  assert.equal(run.status,0,run.stdout+run.stderr);assert.match(run.stdout,/actual312 owning world/);
});
