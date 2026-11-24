import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('owning player fire host executes complete tap, hold, sound and freeze obligations',()=>{
 assert.ok(fs.existsSync(path.join(root,'src/game/PlayerFireHost.hpp')),'actual Z fire/charge owning host missing');
 const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
 const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
 const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
 const output=path.join(root,'artifacts/tests/player-fire-host.cjs');
 const build=spawnSync(python,[compiler,'tests/native/player-fire-host.cpp','src/game/PlayerFireHost.cpp','src/game/PlayerFireRules.cpp',
 'src/game/FreezeRules.cpp','src/game/PlayerRules.cpp','-Isrc','-std=c++20','-O1','-fexceptions','-fno-fast-math','-ffp-contract=off',
 '-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
 assert.equal(build.status,0,build.stdout+build.stderr);const run=spawnSync(process.execPath,[output],{cwd:root,encoding:'utf8'});
 assert.equal(run.status,0,run.stdout+run.stderr);assert.match(run.stdout,/real Z tap\/hold\/release/);
});
