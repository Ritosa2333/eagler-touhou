import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
import {loadLiveMain} from './helpers/live-main.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),directory=path.join(root,'artifacts/player-bomb-position-tests');
fs.mkdirSync(directory,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler,output=path.join(directory,'core.mjs');
const names=['player_bomb_position_prepare','player_bomb_position_update','player_bomb_position_sources','player_bomb_position_clears','player_bomb_position_options','th12_load_ecl','th12_load_anm','th12_load_sht','malloc','free'];
const result=spawnSync(python,[compiler,'tests/fixtures/player-bomb-position.cpp',...runtimeSources(root),'-std=c++20','-O0','-o',output,
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(n=>'_'+n)),'-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stdout+'\n'+result.stderr);
const create=(await import(pathToFileURL(output))).default,native=JSON.parse(fs.readFileSync(new URL('fixtures/player-bomb-position-original.json',import.meta.url))),
 gates=JSON.parse(fs.readFileSync(new URL('fixtures/player-bomb-gates-original.json',import.meta.url))),
 respawn=JSON.parse(fs.readFileSync(new URL('fixtures/player-respawn-original.json',import.meta.url)));
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
function compareRow(core,row,establishRespawn=false){
  const i=row.initial;core._player_bomb_position_prepare(i.loadout,i.state,i.stateAge,i.stock,i.fixedX,i.fixedY,
    Number(i.messageActive),i.liveEnemies??1,Number(establishRespawn));
  const actual=[...new Float32Array(core.HEAPU8.buffer,core._player_bomb_position_update(i.held),9)],a=row.after;
  assert.deepEqual(actual,[a.state,a.stateAge,...a.playerPosition,...a.fixedPosition,a.bombActive,...a.bombPosition],JSON.stringify(i));
}
test('six native Bombs capture the pre-movement center; deathbomb skips movement for that Player16 pass',async()=>{
 const core=await prepared();
 for(const row of native.rows)compareRow(core,row);
});
test('native MSG and live-enemy gates apply to normal Bombs, while respawn age60 falls through to movement and Bomb',async()=>{
 const core=await prepared();
 for(const row of gates.rows)compareRow(core,row);
});
test('actual Player16 respawn restores death-position damage sources and every circle/full clear in native order',async()=>{
 const core=await prepared(),kinds={'0x40caa0':0,'0x4286f0':1,'0x40d230':2,'0x428750':3};
 for(const row of respawn.rows){
  compareRow(core,row,row.initial.state===0);
  const p=core._player_bomb_position_sources(),count=new Float32Array(core.HEAPU8.buffer,p,1)[0];
  const actual=[...new Float32Array(core.HEAPU8.buffer,p+4,count*8)],sources=row.after.sources;
  assert.equal(count,sources.length);
  assert.deepEqual(actual,sources.flatMap(s=>[...s.position,s.radius,s.growth,s.previousTicks,s.ticks,s.damage,s.flags]),JSON.stringify(row.initial));
  const optionPtr=core._player_bomb_position_options(),optionCount=new Int32Array(core.HEAPU8.buffer,optionPtr,1)[0];
  assert.deepEqual([...new Int32Array(core.HEAPU8.buffer,optionPtr+4,optionCount*2)],row.after.options.flat(),JSON.stringify(row.initial)+' retained options');
  const clear=core._player_bomb_position_clears(row.initial.state,row.initial.stateAge),length=new Float32Array(core.HEAPU8.buffer,clear,1)[0];
  const calls=row.trace.filter(call=>Object.hasOwn(kinds,call.entry));
  assert.equal(length,calls.length);
  assert.deepEqual([...new Float32Array(core.HEAPU8.buffer,clear+4,length*3)],calls.flatMap(call=>
    [kinds[call.entry],call.radius??0,call.protection??call.skipProtected??call.force]),JSON.stringify(row.initial)+' clear calls');
 }
});
