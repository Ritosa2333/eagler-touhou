import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const directory=path.join(root,'artifacts/camera-effects-tests');fs.mkdirSync(directory,{recursive:true});
const {python,compiler}=emscripten(root);
const output=path.join(directory,'core.mjs');
const names=['camera_prepare','camera_birth','camera_update','camera_snapshot'];
const result=spawnSync(python,[compiler,'tests/fixtures/camera-effects.cpp','src/game/CameraEffects.cpp',
 '-std=c++20','-O2','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(x=>'_'+x)),'-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),
 EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stdout+'\n'+result.stderr);
const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('./fixtures/camera-effects-original.json',import.meta.url)));
test('original Camera14 profile8 matches full Bomb shake lifetimes, overlap priority, pause gates, and timer rates',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(native.wholeGameOracle,false);
 const c=await create();
 for(const row of native.rows){
  c._camera_prepare();
  for(const f of row.frames){
   if(f.born)c._camera_birth(...f.born);
   c._camera_update(f.rate,f.flags,f.stopping);
   const values=[...new Float32Array(c.HEAPU8.buffer,c._camera_snapshot(),5+2*f.timers.length)];
   assert.deepEqual(values,[...f.rng,...f.offsets,f.timers.length,...f.timers.flat()],`${row.name} tick${f.tick}`);
  }
 }
});
