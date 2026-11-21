import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('explicit held-host result freezes only current command and clock, with faults/reentry kept separate',()=>{
  assert.ok(fs.readFileSync(path.join(root,'src/ecl/EclVm.hpp'),'utf8').includes('holdCurrentCommand'),'native held-host result is missing');
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  for(const sanitize of [false,true]){
    const output=path.join(root,`artifacts/tests/ecl-host-hold${sanitize?'-ubsan':''}.cjs`);
    const build=spawnSync(python,[compiler,'tests/native/ecl-host-hold.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp',
      '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off',
      ...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),'-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const run=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);assert.match(run.stdout,/retains nativePC\/time/);
  }
});
