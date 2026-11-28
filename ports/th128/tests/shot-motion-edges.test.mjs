import assert from 'node:assert/strict';import test from 'node:test';import fs from 'node:fs';import path from 'node:path';import {fileURLToPath} from 'node:url';import {spawnSync} from 'node:child_process';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('ordinary hit-slowed shots at nonzero X keep real native tiny lateral velocity without a false unsupported stop',()=>{
 const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/shot-motion-edge-fixtures.json')));
 const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py'),sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
 const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
 const output=path.join(root,'artifacts/tests/shot-motion-edges.cjs'),build=spawnSync(python,[compiler,'tests/native/shot-motion-edges.cpp','src/game/PlayerShotTickRules.cpp','-Isrc','-std=c++20','-O1',
 '-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
 assert.equal(build.status,0,build.stdout+build.stderr);
 const input=report.cases.map(sample=>{const bytes=Buffer.from(sample.next,'hex');return [sample.before,...[0x20,0x24,0x28].map(offset=>bytes.readUInt32LE(offset))].join(' ');}).join('\n')+'\n';
 const run=spawnSync(process.execPath,[output],{cwd:root,input,encoding:'utf8'});assert.equal(run.status,0,run.stdout+run.stderr);
 assert.deepEqual(run.stdout.trim().split(/\r?\n/),report.cases.map(sample=>[0x14,0x18,0x1c].map(offset=>Buffer.from(sample.next,'hex').readUInt32LE(offset)).join(' ')));
});
