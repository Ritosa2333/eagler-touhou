import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('actual MBoss189 child trees retain original tail/cached-next order and shared authored state',()=>{
    const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/enemy-child-fixtures.json')));
    assert.equal(report.executable.sha256,report.executable.sha256After);assert.equal(report.allocationExecuted,false);
    assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/enemy_child_probe.py'))).digest('hex'),report.generatorSha256);
    assert.equal(report.requestCases.length,3);assert.equal(report.linkCases.length,2);
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'))),directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
    for(const sanitize of [false,true]){
        const output=path.join(root,`artifacts/tests/enemy-children${sanitize?'-ubsan':''}.cjs`);
        const build=spawnSync(python,[compiler,'tests/native/enemy-children.cpp','src/game/EnemyVisuals.cpp','src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp',
            '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off','-fexceptions','-sDISABLE_EXCEPTION_CATCHING=0',
            ...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),'-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
        assert.equal(build.status,0,build.stdout+build.stderr);
        const run=spawnSync(process.execPath,[output,path.join(directory,'raw/bullet.anm'),path.join(directory,'raw/enemy.anm'),path.join(directory,'raw/ascii.anm')],{cwd:root,encoding:'utf8',timeout:30000});
        assert.equal(run.status,0,run.stdout+run.stderr);assert.match(run.stdout,/actual189 children match global Scene order/);
    }
});
