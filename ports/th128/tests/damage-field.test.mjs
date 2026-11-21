import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const bits=value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();};
test('damage field birth, pool cursor, growth, circle queries and scoring match original bytes in WASM and UBSan',()=>{
    assert.ok(fs.existsSync(path.join(root,'src/game/DamageFieldPool.cpp')),'portable original damage-field pool is missing');
    const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/damage-field-fixtures.json')));
    assert.equal(report.executable.sha256,report.executable.sha256After);
    assert.equal(report.windowsGameProcessLaunched,false);
    assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/damage_field_probe.py'))).digest('hex'),report.generatorSha256);
    const input=[],expected=[];
    for(const sample of report.birthCases){input.push(`B ${sample.duration}`);expected.push(sample.memory.slot.after);}
    for(const sample of report.tailCases){input.push(`T ${sample.memory.slot.before}`);expected.push(sample.memory.slot.after);}
    for(const sample of report.motionCases){input.push(`M ${sample.memory.slot.before}`);expected.push(sample.memory.slot.after);}
    for(const sample of report.queryCases){
        input.push(`Q ${sample.sourceBefore} ${sample.position.map(bits).join(' ')} ${sample.memory.score.beforeI32}`);
        expected.push(`${sample.damage} ${sample.outFlagAfter} ${sample.scoreAfter} ${sample.sourceAfter}`);
    }
    for(const sample of report.poolBirthCases){
        input.push(`P ${sample.cursorBefore} ${Number(sample.full)} ${sample.occupied.length} ${sample.occupied.join(' ')}`);
        expected.push(`${sample.selectedSlot} ${sample.cursorAfter} ${sample.selectedAfter}`);
    }
    for(const [shots,sources,score,damage,next] of [[260,0,0,160,2],[260,150,0,310,4],[260,2500,0,2000,21],
        [0,0,100,0,100],[1,0,0,1,1],[26,0,0,26,1],[160,1900,999999998,2000,999999999]]){
        input.push(`F ${shots} ${sources} ${score}`);expected.push(`${damage} ${next}`);
    }
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    for(const sanitize of [false,true]){
        const output=path.join(root,`artifacts/tests/damage-field${sanitize?'-ubsan':''}.cjs`);
        const build=spawnSync(python,[compiler,'tests/native/damage-field.cpp','src/game/DamageFieldPool.cpp','-Isrc','-std=c++20','-O1',
            '-Wall','-Wextra','-Werror','-fno-exceptions','-fno-fast-math','-ffp-contract=off',...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),
            '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
        assert.equal(build.status,0,build.stdout+build.stderr);
        const run=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000});
        assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
    }
});
