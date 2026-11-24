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
const families=[
  {root:193,children:[190,191,192],layers:[6,4,6],sprites:[472,473,473],modes:[0,1,1]},
  {root:201,children:[198,199,200],layers:[6,4,6],sprites:[472,473,473],modes:[0,1,1]},
  {root:205,children:[202,203,204],layers:[6,4,6],sprites:[472,473,473],modes:[0,1,1]},
  {root:213,children:[210,211,212],layers:[6,4,6],sprites:[472,473,473],modes:[0,1,1]},
  {root:217,children:[214,215,216],layers:[6,4,6],sprites:[472,473,473],modes:[0,1,1]},
];

test('boss child families 193/201/205/213/217 first-generation tick identity',()=>{
  const visuals=fs.readFileSync(path.join(root,'src/game/EnemyVisuals.cpp'),'utf8');
  for(const family of families){
    const pattern=new RegExp(`ownerScript==${family.root}&&\\(request\\.script==${family.children[0]}\\|\\|request\\.script==${family.children[1]}\\|\\|request\\.script==${family.children[2]}\\)`);
    assert.match(visuals,pattern,`strict whitelist ${family.root}`);
  }
  assert.match(visuals,/parent\.parent\|\|!originalRequest/,'grandchild/other topology guard');
  assert.match(visuals,/child\.vm\.bind\(registry_,child\.bank,request\.script,runtime\)/,'real child bind');

  const generation=path.join(root,'local/generations/8c71c316a4b24e68827f9fc560242050/ja');
  const bulletPath=path.join(generation,'raw/bullet.anm');
  const metadata=JSON.parse(fs.readFileSync(path.join(generation,'animations/bullet.json')));
  const scripts=metadata.scripts;
  for(const family of families){
    const rootScript=scripts[family.root];
    const spawns=rootScript.commands.filter(c=>c.opcode===88);
    assert.deepEqual(spawns.map(c=>[c.time,le32(c.raw)]),[[0,family.children[0]],[3,family.children[1]],[4,family.children[2]]],`${family.root} authored t0/t3/t4`);
    assert.equal(spawns.length,3,`${family.root} one-layer spawn count`);
    for(let index=0;index<3;index++){
      const child=scripts[family.children[index]];
      assert.equal(child.commands.some(c=>c.opcode===88),false,`${family.children[index]} must not nest`);
      const sprite=child.commands.find(c=>c.opcode===3);
      const layer=child.commands.find(c=>c.opcode===68);
      const mode=child.commands.find(c=>c.opcode===67);
      assert.equal(le32(sprite.raw),family.sprites[index],`${family.children[index]} sprite`);
      assert.equal(le32(layer.raw),family.layers[index],`${family.children[index]} layer`);
      if(family.modes[index]===0) assert.equal(mode,undefined,`${family.children[index]} mode0`);
      else assert.equal(le32(mode.raw),family.modes[index],`${family.children[index]} mode`);
      assert.ok(child.commands.some(c=>c.time===30&&c.opcode===1),`${family.children[index]} authored retirement`);
    }
    const loop=rootScript.commands.find(c=>c.opcode===4);
    assert.ok(loop&&loop.time===6&&le32(loop.raw)===0x18,`${family.root} authored op4 loop`);
  }

  const bulletBytes=Buffer.from(fs.readFileSync(bulletPath));
  assert.equal(createHash('sha256').update(bulletBytes).digest('hex'),'9860822687f33e740f2a5e1fb25a4b05cf27b0fc6d15ed9626fb903986f7784c');
  const first=scripts[193].commands.find(c=>c.opcode===88&&c.time===0);
  const raw=Buffer.concat([Buffer.from('58000c0000000000','hex'),Buffer.from(first.raw,'hex')]);
  assert.equal(raw.length,12);
  assert.equal(raw.readUInt32LE(8),190);
  const at=bulletBytes.indexOf(raw,scripts[193].offset);
  assert.ok(at>=0,'root193 first spawn raw location');
  const mutated=Buffer.from(bulletBytes);
  mutated.writeUInt32LE(189,at+8);
  const mutatedPath=path.join(root,'artifacts/tests/boss-child-family-matrix-unknown-193.anm');
  fs.mkdirSync(path.dirname(mutatedPath),{recursive:true});
  fs.writeFileSync(mutatedPath,mutated);

  const output=path.join(root,'artifacts/tests/boss-child-family-matrix.cjs');
  const build=spawnSync(python,[compiler,'tests/native/boss-child-family-matrix.cpp',
    'src/game/EnemyVisuals.cpp','src/animation/AnmLogic.cpp','-Isrc','-std=c++20','-O1',
    '-Wall','-Wextra','-Werror','-fexceptions','-fno-fast-math','-ffp-contract=off',
    '-fsanitize=undefined','-fno-sanitize-recover=all','-sENVIRONMENT=node','-sEXIT_RUNTIME=1',
    '-sNODERAWFS=1','-sALLOW_MEMORY_GROWTH=1','-o',output],{cwd:root,env:environment,encoding:'utf8',timeout:240000});
  assert.equal(build.status,0,build.stdout+build.stderr);
  const run=spawnSync(process.execPath,[output,bulletPath,mutatedPath],{cwd:root,encoding:'utf8',timeout:120000});
  assert.equal(run.status,0,run.stdout+run.stderr);
  assert.match(run.stdout,/PASS boss child families 193\/201\/205\/213\/217 first-gen tick births, identity retirement, pose, layers\/sprites\/modes and removal/);
});
