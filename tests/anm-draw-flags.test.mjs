import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('TH12865 anchors and67 render modes use original20/22/24 flag shifts, not TH12 inherited19/21/23',()=>{
    const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/draw-pipeline-fixtures.json')));
    assert.ok(report.flagCases?.length,'bounded original65/66/67 flag evidence is missing');
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    const output=path.join(root,'artifacts/tests/anm-draw-flags.cjs');
    const build=spawnSync(python,[compiler,'tests/native/anm-draw-flags.cpp','src/animation/AnmLogic.cpp',
        '-Isrc','-std=c++20','-O1','-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
        '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sNODERAWFS=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const samples=report.flagCases.filter(sample=>sample.stop.reason==='boundary');
    const input=samples.map(sample=>`${sample.opcode} ${sample.value}`).join('\n')+'\n';
    const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json')));
    const resource=path.join(root,'local',path.dirname(catalog.variants.ja.manifest),'raw/text.anm');
    const run=spawnSync(process.execPath,[output,resource],{cwd:root,input,encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);
    const actual=run.stdout.trim().split(/\r?\n/).map(line=>line.split(/\s+/).map(Number));
    assert.deepEqual(actual,samples.map(sample=>[sample.flagsAfter,sample.anchorX,sample.anchorY,sample.mode,sample.blend]));
});
