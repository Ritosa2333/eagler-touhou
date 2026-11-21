import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const bits=value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();};
test('original312 pair setup does not prematurely change polar values, mode4 easing retains exact float stores',()=>{
  assert.ok(fs.readFileSync(path.join(root,'src/game/EnemyRandomMovement.hpp'),'utf8').includes('beginRandomMovement'),'original312 pair interpolation is missing');
  const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/enemy-move312-fixtures.json'))),input=[],expected=[];
  for(const sample of report.initializationCases){
    const value=sample.after,track=value.trackWords;
    input.push(['I',sample.input.duration,sample.input.mode,0,bits(value.angle),bits(sample.input.speed)].join(' '));
    expected.push([track[0],track[1],track[2],track[3],track[8]|0,track[9],track[10],track[13],track[14],value.modeFlags,bits(.5),bits(1)].join(' '));
  }
  for(const sample of report.stepCases){
    const value=sample.after,track=value.trackWords;
    input.push(['T',sample.input.duration,sample.input.mode,sample.input.elapsed,bits(sample.input.angle),bits(sample.input.speed)].join(' '));
    expected.push([value.angleBits,value.speedBits,track[8]|0,track[9],track[10],track[13],track[14]].join(' '));
  }
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  for(const sanitize of [false,true]){
    const output=path.join(root,`artifacts/tests/enemy312-interpolation${sanitize?'-ubsan':''}.cjs`);
    const build=spawnSync(python,[compiler,'tests/native/enemy312-interpolation-vectors.cpp','src/game/EnemyRandomMovement.cpp','src/game/EnemyRules.cpp',
      '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off',
      ...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),'-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const run=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
  }
});
