import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const bits=value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();};
test('aim2/3 ring angles, separately rounded layers and velocities match original geometry prefixes',()=>{
    const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/mode2-emission-fixtures.json')));
    assert.equal(report.executable.sha256,report.executable.sha256After);
    assert.equal(report.windowsGameProcessLaunched,false);
    const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
    assert.equal(hash(fs.readFileSync(path.join(root,'tools/research/mode2_emission_probe.py'))),report.generatorSha256);
    for(const code of Object.values(report.code))assert.equal(hash(Buffer.from(code.bytes,'hex')),code.sha256);
    const input=[],expected=[];
    for(const sample of report.geometryCases){
        const emitter=sample.input.emitter;
        input.push([emitter.count,emitter.layers,emitter.aimMode,sample.input.bulletIndex,sample.input.layerIndex,
            ...[emitter.angle,emitter.spacing,emitter.speed,emitter.endSpeed,sample.input.aimAngle].map(bits)].join(' '));
        expected.push(['rawAngle','angle','speed','velocityX','velocityY'].map(name=>sample.geometryBits[name]).join(' '));
    }
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    for(const sanitize of [false,true]){
        const output=path.join(root,`artifacts/tests/mode2-emission${sanitize?'-ubsan':''}.cjs`);
        const build=spawnSync(python,[compiler,'tests/native/mode2-emission.cpp','src/game/BulletRules.cpp','-Isrc','-std=c++20','-O1',
            '-Wall','-Wextra','-Werror','-fno-exceptions','-fno-fast-math','-ffp-contract=off',...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),
            '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
        assert.equal(build.status,0,build.stdout+build.stderr);
        const run=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000});
        assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/).map(line=>line.trim()),expected);
    }
});
