import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('TH128 original circular Boss HP, warning VM and exact phase countdown use original assets and CPU fixtures',()=>{
  assert.ok(fs.existsSync(path.join(root,'src/game/BossHud.cpp')),'original Boss HUD implementation is missing');
  const evidence=JSON.parse(fs.readFileSync(path.join(root,'local/research/boss-hud-fixtures.json')));
  assert.equal(evidence.apiReturnsAssigned,false);
  assert.equal(createHash('sha256').update(fs.readFileSync(path.join(root,'tools/research/boss_hud_probe.py'))).digest('hex'),evidence.generatorSha256);
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  const output=path.join(root,'artifacts/tests/boss-hud.cjs');
  const build=spawnSync(python,[compiler,'tests/native/boss-hud.cpp','src/game/BossHud.cpp','src/game/RingGeometry.cpp','src/game/EnemyPhaseRules.cpp','src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp',
    '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
    '-sENVIRONMENT=node','-sEXIT_RUNTIME=1','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'))),directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
  const cases=[];const expected=[];
  for(const sample of evidence.fillCases){cases.push(`F ${sample.input.join(' ')}`);expected.push(`${sample.fillBits} ${sample.targetBits}`);}
  for(const sample of evidence.timerCases){cases.push(`T ${sample.input.map(value=>typeof value==='boolean'?Number(value):value).join(' ')}`);expected.push(`${sample.irq} ${sample.sound}`);}
  for(const sample of evidence.markerCases){cases.push(`M ${sample.angle}`);expected.push(sample.positionBits.join(' '));}
  for(const sample of evidence.ringCases){cases.push(`R ${sample.input.join(' ')}`);expected.push(sample.vertices.flat().join(' '));}
  const run=spawnSync(process.execPath,[output,path.join(directory,'raw/front.anm')],{cwd:root,input:cases.join('\n')+'\n',encoding:'utf8',timeout:30000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  const lines=run.stdout.trim().split(/\r?\n/);assert.equal(lines.pop(),'original Boss HUD lifecycle and frame countdown passed');
  assert.deepEqual(lines,expected);
});
