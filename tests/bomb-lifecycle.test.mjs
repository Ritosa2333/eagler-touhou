import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
import {loadLiveMain} from './helpers/live-main.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const directory=path.join(root,'artifacts/bomb-lifecycle-tests');fs.mkdirSync(directory,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe');
const compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const output=path.join(directory,'core.mjs');
const names=['bomb_prepare','bomb_phase','bomb_snapshot','bomb_remove_main','th12_load_ecl','th12_load_anm','malloc','free'];
const build=spawnSync(python,[compiler,'tests/fixtures/bomb-lifecycle.cpp',...runtimeSources(root),
 '-std=c++20','-O0','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432','-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(n=>'_'+n)),
 '-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),
 EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(build.status,0,build.stdout+'\n'+build.stderr);
const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('./fixtures/bomb-lifecycle-visual-original.json',import.meta.url)));
const nativeOther=JSON.parse(fs.readFileSync(new URL('./fixtures/bomb-lifecycle-other-mesh-tracked-original.json',import.meta.url)));
const snapshot=c=>[...new Float32Array(c.HEAPU8.buffer,c._bomb_snapshot(),17)];
async function prepare(row,existing=null){
 const c=existing??await create();loadLiveMain(c);
 for(const [name,index] of [['bullet.anm',0],['pl00.anm',4],['pl01.anm',5],['pl02.anm',6],['front.anm',7]]){
  const bytes=fs.readFileSync(new URL('../site/assets/'+name,import.meta.url)),p=c._malloc(bytes.length);
  c.HEAPU8.set(bytes,p);assert.equal(c._th12_load_anm(index,p,bytes.length),1);c._free(p);
 }
 c._bomb_prepare(row.loadout>>1,row.loadout%2);return c;
}
function expectedFrame(frame){
 const main=Object.hasOwn(frame,'trackedMain')?frame.trackedMain:frame.main;
 return [frame.state,frame.age,main?1:0,main?.script??-1,main?.clock??-1,
 frame.background?1:0,frame.background?.script??-1,frame.background?.clock??-1,...frame.rng,frame.cameraCount,...frame.cameraOffsets,
 main?main.primary>>>24:-1,main?main.secondary>>>24:-1,
 frame.background?frame.background.primary>>>24:-1,frame.background?frame.background.secondary>>>24:-1];}
test('actual Bomb17+ANM27 matches two full Marisa A and Sanae B lifetimes, camera RNG, and front1 handles',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(native.wholeGameOracle,false);
 assert.equal(native.draw42ResetExecuted,true);
 for(const row of native.records){
  const c=await prepare(row);
  for(const frame of row.frames){
   c._bomb_phase(row.starts.includes(frame.frame));
   assert.deepEqual(snapshot(c),expectedFrame(frame),`${row.name} frame${frame.frame}`);
  }
 }
});
test('the same Core repeatedly starts MA/SB sessions without inheriting already delivered exit IRQs',async()=>{
 const c=await create();
 for(const loadout of [2,2,5,5]){
  const row=native.records.find(row=>row.loadout===loadout);await prepare(row,c);
  for(const frame of row.frames){
   c._bomb_phase(row.starts.includes(frame.frame));
   assert.deepEqual(snapshot(c),expectedFrame(frame),`same Core loadout${loadout} frame${frame.frame}`);
  }
 }
});
test('original remaining four Bombs retain actual camera requests, front1 fade and complete game RNG lifetimes',async()=>{
 assert.equal(nativeOther.meshAllocationGate,1);
 assert.equal(nativeOther.meshGateSyntheticDisplayEnvironment,true);
 assert.equal(nativeOther.wholeGameOracle,false);
 for(const row of nativeOther.records){
  const c=await prepare(row);
  for(const frame of row.frames){
   c._bomb_phase(row.starts.includes(frame.frame));
   assert.deepEqual(snapshot(c),expectedFrame(frame),`${row.name} frame${frame.frame}`);
  }
 }
});
test('missing actual main handle stops the Bomb before subsequent sources, leaves, or implicit rebinds',async()=>{
 for(const row of native.records){
  const c=await prepare(row);c._bomb_phase(1);c._bomb_remove_main();c._bomb_phase(0);
  const s=snapshot(c);assert.equal(s[0],0);assert.equal(s[1],1);assert.equal(s[2],0);
  assert.equal(s[7],41,'front1 receives exit IRQ1 on the same Bomb pass');
 }
});
