import assert from 'node:assert/strict';import test from 'node:test';import fs from 'node:fs';import path from 'node:path';import {spawnSync} from 'node:child_process';import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('real typed4 profile/ANM39 joins radial pool birth, half-speed startup and full ETEX1 update',()=>{
 const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py'),sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
 const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
 const output=path.join(root,'artifacts/tests/typed4-host.cjs'),build=spawnSync(python,[compiler,'tests/native/typed4-host.cpp','src/game/BulletUpdateHost.cpp','src/game/BulletManager.cpp','src/game/BulletRules.cpp',
 'src/game/BulletVisuals.cpp','src/game/BulletTickRules.cpp','src/game/BulletExtension1.cpp','src/game/BulletExtension4.cpp','src/game/BulletExtension16.cpp','src/game/BulletCollisionRules.cpp','src/game/GrazeRules.cpp','src/animation/AnmLogic.cpp',
 '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
 assert.equal(build.status,0,build.stdout+build.stderr);
 const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'))),directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
 const profile=JSON.parse(fs.readFileSync(path.join(directory,'animations/bullet-native-profiles.json'))).profiles[0],file=path.join(root,'artifacts/tests/profile4.bin');fs.writeFileSync(file,Buffer.from(profile.hex,'hex'));
 const run=spawnSync(process.execPath,[output,file,path.join(directory,'raw/bullet.anm')],{cwd:root,encoding:'utf8'});assert.equal(run.status,0,run.stdout+run.stderr);assert.match(run.stdout,/actual type4/);
});
