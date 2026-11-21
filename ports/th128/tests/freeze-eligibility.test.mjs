import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';import {fileURLToPath} from 'node:url';import {spawnSync} from 'node:child_process';import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('native freeze eligibility reads independent profile118 rather than large-ball appearance10c',()=>{
 const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/large-freeze-fixtures.json')));
 assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/large_freeze_probe.py'))).digest('hex'),report.generatorSha256);
 assert.equal(report.apiReturnsAssigned,false);for(const type of [18,20])assert.equal(report.profiles.find(profile=>profile.type===type).freezePolicy,0);
 const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py'),sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
 const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
 const output=path.join(root,'artifacts/tests/freeze-eligibility.cjs');const build=spawnSync(python,[compiler,'tests/native/freeze-eligibility.cpp','src/game/IceContact.cpp','-Isrc','-std=c++20','-O1','-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});assert.equal(build.status,0,build.stdout+build.stderr);
 const run=spawnSync(process.execPath,[output],{cwd:root,input:report.cases.map(sample=>[sample.policy,sample.age,Number(sample.override)].join(' ')).join('\n')+'\n',encoding:'utf8'});assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),report.cases.map(sample=>String(Number(sample.eligible))));
});
