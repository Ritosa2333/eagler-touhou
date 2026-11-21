import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('real SHT shot host creates authored records and ANMs only with explicit live callback bindings',()=>{
  assert.ok(fs.existsSync(path.join(root,'src/game/PlayerShotHost.cpp')),'actual player-shot birth host is missing');
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  fs.mkdirSync(path.join(root,'artifacts/tests'),{recursive:true});const output=path.join(root,'artifacts/tests/player-shot-host.cjs');
  const build=spawnSync(python,[compiler,'tests/native/player-shot-host.cpp','src/game/PlayerShotHost.cpp','src/game/PlayerShotMotion.cpp',
    'src/game/PlayerShotBirthRules.cpp','src/game/PlayerShotTickRules.cpp','src/game/ShotEnemyCollisionRules.cpp','src/game/EnemyRules.cpp','src/game/DamageFieldPool.cpp','src/animation/AnmLogic.cpp','-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions',
    '-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sNODERAWFS=1',
    '-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8')),
    raw=path.join(path.dirname(path.join(root,'local',catalog.variants.ja.manifest)),'raw');
  const run=spawnSync(process.execPath,[output,path.join(raw,'pl00a.sht'),path.join(raw,'pl00.anm')],{cwd:root,encoding:'utf8',timeout:30000});
  assert.equal(run.status,0,run.stdout+run.stderr);
});
