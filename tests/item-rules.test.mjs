import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const bits=value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();};
test('ordinary item motion and exact player-contact bounds match original byte prefixes in WASM and UBSan',()=>{
    const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/item-fixtures.json')));
    assert.equal(report.executable.sha256,report.executable.sha256After);
    assert.equal(report.windowsGameProcessLaunched,false);
    assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/item_probe.py'))).digest('hex'),report.generatorSha256);
    const input=[],expected=[];
    for(const sample of report.tickCases){
        input.push(['M',...sample.position.map(bits),...sample.velocity.map(bits),sample.playerState,bits(sample.playerY)].join(' '));
        const image=Buffer.from(sample.memory.item.after,'hex');
        expected.push([sample.stop.address==='0x0042872c'?1:0,...Array.from({length:6},(_,index)=>image.readUInt32LE(0x9d4+index*4))].join(' '));
    }
    for(const sample of report.contactCases){
        input.push(['C',...sample.position.map(bits),Number(sample.focused)].join(' '));
        expected.push(sample.stop.address==='0x00428b52'?'1':sample.stop.address==='0x00428f92'?'2':'0');
    }
    for(const sample of report.pointCases){
        input.push(['P',sample.value,bits(sample.playerY),sample.state].join(' '));expected.push(String(sample.reward));
    }
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    for(const sanitize of [false,true]){
        const output=path.join(root,`artifacts/tests/item-rules${sanitize?'-ubsan':''}.cjs`);
        const build=spawnSync(python,[compiler,'tests/native/item-rules.cpp','src/game/ItemRules.cpp','-Isrc','-std=c++20','-O1',
            '-Wall','-Wextra','-Werror','-fno-exceptions','-fno-fast-math','-ffp-contract=off',...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),
            '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
        assert.equal(build.status,0,build.stdout+build.stderr);
        const run=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000});
        assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
    }
});
