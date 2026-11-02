import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const dir=path.join(root,'artifacts/bullet-morph-animation-tests');fs.mkdirSync(dir,{recursive:true});
const python=emscripten(root).python;
const compiler=emscripten(root).compiler,output=path.join(dir,'core.mjs');
const built=spawnSync(python,[compiler,'tests/fixtures/bullet-morph-animation.cpp',...runtimeSources(root),
 '-std=c++20','-O0','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432',
 '-sEXPORTED_FUNCTIONS=["_morph_bullet_tick","_morph_bullet_state","_th12_start","_th12_load_anm","_th12_load_ecl","_malloc","_free"]',
 '-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],{cwd:root,encoding:'utf8',
 env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,
 PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('fixtures/bullet-morph-animation-original.json',import.meta.url)));
function load(c,bytes,method,bank=null){const at=c._malloc(bytes.length);c.HEAPU8.set(bytes,at);try{
 assert.equal(bank===null?method(at,bytes.length):method(bank,at,bytes.length),1);}finally{c._free(at);}}
test('actual800 resets appearance and replays its spawn transition;1000000 alone forces the immediate body',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 for(const row of native.records){
  const c=await create();load(c,fs.readFileSync(new URL('../local/retail/bullet.anm',import.meta.url)),c._th12_load_anm,0);
  const raw=Buffer.from(row.syntheticEclHex,'hex'),header=raw.readUInt32LE(36);
  raw.fill(0,40,header);raw.write('main',40);load(c,raw,c._th12_load_ecl);c._th12_start(0,0,1,4660);
  for(const frame of row.frames){
   if(frame.inputTick>=0)c._morph_bullet_tick(Number(row.teleportAt10&&frame.inputTick===10));
   const actual=[...new Int32Array(c.HEAPU8.buffer,c._morph_bullet_state(),15)],label=row.name+' tick'+frame.inputTick;
   assert.equal(actual[0],frame.phase,label+' lifetime');
   assert.deepEqual(actual.slice(13),frame.rng,label+' game RNG');
   if(!frame.phase)continue;
   assert.deepEqual(actual.slice(1,13),[...frame.typeColor,frame.age,frame.animationTime,frame.script,frame.sprite,...frame.integers],label+' current appearance and ANM');
  }
 }
});
