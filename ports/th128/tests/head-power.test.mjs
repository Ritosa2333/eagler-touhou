import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('head percent matches original position, thresholds, style7 color and scale',()=>{
 assert.ok(fs.existsSync(path.join(root,'src/game/HeadPower.hpp')),'actual head power presentation missing');
 const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/head-power-fixtures.json')));
 assert.deepEqual(report.glyphCases.map(sample=>sample.sprite),[258,259,260,261,262,263,264,265,266,267,269]);
 const bits=value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();};
 const input=report.cases.map(({input})=>[input.power,input.held,input.x,input.y].join(' ')).join('\n')+'\n';
 const expected=report.cases.map(({after})=>[after.percent,after.color,bits(after.scale),bits(after.x),bits(after.y)].join(' '));
 const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
 const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
 const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
 const output=path.join(root,'artifacts/tests/head-power.cjs');
 const build=spawnSync(python,[compiler,'tests/native/head-power-vectors.cpp','src/game/HeadPower.cpp','-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror',
 '-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
 assert.equal(build.status,0,build.stdout+build.stderr);
 const run=spawnSync(process.execPath,[output],{cwd:root,input,encoding:'utf8'});assert.equal(run.status,0,run.stdout+run.stderr);
 assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
});
