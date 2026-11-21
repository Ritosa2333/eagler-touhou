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
const sources=['src/game/StageWorld.cpp','src/game/EnemyRules.cpp','src/game/EnemyRandomMovement.cpp',
  'src/game/EnemyPhaseRules.cpp','src/game/EnemyDropRules.cpp','src/game/BulletRules.cpp',
  'src/game/BulletManager.cpp','src/game/ShotEnemyCollisionRules.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp'];

test('452 layer offset is consumer-wired and 529 stays a bounded original callback clear',()=>{
  const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/enemy-stage-fields-fixtures.json')));
  assert.equal(report.schema,'th128-enemy-stage-fields/1');
  assert.equal(report.executable.sha256,report.executable.sha256After);
  assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/enemy_stage_fields_probe.py'))).digest('hex'),report.generatorSha256);
  assert.deepEqual(report.dispatch['452'],{index:92,handler:'0x41894e'});
  assert.deepEqual(report.dispatch['529'],{index:128,handler:'0x41a286'});
  assert.equal(report.table4b1e54[0],0);
  assert.equal(report.table4b1e54[2],0x41e860);
  assert.equal(report.table4b1e54[3],0x41e9b0);
  assert.equal(report.collisionRadius4a6820,9216);
  const stored=Object.fromEntries(report.cases.map(item=>[`${item.opcode}:${item.argument}`,item]));
  assert.equal(stored['452:2'].field238,2);
  assert.equal(stored['452:-3'].field238,-3);
  for(const selector of [0,2,3,40,100])assert.notEqual(stored[`529:${selector}`].field3fa0,'0xa5a5a5a5');
  assert.equal(stored['529:0'].field3fa0,'0x00000000');
  assert.equal(stored['529:2'].field3fa0,'0x0041e860');
  assert.equal(stored['529:3'].field3fa0,'0x0041e9b0');
  assert.equal(stored['529:2'].field3fa4,0);
  assert.equal(stored['529:100'].field3fa0,'0x7fffffff');
  assert.ok(report.fieldScan['0x238'].some(item=>item[0]==='0x00418957'));
  assert.ok(report.fieldScan['0x238'].some(item=>item[0]==='0x00418de1'));
  assert.ok(report.fieldScan['0x3fa0'].some(item=>item[0]==='0x004164ee'));
  const header=fs.readFileSync(path.join(root,'src/game/StageWorld.hpp'),'utf8');
  const world=fs.readFileSync(path.join(root,'src/game/StageWorld.cpp'),'utf8');
  assert.match(header,/animationLayerOffset/,'the entity snapshot lacks the stored layer offset');
  assert.match(world,/opcode==452/,'452 is not dispatched');
  assert.match(world,/entity\.animation\.layerOffset=offset/,'452 does not feed the animation layer consumer');
  assert.match(world,/opcode==529/,'529 is not dispatched');
  assert.match(world,/ECL529 native enemy-update callback selector is not ported/,'529 callback selectors are not explicitly rejected');
  assert.ok(world.includes('448,452,'),'452 is not registered for world entities');
  assert.ok(world.includes('525,529,'),'529 is not registered for world entities');
  fs.mkdirSync(path.join(root,'artifacts/tests'),{recursive:true});
  const output=path.join(root,'artifacts/tests/enemy-stage-fields.cjs');
  const build=spawnSync(python,[compiler,'tests/native/enemy-stage-fields.cpp',...sources,'-Isrc','-std=c++20','-O1',
    '-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off','-sENVIRONMENT=node',
    '-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env:environment,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8',timeout:60000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS 452 layer offset consumer and bounded 529 clear selector/);
});
