import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('real bullet host joins native-prefix motion, gate-free proven misses and actual ANM ticks',()=>{
  assert.ok(fs.existsSync(path.join(root,'src/game/BulletUpdateHost.cpp')),'actual bullet update host is missing');
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  const directory=path.join(root,'artifacts/tests');fs.mkdirSync(directory,{recursive:true});
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
  const animation=path.join(path.dirname(path.join(root,'local',catalog.variants.ja.manifest)),'raw/bullet.anm');
  for(const sanitized of [false,true]){
    const output=path.join(directory,`bullet-update-host${sanitized?'-ubsan':''}.cjs`);
    const flags=sanitized?['-fsanitize=undefined','-fno-sanitize-recover=all']:[];
    const build=spawnSync(python,[compiler,'tests/native/bullet-update-host.cpp','src/game/BulletUpdateHost.cpp',
      'src/game/BulletManager.cpp','src/game/BulletRules.cpp','src/game/BulletVisuals.cpp','src/game/BulletTickRules.cpp','src/game/BulletExtension16.cpp','src/game/BulletExtension4.cpp','src/game/BulletExtension1.cpp',
      'src/game/BulletCollisionRules.cpp','src/game/GrazeRules.cpp','src/animation/AnmLogic.cpp','-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror',
      '-fno-fast-math','-ffp-contract=off',...flags,'-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1',
      '-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const run=spawnSync(process.execPath,[output,path.join(root,'local/runtime/bullet-profile-8.bin'),animation],
      {cwd:root,encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);
  }
});
