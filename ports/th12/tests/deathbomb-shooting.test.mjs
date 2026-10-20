import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
import {loadLiveMain} from './helpers/live-main.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),directory=path.join(root,'artifacts/deathbomb-shooting-tests');
const temp=path.join(directory,"tmp");fs.mkdirSync(temp,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler,output=path.join(directory,'core.mjs');
const names=['deathbomb_shooting_prepare','deathbomb_shooting_tick','deathbomb_shooting_snapshot','deathbomb_shooting_options','th12_load_ecl','th12_load_anm','th12_load_sht','malloc','free'];
const result=spawnSync(python,[compiler,'tests/fixtures/deathbomb-shooting.cpp',...runtimeSources(root),'-std=c++20','-O0','-o',output,
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(n=>'_'+n)),'-sEXPORTED_RUNTIME_METHODS=["HEAPU8","UTF8ToString"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EMCC_TEMP_DIR:temp,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stdout+'\n'+result.stderr);
const create=(await import(pathToFileURL(output))).default,native=JSON.parse(fs.readFileSync(new URL('fixtures/deathbomb-shooting-original.json',import.meta.url)));
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


test('successful deathbomb fires and advances the shot clock immediately for all six loadouts',async()=>{
 assert.equal(native.rows.length,18);const core=await prepared();let count=0;
 for(const row of native.rows){
  core._deathbomb_shooting_prepare(row.loadout,row.shotAge);
  // Replay placement follows native option construction. Preserve the
  // observed pre-callback option inputs, rather than rebasing them to XY.
  const input=core._malloc(48);
  new Int32Array(core.HEAPU8.buffer,input,12).set(row.initialOptions.flat());
  core._deathbomb_shooting_options(input);core._free(input);
  for(const [tick,expected] of row.frames.entries()){
   if(tick)core._deathbomb_shooting_tick(tick===1?11:9);
   const s=JSON.parse(core.UTF8ToString(core._deathbomb_shooting_snapshot())),p=s.player,label=`loadout${row.loadout}/shot${row.shotAge}/tick${tick}`;
   assert.deepEqual({position:[p.xFixed,p.yFixed],state:p.state,stateAge:p.stateAge,shotClock:p.fireFrame,
    rng:[s.rng.seed,s.rng.calls],stock:s.economy.resources[7],focused:p.focused},Object.fromEntries(Object.entries(expected).filter(([k])=>k!=='friendly')),label);
   const friendly=s.bullets.filter(b=>b.friendly&&b.active);assert.equal(friendly.length,expected.friendly.length,label+'/count');
   for(const n of expected.friendly){
    const b=friendly.find(b=>b.physicalSlot===n.slot);assert.ok(b,label+'/slot'+n.slot);
    assert.equal(b.age,n.age,label+'/age'+n.slot);assert.equal(b.weapon[0],n.phase,label+'/phase'+n.slot);
    assert.equal(b.weapon[4],n.damage,label+'/damage'+n.slot);
    for(let axis=0;axis<2;axis++)assert.ok(Math.abs(b.position[axis]-n.position[axis])<=4e-5,label+'/position'+n.slot+'/'+axis);
   }
   count++;
  }
 }
 assert.equal(count,162);
});
