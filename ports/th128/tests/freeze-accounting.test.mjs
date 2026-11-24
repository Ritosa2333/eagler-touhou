import assert from 'node:assert/strict';import test from 'node:test';import fs from 'node:fs';import path from 'node:path';import {spawnSync} from 'node:child_process';import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('original mask-area bonus and bounded raw score match native conversion/branches',()=>{
 assert.ok(fs.existsSync(path.join(root,'src/game/FreezeAccounting.hpp')),'original freeze completion accounting missing');
 const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/freeze-bonus-fixtures.json')));
 const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py'),sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
 const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
 const output=path.join(root,'artifacts/tests/freeze-accounting.cjs'),build=spawnSync(python,[compiler,'tests/native/freeze-accounting.cpp','src/game/FreezeAccounting.cpp',
 '-Isrc','-std=c++20','-O1','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
 assert.equal(build.status,0,build.stdout+build.stderr);
 const input=report.cases.map(sample=>[sample.input.pixels,sample.input.score].join(' ')).join('\n')+'\n';
 const run=spawnSync(process.execPath,[output],{cwd:root,input,encoding:'utf8'});assert.equal(run.status,0,run.stdout+run.stderr);
 const actual=run.stdout.trim().split(/\r?\n/).map(line=>line.split(' ').map(Number));
 assert.equal(actual.length,report.cases.length);actual.forEach(([percent,bonus,score],index)=>{
  assert.equal(percent,report.cases[index].percent);assert.equal(bonus,report.cases[index].bonus);assert.equal(score,report.cases[index].score);
 });
});
