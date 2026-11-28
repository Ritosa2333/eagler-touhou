import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const compiler=path.resolve(process.env.TH128_EMCC??path.join(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py'));
const sdk=path.resolve(path.dirname(compiler),'../..');
const python=process.env.TH128_PYTHON??path.join(sdk,'python/3.13.3_64bit/python.exe');
const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
  EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2',PYTHONUTF8:'1'};

test('world reaches original first birth through linked ECL and stops unimplemented behavior',()=>{
  fs.mkdirSync(path.join(root,'artifacts/tests'),{recursive:true});
  const output=path.join(root,'artifacts/tests/stage-world.cjs');
  const build=spawnSync(python,[compiler,'tests/native/stage-world.cpp','src/game/StageWorld.cpp','src/game/EnemyRandomMovement.cpp','src/game/EnemyRules.cpp','src/game/EnemyPhaseRules.cpp','src/game/EnemyDropRules.cpp','src/game/BulletRules.cpp','src/game/BulletManager.cpp','src/game/ShotEnemyCollisionRules.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp',
    '-Isrc','-std=c++20','-O1','-fno-fast-math','-ffp-contract=off','-fexceptions','-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const local=path.join(root,'local');
  const catalog=JSON.parse(fs.readFileSync(path.join(local,'catalog.json'),'utf8'));
  const manifestPath=path.join(local,catalog.variants.ja.manifest);
  const manifest=JSON.parse(fs.readFileSync(manifestPath,'utf8'));
  const files=manifest.ecl.map(item=>path.join(path.dirname(manifestPath),'raw',item.name));
  const result=spawnSync(process.execPath,[output,...files],{cwd:root,encoding:'utf8',timeout:30000});
  assert.equal(result.status,0,result.stdout+result.stderr);
  assert.match(result.stdout,/actual script-driven birth/);
  console.log(result.stdout);
});
