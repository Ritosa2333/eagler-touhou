import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const scratch=path.join(root,'artifacts/dsh-type25-final-runtime-rebase');
const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk=path.resolve(path.dirname(compiler),'..','..');
const python=path.join(sdk,'python/3.13.3_64bit/python.exe');
const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
  EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
  EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};

test('current Type25 two-session WorldExports rebase',()=>{
  const manager=fs.readFileSync(path.join(scratch,'src/game/BulletManager.cpp'),'utf8');
  const managerHeader=fs.readFileSync(path.join(scratch,'src/game/BulletManager.hpp'),'utf8');
  const visuals=fs.readFileSync(path.join(scratch,'src/game/BulletVisuals.cpp'),'utf8');
  const exports=fs.readFileSync(path.join(scratch,'src/runtime/WorldExports.cpp'),'utf8');
  assert.match(managerHeader,/Extension256 extension256/,'Extension256 retained');
  assert.match(manager,/same256\(slot\.extension256/,'extension256 audit');
  assert.match(manager,/candidate\.script!=285|candidate\.script!=285/,'Type30 profile retained');
  assert.match(manager,/type30 requires its original profile/,'Type30 gate retained');
  assert.match(visuals,/info\.type==30/,'Type30 visual retained');
  assert.match(exports,/supportedBulletTypes.*30/,'Type30 capability retained');
  assert.match(exports,/install_type25_conversion/,'install export present');

  const tree=path.join(scratch,'compile-tree/src');
  const sources=[...fs.readdirSync(path.join(tree,'game')).filter(name=>name.endsWith('.cpp')).map(name=>path.join(tree,'game',name)),
    ...fs.readdirSync(path.join(tree,'animation')).filter(name=>name.endsWith('.cpp')).map(name=>path.join(tree,'animation',name)),
    ...fs.readdirSync(path.join(tree,'ecl')).filter(name=>name.endsWith('.cpp')).map(name=>path.join(tree,'ecl',name)),
    path.join(tree,'runtime/WorldExports.cpp'),path.join(root,'tests/native/type25-final-runtime.cpp')];
  const output=path.join(scratch,'final-runtime.cjs');
  const build=spawnSync(python,[compiler,...sources,'-I'+tree,'-I'+path.join(tree,'game'),'-I'+path.join(tree,'runtime'),
    '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off',
    '-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1',
    '-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:600000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const stage=path.join(root,'local/generations/8c71c316a4b24e68827f9fc560242050/ja');
  const run=spawnSync(process.execPath,[output,path.join(root,'local/runtime/bullet-profile-8.bin'),
    path.join(root,'artifacts/dsh-type25/type25-profile.bin'),path.join(root,'artifacts/dsh-type30-profile/type30-profile.bin'),
    path.join(root,'artifacts/dsh-readonly-inputs/bullet.anm'),path.join(stage,'raw/enemy.anm'),
    path.join(stage,'raw/logo_a1_3.anm'),path.join(stage,'raw/pl00a.sht')],{cwd:root,encoding:'utf8',timeout:180000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS actual Type25 WorldExports two-session install\/type25\/prepass\/reset\/no-dual-draw flow/);
});
