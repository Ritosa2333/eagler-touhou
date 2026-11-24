import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const bits=value=>{const bytes=Buffer.alloc(4);bytes.writeFloatLE(value);return bytes.readUInt32LE();};
test('type16 mode2 unit timer, deceleration, aimed restart and real sound boundary match original paths',()=>{
    assert.ok(fs.existsSync(path.join(root,'src/game/BulletExtension16.cpp')),'native type16 mode2 lifecycle is missing');
    const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/bullet-extension16-fixtures.json')));
    assert.equal(report.executable.sha256,report.executable.sha256After);
    assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/bullet_extension16_probe.py'))).digest('hex'),report.generatorSha256);
    const pack=state=>[Number((state.active&16)!==0),state.timer[0]|0,state.timer[1]|0,state.timer[2],state.period,state.limit,state.count,state.mode,state.angleMode,
        state.extensionSpeedBits,state.extensionAngleBits,state.speedBits,state.angleBits,...state.velocityBits].join(' ');
    const input=[],expected=[];
    for(const sample of report.initializationCases){input.push(['I',bits(sample.offset),bits(sample.speed),sample.period,sample.limit,sample.mode].join(' '));expected.push(pack(sample.stateAfter));}
    for(const sample of report.tickCases){input.push(['T',sample.time,sample.period,sample.limit,sample.count,bits(sample.speed),bits(sample.angle),bits(sample.offset),sample.sound].join(' '));expected.push(pack(sample.stateAfter));}
    const sound=report.updateSound;input.push(['T',sound.time,sound.period,sound.limit,sound.count,bits(sound.speed),bits(sound.angle),bits(sound.offset),sound.sound].join(' '));expected.push('sound 40');
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    for(const sanitize of [false,true]){
        const output=path.join(root,`artifacts/tests/bullet-extension16${sanitize?'-ubsan':''}.cjs`);
        const build=spawnSync(python,[compiler,'tests/native/bullet-extension16.cpp','src/game/BulletExtension16.cpp','src/game/BulletRules.cpp',
            '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off',...(sanitize?['-fsanitize=undefined','-fno-sanitize-recover=all']:[]),
            '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
        assert.equal(build.status,0,build.stdout+build.stderr);
        const run=spawnSync(process.execPath,[output],{cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000});
        assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
    }
});
test('whole original pre-ANM update agrees with real host extension/motion ordering and both cooldown sites',()=>{
    const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/bullet-extension16-fixtures.json')));
    const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
    const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
    const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
    const output=path.join(root,'artifacts/tests/extension16-integrated.cjs');
    const build=spawnSync(python,[compiler,'tests/native/extension16-integrated.cpp','src/game/BulletUpdateHost.cpp','src/game/BulletManager.cpp',
        'src/game/BulletExtension16.cpp','src/game/BulletExtension4.cpp','src/game/BulletExtension1.cpp','src/game/BulletRules.cpp','src/game/BulletTickRules.cpp','src/game/BulletVisuals.cpp',
        'src/game/BulletCollisionRules.cpp','src/game/GrazeRules.cpp','src/animation/AnmLogic.cpp',
        '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
        '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'))),directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
    const input=report.integratedCases.map(sample=>[Number(sample.half),Number(sample.initialActive),sample.initialTime,sample.cooldown].join(' '));
    const expected=report.integratedCases.map(sample=>[...sample.positionBits,...sample.stateAfter.velocityBits,sample.lifeClockAfter,
        sample.cooldownAfter,sample.graceAfter,sample.stateAfter.timer[1],sample.stateAfter.active].join(' '));
    const run=spawnSync(process.execPath,[output,path.join(root,'local/runtime/bullet-profile-8.bin'),path.join(directory,'raw/bullet.anm')],
        {cwd:root,input:input.join('\n')+'\n',encoding:'utf8',timeout:30000});
    assert.equal(run.status,0,run.stdout+run.stderr);assert.deepEqual(run.stdout.trim().split(/\r?\n/),expected);
});
