import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const work=path.join(root,'artifacts/dsh-route-choice-integration');
const compiler=process.env.TH128_EMCC??path.join(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk=path.resolve(path.dirname(compiler),'../..');
const python=process.env.TH128_PYTHON??path.join(sdk,'python/3.13.3_64bit/python.exe');

test('message external wait preserves default behavior and releases MSG30 once',()=>{
  assert.ok(fs.existsSync(compiler),`missing compiler ${compiler}`);
  assert.ok(fs.existsSync(python),`missing python ${python}`);
  fs.mkdirSync(work,{recursive:true});
  const cache=path.join(work,'emcache');
  if(!fs.existsSync(cache)){
    const shared=path.resolve(root,'../../work/emscripten-cache-4.0.22');
    assert.ok(fs.existsSync(shared),`missing cache ${shared}`);
    fs.cpSync(shared,cache,{recursive:true,filter:source=>!path.basename(source).endsWith('.lock')});
  }
  const temporary=path.join(work,'temp');
  fs.mkdirSync(temporary,{recursive:true});
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:cache.replaceAll('\\','/'),EMSDK_PYTHON:python,PYTHONUTF8:'1',
    EMCC_CORES:'2',BINARYEN_CORES:'2',TEMP:temporary,TMP:temporary,TMPDIR:temporary};
  const output=path.join(work,'message-external-wait.cjs');
  const build=spawnSync(python,[compiler,'tests/native/message-external-wait.cpp','src/game/MessageHost.cpp',
    '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math',
    '-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node',
    '-sEXIT_RUNTIME=1','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],
  {cwd:root,env,encoding:'utf8',timeout:180000});
  assert.ifError(build.error);
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8',timeout:30000});
  assert.ifError(run.error);
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/external wait/);
});
