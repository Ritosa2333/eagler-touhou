import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const directory=path.join(root,'artifacts/camera-pulse-tests');fs.mkdirSync(directory,{recursive:true});
const {python,compiler}=emscripten(root);
const output=path.join(directory,'core.mjs'),names=['camera_prepare','camera_birth','camera_update','camera_snapshot'];
const result=spawnSync(python,[compiler,'tests/fixtures/camera-pulse.cpp','src/game/CameraEffects.cpp',
 '-std=c++20','-O2','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(x=>'_'+x)),'-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),
 EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stdout+'\n'+result.stderr);
const create=(await import(pathToFileURL(output))).default,native=JSON.parse(fs.readFileSync(new URL('./fixtures/camera-pulse-original.json',import.meta.url)));
test('original profile1 fade/burst, nonpositive lifetime, mixed14 ordering and independent pause gates',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const c=await create();
 for(const row of native.records){
  c._camera_prepare();
  for(const frame of row.frames){
   for(const birth of frame.births)c._camera_birth(...birth);
   c._camera_update(frame.rate,frame.flags,frame.stopping);
   const actual=[...new Float32Array(c.HEAPU8.buffer,c._camera_snapshot(),5+3*frame.nodes.length)];
   const expected=[...frame.rng,...frame.offsets,frame.nodes.length,
    ...frame.nodes.flatMap(n=>[n.callback==='0x4526e0'?1:8,...n.timer])];
   assert.deepEqual(actual,expected,`${row.name} input${frame.tick}`);
  }
 }
});
