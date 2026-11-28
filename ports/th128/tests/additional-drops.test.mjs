import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const bits=value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();};
test('additional drops preserve original scatter, RNG order, angle tails and actual item births',()=>{
    assert.ok(fs.existsSync(path.join(root,'src/game/EnemyDropEmission.cpp')),'actual additional drop operation is missing');
    const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/enemy-drop-emission-fixtures.json')));
    assert.equal(report.executable.sha256,report.executable.sha256After);
    assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/enemy_drop_emission_probe.py'))).digest('hex'),report.generatorSha256);
    const input=[],expected=[];
    for(const sample of report.entryCases){input.push(['E',sample.seedBefore,bits(sample.width),bits(sample.height)].join(' '));
        expected.push([sample.itemKind,sample.seedAfter,sample.callsAfter,...sample.positionBits,sample.angleBits,sample.speedBits].join(' '));}
    for(const sample of report.angleCases){input.push(['A',sample.seedBefore,bits(sample.angleBefore)].join(' '));
        expected.push([sample.angleAfterBits,sample.seedAfter,sample.callsAfter].join(' '));}
    for(const sample of report.geometryCases){input.push(['G',sample.seedBefore,bits(sample.angleBefore),bits(sample.width),bits(sample.height)].join(' '));
        expected.push([sample.seedAfter,sample.callsAfter,...sample.positionBits].join(' '));}
    for(const sample of report.emptyCases){input.push(`Z ${sample.seedBefore}`);expected.push(`${sample.seedAfter} ${sample.callsAfter}`);}
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    const output=path.join(root,'artifacts/tests/additional-drops.cjs');
    const build=spawnSync(python,[compiler,'tests/native/additional-drops.cpp','src/game/EnemyDropEmission.cpp','src/game/EnemyDropRules.cpp',
        'src/game/ItemHost.cpp','src/game/ItemRules.cpp','src/game/PlayerShotMotion.cpp','src/game/EnemyRules.cpp','src/animation/AnmLogic.cpp',
        '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
        '-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'))),raw=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
    const run=spawnSync(process.execPath,[output,path.join(raw,'raw/bullet.anm')],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
});
