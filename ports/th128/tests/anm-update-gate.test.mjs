import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('actual111 mode flags and original control/IRQ gate preserve paused VM state',()=>{
    const file=path.join(root,'local/research/anm-update-gate-fixtures.json');
    assert.ok(fs.existsSync(file),'original ANM111 control gate fixture is missing');
    const report=JSON.parse(fs.readFileSync(file));assert.equal(report.executable.sha256,report.executable.sha256After);
    assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/anm_update_gate_probe.py'))).digest('hex'),report.generatorSha256);
    const input=[],expected=[];
    for(const sample of report.handlerCases){input.push(['H',sample.previous,sample.value].join(' '));expected.push(String(sample.after));}
    for(const sample of report.gateCases){input.push(['G',sample.flags2,sample.irq,Number(sample.present),sample.controlFlags].join(' '));expected.push(String(Number(sample.paused)));}
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    fs.mkdirSync(path.join(root,'artifacts/tests'),{recursive:true});
    for(const sanitize of [false,true]){
        const output=path.join(root,`artifacts/tests/anm-update-gate${sanitize?'-ubsan':''}.cjs`);
        const build=spawnSync(python,[compiler,'tests/native/anm-update-gate.cpp','src/animation/AnmLogic.cpp','-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off',
            ...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),'-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
        assert.equal(build.status,0,build.stdout+build.stderr);
        const run=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000});
        assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
    }
});
