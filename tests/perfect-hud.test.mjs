import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('actual front0/47 and native charge-child draw flags satisfy required Perfect Freeze HUD work',()=>{
    assert.ok(fs.existsSync(path.join(root,'src/game/PerfectHud.cpp')),'real front Perfect Freeze HUD host is missing');
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    const output=path.join(root,'artifacts/tests/perfect-hud.cjs');
    const build=spawnSync(python,[compiler,'tests/native/perfect-hud.cpp','src/game/PerfectHud.cpp','src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp',
        '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
        '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'))),directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
    const run=spawnSync(process.execPath,[output,path.join(directory,'raw/front.anm')],{cwd:root,encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);assert.match(run.stdout,/actual front0\/47 lifecycle/);
});
