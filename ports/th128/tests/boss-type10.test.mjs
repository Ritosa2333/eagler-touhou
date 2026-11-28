import assert from 'node:assert/strict';import test from 'node:test';import path from 'node:path';import fs from 'node:fs';import {fileURLToPath} from 'node:url';import {spawnSync} from 'node:child_process';import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('third midboss type10 uses the actual45 profile/ANM with all original color mappings',()=>{
 const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/boss-type10-fixtures.json')));assert.equal(report.apiReturnsAssigned,false);
 const profile=Buffer.from(report.profile.hex,'hex');assert.equal(createHash('sha256').update(profile).digest('hex'),report.profile.sha256);
 const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py'),sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
 const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
 const output=path.join(root,'artifacts/tests/boss-type10.cjs'),profileFile=path.join(root,'artifacts/tests/boss-type10.bin');fs.writeFileSync(profileFile,profile);
 const build=spawnSync(python,[compiler,'tests/native/boss-type10.cpp','src/game/BulletManager.cpp','src/game/BulletRules.cpp','src/game/BulletVisuals.cpp','src/animation/AnmLogic.cpp','-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});assert.equal(build.status,0,build.stdout+build.stderr);
 const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'))),directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
 const run=spawnSync(process.execPath,[output,profileFile,path.join(directory,'raw/bullet.anm')],{cwd:root,encoding:'utf8'});assert.equal(run.status,0,run.stdout+run.stderr);
});
