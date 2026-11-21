import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const compiler=process.env.TH128_EMCC??path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk=path.resolve(path.dirname(compiler),'../..');
const python=process.env.TH128_PYTHON??path.join(sdk,'python/3.13.3_64bit/python.exe');
const environment={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
  EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
  EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
const scratch=path.join(root,'artifacts/dsh-type25-transaction/src/game');
const sources=[path.join(scratch,'BulletManager.cpp'),path.join(scratch,'BulletVisuals.cpp'),
  path.join(scratch,'Type25TransactionHost.cpp'),'src/game/BulletRules.cpp','src/game/BulletTickRules.cpp',
  'src/game/BulletUpdateHost.cpp','src/game/BulletCollisionRules.cpp','src/game/GrazeRules.cpp',
  'src/game/IceContact.cpp','src/game/BulletExtension1.cpp','src/game/BulletExtension4.cpp',
  'src/game/BulletExtension16.cpp','src/animation/AnmLogic.cpp'];

test('type25 live transaction scratch preserves fields, validates old owner pre-RNG and fails closed',()=>{
  const manager=fs.readFileSync(path.join(scratch,'BulletManager.cpp'),'utf8');
  const visuals=fs.readFileSync(path.join(scratch,'BulletVisuals.cpp'),'utf8');
  const host=fs.readFileSync(path.join(scratch,'Type25TransactionHost.cpp'),'utf8');
  assert.match(manager,/state\.motionState != 1/,'active-only preflight');
  assert.match(manager,/changed a preserved live field/,'allowed-field postcondition');
  assert.match(visuals,/createConverted/,'converted visual owner');
  assert.match(visuals,/forbids a birth interrupt/,'no conversion birthInterrupt');
  assert.match(visuals,/!conversion && !tick/,'no normal-birth extra tick');
  assert.ok(host.indexOf('source visual is stale')<host.indexOf('createConverted'),
    'old visual validation must precede new bind');
  assert.match(host,/is_nothrow_move_assignable_v/,'no post-commit allocation/failure');
  assert.match(host,/secondary\.state\(\)\.script != 103/,'secondary owner');
  const patch=fs.readFileSync(path.join(root,'artifacts/dsh-type25-transaction/proposed.patch'),'utf8');
  assert.match(patch,/StageWorld::preflightBulletConversion/,'missing StageWorld preflight definition');
  assert.match(patch,/StageWorld::commitBulletConversion/,'missing StageWorld commit definition');
  assert.match(patch,/setBulletConversionHost/,'StageWorld host definition');

  const type8=path.join(root,'local/runtime/bullet-profile-8.bin');
  const type25=path.join(root,'artifacts/dsh-type25/type25-profile.bin');
  const bullet=path.join(root,'artifacts/dsh-readonly-inputs/bullet.anm');
  assert.equal(createHash('sha256').update(fs.readFileSync(bullet)).digest('hex'),
    '9860822687f33e740f2a5e1fb25a4b05cf27b0fc6d15ed9626fb903986f7784c');
  assert.equal(createHash('sha256').update(fs.readFileSync(type25)).digest('hex'),
    '080190229ed68ee1120238bb7308ae44c16cdc903c2a84fba65a7c273b2236dd');
  assert.equal(createHash('sha256').update(fs.readFileSync(type8)).digest('hex'),
    '7427ad4aa6459742ffaa628c7a8e303029a9692b7b3ffbdd5b7f9a3207f0d80e');

  fs.mkdirSync(path.join(root,'artifacts/dsh-type25-transaction'),{recursive:true});
  const output=path.join(root,'artifacts/dsh-type25-transaction/type25-live-transaction.cjs');
  const build=spawnSync(python,[compiler,'tests/native/type25-live-transaction.cpp',...sources,
    '-Iartifacts/dsh-type25-transaction/src','-Isrc','-Isrc/game','-std=c++20','-O1',
    '-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off',
    '-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1',
    '-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env:environment,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output,type8,type25,bullet],{cwd:root,encoding:'utf8',timeout:120000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS type25 live transaction/);
});
