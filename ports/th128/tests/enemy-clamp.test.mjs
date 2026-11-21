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
const bits=value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();};
test('complete original416030 combined quantization, velocity, clamp and absolute rebuild match every word',()=>{
  assert.ok(fs.readFileSync(path.join(root,'src/game/EnemyRules.hpp'),'utf8').includes('combinePositionWithArea'),'actual404 combined-position clamp is missing');
  const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/enemy-clamp-fixtures.json')));
  assert.equal(report.executable.sha256,report.executable.sha256After);assert.equal(report.apiReturnsAssigned,false);
  assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/enemy_clamp_probe.py'))).digest('hex'),report.generatorSha256);
  const input=report.combineCases.map(({input})=>[input.flags,...input.absolute.map(bits),...input.relative.map(bits),...input.combined.map(bits),...input.area.map(bits)].join(' ')).join('\n')+'\n';
  const expected=report.combineCases.map(({after})=>[...after.combined,...after.velocity,...after.absolute].join(' '));
  for(const sanitize of [false,true]){
    const output=path.join(root,`artifacts/tests/enemy-clamp${sanitize?'-ubsan':''}.cjs`);
    const build=spawnSync(python,[compiler,'tests/native/enemy-clamp-vectors.cpp','src/game/EnemyRules.cpp','-Isrc','-std=c++20','-O1',
      '-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off',...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),
      '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const run=spawnSync(process.execPath,[output],{cwd:root,input,encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/).map(line=>line.trim()),expected);
  }
});
test('authored404 clamps after real movement/combine, keeps constructor guard and does not bypass transform flags',()=>{
  const output=path.join(root,'artifacts/tests/enemy-clamp-world.cjs');
  const build=spawnSync(python,[compiler,'tests/native/enemy-clamp-world.cpp','src/game/StageWorld.cpp','src/game/EnemyRules.cpp','src/game/EnemyRandomMovement.cpp',
    'src/game/EnemyPhaseRules.cpp','src/game/EnemyDropRules.cpp','src/game/BulletRules.cpp','src/game/BulletManager.cpp',
    'src/game/ShotEnemyCollisionRules.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp','-Isrc','-std=c++20','-O1',
    '-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1',
    '-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8',timeout:30000});
  assert.equal(run.status,0,run.stdout+run.stderr);assert.match(run.stdout,/actual404 registration/);
});
