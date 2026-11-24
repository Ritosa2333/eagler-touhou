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
  EMSDK_PYTHON:python,PYTHONUTF8:'1',EMCC_CORES:'2',BINARYEN_CORES:'2'};

test('actual cancellation roots 377 and 369/373/379/383 accept native drift',()=>{
  const bullet=path.join(root,'local/generations/8c71c316a4b24e68827f9fc560242050/ja/raw/bullet.anm');
  assert.equal(createHash('sha256').update(fs.readFileSync(bullet)).digest('hex'),
    '9860822687f33e740f2a5e1fb25a4b05cf27b0fc6d15ed9626fb903986f7784c');
  const output=path.join(root,'artifacts/tests/cancellation-sibling-drift.cjs');
  const build=spawnSync(python,[compiler,'tests/native/cancellation-sibling-drift.cpp',
    'src/game/CancellationEffects.cpp','src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp',
    '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math',
    '-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
    '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],
    {cwd:root,env:environment,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  fs.mkdirSync(path.dirname(output),{recursive:true});
  const run=spawnSync(process.execPath,[output,bullet],{cwd:root,encoding:'utf8',timeout:120000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS cancellation drift 377\/376 and sibling 369\/373\/379\/383 native clock1 interpolation\/RNG\/cleanup/);
});
