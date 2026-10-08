import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const directory=path.join(root,'artifacts/bullet-circle-precision');fs.mkdirSync(directory,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe');
const compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const output=path.join(directory,'core.mjs');
const result=spawnSync(python,[compiler,'tests/fixtures/bullet-circle-precision.cpp','-std=c++20','-O2',
 '-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sEXPORTED_FUNCTIONS=["_circle_collision"]'],{cwd:root,encoding:'utf8',
 env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,
 PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stdout+'\n'+result.stderr);
const createFixture=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('./fixtures/bullet-circle-precision-original.json',import.meta.url)));
test('original437980 retains unrounded deltas at normal circle hit, graze and outside boundaries',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const core=await createFixture();let precisionCases=0;
 for(const row of native.rows){
  for(const type of [26,27])assert.equal(core._circle_collision(type,...row.center,...row.player,row.playerHit),row.nativeReturn,JSON.stringify({type,...row}));
  if(row.nativeReturn!==row.earlyRoundedReturn)++precisionCases;
 }
 assert.equal(native.rows.length,72);assert.equal(precisionCases,48);
 assert.deepEqual([...new Set(native.rows.map(row=>row.nativeReturn))].sort(),[0,1,2]);
});
