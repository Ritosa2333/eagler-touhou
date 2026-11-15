import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';
const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
test('owning ice host retains native lists, delayed contacts, cloned bullets, masks and shatter obligations',()=>{
 assert.ok(fs.existsSync(path.join(root,'src/game/IceHost.hpp')),'owning original ice host missing');
 const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
 const sdk=path.resolve(path.dirname(compiler),'../..'),python=path.join(sdk,'python/3.13.3_64bit/python.exe');
 const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
 const output=path.join(root,'artifacts/tests/ice-host.cjs');
 const build=spawnSync(python,[compiler,'tests/native/ice-host.cpp','src/game/IceHost.cpp','src/game/IceBirthRules.cpp','src/game/IceMaskRules.cpp','src/game/IceContact.cpp',
 'src/game/BulletVisuals.cpp','src/game/BulletRules.cpp','src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp',
 '-Isrc','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-o',output],{cwd:root,env,encoding:'utf8',timeout:180000});
 assert.equal(build.status,0,build.stdout+build.stderr);
 const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'))),directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
 const masks=JSON.parse(fs.readFileSync(path.join(directory,'animations/ice-masks.json'))),maskFile=path.join(root,'artifacts/tests/ice-masks.bin');
 fs.writeFileSync(maskFile,Buffer.concat(masks.profiles.map(profile=>Buffer.from(profile.hex,'hex'))));
 const run=spawnSync(process.execPath,[output,path.join(directory,'raw/ice.anm'),path.join(directory,'raw/bullet.anm'),maskFile],{cwd:root,encoding:'utf8',timeout:30000});
 assert.equal(run.status,0,run.stdout+run.stderr);assert.match(run.stdout,/native ice ownership and delayed propagation/);
});
