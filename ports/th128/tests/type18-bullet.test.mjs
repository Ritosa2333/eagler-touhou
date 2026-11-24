import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const bits=value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();};
for(const selectedType of [18,20])test(`typed${selectedType} profile, native spawn/update and actual animation lifecycles preserve original resources`,()=>{
    const report=JSON.parse(fs.readFileSync(path.join(root,`local/research/type${selectedType}-bullet-fixtures.json`)));
    const mode=JSON.parse(fs.readFileSync(path.join(root,'local/research/mode2-emission-fixtures.json')));
    assert.equal(report.executable.sha256,report.executable.sha256After);
    const hash=bytes=>createHash('sha256').update(bytes).digest('hex');
    assert.equal(hash(fs.readFileSync(path.join(root,'tools/research/type18_bullet_probe.py'))),report.generatorSha256);
    const selectedProfile=selectedType===20?report.profile:mode.profile;
    const profile=Buffer.from(selectedProfile.bytes,'hex');assert.equal(hash(profile),selectedProfile.sha256);
    const profilePath=path.join(root,`artifacts/tests/type${selectedType}-profile.bin`);fs.writeFileSync(profilePath,profile);
    const input=[],expected=[],categories=['NeedsAnimation','NeedsCollision','NeedsExtension','Removed'];
    for(const sample of report.spawnCases){
        input.push(`S ${sample.color} ${sample.extension} ${sample.flagsBefore}`);const state=sample.state;
        expected.push([state.flags,state.motionState,state.animationInterrupt,state.extensionIndex,state.profileValue,
            ...state.sizeBits.slice(0,1),state.sizeBits[2],...state.positionBits,...state.velocityBits].join(' '));
    }
    for(const sample of report.tickCases){
        const state=sample.input,next=sample.next;
        input.push(['T',state.type,state.motionState,state.age,state.lifeClock,bits(state.lifeTime),state.integer0,state.flags,
            state.activeExtensions,state.grace,state.cooldown,Number(state.hasSprite),bits(state.spriteWidth),bits(state.spriteHeight),
            bits(state.clock),Number(state.extensionListEmpty),...state.positionBits,...state.velocityBits].join(' '));
        expected.push([categories.indexOf(sample.continuation),sample.boundary,sample.previousLifeClock,next.motionState,next.lifeClock,
            bits(next.lifeTime),next.flags,next.grace,next.cooldown,...next.positionBits,...next.velocityBits].join(' '));
    }
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    const output=path.join(root,`artifacts/tests/type${selectedType}-bullet.cjs`);
    const build=spawnSync(python,[compiler,'tests/native/type18-bullet.cpp','src/game/BulletManager.cpp','src/game/BulletRules.cpp',
        'src/game/BulletTickRules.cpp','src/game/BulletVisuals.cpp','src/game/CancellationEffects.cpp','src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp',
        '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
        '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'))),raw=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
    const run=spawnSync(process.execPath,[output,profilePath,path.join(raw,'raw/bullet.anm'),...(selectedType===20?['20']:[])],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:60000});
    assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
});
