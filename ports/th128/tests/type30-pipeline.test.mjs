import assert from 'node:assert/strict';
import test from 'node:test';
import crypto from 'node:crypto';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const generation=path.join(root,'local/generations/8c71c316a4b24e68827f9fc560242050');
const AsyncFunction=Object.getPrototypeOf(async()=>{}).constructor;

function functionBody(name){
  const source=fs.readFileSync(path.join(root,'site/play.mjs'),'utf8');
  const marker=`async function ${name}(`;
  const start=source.indexOf(marker);
  assert.notEqual(start,-1,`missing ${name}`);
  const following=source.indexOf('\nfunction ',start+marker.length);
  const text=source.slice(start,following===-1?source.length:following);
  const open=text.indexOf('{'),close=text.lastIndexOf('}');
  return text.slice(open+1,close);
}

test('production type30 profile, loader contract, admission and registration path',async()=>{
  const profile=JSON.parse(fs.readFileSync(path.join(generation,'ja/animations/bullet-type30-profile.json'),'utf8'));
  const zh=JSON.parse(fs.readFileSync(path.join(generation,'zh/animations/bullet-type30-profile.json'),'utf8'));
  assert.deepEqual(zh,profile);
  assert.equal(profile.schema,'th128-original-typed-bullet-profile/1');
  assert.equal(profile.type,30);
  assert.equal(profile.bytes,288);
  assert.equal(profile.sha256,'4950c5e4d6027c3d401708d3ca097ec0a337c642a1068b8287926423460ba5a3');
  assert.equal(profile.script,285);
  assert.equal(profile.secondaryScript,286);
  assert.equal(profile.freezePolicy,1);
  assert.equal(profile.identitySpriteRemap,true);
  assert.deepEqual(profile.colorRemaps[0],[-1,-1,0,373]);
  assert.deepEqual(profile.cancellationScripts,[381]);
  assert.deepEqual(profile.supportedTypes,[30]);
  assert.equal(crypto.createHash('sha256').update(Buffer.from(profile.hex,'hex')).digest('hex'),profile.sha256);

  const source=fs.readFileSync(path.join(root,'site/play.mjs'),'utf8');
  assert.match(source,/function supportsBulletType30\(\)/);
  assert.match(source,/animations\/bullet-type30-profile\.json/);
  assert.match(source,/_th128_world_add_bullet_profile\(30,pointer,type30Bytes\.length\)/);
  assert.match(fs.readFileSync(path.join(root,'src/runtime/WorldExports.cpp'),'utf8'),/supportedBulletTypes[^\n]*\[4,6,8,10,13,16,18,20,21,23,25,29,30\]/);

  const loader=new AsyncFunction('crypto','type30SourceExeSha256','type30SourceAddress','type30ProfileBytes',
    'type30ProfileSha256','type30ColorRemaps','type30CancellationScripts','type30Callback','type6ProfileFields','profile',
    functionBody('type30ProfileBytesFromRecord'));
  const args=[crypto,'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4','0x4b4690',288,
    profile.sha256,profile.colorRemaps,[381],profile.callback,new Set(Object.keys(profile))];
  const bytes=await loader(...args,profile);
  assert.equal(bytes.length,288);
  assert.equal(crypto.createHash('sha256').update(Buffer.from(bytes)).digest('hex'),profile.sha256);
  const badSecondary=structuredClone(profile);badSecondary.secondaryScript=285;
  await assert.rejects(loader(...args,badSecondary),/type30 bullet profile provenance/);
  const badAdmission=structuredClone(profile);badAdmission.supportedTypes=[30,31];
  await assert.rejects(loader(...args,badAdmission),/type30 bullet profile provenance/);
  const changedRaw=structuredClone(profile);changedRaw.colorRemaps[0][3]=381;
  await assert.rejects(loader(...args,changedRaw),/type30 bullet profile provenance/);
});

test('production canonical Manager tick and cached381 cancellation family',()=>{
  const scratch=fs.mkdtempSync(path.join(os.tmpdir(),'th128-type30-'));
  const response=path.join(scratch,'sources.rsp');
  const sources=['tests/native/type30-pipeline.cpp',
    ...fs.readdirSync(path.join(root,'src/game')).filter(name=>name.endsWith('.cpp')).map(name=>`src/game/${name}`),
    'src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp'];
  fs.writeFileSync(response,sources.map(source=>`"${source}"`).join('\n')+'\n');
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
    EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  const executable=path.join(scratch,'type30-pipeline.cjs');
  const build=spawnSync(python,[compiler,'@'+response,'-Isrc','-Isrc/game','-std=c++20','-O1','-Wall','-Wextra','-Werror',
    '-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
    '-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',executable],
    {cwd:root,env,encoding:'utf8',timeout:600000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const profile=JSON.parse(fs.readFileSync(path.join(generation,'ja/animations/bullet-type30-profile.json'),'utf8'));
  const profilePath=path.join(scratch,'type30-profile.bin');
  fs.writeFileSync(profilePath,Buffer.from(profile.hex,'hex'));
  const run=spawnSync(process.execPath,[executable,profilePath,
    path.join(generation,'ja/raw/bullet.anm'),path.join(root,'tests/fixtures/type30/native-jump-table.bin'),
    path.join(root,'tests/fixtures/type30/native-dispatch-code.bin')],{cwd:root,encoding:'utf8',timeout:180000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS type30 canonical production Manager\/constructor\/visual\/UpdateHost\/cancel381 rng=24 retirement=40/);
});
