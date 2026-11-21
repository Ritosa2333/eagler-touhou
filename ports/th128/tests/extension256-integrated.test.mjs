import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('actual 800000->256 chain drives native timer with next type16 and full lifetime',()=>{
  const fixture=JSON.parse(fs.readFileSync(path.join(root,'artifacts/dsh-extension256/fixture.json'),'utf8'));
  assert.equal(fixture.schema,'th128-original-extension256/1');
  assert.equal(fixture.record800000.type,0x800000);
  assert.equal(fixture.record256.type,256);
  assert.deepEqual(fixture.record256.parameterWords,[1,2,0,256,120,0,3379831792,3379831792]);
  assert.deepEqual(fixture.init.timer,[119,120,1123024896,4943704,1]);
  assert.deepEqual(fixture.vectors.map(v=>v.activeExtensions),[256,256,0]);
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
    EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  const output=path.join(root,'artifacts/tests/extension256-integrated.cjs');
  const sources=['tests/native/extension256-integrated.cpp','src/game/BulletUpdateHost.cpp','src/game/BulletManager.cpp',
    'src/game/BulletExtension256.cpp','src/game/BulletExtension800000.cpp','src/game/BulletExtension64.cpp',
    'src/game/BulletExtension16.cpp','src/game/BulletExtension4.cpp','src/game/BulletExtension1.cpp',
    'src/game/BulletRules.cpp','src/game/BulletTickRules.cpp','src/game/BulletVisuals.cpp',
    'src/game/BulletCollisionRules.cpp','src/game/GrazeRules.cpp','src/animation/AnmLogic.cpp'];
  const build=spawnSync(python,[compiler,...sources,'-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror',
    '-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
    '-sENVIRONMENT=node','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-sEXIT_RUNTIME=1','-o',output],
    {cwd:root,env,encoding:'utf8',timeout:300000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
  const directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
  const run=spawnSync(process.execPath,[output,path.join(root,'local/runtime/bullet-profile-8.bin'),
    path.join(directory,'raw/bullet.anm')],{cwd:root,encoding:'utf8',timeout:120000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS actual 800000->256 chain/);
});
