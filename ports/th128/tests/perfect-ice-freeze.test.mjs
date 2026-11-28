import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('perfect ice freeze batch clones source, consumes once, uses selector1 and retires without deferred completion',()=>{
  const fixture=JSON.parse(fs.readFileSync(path.join(root,'local/research/perfect-bullet-integration-fixtures.json'),'utf8'));
  assert.equal(fixture.schema,'th128-perfect-bullet-integration/1');
  assert.equal(fixture.freezeBody.motionStateField,'Birth.state.motionState (+0xa2a)');
  assert.equal(fixture.freezeBody.retireMappedToManagerConsume,true);
  assert.equal(fixture.code.bullet_reset.endExclusive,'0x408055');
  assert.ok(fixture.acceptanceCases.some(value=>value.id==='perfect-source-retires-via-consume-once'));

  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  const environment={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
    EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  const output=path.join(root,'artifacts/tests/perfect-ice-freeze.cjs');
  fs.mkdirSync(path.dirname(output),{recursive:true});
  const build=spawnSync(python,[compiler,'tests/native/perfect-ice-freeze.cpp','src/game/IceHost.cpp',
    'src/game/IceBirthRules.cpp','src/game/IceMaskRules.cpp','src/game/IceContact.cpp','src/game/BulletManager.cpp','src/game/BulletVisuals.cpp',
    'src/game/BulletRules.cpp','src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp','-Isrc','-std=c++20','-O1',
    '-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined',
    '-fno-sanitize-recover=all','-sENVIRONMENT=node','-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=67108864','-sEXIT_RUNTIME=1','-o',output],
  {cwd:root,env:environment,encoding:'utf8',timeout:180000});
  assert.equal(build.status,0,build.stdout+build.stderr);

  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
  const directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
  const masks=JSON.parse(fs.readFileSync(path.join(directory,'animations/ice-masks.json')));
  const maskFile=path.join(root,'artifacts/tests/perfect-ice-masks.bin');
  fs.writeFileSync(maskFile,Buffer.concat(masks.profiles.map(profile=>Buffer.from(profile.hex,'hex'))));
  const type25=JSON.parse(fs.readFileSync(path.join(directory,'animations/bullet-type25-profile.json')));
  const type13=JSON.parse(fs.readFileSync(path.join(directory,'animations/bullet-type13-profile.json')));
  const type25File=path.join(root,'artifacts/tests/perfect-type25.bin');
  const type13File=path.join(root,'artifacts/tests/perfect-type13.bin');
  fs.writeFileSync(type25File,Buffer.from(type25.hex,'hex'));fs.writeFileSync(type13File,Buffer.from(type13.hex,'hex'));
  const run=spawnSync(process.execPath,[output,path.join(directory,'raw/ice.anm'),path.join(directory,'raw/bullet.anm'),
    maskFile,type25File,type13File],{cwd:root,encoding:'utf8',timeout:60000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/perfect ice freeze: type25 override, type13 freeze/);
});
