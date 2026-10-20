import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),dir=path.join(root,'artifacts/ecl-polar-normalization-tests');
fs.mkdirSync(dir,{recursive:true});
const temp=path.join(dir,'tmp');fs.mkdirSync(temp,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler,output=path.join(dir,'core.mjs');
const built=spawnSync(python,[compiler,'tests/fixtures/ecl-polar-normalization.cpp',...runtimeSources(root),
 '-std=c++20','-O0','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432','-sEXPORTED_FUNCTIONS=["_ecl_polar"]',
 '-sEXPORTED_RUNTIME_METHODS=["HEAPF32"]'],{cwd:root,encoding:'utf8',env:{...process.env,
 EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,EMCC_TEMP_DIR:temp,
 PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('fixtures/ecl-polar-normalization-native.json',import.meta.url)));
test('ECL81 normalizes before polar output, including the actual stage3 laser birth and pi/tau boundaries',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const c=await create();
 for(const row of native.records){
  const at=c._ecl_polar(row.input.angle,row.input.length)/4;
  assert.deepEqual([...c.HEAPF32.subarray(at,at+2)],row.output,row.name+' exact original float32 output');
  assert.deepEqual(row.trace.map(e=>e.entry),['0x468880','0x469270']);
 }
 const retail=native.records[0];
 assert.deepEqual(retail.output,[49.472938537597656,40.6008415222168]);
 assert.notDeepEqual(retail.output,[Math.fround(Math.cos(retail.input.angle)*64),Math.fround(Math.sin(retail.input.angle)*64)],
  'direct trigonometry reproduces the old laser birth error');
});
