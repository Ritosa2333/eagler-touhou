import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('526 owning host binds actual text82, preserves row order and retires all replaced geometry',()=>{
    assert.ok(fs.existsSync(path.join(root,'src/game/Geometry526Host.hpp')),'owning text82 geometry526 host is missing');
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json')));
    for(const sanitize of [false,true]){
        const output=path.join(root,`artifacts/tests/geometry526-host${sanitize?'-ubsan':''}.cjs`);
        const build=spawnSync(python,[compiler,'tests/native/geometry526-host-lifecycle.cpp','src/game/Geometry526Host.cpp','src/game/Geometry526.cpp',
            'src/animation/AnmScene.cpp','src/animation/AnmLogic.cpp','-Isrc','-std=c++20','-O1',
            '-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off',
            ...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),'-sENVIRONMENT=node','-sEXIT_RUNTIME=1',
            '-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],
            {cwd:root,env,encoding:'utf8',timeout:180000});
        assert.equal(build.status,0,build.stdout+build.stderr);
        for(const variant of ['ja','zh']){
            const resource=path.join(root,'local',path.dirname(catalog.variants[variant].manifest),'raw/text.anm');
            const run=spawnSync(process.execPath,[output,resource],{cwd:root,encoding:'utf8',timeout:30000});
            assert.equal(run.status,0,`${variant}: ${run.stdout}${run.stderr}`);
            assert.match(run.stdout,/16 ordered strips/);
        }
    }
});
