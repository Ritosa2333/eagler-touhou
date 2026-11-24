import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const scratch=path.join(root,'artifacts/t16');
test('actual C1/D3 type16 record uses cached 0x800000 position angle and preserves old cases',()=>{
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..');
  const python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  assert.ok(fs.existsSync(compiler)&&fs.existsSync(python));
  fs.mkdirSync(path.join(scratch,'emcache'),{recursive:true});
  fs.mkdirSync(path.join(scratch,'emcc-tmp'),{recursive:true});
  const env={...process.env,
    EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
    EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  const output=path.join(scratch,'extension16-mode1-angle3.cjs');
  const build=spawnSync(python,[compiler,
    'tests/native/extension16-mode1-angle3.cpp','src/game/BulletExtension16.cpp','src/game/BulletRules.cpp',
    '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off',
    '-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],
    {cwd:root,env,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8',timeout:30000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS extension16 C1\/D3 snapshot-angle, sound order, Z preserve and old C0\/2 cases/);
});
