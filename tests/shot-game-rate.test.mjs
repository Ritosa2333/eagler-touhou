import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';import path from 'node:path';
import {spawnSync} from 'node:child_process';import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),folder=path.join(root,'artifacts/shot-game-rate-tests'),temp=path.join(folder,'tmp');fs.mkdirSync(temp,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler,output=folder+'/core.mjs';
const built=spawnSync(python,[compiler,'tests/fixtures/shot-game-rate.cpp','-std=c++20','-O0','-o',output,
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_shot_rate_begin","_shot_rate_tick","_shot_rate_clock","_shot_rate_emitted"]','-sEXPORTED_RUNTIME_METHODS=["UTF8ToString"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EMSDK_PYTHON:python,EM_CACHE:root+'/artifacts/emscripten-cache',EMCC_TEMP_DIR:temp,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default,original=JSON.parse(fs.readFileSync(new URL('./fixtures/shot-game-rate-original.json',import.meta.url)));
test('fractional shoot clocks emit once per changed integer age and use the original rate-scaled wrap',async()=>{
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(original.rows.length,6);const c=await create();let count=0;
 for(const row of original.rows){c._shot_rate_begin();for(const [tick,expected] of row.frames.entries()){
  c._shot_rate_tick(Number(expected.held),row.rate);
  assert.equal(c._shot_rate_clock(),expected.clock,`clock/rate${row.rate}/release${row.release}/${tick}`);
  const emitted=c._shot_rate_emitted();assert.deepEqual(emitted<0?[]:[emitted],expected.emissions,`emission/rate${row.rate}/release${row.release}/${tick}`);count++;
 }}
 assert.equal(count,480);
});
