import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const generation=path.join(root,'local/generations/8c71c316a4b24e68827f9fc560242050/ja');
test('actual WorldExports type30 constructor 285/286, first world tick, draw, reset, colors0/6 and strict rejection',()=>{
  const scratch=fs.mkdtempSync(path.join(os.tmpdir(),'th128-type30-worldexports-'));
  const sources=['tests/native/type30-worldexports.cpp',
    ...fs.readdirSync(path.join(root,'src/game')).filter(name=>name.endsWith('.cpp')).map(name=>`src/game/${name}`),
    'src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp'];
  const response=path.join(scratch,'sources.rsp');
  fs.writeFileSync(response,sources.map(source=>`"${source}"`).join('\n')+'\n');
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
    EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  const output=path.join(scratch,'type30-worldexports.cjs');
  const build=spawnSync(python,[compiler,'@'+response,'-Isrc','-Isrc/game','-std=c++20','-O1','-Wall','-Wextra','-Werror',
    '-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
    '-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],
    {cwd:root,env,encoding:'utf8',timeout:900000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const profile=JSON.parse(fs.readFileSync(path.join(root,'local/generations/8c71c316a4b24e68827f9fc560242050/ja/animations/bullet-type30-profile.json'),'utf8'));
  const profilePath=path.join(scratch,'type30-profile.bin');
  fs.writeFileSync(profilePath,Buffer.from(profile.hex,'hex'));
  const run=spawnSync(process.execPath,[output,profilePath,path.join(root,'local/runtime/bullet-profile-8.bin'),
    path.join(generation,'raw/bullet.anm'),path.join(generation,'raw/enemy.anm'),path.join(generation,'raw/pl00a.sht')],
    {cwd:root,encoding:'utf8',timeout:180000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS actual WorldExports type30 constructor285\/286 draw\/state\/life\/reset\/0\+6\/missing\/wrongpair/);
});
