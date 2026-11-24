import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const compiler=process.env.TH128_EMCC??path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk=path.resolve(path.dirname(compiler),'../..');
const python=process.env.TH128_PYTHON??path.join(sdk,'python/3.13.3_64bit/python.exe');
const environment={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
  EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
  EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
const sources=['src/game/StageWorld.cpp','src/game/EnemyRules.cpp','src/game/EnemyRandomMovement.cpp',
  'src/game/EnemyPhaseRules.cpp','src/game/EnemyDropRules.cpp','src/game/BulletRules.cpp',
  'src/game/BulletManager.cpp','src/game/ShotEnemyCollisionRules.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp'];

test('three-fairy StageWorld wiring: 613 restart, HP0 continuation and owner release',()=>{
  const header=fs.readFileSync(path.join(root,'src/game/StageWorld.hpp'),'utf8');
  const world=fs.readFileSync(path.join(root,'src/game/StageWorld.cpp'),'utf8');
  assert.match(header,/synchronizeBoss/,'the cross-Boss restart rule is not declared');
  assert.match(world,/synchronizeBoss\(/,'the cross-Boss restart rule is not implemented');
  assert.match(world,/if\(opcode==613\)/,'opcode 613 is not dispatched');
  assert.ok(world.includes('424,427,613,304'),'opcode 613 is not registered for world entities');
  assert.match(world,/bosses_\.owners\[std::size_t\(slot\)\]==entity\.state\.id/,
    'retirement does not verify the current owner before releasing a slot');
  assert.match(world,/clearUnownedBossGauges\(bossGauges_,bosses_\)/,
    'retirement does not clear the released HUD group');
  assert.match(world,/phaseSwitched/,'the immediate phase continuation is not tracked');
  const fixtures=JSON.parse(fs.readFileSync(path.join(root,'local/research/thirdstage-boss-fixtures.json'),'utf8'));
  assert.equal(fixtures.executable.sha256,fixtures.executable.sha256After);
  assert.equal(fixtures.code.handlers['613'],'0x41c42f');
  assert.ok(fixtures.routes.every(route=>route.chainA.some(event=>event.opcode===613&&event.argument===1)));
  fs.mkdirSync(path.join(root,'artifacts/tests'),{recursive:true});
  const output=path.join(root,'artifacts/tests/multi-boss-world.cjs');
  const build=spawnSync(python,[compiler,'tests/native/multi-boss-world.cpp',...sources,'-Isrc','-std=c++20','-O1',
    '-fexceptions','-fno-fast-math','-ffp-contract=off','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],
    {cwd:root,env:environment,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8',timeout:60000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS three-fairy cross-Boss sync, HP0 continuation and owner release/);
});
