import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const bits=value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();};
test('actual312 direction keeps area/player branch thresholds, near-vertical avoidance and exact2/3 RNG calls',()=>{
  assert.ok(fs.existsSync(path.join(root,'src/game/EnemyRandomMovement.hpp')),'original312 randomized movement rules are missing');
  const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/enemy-move312-fixtures.json')));
  assert.equal(report.executable.sha256,report.executable.sha256After);assert.equal(report.apiReturnsAssigned,false);
  assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/enemy_move312_probe.py'))).digest('hex'),report.generatorSha256);
  const input=report.initializationCases.map(({input})=>[input.seed,input.randomCalls,bits(input.position[0]),bits(input.position[1]),
    bits(input.playerX),...input.area.map(bits)].join(' ')).join('\n')+'\n';
  const expected=report.initializationCases.map(({after})=>[bits(after.angle),after.seed,after.randomCalls].join(' '));
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  for(const sanitize of [false,true]){
    const output=path.join(root,`artifacts/tests/enemy312-direction${sanitize?'-ubsan':''}.cjs`);
    const build=spawnSync(python,[compiler,'tests/native/enemy312-direction-vectors.cpp','src/game/EnemyRandomMovement.cpp','src/game/EnemyRules.cpp',
      '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off',
      ...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),'-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const run=spawnSync(process.execPath,[output],{cwd:root,input,encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
  }
});
