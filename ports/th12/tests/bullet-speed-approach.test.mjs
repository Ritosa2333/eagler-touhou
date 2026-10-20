import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';

const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),
 folder=path.join(root,'artifacts/bullet-speed-approach-tests');
fs.mkdirSync(folder,{recursive:true});
const temporary=path.join(folder,'compiler-temp');fs.mkdirSync(temporary,{recursive:true});
const python=emscripten(root).python,
 compiler=emscripten(root).compiler,
 exports=['approach_reset','approach_set','approach_initialize','approach_tick','approach_floats','approach_words',
  'approach_world_tick','approach_world_floats','approach_world_words','th12_start','th12_load_anm','th12_load_ecl','malloc','free'],
 output=path.join(folder,'core.mjs');
const built=spawnSync(python,[compiler,'tests/fixtures/bullet-speed-approach.cpp',...runtimeSources(root),
 '-std=c++20','-O0','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(exports.map(name=>'_'+name)),
 '-sEXPORTED_RUNTIME_METHODS=["HEAPU8","HEAPU32","HEAPF32"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),
 EMCC_TEMP_DIR:temporary,EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default,
 original=JSON.parse(fs.readFileSync(new URL('fixtures/bullet-speed-approach-original.json',import.meta.url)));

function floats(c,method){const at=method()/4;return [...c.HEAPF32.subarray(at,at+6)];}
function words(c,method,count){const at=method()/4;return [...c.HEAPU32.subarray(at,at+count)];}
function near(actual,expected,label){
 for(let i=0;i<expected.length;i++)assert.ok(Math.abs(actual[i]-expected[i])<=.000002,
  `${label} field${i}: core=${actual[i]} native=${expected[i]}`);
}
function load(c,bytes,method,bank=null){
 const at=c._malloc(bytes.length);c.HEAPU8.set(bytes,at);
 try{assert.equal(bank===null?method(at,bytes.length):method(bank,at,bytes.length),1);}
 finally{c._free(at);}
}

test('original20000000 target-speed endpoints, angle selection and concurrent retry order',async()=>{
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(original.retailDifficultyScope.rankMask&15,15);
 assert.deepEqual(original.retailDifficultyScope.difficulties,['Easy','Normal','Hard','Lunatic']);
 const c=await create();
 for(const row of original.motionCases){
  c._approach_reset(row.angle,row.speed,...row.player);
  row.records.forEach((r,index)=>c._approach_set(index,+r.concurrent,r.kind,r.c,r.d,r.a,r.b));
  c._approach_initialize();
  for(const sample of row.samples){
   if(sample.step)c._approach_tick();
   const label=row.name+' step'+sample.step;
   near(floats(c,c._approach_floats),[...sample.position,...sample.velocity,sample.speed,sample.angle],label);
   assert.deepEqual(words(c,c._approach_words,10),[sample.active,sample.cursor,sample.approachTicks,
    sample.approachDuration>>>0,...sample.approachDeltaWords,0],label+' exact state and delta words');
  }
 }
});

test('original anchor type28 stays until60 motion updates before actual morph ANM and RNG',async()=>{
 for(const row of original.animationCases){
  const c=await create();
  load(c,fs.readFileSync(new URL('../local/retail/bullet.anm',import.meta.url)),c._th12_load_anm,0);
  const raw=Buffer.from(row.syntheticEclHex,'hex'),header=raw.readUInt32LE(36);
  raw.fill(0,40,header);raw.write('main',40);load(c,raw,c._th12_load_ecl);c._th12_start(0,0,1,4660);
  for(const frame of row.frames){
   if(frame.inputTick>=0)c._approach_world_tick();
   const label=row.name+' input'+frame.inputTick;
   assert.deepEqual(words(c,c._approach_world_words,19),[frame.phase,...frame.typeColor,frame.age,
    frame.active,frame.cursor,frame.approachTicks,frame.animationTime,frame.script,frame.sprite,
    ...frame.integers,...frame.rng,0],label+' lifetime, appearance and shared RNG');
   near(floats(c,c._approach_world_floats),[...frame.position,...frame.velocity,frame.speed,frame.angle],label);
  }
 }
});
