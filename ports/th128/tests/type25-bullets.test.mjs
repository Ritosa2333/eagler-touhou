import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {fileURLToPath} from 'node:url';
import {spawnSync} from 'node:child_process';
import {createHash} from 'node:crypto';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const compiler=process.env.TH128_EMCC??path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk=path.resolve(path.dirname(compiler),'../..');
const python=process.env.TH128_PYTHON??path.join(sdk,'python/3.13.3_64bit/python.exe');
const environment={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
  EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
  EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
const sources=['src/game/BulletManager.cpp','src/game/BulletRules.cpp','src/game/BulletTickRules.cpp',
  'src/game/BulletVisuals.cpp','src/game/BulletUpdateHost.cpp','src/game/BulletCollisionRules.cpp',
  'src/game/GrazeRules.cpp','src/game/IceContact.cpp','src/game/BulletExtension1.cpp',
  'src/game/BulletExtension4.cpp','src/game/BulletExtension16.cpp','src/animation/AnmLogic.cpp'];

test('type25 original profile, generic birth, identity ANM, secondary tick, freeze and removal',()=>{
  const report=JSON.parse(fs.readFileSync(path.join(root,'local/research/type25-bullet-fixtures.json')));
  assert.equal(report.schema,'th128-original-type25-bullet/1');
  assert.equal(report.executable.sha256,'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4');
  assert.equal(report.executable.sha256,report.executable.sha256After);
  const profile=report.profile;
  assert.equal(profile.address,'0x004b40f0');
  assert.equal(profile.bytes,288);
  assert.equal(profile.script,102);
  assert.equal(profile.secondaryScript,103);
  assert.equal(profile.hitboxSize,6);
  assert.equal(profile.auxiliarySize,22);
  assert.equal(profile.appearance,2);
  assert.equal(profile.selector,8);
  assert.equal(profile.freezePolicy,1);
  assert.equal(profile.identitySpriteRemap,true);
  assert.equal(profile.sha256,createHash('sha256').update(Buffer.from(profile.hex,'hex')).digest('hex'));
  assert.ok(report.callbackCases.every(item=>item.source===item.eax&&item.stop.reason==='returned'));
  assert.ok(report.spawnCases.every(item=>item.state.cancellationScript===0x173&&
    item.state.profileValue===2&&item.state.type===25));
  assert.ok(report.tickCases.length>=4&&report.tickCases.every(item=>item.continuation));
  const code=report.code;
  assert.ok(code.dedicatedSpawn.some(item=>item[2].includes('0x4b40f0')));
  assert.ok(code.callback.some(item=>item[2].includes('0x4b24d4')));
  assert.ok(code.updateSecondaryOrder.some(item=>item[2].includes('0x4f0')));
  assert.ok(code.freezePolicy.some(item=>item[2].includes('0x118')));
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json')));
  for(const variant of ['ja','zh']){
    const base=path.dirname(path.join(root,'local',catalog.variants[variant].manifest));
    const asset=JSON.parse(fs.readFileSync(path.join(base,'animations/bullet-type25-profile.json')));
    assert.equal(asset.schema,'th128-original-typed-bullet-profile/1');
    assert.deepEqual(asset.supportedTypes,[25]);
    assert.equal(asset.type,25);
    assert.equal(asset.hex,profile.hex);
    assert.equal(asset.sha256,profile.sha256);
    assert.equal(asset.secondaryScript,103);
    assert.equal(asset.freezePolicy,1);
    assert.equal(asset.identitySpriteRemap,true);
  }
  const manager=fs.readFileSync(path.join(root,'src/game/BulletManager.cpp'),'utf8');
  const visuals=fs.readFileSync(path.join(root,'src/game/BulletVisuals.cpp'),'utf8');
  const update=fs.readFileSync(path.join(root,'src/game/BulletUpdateHost.cpp'),'utf8');
  const ticks=fs.readFileSync(path.join(root,'src/game/BulletTickRules.cpp'),'utf8');
  const rules=fs.readFileSync(path.join(root,'src/game/BulletRules.cpp'),'utf8');
  assert.match(manager,/selectedType!=25/,'type25 profile is not accepted');
  assert.match(manager,/cancellationMode_==8/,'type25 cancellation selector is not consumed');
  assert.match(manager,/identitySpriteRemap/,'identity sentinel is not exposed');
  assert.match(visuals,/info\.type==25/,'type25 visual dispatch is missing');
  assert.match(visuals,/secondaryScript!=103/,'type25 secondary ANM is not required');
  assert.match(visuals,/identityRemap_/,'identity remap path is missing');
  assert.ok(!update.includes('secondary bullet update completion order is not implemented'),'secondary update is still rejected');
  assert.match(ticks,/type!=25/,'type25 tick domain is missing');
  assert.match(rules,/type!=25/,'type25 spawn prefix domain is missing');
  fs.mkdirSync(path.join(root,'artifacts/tests'),{recursive:true});
  const profilePath=path.join(root,'artifacts/tests/type25-profile.bin');
  fs.writeFileSync(profilePath,Buffer.from(profile.hex,'hex'));
  const animation=path.join(root,'artifacts/dsh-readonly-inputs/bullet.anm');
  assert.equal(createHash('sha256').update(fs.readFileSync(animation)).digest('hex'),
    '9860822687f33e740f2a5e1fb25a4b05cf27b0fc6d15ed9626fb903986f7784c');
  const output=path.join(root,'artifacts/tests/type25-bullets.cjs');
  const build=spawnSync(python,[compiler,'tests/native/type25-bullets.cpp',...sources,'-Isrc','-std=c++20','-O1',
    '-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off',
    '-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1',
    '-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env:environment,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output,profilePath,animation],{cwd:root,encoding:'utf8',timeout:120000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS type25 profile, generic birth, identity ANM, secondary tick, freeze and removal/);
});
