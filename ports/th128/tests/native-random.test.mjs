import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const compiler=process.env.TH128_EMCC ?? path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk=path.resolve(path.dirname(compiler),'../..');
const bundledPython=path.join(sdk,'python/3.13.3_64bit/python.exe');
const python=process.env.TH128_PYTHON ?? (fs.existsSync(bundledPython)?bundledPython:'python');
const environment={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
  EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,PYTHONUTF8:'1',EMCC_CORES:'2',BINARYEN_CORES:'2'};

test('portable C++ RNG matches original-byte golden vectors', {skip:!fs.existsSync(compiler)},()=>{
  const directory=path.join(root,'artifacts/tests');fs.mkdirSync(directory,{recursive:true});
  const output=path.join(directory,'random.cjs');
  const build=spawnSync(python,[compiler,'tests/native/random.cpp','-Isrc','-std=c++20','-O1','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,encoding:'utf8',env:environment});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8'});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/native RNG vectors/);
});
