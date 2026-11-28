import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('portable actual-shot motion adapter matches every recorded native normalizer and FSINCOS result',()=>{
  assert.ok(fs.existsSync(path.join(root,'src/game/PlayerShotMotion.cpp')),'actual shot motion adapter is missing');
  const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/player-shot-birth-fixtures.json'),'utf8'));
  const cases=report.motionCases;
  assert.equal(cases.length,114);
  const inputLines=cases.map(sample=>{
    const raw=Buffer.from(sample.rawRecord,'hex');return `${raw.readUInt32LE(0x14)} ${raw.readUInt32LE(0x18)}`;
  });
  const expected=cases.map(sample=>[sample.next.angleBits,...sample.next.velocityBits].join(' '));
  const bounds=JSON.parse(fs.readFileSync(path.join(root,'local/research/player-shot-tick-fixtures.json'),'utf8'));
  for(const sample of bounds.rotatedBoundsCases){
    inputLines.push(`${sample.rotationBits} 1065353216`);
    expected.push([sample.rotationBits,...sample.basisEvidence.cosineSineBits,0].join(' '));
  }
  const input=inputLines.join('\n')+'\n';
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  fs.mkdirSync(path.join(root,'artifacts/tests'),{recursive:true});
  const output=path.join(root,'artifacts/tests/player-shot-motion.cjs');
  const build=spawnSync(python,[compiler,'tests/native/player-shot-motion.cpp','src/game/PlayerShotMotion.cpp','src/game/EnemyRules.cpp',
    '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off',
    '-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output],{cwd:root,input,encoding:'utf8',timeout:30000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
});
