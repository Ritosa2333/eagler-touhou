import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const signed=value=>value|0;
test('414 registration and full phase health/time decisions match complete original instruction returns',()=>{
    assert.ok(fs.existsSync(path.join(root,'src/game/EnemyPhaseRules.hpp')),'owning phase rule module is missing');
    const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/enemy-phase-fixtures.json')));
    assert.equal(report.executable.sha256,report.executable.sha256After);
    assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/enemy_phase_probe.py'))).digest('hex'),report.generatorSha256);
    const input=[],expected=[];
    for(const sample of report.registrationCases){input.push(['R',sample.input.slot,sample.input.threshold,sample.input.limit,
        ...sample.before.records.flatMap(record=>[signed(record[0]),signed(record[1]),record[2],record[3]])].join(' '));
        expected.push(sample.after.records.flat().join(' '));}
    for(const sample of report.queryCases){const before=sample.before,after=sample.after;
        input.push(['Q',before.life,before.remaining,before.threshold,signed(before.timer[0]),signed(before.timer[1]),...before.timer.slice(2),before.flags,
            ...before.records.flatMap(record=>[signed(record[0]),signed(record[1]),record[2],record[3]]),0xa5a5a5a5,0xa5a5a5a5,
            sample.input.spellFlags,30,0,1,9].join(' '));
        expected.push([sample.target,after.life,after.remaining,after.threshold,signed(after.timer[0]),signed(after.timer[1]),...after.timer.slice(2),after.flags,
            ...after.records.flat(),...sample.hudCountdown,sample.spellFlagsAfter,sample.spell80After,sample.managerWord18].join(' '));}
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    for(const sanitize of [false,true]){
        const output=path.join(root,`artifacts/tests/enemy-phase${sanitize?'-ubsan':''}.cjs`);
        const build=spawnSync(python,[compiler,'tests/native/enemy-phase.cpp','src/game/EnemyPhaseRules.cpp','-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off',
            ...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),'-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
        assert.equal(build.status,0,build.stdout+build.stderr);
        const run=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000});
        assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
    }
});
