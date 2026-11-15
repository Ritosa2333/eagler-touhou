import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('respawn entry and post-field pending control interrupts match original instruction fixtures',()=>{
    assert.ok(fs.existsSync(path.join(root,'src/game/PlayerRespawnRules.cpp')),'portable respawn entry/control plans are missing');
    const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/player-respawn-fixtures.json')));
    assert.equal(report.executable.sha256,report.executable.sha256After);
    assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/player_respawn_probe.py'))).digest('hex'),report.generatorSha256);
    const input=[],expected=[];
    for(const sample of report.entryCases){
        const radius=report.radiusCases.find(value=>value.elapsed===sample.elapsed)?.radiusBits;
        if(sample.elapsed<30&&!Number.isInteger(radius))continue;
        input.push(`E ${sample.elapsed}`);
        expected.push(`${sample.rawVertical} ${Buffer.from(sample.memory.pixelVertical.after,'hex').readUInt32LE()} ${sample.elapsed<30?radius:0}`);
    }
    for(const sample of report.controlCases){input.push(`C ${sample.selector} ${sample.value}`);expected.push(`2 ${sample.pendingInterrupts.join(' ')}`);}
    input.push('X');expected.push('boundary');
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    const output=path.join(root,'artifacts/tests/player-respawn.cjs');
    const build=spawnSync(python,[compiler,'tests/native/player-respawn.cpp','src/game/PlayerRespawnRules.cpp','-Isrc','-std=c++20','-O1',
        '-Wall','-Wextra','-Werror','-fno-exceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
        '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const run=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
});
