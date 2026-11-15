import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {fileURLToPath} from 'node:url';

const root=path.resolve(path.dirname(fileURLToPath(import.meta.url)),'..');
const scratch=path.join(root,'artifacts/dsh-x-root-runtime-integration');

test('actual WorldExports X root phase120 cleanup, sound and reset',()=>{
  const compiler=path.resolve(root,'../../work/tools/emsdk-package/emsdk-main/upstream/emscripten/emcc.py');
  const sdk=path.resolve(path.dirname(compiler),'..','..');
  const python=path.join(sdk,'python/3.13.3_64bit/python.exe');
  assert.ok(fs.existsSync(compiler)&&fs.existsSync(python));
  const host=fs.readFileSync(path.join(scratch,'src/game/PerfectFreezeHost.cpp'),'utf8');
  assert.match(host,/freezeDone_=true;\n        removeEffect\(\);\n        if\(activation_\)/,'phase120 removeEffect after freeze');
  assert.ok(!fs.readFileSync(path.join(root,'src/game/PerfectFreezeHost.cpp'),'utf8')
    .includes('freezeDone_=true;\n        removeEffect();'),'production remains unpatched');
  const sources=[path.relative(root,path.join(scratch,'root-runtime.cpp')),
    path.relative(root,path.join(scratch,'src/runtime/WorldExports.cpp')),
    path.relative(root,path.join(scratch,'src/game/PerfectFreezeHost.cpp')),
    ...fs.readdirSync(path.join(root,'src/game')).filter(name=>name.endsWith('.cpp')&&name!=='PerfectFreezeHost.cpp')
      .map(name=>`src/game/${name}`),
    ...fs.readdirSync(path.join(root,'src/animation')).filter(name=>name.endsWith('.cpp')).map(name=>`src/animation/${name}`),
    ...fs.readdirSync(path.join(root,'src/ecl')).filter(name=>name.endsWith('.cpp')).map(name=>`src/ecl/${name}`)];
  const rsp=path.join(scratch,'sources.rsp');
  fs.writeFileSync(rsp,sources.map(source=>`"${source}"`).join('\n')+'\n');
  const env={...process.env,EM_CONFIG:path.join(sdk,'.emscripten').replaceAll('\\','/'),
    EM_CACHE:path.resolve(root,'../../work/emscripten-cache-4.0.22').replaceAll('\\','/'),
    EMSDK_PYTHON:python,EMCC_CORES:'2',BINARYEN_CORES:'2'};
  const output=path.join(scratch,'root-runtime.cjs');
  const build=spawnSync(python,[compiler,'@'+rsp,
    '-I'+path.relative(root,path.join(scratch,'src')),'-Isrc','-Isrc/runtime','-Isrc/game',
    '-std=c++20','-O1','-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off',
    '-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sNODERAWFS=1',
    '-sEXIT_RUNTIME=1','-sALLOW_MEMORY_GROWTH=1','-o',output],
    {cwd:root,env,encoding:'utf8',timeout:600000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const catalog=JSON.parse(fs.readFileSync(path.join(root,'local/catalog.json'),'utf8'));
  const directory=path.dirname(path.join(root,'local',catalog.variants.ja.manifest));
  const run=spawnSync(process.execPath,[output,directory,path.join(root,'local/runtime/bullet-profile-8.bin'),
    path.join(scratch,'masks.bin'),path.join(scratch,'type13.bin'),path.join(scratch,'type25.bin'),
    path.join(directory,'raw/enemy.anm')],{cwd:root,encoding:'utf8',timeout:180000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS phase120 root draw 2 visible nodes->0, sound 47\/69\/51, protection10, reset clean/);
});
