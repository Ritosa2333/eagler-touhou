import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('confirmed player overlap requires all authored effects, ordered sounds and real player ANM reset',()=>{
  assert.ok(fs.existsSync(path.join(root,'src/game/PlayerHitHost.cpp')),'composed player hit host is missing');
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  const directory=path.join(root,'artifacts/tests');fs.mkdirSync(directory,{recursive:true});
  const output=path.join(directory,'player-hit-host.cjs');
  const build=spawnSync(python,[compiler,'tests/native/player-hit-host.cpp','src/game/PlayerHitHost.cpp',
    'src/game/PlayerHitRules.cpp','src/game/PlayerRules.cpp','src/game/PlayerDeathRules.cpp','src/game/PlayerRespawnRules.cpp','src/animation/AnmLogic.cpp','-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror',
    '-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
    '-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
  const animation=path.join(path.dirname(path.join(root,'local',catalog.variants.ja.manifest)),'raw/pl00.anm');
  const metadata=JSON.parse(fs.readFileSync(path.join(path.dirname(animation),'../animations/pl00.json'),'utf8'));
  const mutant=fs.readFileSync(animation),offset=metadata.scripts[0].offset;
  mutant.writeInt16LE(7,offset);mutant.writeUInt16LE(16,offset+2);mutant.writeInt16LE(0,offset+4);mutant.writeUInt16LE(3,offset+6);
  mutant.writeFloatLE(10004,offset+8);mutant.writeFloatLE(10016,offset+12);
  mutant.fill(0,offset+16,offset+24);mutant.writeInt16LE(-1,offset+16);mutant.writeUInt16LE(8,offset+18);
  const mutantPath=path.join(directory,'player-camera-mutant.anm');fs.writeFileSync(mutantPath,mutant);
  const run=spawnSync(process.execPath,[output,animation,mutantPath],{cwd:root,encoding:'utf8',timeout:30000});
  assert.equal(run.status,0,run.stdout+run.stderr);
});
