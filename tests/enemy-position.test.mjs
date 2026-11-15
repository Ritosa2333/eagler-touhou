import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('actual301 mode4 position interpolator matches original initialization and complete step returns',()=>{
    const file=path.join(root,'local/research/enemy-position-fixtures.json');assert.ok(fs.existsSync(file),'native301 evidence is missing');
    const report=JSON.parse(fs.readFileSync(file));assert.equal(report.executable.sha256,report.executable.sha256After);
    assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/enemy_position_probe.py'))).digest('hex'),report.generatorSha256);
    const bits=value=>{const b=Buffer.alloc(4);b.writeFloatLE(value);return b.readUInt32LE();};const input=[],expected=[];
    for(const sample of report.initializationCases){const before=sample.input,after=sample.after;
        input.push(['I',...before.position.map(bits),bits(before.x),bits(before.y),before.duration,before.mode].join(' '));
        expected.push([after.duration,after.mode,after.timer[0]|0,after.timer[1]|0,after.timer[2],...after.start,...after.target].join(' '));}
    for(const sample of report.stepCases){const before=sample.input,after=sample.after;
        input.push(['T',...before.start.map(bits),...before.target.map(bits),before.duration,before.elapsed].join(' '));
        expected.push([after.duration,after.timer[0]|0,after.timer[1]|0,after.timer[2],...after.position].join(' '));}
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    for(const sanitize of [false,true]){
        const output=path.join(root,`artifacts/tests/enemy-position${sanitize?'-ubsan':''}.cjs`);
        const build=spawnSync(python,[compiler,'tests/native/enemy-position.cpp','src/game/EnemyRules.cpp','-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off',
            ...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),'-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
        assert.equal(build.status,0,build.stdout+build.stderr);
        const run=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000});
        assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
    }
});
