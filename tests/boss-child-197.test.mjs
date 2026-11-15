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
const le32=(hex)=>Buffer.from(hex,'hex').readUInt32LE(0);

test('boss child197 one-layer topology matches real bullet.anm',()=>{
  const visuals=fs.readFileSync(path.join(root,'src/game/EnemyVisuals.cpp'),'utf8');
  assert.match(visuals,/ownerScript==197&&\(request\.script==194\|\|request\.script==195\|\|request\.script==196\)/,'strict 197 whitelist');
  assert.match(visuals,/parent\.parent\|\|!originalRequest/,'grandchild/other topology guard');
  assert.match(visuals,/child\.vm\.bind\(registry_,child\.bank,request\.script,runtime\)/,'real child bind');

  const generation=path.join(root,'local/generations/8c71c316a4b24e68827f9fc560242050/ja');
  const bullet=path.join(generation,'raw/bullet.anm');
  const metadata=JSON.parse(fs.readFileSync(path.join(generation,'animations/bullet.json')));
  const scripts=metadata.scripts;
  const spawns=scripts[197].commands.filter(c=>c.opcode===88);
  assert.deepEqual(spawns.map(c=>[c.time,le32(c.raw)]),[[0,194],[3,195],[4,196]]);
  assert.equal(scripts[197].commands.filter(c=>c.opcode===88).length,3);
  assert.equal(scripts[194].commands.some(c=>c.opcode===88),false);
  assert.equal(scripts[195].commands.some(c=>c.opcode===88),false);
  assert.equal(scripts[196].commands.some(c=>c.opcode===88),false);
  const op3=(index)=>scripts[index].commands.find(c=>c.opcode===3);
  const layer=(index)=>scripts[index].commands.find(c=>c.opcode===68);
  const mode=(index)=>scripts[index].commands.find(c=>c.opcode===67);
  assert.equal(le32(op3(194).raw),472);
  assert.equal(le32(layer(194).raw),6);
  assert.equal(mode(194),undefined);
  assert.equal(le32(op3(195).raw),473);
  assert.equal(le32(layer(195).raw),4);
  assert.equal(le32(mode(195).raw),1);
  assert.equal(le32(op3(196).raw),473);
  assert.equal(le32(layer(196).raw),6);
  assert.equal(le32(mode(196).raw),1);
  for(const index of [194,195,196]) assert.ok(scripts[index].commands.some(c=>c.time===30&&c.opcode===1));
  const loop=scripts[197].commands.find(c=>c.opcode===4);
  assert.ok(loop&&loop.time===6&&le32(loop.raw)===0x18,'actual S197 op4 loop target');

  const bulletBytes=Buffer.from(fs.readFileSync(bullet));
  const scriptOffset=scripts[197].offset;
  const firstSpawn=Buffer.from('58000c0000000000c2000000','hex');
  const at=bulletBytes.indexOf(firstSpawn,scriptOffset);
  assert.ok(at>=0,'S197 t0 op88 child194 raw location');
  assert.equal(bulletBytes.readUInt32LE(at+8),194);
  const mutated=Buffer.from(bulletBytes);
  mutated.writeUInt32LE(193,at+8);
  assert.equal(createHash('sha256').update(bulletBytes).digest('hex'),'9860822687f33e740f2a5e1fb25a4b05cf27b0fc6d15ed9626fb903986f7784c');
  const mutatedPath=path.join(root,'artifacts/tests/boss-child-197-unknown-193.anm');
  fs.mkdirSync(path.dirname(mutatedPath),{recursive:true});
  fs.writeFileSync(mutatedPath,mutated);

  const output=path.join(root,'artifacts/tests/boss-child-197.cjs');
  const build=spawnSync(python,[compiler,'tests/native/boss-child-197.cpp',
    'src/game/EnemyVisuals.cpp','src/animation/AnmLogic.cpp','-Isrc','-std=c++20','-O1',
    '-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off',
    '-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1',
    '-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env:environment,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output,bullet,mutatedPath],{cwd:root,encoding:'utf8',timeout:120000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS boss child197 t0\/t3\/t4 births, layers\/sprites, parent pose, lifecycle\/retire and unknown reject/);
});
