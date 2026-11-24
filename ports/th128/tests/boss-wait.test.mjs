import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
test('original420 query checks only actual first-three owning boss words',()=>{
  assert.ok(fs.readFileSync(path.join(root,'src/game/EnemyRules.hpp'),'utf8').includes('bossWaitPending'),'original boss-owner query is missing');
  const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/boss-wait-fixtures.json')));
  assert.equal(report.executable.sha256,report.executable.sha256After);assert.equal(report.hostCallExecutedInCoreTail,false);
  assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/boss_wait_probe.py'))).digest('hex'),report.generatorSha256);
  const input=report.handlerCases.map(sample=>sample.owners.join(' ')).join('\n')+'\n';
  const expected=report.handlerCases.map(sample=>String(Number(sample.result===-1)));
  for(const sanitize of [false,true]){
    const output=path.join(root,`artifacts/tests/boss-wait${sanitize?'-ubsan':''}.cjs`);
    const build=spawnSync(python,[compiler,'tests/native/boss-wait-vectors.cpp','src/game/EnemyRules.cpp','-Isrc','-std=c++20','-O1',
      '-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off',...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),
      '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const run=spawnSync(process.execPath,[output],{cwd:root,input,encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
  }
});
test('real412-driven boss wait holds root only and resumes after owner cancellation',()=>{
  const output=path.join(root,'artifacts/tests/boss-wait-world.cjs');
  const build=spawnSync(python,[compiler,'tests/native/boss-wait-world.cpp','src/game/StageWorld.cpp','src/game/EnemyRules.cpp','src/game/EnemyRandomMovement.cpp',
    'src/game/EnemyPhaseRules.cpp','src/game/EnemyDropRules.cpp','src/game/BulletRules.cpp','src/game/BulletManager.cpp',
    'src/game/ShotEnemyCollisionRules.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp','-Isrc','-std=c++20','-O1',
    '-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
    '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8',timeout:30000});
  assert.equal(run.status,0,run.stdout+run.stderr);assert.match(run.stdout,/actual412 boss ownership gates420/);
});
