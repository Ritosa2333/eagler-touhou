import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const directory=path.join(root,'artifacts/bullet-offscreen-sprites');fs.mkdirSync(directory,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe');
const compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const output=path.join(directory,'core.mjs');
const names=['malloc','free','bounds_query','bounds_load','bounds_begin','bounds_tick','bounds_snapshot'];
const result=spawnSync(python,[compiler,'tests/fixtures/bullet-offscreen-sprites.cpp','src/game/AnmLogic.cpp','src/game/BulletTargetMotion.cpp',
 '-std=c++20','-O2','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(x=>'_'+x)),'-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),
 EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stdout+'\n'+result.stderr);
const createFixture=(await import(pathToFileURL(output))).default;
const original=JSON.parse(fs.readFileSync(new URL('./fixtures/bullet-offscreen-sprites-original.json',import.meta.url)));
test('actual409900 raw sprite dimensions retain strict edge boundaries without premature float rounding',async()=>{
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const core=await createFixture();for(const row of original.points)assert.equal(!!core._bounds_query(...row.position,...row.dimensions),row.outside,JSON.stringify(row));
 assert.equal(original.points.length,86);
 const replayPoint=original.points.filter(row=>row.edge==='actual4152');assert.deepEqual(replayPoint.map(row=>row.outside),[true,false]);
});
test('original4099e0 tests current sprite before the ANM tail and skips bounds when no sprite is bound',async()=>{
 const core=await createFixture();
 for(const row of original.lifecycles){
  const bytes=Buffer.from(row.syntheticBankHex,'hex'),pointer=core._malloc(bytes.length);core.HEAPU8.set(bytes,pointer);assert.equal(core._bounds_load(pointer,bytes.length),1);core._free(pointer);
  core._bounds_begin(...row.position,row.type,row.phase,row.age);
  for(const frame of row.frames){
   if(frame.pass)core._bounds_tick();
   const actual=[...new Float32Array(core.HEAPU8.buffer,core._bounds_snapshot(),7)];
   assert.deepEqual(actual,[frame.phase,frame.dimensions?1:0,...(frame.dimensions??[0,0]),frame.tailVisits,frame.clock,0],row.name+' pass'+frame.pass);
  }
 }
});
