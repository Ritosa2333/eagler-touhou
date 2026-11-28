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

test('original host-miss domain is explicit, range-bounded and leaves bare VMs faulting',()=>{
  const vmHeader=fs.readFileSync(path.join(root,'src/ecl/EclVm.hpp'),'utf8');
  const vmSource=fs.readFileSync(path.join(root,'src/ecl/EclVm.cpp'),'utf8');
  const worldSource=fs.readFileSync(path.join(root,'src/game/StageWorld.cpp'),'utf8');
  const rules=fs.readFileSync(path.join(root,'src/ecl/HostResultRules.hpp'),'utf8');
  assert.match(vmHeader,/enableOriginalHostMissDomain/,'the explicit host-miss domain is not declared');
  assert.match(vmSource,/enableOriginalHostMissDomain/,'the explicit host-miss domain is not implemented');
  assert.match(vmSource,/isOriginalHostTableOpcode/,'the original host-table predicate is not consumed');
  assert.match(vmSource,/hostResultAdvances/,'the original result contract is not consumed');
  assert.match(rules,/isOriginalHostTableOpcode/,'the research helper range predicate is missing');
  assert.match(worldSource,/enableOriginalHostMissDomain/,'StageWorld does not configure the domain per entity VM');
  assert.ok(vmSource.includes('unregistered host opcode'),'the bare-VM unregistered fault must stay');
  fs.mkdirSync(path.join(root,'artifacts/tests'),{recursive:true});
  const output=path.join(root,'artifacts/tests/ecl-host-domain.cjs');
  const build=spawnSync(python,[compiler,'tests/native/ecl-host-domain.cpp',...sources,'-Isrc','-std=c++20','-O1',
    '-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off','-sENVIRONMENT=node',
    '-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env:environment,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8',timeout:60000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS original host-miss domain is explicit, range-bounded and does not replace registered callbacks/);
});
