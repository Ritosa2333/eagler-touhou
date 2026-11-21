import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const scratch=path.join(root,'artifacts/dsh-type25-live-integration/src/game');
const compiler=process.env.TH128_EMCC??path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk=path.resolve(path.dirname(compiler),'../..');
const python=process.env.TH128_PYTHON??path.join(sdk,'python/3.13.3_64bit/python.exe');
const environment={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
  EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
  EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
const sources=[path.join(scratch,'StageWorld.cpp'),path.join(scratch,'BulletManager.cpp'),
  path.join(scratch,'BulletVisuals.cpp'),path.join(scratch,'Type25TransactionHost.cpp'),
  'src/game/BulletRules.cpp','src/game/ShotEnemyCollisionRules.cpp','src/game/EnemyRules.cpp',
  'src/game/EnemyRandomMovement.cpp','src/game/EnemyPhaseRules.cpp','src/game/EnemyDropRules.cpp',
  'src/game/EnemyBulletCallbackRules.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp','src/animation/AnmLogic.cpp'];

test('real StageWorld/Manager ECL529 selector3 transaction',()=>{
  const stage=fs.readFileSync(path.join(scratch,'StageWorld.cpp'),'utf8');
  const manager=fs.readFileSync(path.join(scratch,'BulletManager.cpp'),'utf8');
  const visuals=fs.readFileSync(path.join(scratch,'BulletVisuals.cpp'),'utf8');
  const host=fs.readFileSync(path.join(scratch,'Type25TransactionHost.cpp'),'utf8');
  const hostHeader=fs.readFileSync(path.join(scratch,'Type25TransactionHost.hpp'),'utf8');
  assert.match(stage,/if\(selector==3\)\{entity\.callbackSelector=3;return true;\}/,'selector3 install');
  assert.match(stage,/for\(std::size_t slot=0;slot<bullet::Manager::Capacity;\+\+slot\)/,'full 2000 slot order');
  assert.match(stage,/dx\*dx\+dy\*dy>9216\.0/,'inclusive 96 boundary');
  assert.match(stage,/entity\.variables\[4\]=std::bit_cast<std::uint32_t>\(next\)/,'var4 once');
  assert.match(stage,/callbackSelector!=2/,'selector2 preserved');
  assert.match(stage,/setBulletConversionHost/,'host storage/setter');
  assert.match(manager,/state\.motionState != 1/,'active-only preflight');
  assert.match(manager,/liveBirth/,'authoritative live snapshot');
  assert.match(manager,/changed a preserved live field/,'preserved-field transaction');
  assert.match(host,/caller is stale or not the authoritative live bullet/,'authoritative caller gate');
  assert.match(host,/sticky_ = true/,'sticky transaction fault');
  assert.match(hostHeader,/faulted\(\) const \{ return sticky_; \}/,'sticky fault accessor');
  assert.match(hostHeader,/setFailAfterCommitForTest/,'post-commit fault path');
  assert.match(stage,/bulletConversionHost_=\{\}/,'reset clears host references');
  assert.match(visuals,/createConverted/,'converted Visual');
  assert.ok(!visuals.includes('forbids a birth interrupt'),'no guessed birthInterrupt rejection');
  assert.match(visuals,/!conversion && !tick/,'no extra birth tick');
  assert.match(host,/is_nothrow_move_assignable_v/,'no post-commit map failure');
  assert.match(host,/source visual is stale/,'old visual prevalidation');

  const type8=path.join(root,'local/runtime/bullet-profile-8.bin');
  const type25=path.join(root,'artifacts/dsh-type25/type25-profile.bin');
  const bullet=path.join(root,'artifacts/dsh-readonly-inputs/bullet.anm');
  assert.equal(createHash('sha256').update(fs.readFileSync(type8)).digest('hex'),
    '7427ad4aa6459742ffaa628c7a8e303029a9692b7b3ffbdd5b7f9a3207f0d80e');
  assert.equal(createHash('sha256').update(fs.readFileSync(type25)).digest('hex'),
    '080190229ed68ee1120238bb7308ae44c16cdc903c2a84fba65a7c273b2236dd');
  assert.equal(createHash('sha256').update(fs.readFileSync(bullet)).digest('hex'),
    '9860822687f33e740f2a5e1fb25a4b05cf27b0fc6d15ed9626fb903986f7784c');

  const output=path.join(root,'artifacts/dsh-type25-live-integration/type25-live-integration.cjs');
  const build=spawnSync(python,[compiler,'tests/native/type25-live-integration.cpp',...sources,
    '-Iartifacts/dsh-type25-live-integration/src','-Isrc','-Isrc/game','-std=c++20','-O1',
    '-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off',
    '-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1',
    '-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env:environment,encoding:'utf8',timeout:300000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output,type8,type25,bullet],{cwd:root,encoding:'utf8',timeout:120000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS real StageWorld\/Manager ECL529 selector3 transaction/);
});
