import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
import {loadLiveMain} from './helpers/live-main.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),directory=path.join(root,'artifacts/player-game-rate-tests');
const temp=path.join(directory,"tmp");fs.mkdirSync(temp,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler,output=path.join(directory,'core.mjs');
const names=['player_rate_prepare','player_rate_tick','player_rate_snapshot','th12_load_ecl','th12_load_anm','th12_load_sht','malloc','free'];
const result=spawnSync(python,[compiler,'tests/fixtures/player-game-rate.cpp',...runtimeSources(root),'-std=c++20','-O0','-o',output,
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(n=>'_'+n)),'-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EMCC_TEMP_DIR:temp,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stdout+'\n'+result.stderr);
const create=(await import(pathToFileURL(output))).default,native=JSON.parse(fs.readFileSync(new URL('fixtures/player-game-rate-original.json',import.meta.url)));
async function prepared(){
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(native.wholeGameOracle,false);
 const core=await create();loadLiveMain(core);
 const resources=[['bullet.anm','_th12_load_anm',0],['pl00.anm','_th12_load_anm',4],['pl01.anm','_th12_load_anm',5],['pl02.anm','_th12_load_anm',6],['front.anm','_th12_load_anm',7]];
 for(let i=0;i<6;i++)resources.push([`pl0${i>>1}${i%2?'b':'a'}.sht`,'_th12_load_sht',i]);
 for(const [name,method,index] of resources){
  const bytes=fs.readFileSync(new URL('../site/assets/'+name,import.meta.url)),at=core._malloc(bytes.length);
  core.HEAPU8.set(bytes,at);assert.equal(core[method](index,at,bytes.length),1);core._free(at);
 }
 return core;
}

test('all six retail loadouts use slowed normal/focused straight/diagonal movement and Player16 clocks',async()=>{
 assert.equal(native.rows.length,144);const core=await prepared();let count=0;
 for(const row of native.rows){
  core._player_rate_prepare(row.loadout,row.rate);
  for(let tick=0;tick<row.frames.length;tick++){
   if(tick)core._player_rate_tick(row.held);
   const actual=[...new Int32Array(core.HEAPU8.buffer,core._player_rate_snapshot(),6)],s=row.frames[tick];
   assert.deepEqual(actual,[...s.fixed,s.stateAge,s.totalAge,s.invulnerability,s.focused],`loadout${row.loadout}/rate${row.rate}/held${row.held}/tick${tick}`);count++;
  }
 }
 assert.equal(count,1296);
});
