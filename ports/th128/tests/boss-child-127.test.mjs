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

test('boss child127 one-layer topology matches real bullet.anm',()=>{
  const visuals=fs.readFileSync(path.join(root,'src/game/EnemyVisuals.cpp'),'utf8');
  assert.match(visuals,/ownerScript==127&&\(request\.script==126\|\|request\.script==121\)/,'strict 127 whitelist');
  assert.match(visuals,/parent\.parent\|\|!originalRequest/,'grandchild/other topology guard');
  assert.match(visuals,/child\.vm\.bind\(registry_,child\.bank,request\.script,runtime\)/,'real child bind');

  const fixture=JSON.parse(fs.readFileSync(path.join(root,'local/research/boss-child-127-fixtures.json')));
  assert.equal(fixture.schema,'th128-boss-child-127-fixtures/1');
  const bullet=path.join(root,fixture.source.bullet);
  const ascii=path.join(root,fixture.source.ascii);
  assert.equal(createHash('sha256').update(fs.readFileSync(bullet)).digest('hex'),fixture.source.bulletSha256);
  assert.equal(createHash('sha256').update(fs.readFileSync(ascii)).digest('hex'),fixture.source.asciiSha256);
  const spawns=fixture.root127.script.commands.filter(c=>c.opcode===88);
  assert.deepEqual(spawns.map(c=>[c.time,Buffer.from(c.raw,'hex').readUInt32LE(0)]),[[0,126],[1,121]]);
  assert.equal(fixture.root127.script.commands.some(c=>c.opcode===88&&c.time>1),false);
  assert.equal(fixture.child126.script.commands.some(c=>c.opcode===88),false);
  assert.equal(fixture.child121.script.commands.some(c=>c.opcode===88),false);
  assert.equal(fixture.child126.layer,16);
  assert.equal(fixture.child126.sprite,313);
  assert.equal(fixture.child126.spriteBank,11);
  assert.equal(fixture.child121.sprite,452);
  assert.equal(fixture.child121.layer,16);
  assert.equal(fixture.source.child126Op88FileOffset,705524);
  const mutated=Buffer.from(fs.readFileSync(bullet));
  assert.equal(mutated.readUInt32LE(fixture.source.child126Op88FileOffset),126);
  mutated.writeUInt32LE(125,fixture.source.child126Op88FileOffset);
  const mutatedPath=path.join(root,'artifacts/tests/boss-child-127-unknown-125.anm');
  fs.mkdirSync(path.dirname(mutatedPath),{recursive:true});
  fs.writeFileSync(mutatedPath,mutated);

  fs.mkdirSync(path.join(root,'artifacts/tests'),{recursive:true});
  const output=path.join(root,'artifacts/tests/boss-child-127.cjs');
  const build=spawnSync(python,[compiler,'tests/native/boss-child-127.cpp',
    'src/game/EnemyVisuals.cpp','src/animation/AnmLogic.cpp','-Isrc','-std=c++20','-O1',
    '-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off',
    '-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1',
    '-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env:environment,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output,bullet,ascii,mutatedPath],{cwd:root,encoding:'utf8',timeout:120000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS boss child127 t0\/t1 births, ascii313, parent pose\/layer\/slot, reload\/retire and budget/);
});
