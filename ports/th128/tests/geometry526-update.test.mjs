import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const bits=value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();};
const words=hex=>{const bytes=Buffer.from(hex,'hex');return Array.from({length:bytes.length/4},(_,index)=>bytes.readUInt32LE(index*4));};
test('526 effect growth, colors, border clamps, wave positions, retained angles and strips match original update',context=>{
    const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/geometry526-update-fixtures.json')));
    assert.equal(report.executable.sha256,report.executable.sha256After);
    assert.equal(report.normalize.dll.sha256,report.normalize.dll.sha256After);
    assert.equal(report.apiReturnsAssigned,false);assert.equal(report.allocationExecuted,false);
    assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/geometry526_update_probe.py'))).digest('hex'),report.generatorSha256);
    const input=report.updateCases.map(({input})=>[input.columns,input.rows,bits(input.radius),bits(input.scale),input.color,
        ...input.position.map(bits),...input.angles.map(bits),bits(input.delta)].join(' '));
    const expected=report.updateCases.map(sample=>[bits(sample.after.scale),...sample.after.angles.map(bits),
        ...words(sample.verticesAfter),...words(sample.positionsAfter),...words(sample.stripsAfter)]);
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    for(const sanitize of [false,true]){
        const output=path.join(root,`artifacts/tests/geometry526-update${sanitize?'-ubsan':''}.cjs`);
        const build=spawnSync(python,[compiler,'tests/native/geometry526-update-vectors.cpp','src/game/Geometry526.cpp','-Isrc','-std=c++20','-O1',
            '-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off',...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),
            '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
        assert.equal(build.status,0,build.stdout+build.stderr);
        const run=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000,maxBuffer:1024*1024});
        assert.equal(run.status,0,run.stdout+run.stderr);
        const actual=run.stdout.trim().split(/\r?\n/).map(line=>line.trim().split(/\s+/).map(Number));
        let largestPixelDifference=0,differentPositions=0;
        const scalar=word=>{const bytes=Buffer.alloc(4);bytes.writeUInt32LE(word);return bytes.readFloatLE();};
        assert.equal(actual.length,expected.length);
        for(let index=0;index<expected.length;++index){
            const sample=report.updateCases[index],vertexCount=sample.input.columns*sample.input.rows;
            const stripsStart=3+vertexCount*10;
            assert.equal(actual[index].length,expected[index].length);
            for(let field=0;field<expected[index].length;++field){
                const wanted=expected[index][field],got=actual[index][field];
                if(wanted===got)continue;
                const meshPosition=(field>=3&&field<3+vertexCount*7&&(field-3)%7<2)
                    ||(field>=stripsStart&&(field-stripsStart)%7<2);
                assert.ok(meshPosition,`non-position word differs: case=${index}, field=${field}, ${got} != ${wanted}`);
                const difference=Math.abs(scalar(wanted)-scalar(got));
                assert.ok(Math.abs(wanted-got)<=2&&Number.isFinite(difference)&&difference<=1/16384,
                    `position exceeds portable RSQRT precision: case=${index}, field=${field}, delta=${difference}`);
                largestPixelDifference=Math.max(largestPixelDifference,difference);++differentPositions;
            }
        }
        context.diagnostic(`UBSan=${sanitize}: approximate native SSE2 RSQRT vs portable float seed; ${differentPositions} position words differ, maximum ${largestPixelDifference} pixels; every color/UV/base position/angle word is exact`);
    }
});
