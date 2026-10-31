import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
import {loadLiveMain} from './helpers/live-main.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),directory=path.join(root,'artifacts/friendly-repress-tests');
fs.mkdirSync(directory,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler,output=path.join(directory,'core.mjs');
const names=['friendly_repress_prepare','friendly_repress_tick','friendly_repress_size','th12_load_ecl','th12_load_anm','th12_load_sht','malloc','free'];
const result=spawnSync(python,[compiler,'tests/fixtures/friendly-repress.cpp',...runtimeSources(root),'-std=c++20','-O0','-o',output,
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(n=>'_'+n)),'-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stdout+'\n'+result.stderr);
const create=(await import(pathToFileURL(output))).default,native=JSON.parse(fs.readFileSync(new URL('fixtures/friendly-repress-original.json',import.meta.url)));
const nativeBomb=JSON.parse(fs.readFileSync(new URL('fixtures/friendly-bomb-options-original.json',import.meta.url)));
test('six-loadout Player16 matches actual option resets, laser release/repress and unchanged-count options on Bomb',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(native.wholeGameOracle,false);
 const core=await create();loadLiveMain(core);
 const resources=[['bullet.anm','_th12_load_anm',0],['pl00.anm','_th12_load_anm',4],['pl01.anm','_th12_load_anm',5],['pl02.anm','_th12_load_anm',6],['front.anm','_th12_load_anm',7]];
 for(let i=0;i<6;i++)resources.push([`pl0${i>>1}${i%2?'b':'a'}.sht`,'_th12_load_sht',i]);
 for(const [name,method,index] of resources){
  const bytes=fs.readFileSync(new URL('../site/assets/'+name,import.meta.url)),at=core._malloc(bytes.length);
  core.HEAPU8.set(bytes,at);assert.equal(core[method](index,at,bytes.length),1);core._free(at);
 }
 const f32=value=>Array.isArray(value)?value.map(f32):Math.fround(value);
 for(const record of [...native.records,...nativeBomb.records]){
  core._friendly_repress_prepare(record.loadout,...record.initialFixed,record.power);
  for(const row of record.frames){
   const at=core._friendly_repress_tick(row.held),actual=JSON.parse(new TextDecoder().decode(core.HEAPU8.subarray(at,at+core._friendly_repress_size())));
   assert.deepEqual(actual.playerFixed,row.playerFixed,`loadout${record.loadout} frame${row.frame} player`);
   assert.deepEqual(actual.options,row.options.map(o=>o.fixed),`loadout${record.loadout} frame${row.frame} options`);
   assert.equal(actual.shootAge,row.shootAge,`loadout${record.loadout} frame${row.frame} shooting timer`);
   const expected=row.friendly.map(b=>[b.slot,b.phase,b.age,...b.position,b.type,b.option,b.damage,...b.hitbox]);
   assert.deepEqual(f32(actual.friendly),f32(expected),`loadout${record.loadout} frame${row.frame} friendly slots`);
  }
 }
 const overlap=native.records.find(r=>r.loadout===2).frames[15].friendly.filter(b=>b.type===2&&b.option===3);
 assert.deepEqual(overlap.map(b=>b.phase),[2,1],'the old ANM exit tail overlaps a fresh active beam');
});
