import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const work=path.join(root,'artifacts/dsh-stage-transition-owner');
const compiler=process.env.TH128_EMCC??path.join(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk=path.resolve(path.dirname(compiler),'../..');
const python=process.env.TH128_PYTHON??path.join(sdk,'python/3.13.3_64bit/python.exe');

test('pre-Boss MSG0 does not complete an ordinary stage transition',()=>{
  assert.ok(fs.existsSync(compiler),`missing compiler ${compiler}`);
  fs.mkdirSync(work,{recursive:true});
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:path.resolve(root,'artifacts/dsh-route-choice-integration/emcache').replaceAll('\\','/'),
    EMSDK_PYTHON:python,PYTHONUTF8:'1',EMCC_CORES:'2',BINARYEN_CORES:'2'};
  const output=path.join(work,'stage-transition-owner.cjs');
  const build=spawnSync(python,[compiler,'tests/native/stage-transition-owner.cpp','-Isrc',
    '-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions','-sENVIRONMENT=node',
    '-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
  assert.ifError(build.error);
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8',timeout:30000});
  assert.ifError(run.error);
  assert.equal(run.status,0,run.stdout+run.stderr);
});
