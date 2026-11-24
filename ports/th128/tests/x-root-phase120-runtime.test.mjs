import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
const sdk=path.resolve(path.dirname(compiler),'..','..');
const python=path.join(sdk,'python/3.13.3_64bit/python.exe');
const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
  EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
  EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
const common=['-Isrc','-Isrc/runtime','-Isrc/game','-std=c++20','-O1','-Wall','-Wextra','-Werror',
  '-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
  '-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1'];

test('production X root phase120 runtime and failed-freeze ownership',()=>{
  const host=fs.readFileSync(path.join(root,'src/game/PerfectFreezeHost.cpp'),'utf8');
  const hostMatch=/freezeDone_=true;\n        removeEffect\(\);\n        if\(activation_\)/.exec(host);
  assert.ok(hostMatch,'production phase120 removeEffect is present after freeze');
  const exports=fs.readFileSync(path.join(root,'src/runtime/WorldExports.cpp'),'utf8');
  assert.match(exports,/supportedBulletTypes\\":\[4,6,8,10,13,16,18,20,21,23,25,29,30\]/,'Type30 capability preserved');
  assert.match(exports,/perfectFreezeNormalEmptyLasers\\":true/,'conservative X capability preserved');
  assert.match(exports,/51,0,1/,'51 STOP preserved');
  assert.match(exports,/47,perfectSoundX,0/,'47 positional PLAY preserved');

  const stage=path.join(root,'local/generations/8c71c316a4b24e68827f9fc560242050/ja');
  const masks=path.join(root,'artifacts/dsh-x-root-runtime-integration/masks.bin');
  const type13=path.join(root,'artifacts/dsh-x-root-runtime-integration/type13.bin');
  const type25=path.join(root,'artifacts/dsh-x-root-runtime-integration/type25.bin');
  const runtime=path.join(root,'artifacts/dsh-x-root-runtime-integration/production-runtime.cjs');
  const sources=['tests/native/x-root-phase120-runtime.cpp','src/runtime/WorldExports.cpp',
    ...fs.readdirSync(path.join(root,'src/game')).filter(name=>name.endsWith('.cpp')).map(name=>`src/game/${name}`),
    ...fs.readdirSync(path.join(root,'src/animation')).filter(name=>name.endsWith('.cpp')).map(name=>`src/animation/${name}`),
    ...fs.readdirSync(path.join(root,'src/ecl')).filter(name=>name.endsWith('.cpp')).map(name=>`src/ecl/${name}`)];
  const rsp=path.join(root,'artifacts/dsh-x-root-runtime-integration/production-sources.rsp');
  fs.writeFileSync(rsp,sources.map(source=>`"${source}"`).join('\n')+'\n');
  const build=spawnSync(python,[compiler,'@'+rsp,...common,'-o',runtime],
    {cwd:root,env,encoding:'utf8',timeout:600000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[runtime,stage,path.join(root,'local/runtime/bullet-profile-8.bin'),
    masks,type13,type25,path.join(stage,'raw/enemy.anm')],{cwd:root,encoding:'utf8',timeout:180000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS phase120 root draw 2 visible nodes->0, sound 47\/69\/51, protection10, reset clean/);

  const failure=path.join(root,'artifacts/dsh-x-root-runtime-integration/host-failure.cjs');
  const failureBuild=spawnSync(python,[compiler,'tests/native/x-root-phase120-host-failure.cpp',
    'src/game/PerfectFreezeHost.cpp','src/game/FreezeRules.cpp','src/animation/AnmLogic.cpp',
    'src/animation/AnmScene.cpp','-Isrc','-Isrc/game','-std=c++20','-O1','-Wall','-Wextra','-Werror',
    '-fexceptions','-fno-fast-math','-ffp-contract=off','-fsanitize=undefined','-fno-sanitize-recover=all',
    '-sENVIRONMENT=node','-sNODERAWFS=1','-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',failure],
    {cwd:root,env,encoding:'utf8',timeout:300000});
  assert.equal(failureBuild.status,0,failureBuild.stdout+failureBuild.stderr);
  const failureRun=spawnSync(process.execPath,[failure,path.join(stage,'raw/ice.anm')],
    {cwd:root,encoding:'utf8',timeout:120000});
  assert.equal(failureRun.status,0,failureRun.stdout+failureRun.stderr);
  assert.match(failureRun.stdout,/PASS phase120 failed freeze keeps 3 owner nodes, sticky, cleanup once/);
});
