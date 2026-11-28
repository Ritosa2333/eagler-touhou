import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('actual WorldExports stage intro title/hint/caption/reset/restart through native world ticks',()=>{
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..');
  const python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const scratch=path.join(root,'artifacts/tests/stage-intro-world');
  fs.mkdirSync(scratch,{recursive:true});
  const sources=['tests/native/stage-intro-world.cpp','src/runtime/WorldExports.cpp',
    ...fs.readdirSync(path.join(root,'src/game')).filter(name=>name.endsWith('.cpp')).map(name=>`src/game/${name}`),
    'src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp'];
  const rsp=path.join(scratch,'sources.rsp');
  fs.writeFileSync(rsp,sources.map(source=>`"${source}"`).join('\n')+'\n');
  const env={...process.env,
    EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
    EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  const output=path.join(root,'artifacts/tests/stage-intro-world.cjs');
  const build=spawnSync(python,[compiler,'@'+rsp,
    '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off',
    '-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sNODERAWFS=1',
    '-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],
    {cwd:root,env,encoding:'utf8',timeout:600000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const base=path.join(root,'local/generations/8c71c316a4b24e68827f9fc560242050/zh');
  const logo=path.join(base,'raw/logo_a1_1.anm');
  const run=spawnSync(process.execPath,[output,logo],{cwd:root,encoding:'utf8',timeout:180000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS actual WorldExports stage intro tick\/draw\/status\/caption\/reset\/restart/);
});
