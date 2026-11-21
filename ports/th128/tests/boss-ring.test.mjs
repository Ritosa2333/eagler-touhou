import assert from 'node:assert/strict';import test from 'node:test';import fs from 'node:fs';import path from 'node:path';import {spawnSync} from 'node:child_process';import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..'),bits=value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();};
test('524 native radius feeds actual typed4 births and ETEX1 burst-speed lifecycle instead of an ignored emitter store',()=>{
 assert.ok(fs.existsSync(path.join(root,'src/game/BulletExtension1.hpp')),'actual ETEX1 owning rule missing');
 const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/boss-ring-fixtures.json')));
 const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py'),sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
 const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
 const output=path.join(root,'artifacts/tests/boss-ring.cjs'),build=spawnSync(python,[compiler,'tests/native/boss-ring.cpp','src/game/BulletRules.cpp','src/game/BulletExtension1.cpp',
 '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
 assert.equal(build.status,0,build.stdout+build.stderr);const input=[],expected=[];
 for(const sample of report.geometry){input.push(['G',bits(sample.angle),bits(sample.radius)].join(' '));expected.push([...sample.positionBits,...sample.velocityBits].join(' '));}
 for(const sample of report.extension1Steps){input.push(['T',sample.time,bits(sample.angle)].join(' '));const state=sample.after;
  expected.push([Number((state.active&1)!==0),state.timer[0]|0,state.timer[1]|0,state.timer[2],...state.velocityBits].join(' '));}
 const run=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8'});assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
});
