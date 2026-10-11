import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),dir=path.join(root,'artifacts/ecl-main-stop-workers-tests');
fs.mkdirSync(dir,{recursive:true});const temp=path.join(dir,'tmp');fs.mkdirSync(temp,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler,output=path.join(dir,'core.mjs');
const built=spawnSync(python,[compiler,'tests/fixtures/ecl-main-stop-workers.cpp',...runtimeSources(root),
 '-std=c++20','-O0','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1',
 '-sINITIAL_MEMORY=33554432','-sEXPORTED_FUNCTIONS=["_context_workers_setup","_context_workers_tick","_th12_load_ecl","_th12_load_anm","_th12_state","_th12_state_size","_malloc","_free"]',
 '-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],{cwd:root,encoding:'utf8',env:{...process.env,
 EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,EMCC_TEMP_DIR:temp,
 PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('fixtures/ecl-main-stop-workers-original.json',import.meta.url)));
const snapshot=c=>{const at=c._th12_state(),size=c._th12_state_size();return JSON.parse(new TextDecoder().decode(c.HEAPU8.subarray(at,at+size)));};
function load(c,bytes,method,...leading){
 const at=c._malloc(bytes.length);c.HEAPU8.set(bytes,at);
 try{assert.equal(c[method](...leading,at,bytes.length),1);}finally{c._free(at);}
}
test('original primary ECL1/return stops every later worker before RNG/emission; async termination preserves its successor',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(native.records.length,4);
 for(const row of native.records){
  const c=await create();load(c,fs.readFileSync(new URL('../site/assets/bullet.anm',import.meta.url)),'_th12_load_anm',0);
  load(c,Buffer.from(row.syntheticEclHex,'hex'),'_th12_load_ecl');c._context_workers_setup(row.seed);
  for(const frame of row.frames){
   if(frame.inputTick>=0)c._context_workers_tick();
   const s=snapshot(c),label=row.kind+' input'+frame.inputTick;
   assert.equal(s.fixtureFlags,0,label+' no diagnostic behavior masks');
   assert.deepEqual([s.rng.seed,s.rng.calls],frame.gameSeedCounter,label+' exact game RNG');
   assert.deepEqual(s.enemies.map(e=>e.id),frame.enemies.map(e=>e.id),label+' live actors');
   for(const enemy of frame.enemies){
    const actual=s.ecl.filter(ctx=>ctx.enemy===enemy.id&&ctx.routine);
    assert.deepEqual(actual.map(ctx=>[ctx.routine,ctx.byteOffset,ctx.time]),
     enemy.contexts.map(ctx=>[ctx.routine,ctx.pcOffset,ctx.time]),label+' scheduled contexts');
    // Inactive/unread context storage is outside the behavior contract. Once
    // WorkerB runs, compare its three real random results, not only a count.
    for(const ctx of enemy.contexts.filter(ctx=>ctx.routine==='WorkerB'&&ctx.pcOffset>0)){
     const got=actual.find(item=>item.routine===ctx.routine);
     const sparse=new Map(got.locals),words=Array.from({length:3},(_,i)=>sparse.get(i)??0),
      bytes=new ArrayBuffer(12),view=new DataView(bytes);
     words.forEach((word,i)=>view.setUint32(i*4,word,true));
     assert.deepEqual(words.map((_,i)=>view.getFloat32(i*4,true)),ctx.locals,label+' genuine random getter values');
    }
   }
   const bullets=s.bullets.filter(b=>!b.friendly);
   assert.deepEqual(bullets.map(b=>[b.physicalSlot,b.hostile[2],b.type,b.color]),
    frame.bullets.map(b=>[b.slot,b.phase,...b.typeColor]),label+' actual emitted physical slots');
   for(let i=0;i<bullets.length;i++)assert.deepEqual(bullets[i].position.map(Math.fround),frame.bullets[i].position.slice(0,2),label+' emitter position');
   if(frame.inputTick>=1){
    if(row.kind.startsWith('main-'))assert.equal(s.rng.calls,0,label+' primary exit has no worker side effects');
    else {assert.equal(s.rng.calls,6,label+' only surviving WorkerB reads three random values');assert.equal(bullets.length,1);}
   }
  }
 }
});
