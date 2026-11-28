import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('original526 preallocated grid and triangle strip copies match all vertex/UV/position words',()=>{
    assert.ok(fs.existsSync(path.join(root,'src/game/Geometry526.hpp')),'bounded geometry526 grid module is missing');
    const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/geometry526-fixtures.json')));
    assert.equal(report.executable.sha256,report.executable.sha256After);assert.equal(report.allocationExecuted,false);
    assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/geometry526_probe.py'))).digest('hex'),report.generatorSha256);
    const bits=value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();};
    const words=hex=>{const bytes=Buffer.from(hex,'hex');return Array.from({length:bytes.length/4},(_,index)=>bytes.readUInt32LE(index*4));};
    const input=report.gridCases.map(({input})=>[input.columns,input.rows,...[input.x,input.y,input.width,input.height].map(bits)].join(' '));
    const expected=report.gridCases.map(sample=>[...words(sample.verticesAfter),...words(sample.positionsAfter),...words(sample.stripsAfter)].join(' '));
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    for(const sanitize of [false,true]){
        const output=path.join(root,`artifacts/tests/geometry526${sanitize?'-ubsan':''}.cjs`);
        const build=spawnSync(python,[compiler,'tests/native/geometry526-vectors.cpp','src/game/Geometry526.cpp','-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off',
            ...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),'-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
        assert.equal(build.status,0,build.stdout+build.stderr);
        const result=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000,maxBuffer:1024*1024});
        assert.equal(result.status,0,result.stdout+result.stderr);assert.deepEqual(result.stdout.trim().split(/\r?\n/).map(value=>value.trim()),expected);
    }
});
