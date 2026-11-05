import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';
import {spawnSync} from 'node:child_process';import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),folder=path.join(root,'artifacts/bullet-game-rate-tests'),temp=path.join(folder,'tmp');fs.mkdirSync(temp,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler,output=folder+'/core.mjs';
const built=spawnSync(python,[compiler,'tests/fixtures/bullet-game-rate.cpp','src/game/BulletTargetMotion.cpp','-std=c++20','-O0','-o',output,
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_rate_begin","_rate_record","_rate_dispatch","_rate_tick","_rate_snapshot"]','-sEXPORTED_RUNTIME_METHODS=["UTF8ToString"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EMSDK_PYTHON:python,EM_CACHE:root+'/artifacts/emscripten-cache',EMCC_TEMP_DIR:temp,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default,original=JSON.parse(fs.readFileSync(new URL('./fixtures/bullet-game-rate-original.json',import.meta.url)));
const snapshot=c=>JSON.parse(c.UTF8ToString(c._rate_snapshot()));

test('original half/quarter rate applies to alive/spawn/cancel motion, every ETEX clock and lifetime',async()=>{
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(original.rows.length,42);const c=await create();let count=0;
 for(const row of original.rows){
  c._rate_begin(row.rate,row.phase);
  row.records.forEach((r,index)=>c._rate_record(index,r.kind,r.c,r.d,r.a,r.b,Number(r.concurrent)));
  c._rate_dispatch();
  for(let tick=0;tick<row.frames.length;tick++){
   if(tick)c._rate_tick();const actual=snapshot(c),expected=row.frames[tick],label=`rate${row.rate}/phase${row.phase}/${row.records[0]?.kind??0}/tick${tick}`;
   for(const k of ['active','cursor','phase','age'])assert.equal(actual[k],expected[k],label+'/'+k);
   for(const k of ['position','velocity'])expected[k].forEach((v,axis)=>assert.ok(Math.abs(actual[k][axis]-v)<=4e-5,`${label}/${k}/${axis}: ${actual[k][axis]} vs ${v}`));
   for(const k of ['speed','angle'])assert.ok(Math.abs(actual[k]-expected[k])<=2e-6,`${label}/${k}: ${actual[k]} vs ${expected[k]}`);
   count++;
  }
 }
 assert.equal(count,1722);
});
