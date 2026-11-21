import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('type6 original profile loads and births/visualizes through promoted production owners',()=>{
  const profile=JSON.parse(fs.readFileSync(path.join(root,'artifacts/dsh-type6-profile/type6-profile.json'),'utf8'));
  assert.equal(profile.type,6);assert.equal(profile.bytes,288);assert.equal(profile.sourceAddress,'0x4b2b90');
  assert.equal(profile.sha256,'e26b6619e74337ad7e27614e55b095221b8555a8d8152f520e4c74d53c3a93f0');
  assert.equal(profile.sourceExeSha256,'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(profile.script,41);assert.equal(profile.secondaryScript,0);
  assert.equal(profile.hitboxSize,4);assert.equal(profile.auxiliarySize,16);assert.equal(profile.appearance,3);
  assert.equal(profile.selector,6);assert.equal(profile.freezePolicy,0);assert.equal(profile.identitySpriteRemap,false);
  assert.deepEqual(profile.colorRemaps[0],[96,323,336,337]);
  assert.deepEqual(profile.cancellationScripts.slice(0,4),[337,339,341,343]);
  const fixture=JSON.parse(fs.readFileSync(path.join(root,'local/research/type6-profile-fixtures.json'),'utf8'));
  assert.equal(fixture.schema,'th128-type6-profile-fixtures/1');
  assert.deepEqual(fixture.proposedProductionWrites,['src/game/BulletManager.cpp','src/game/BulletVisuals.cpp']);
  assert.equal(fixture.animation.script,41);assert.equal(fixture.animation.commands,39);

  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const environment={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
    EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  const output=path.join(root,'artifacts/tests/type6-profile.cjs');
  fs.mkdirSync(path.dirname(output),{recursive:true});
  const build=spawnSync(python,[compiler,'tests/native/type6-profile.cpp',
    'src/game/BulletManager.cpp','src/game/BulletVisuals.cpp','src/game/BulletRules.cpp',
    'src/game/IceContact.cpp','src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp',
    '-Isrc','-Isrc/game','-std=c++20','-O1','-Wall','-Wextra','-Werror',
    '-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
    '-sENVIRONMENT=node','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=67108864',
    '-sEXIT_RUNTIME=1','-o',output],{cwd:root,env:environment,encoding:'utf8',timeout:180000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const profileFile=path.join(root,'artifacts/tests/type6-profile.bin');
  fs.writeFileSync(profileFile,Buffer.from(profile.hex,'hex'));
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
  const directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
  const run=spawnSync(process.execPath,[output,profileFile,path.join(directory,'raw/bullet.anm')],
    {cwd:root,encoding:'utf8',timeout:60000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/type6 profile exact bytes/);
});
