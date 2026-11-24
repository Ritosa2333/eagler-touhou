import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import test from 'node:test';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const scratch=path.join(root,'artifacts/tests/x-phase120-sound-integration');

test('production phase120 journal emits 47/69/51 once, phase121 adds none, budget fails before freeze',()=>{
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'..','..');
  const python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  assert.ok(fs.existsSync(compiler)&&fs.existsSync(python));
  fs.mkdirSync(scratch,{recursive:true});
  const sources=['tests/native/x-phase120-sound-integration.cpp',
    'src/animation/AnmLogic.cpp','src/animation/AnmScene.cpp','src/ecl/EclProgram.cpp','src/ecl/EclVm.cpp',
    ...fs.readdirSync(path.join(root,'src/game')).filter(name=>name.endsWith('.cpp')).map(name=>`src/game/${name}`)];
  const rsp=path.join(scratch,'sources.rsp');
  fs.writeFileSync(rsp,sources.map(source=>`"${source}"`).join('\n')+'\n');
  const env={...process.env,
    EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
    EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  const output=path.join(scratch,'x-phase120-sound-integration.cjs');
  const build=spawnSync(python,[compiler,'@'+rsp,
    '-Isrc','-Isrc/runtime','-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions',
    '-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
    '-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],
    {cwd:root,env,encoding:'utf8',timeout:600000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
  const directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
  const masks=JSON.parse(fs.readFileSync(path.join(directory,'animations/ice-masks.json')));
  const maskFile=path.join(scratch,'masks.bin');
  fs.writeFileSync(maskFile,Buffer.concat(masks.profiles.map(profile=>Buffer.from(profile.hex,'hex'))));
  const type13=JSON.parse(fs.readFileSync(path.join(directory,'animations/bullet-type13-profile.json')));
  const type25=JSON.parse(fs.readFileSync(path.join(directory,'animations/bullet-type25-profile.json')));
  const type13File=path.join(scratch,'type13.bin');
  const type25File=path.join(scratch,'type25.bin');
  fs.writeFileSync(type13File,Buffer.from(type13.hex,'hex'));
  fs.writeFileSync(type25File,Buffer.from(type25.hex,'hex'));
  const profile8=path.join(root,'local/runtime/bullet-profile-8.bin');
  const run=spawnSync(process.execPath,[output,directory,profile8,maskFile,type13File,type25File,
    path.join(directory,'raw/enemy.anm')],{cwd:root,encoding:'utf8',timeout:180000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  console.log(run.stdout);
  assert.match(run.stdout,/PASS phase120\/121 sound journal 47,69,51/);
});
