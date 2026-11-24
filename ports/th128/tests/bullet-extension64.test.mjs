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
    const output=path.join(root,'artifacts/tests/extension64-integrated.cjs');
    fs.mkdirSync(path.dirname(output),{recursive:true});
    const sources=['tests/native/extension64-integrated.cpp','src/game/BulletUpdateHost.cpp','src/game/BulletManager.cpp',
        'src/game/BulletExtension64.cpp','src/game/BulletExtension800000.cpp','src/game/BulletExtension16.cpp','src/game/BulletExtension4.cpp','src/game/BulletExtension1.cpp',
        'src/game/BulletRules.cpp','src/game/BulletTickRules.cpp','src/game/BulletVisuals.cpp','src/game/BulletCollisionRules.cpp',
        'src/game/GrazeRules.cpp','src/animation/AnmLogic.cpp'];
    const build=spawnSync(python,[compiler,...sources,'-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math',
        '-ffp-contract=off','-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],
        {cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    return output;
};
test('actual BulletUpdateHost initializes, continues, mutates and ends original ETEX64 records',()=>{
    const executable=compile();
    const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json')));
    const directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
    const rows=[
        ['0 0 -1 3 0.5 3 1 0 2.5 7 0','64 1 1'],
        ['1 0 -1 3 0.5 3 1 0 2.5 7 1.25','64 1 1'],
        ['1 0 -1 3 0.5 3 1 2 2.5 7 0','0 3 1'],
        ['1 0 448 3 0.5 3 0 0 2.5 7 0','64 0 0'],
        ['1 0 -1 3 0.5 3 16 0 2.5 7 0','64 0 0'],
        ['1 0 100 3 0.5 3 1 0 0 7 0','64 0 0'],
        ['2 0 -1 3 0.5 3 1 0 2.5 7 0','FAIL type=65'],
        ['3 0 -1 3 0.5 3 1 0 2.5 7 0','FAIL type64'],
        ['4 192 100 3 1000 3 8 0 2.5 7 0','FAIL unsupported type64 unit update'],
    ];
    const run=spawnSync(process.execPath,[executable,path.join(root,'local/runtime/bullet-profile-8.bin'),
        path.join(directory,'raw/bullet.anm')],{cwd:root,input:rows.map(row=>row[0]).join('\n')+'\n',encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);
    const lines=run.stdout.trim().split(/\r?\n/);
    assert.equal(lines.length,rows.length);
    for(let index=0;index<rows.length;++index){
        const parts=lines[index].split(' ');
        if(rows[index][1].startsWith('FAIL')) assert.ok(lines[index].startsWith('FAIL ')&&lines[index].includes(rows[index][1].slice(5)),lines[index]);
        else assert.deepEqual(parts.slice(6,9),rows[index][1].split(' '),lines[index]);
    }
    assert.equal(lines[1].split(' ')[2],String(0x3fa00000),lines[1]);
});
