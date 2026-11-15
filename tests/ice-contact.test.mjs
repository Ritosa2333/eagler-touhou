import assert from 'node:assert/strict';import test from 'node:test';import fs from 'node:fs';import path from 'node:path';import {spawnSync} from 'node:child_process';import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('original ice contact lifetime/delay and inclusive radius boundary match complete helper',()=>{
 assert.ok(fs.existsSync(path.join(root,'src/game/IceContact.hpp')),'actual ice contact rules missing');
 const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/ice-contact-fixtures.json')));
 const input=report.cases.map(({input})=>[input.lifetime,input.delay,input.x,input.y,input.iceRadius,input.bulletRadius].join(' ')).join('\n')+'\n';
 const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py'),sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
 const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
 const output=path.join(root,'artifacts/tests/ice-contact.cjs');const build=spawnSync(python,[compiler,'tests/native/ice-contact-vectors.cpp','src/game/IceContact.cpp','-Isrc','-std=c++20','-O1','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
 assert.equal(build.status,0,build.stdout+build.stderr);const run=spawnSync(process.execPath,[output],{cwd:root,input,encoding:'utf8'});assert.equal(run.status,0,run.stdout+run.stderr);
 assert.deepEqual(run.stdout.trim().split(/\r?\n/),report.cases.map(sample=>String(Number(sample.selected))));
});
