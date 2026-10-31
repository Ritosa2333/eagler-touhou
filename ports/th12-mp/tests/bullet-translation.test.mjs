import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';
import {spawnSync} from 'node:child_process';import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),folder=path.join(root,'artifacts/bullet-translation-tests'),temp=path.join(folder,'tmp');fs.mkdirSync(temp,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler,output=folder+'/core.mjs';
const built=spawnSync(python,[compiler,'tests/fixtures/bullet-translation.cpp','src/game/BulletTargetMotion.cpp','-std=c++20','-O0','-o',output,
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_translation_begin","_translation_tick","_translation_callback","_translation_snapshot"]','-sEXPORTED_RUNTIME_METHODS=["UTF8ToString"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EMSDK_PYTHON:python,EM_CACHE:root+'/artifacts/emscripten-cache',EMCC_TEMP_DIR:temp,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default,original=JSON.parse(fs.readFileSync(new URL('./fixtures/bullet-translation-original.json',import.meta.url)));
const snapshot=c=>JSON.parse(c.UTF8ToString(c._translation_snapshot()));
function compare(a,b,label){
 for(const k of ['active','cursor'])assert.equal(a[k],b[k],label+'/'+k);
 for(const k of ['position','velocity','translation'])b[k].forEach((v,i)=>assert.ok(Math.abs(a[k][i]-v)<.00004,label+'/'+k+i+': '+a[k][i]+' vs '+v));
 for(const k of ['speed','angle'])assert.ok(Math.abs(a[k]-b[k])<.000002,label+'/'+k);
}
test('actual8000000 translation combines with both rotation directions and retains the native timer reset',async()=>{
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const c=await create();let snapshots=0;
 for(const row of original.records){c._translation_begin(row.angle,row.speed,row.duration,Number(row.polar),row.turn??0);compare(snapshot(c),row.frames[0],'birth');snapshots++;
  for(let i=1;i<row.frames.length;i++){c._translation_tick();compare(snapshot(c),row.frames[i],'tick'+i);snapshots++;}}
 assert.equal(snapshots,182);
});
test('direct original callback preserves rate scaling and its exceptional completion mask',async()=>{
 const c=await create();let lastRate;
 for(const row of original.callbacks){if(row.rate!==lastRate){c._translation_begin(-.8,.6,60000,0,0);lastRate=row.rate;}
  assert.equal(c._translation_callback(row.rate,row.elapsed),row.returned);compare(snapshot(c),row.after,'rate'+row.rate+'/elapsed'+row.elapsed);}
});
