import assert from 'node:assert/strict';
import test from 'node:test';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const compiler=path.resolve(process.env.TH128_EMCC??path.join(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py'));
const sdk=path.resolve(path.dirname(compiler),'../..');
const python=process.env.TH128_PYTHON??path.join(sdk,'python/3.13.3_64bit/python.exe');
const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2',PYTHONUTF8:'1'};
const build=(source,output,extra)=>{const r=spawnSync(python,[compiler,source,...extra,'-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off','-fexceptions','-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:240000});assert.equal(r.status,0,r.stdout+r.stderr);};
test('mode11 interpolation curve, timer, quantized velocity and 240/60 endpoints',()=>{
    const output=path.join(root,'artifacts/tests/enemy301-mode11.cjs');
    build('tests/native/enemy301-mode11.cpp',output,['src/game/EnemyRules.cpp']);
    const r=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8',timeout:30000});assert.equal(r.status,0,r.stdout+r.stderr);assert.match(r.stdout,/END -130 160/);
});
test('actual StageWorld301 mode11 reaches authored and mirrored endpoints',()=>{
    const output=path.join(root,'artifacts/tests/enemy301-mode11-world.cjs');
    build('tests/native/enemy301-mode11-world.cpp',output,['src/game/StageWorld.cpp','src/game/EnemyRandomMovement.cpp','src/game/EnemyRules.cpp','src/game/EnemyPhaseRules.cpp','src/game/EnemyDropRules.cpp','src/game/BulletRules.cpp','src/game/BulletManager.cpp','src/game/EnemyBulletCallbackRules.cpp','src/game/ShotEnemyCollisionRules.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp']);
    const r=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8',timeout:30000});assert.equal(r.status,0,r.stdout+r.stderr);assert.match(r.stdout,/PASS mode11 StageWorld 240\/60\/mirrored/);
});
