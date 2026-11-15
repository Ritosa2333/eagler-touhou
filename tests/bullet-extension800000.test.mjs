import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const compile=()=>{
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python};
    const output=path.join(root,'artifacts/tests/extension800000-integrated.cjs');
    fs.mkdirSync(path.dirname(output),{recursive:true});
    const sources=['tests/native/extension800000-integrated.cpp','src/game/BulletUpdateHost.cpp','src/game/BulletManager.cpp',
        'src/game/BulletExtension800000.cpp','src/game/BulletExtension64.cpp','src/game/BulletExtension16.cpp',
        'src/game/BulletExtension4.cpp','src/game/BulletExtension1.cpp','src/game/BulletRules.cpp','src/game/BulletTickRules.cpp',
        'src/game/BulletVisuals.cpp','src/game/BulletCollisionRules.cpp','src/game/GrazeRules.cpp','src/animation/AnmLogic.cpp'];
    const build=spawnSync(python,[compiler,...sources,'-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math',
        '-ffp-contract=off','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],
        {cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    return output;
};
test('actual BulletUpdateHost consumes 0x800000 once, stores five fields and preserves motion/record',()=>{
    const executable=compile();
    const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json')));
    const directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
    const rows=[
        ['0 1 256 1 1 0.5 10 100 0.1','1065353216 1056964608 1092616192 1120403456 1036831949 2 256 1 77 0'],
        ['0 0 256 1 1 0.5 10 100 0.1','1065353216 1056964608 1092616192 1120403456 1036831949 2 256 1 77 0'],
        ['1 0 64 1 1 0.5 10 100 0.1','0 0 0 0 0 1 64 1 77 0'],
        ['2 0 256 1 1 0.5 10 100 0.1','0 0 0 0 0 1 256 1 77 0'],
        ['2 1 0 1 1 0.5 10 100 0.1','FAIL'],
        ['3 1 256 1 1 0.5 10 100 0.1','FAIL'],
        ['0 2 256 1 1 0.5 10 100 0.1','FAIL'],
        ['4 1 256 1 1 0.5 10 100 0.1','1065353216 1056964608 1092616192 1120403456 1036831949 2 256 1 77 0'],
    ];
    const run=spawnSync(process.execPath,[executable,path.join(root,'local/runtime/bullet-profile-8.bin'),
        path.join(directory,'raw/bullet.anm')],{cwd:root,input:rows.map(row=>row[0]).join('\n')+'\n',encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);
    const lines=run.stdout.trim().split(/\r?\n/);
    assert.equal(lines.length,rows.length);
    for(let i=0;i<rows.length;++i){
        if(rows[i][1]==='FAIL') assert.ok(lines[i].startsWith('FAIL '),lines[i]);
        else assert.equal(lines[i],rows[i][1],lines[i]);
    }
});
