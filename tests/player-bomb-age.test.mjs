import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
import {loadLiveMain} from './helpers/live-main.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),directory=path.join(root,'artifacts/player-bomb-age-tests');
fs.mkdirSync(directory,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py'),output=path.join(directory,'core.mjs');
const names=['player_bomb_age_prepare','player_bomb_age_update','th12_load_ecl','th12_load_anm','th12_load_sht','malloc','free'];
const result=spawnSync(python,[compiler,'tests/fixtures/player-bomb-age.cpp',...runtimeSources(root),'-std=c++20','-O0','-o',output,
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(n=>'_'+n)),'-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stdout+'\n'+result.stderr);
const create=(await import(pathToFileURL(output))).default,native=JSON.parse(fs.readFileSync(new URL('fixtures/player-bomb-age-original.json',import.meta.url)));
test('original normal Bomb preserves state age, deathbomb resets60 only before age8, and blocked Bombs preserve stock',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const core=await create();loadLiveMain(core);
 for(const [name,method,index] of [['bullet.anm','_th12_load_anm',0],['pl02.anm','_th12_load_anm',6],['front.anm','_th12_load_anm',7],['pl02b.sht','_th12_load_sht',5]]){
  const bytes=fs.readFileSync(new URL('../site/assets/'+name,import.meta.url)),at=core._malloc(bytes.length);
  core.HEAPU8.set(bytes,at);assert.equal(core[method](index,at,bytes.length),1);core._free(at);
 }
 for(const row of native.rows){
  const i=row.initial;core._player_bomb_age_prepare(i.state,i.stateAge,i.bombs,Number(i.bombActive));
  const actual=[...new Int32Array(core.HEAPU8.buffer,core._player_bomb_age_update(i.held),6)],a=row.after;
  assert.deepEqual(actual,[a.state,a.stateAge,a.invulnerability,a.bombs,a.lives,a.bombActive],JSON.stringify(i));
 }
});
