import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),
 folder=path.join(root,'artifacts/bullet-speed-approach-sound-tests');
fs.mkdirSync(folder,{recursive:true});
const temporary=path.join(folder,'compiler-temp');fs.mkdirSync(temporary,{recursive:true});
const python=emscripten(root).python,
 compiler=emscripten(root).compiler,output=path.join(folder,'core.mjs');
const built=spawnSync(python,[compiler,'tests/fixtures/bullet-speed-approach-sound.cpp',
 'src/game/BulletTargetMotion.cpp','-std=c++20','-O2','-o',output,
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sEXPORTED_FUNCTIONS=["_approach_sound_case"]','-sEXPORTED_RUNTIME_METHODS=["HEAPU32"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),
 EMCC_TEMP_DIR:temporary,EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default,
 original=JSON.parse(fs.readFileSync(new URL('fixtures/bullet-speed-approach-sound-original.json',import.meta.url)));
test('original20000000 sound gate respects cursor, sound zero/negative and zero duration',async()=>{
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const c=await create();
 for(const row of original.cases){
  const at=c._approach_sound_case(row.prefixRecords,row.transformSound,row.duration)/4,
   actual=[...c.HEAPU32.subarray(at,at+5)];
  assert.deepEqual(actual,[row.active,row.cursor,row.soundCalls.length,row.soundCalls.at(-1)?.soundId??0,0],row.name);
  assert.ok(row.soundCalls.every(call=>call.caller==='0x40abf0'),row.name+' shared native caller');
 }
});
