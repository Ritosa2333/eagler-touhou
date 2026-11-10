import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),dir=path.join(root,'artifacts/small-point-reuse-tests');
fs.mkdirSync(dir,{recursive:true});const temp=path.join(dir,'tmp');fs.mkdirSync(temp,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler,output=path.join(dir,'core.mjs');
const built=spawnSync(python,[compiler,'tests/fixtures/small-point-reuse.cpp','src/game/Items.cpp',
 '-std=c++20','-O0','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sEXPORTED_FUNCTIONS=["_small_point_setup","_small_point_tick","_small_point_state"]',
 '-sEXPORTED_RUNTIME_METHODS=["HEAPF32"]'],{cwd:root,encoding:'utf8',env:{...process.env,
 EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,EMCC_TEMP_DIR:temp,
 PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('fixtures/small-point-reuse-native.json',import.meta.url)));
function compare(c,want,label){
 const at=c._small_point_state()/4,actual=[...c.HEAPF32.subarray(at,at+9)],expected=[want.state,want.type,
  ...want.position,...want.velocity,want.age,want.attractionSpeed,want.delay];
 assert.deepEqual(actual,expected,label+' exact original float32 state');
}
test('type9 birth retains prior idle attraction field until Automatic attraction initializes its player speed',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const row=native.records.find(r=>r.kind===9),c=await create();
 assert.deepEqual(row.birthWrites,[]);assert.equal(row.birth.attractionSpeed,row.staleAttractionSpeed);
 c._small_point_setup(row.kind,row.staleAttractionSpeed);compare(c,row.birth,'type9 birth');
 for(const state of row.frames){c._small_point_tick();compare(c,state,'type9 tick'+state.tick);}
 const firstAbove=row.frames.find(f=>f.state===3);
 assert.ok(firstAbove);assert.equal(firstAbove.attractionSpeed,Math.fround(5+Math.fround(.2)));
 assert.notEqual(firstAbove.attractionSpeed,row.staleAttractionSpeed);
 assert.ok(row.attractionWrites.some(w=>w.tick===firstAbove.tick),'actual native overwrite observed at first attraction');
});
test('ordinary-item constructor still resets the same field explicitly',async()=>{
 const row=native.records.find(r=>r.kind===1),c=await create();
 assert.equal(row.birth.attractionSpeed,0);assert.ok(row.birthWrites.some(w=>w.pc==='0x427968'&&w.word===0));
 c._small_point_setup(row.kind,row.staleAttractionSpeed);compare(c,row.birth,'ordinary birth');
 for(const state of row.frames){c._small_point_tick();compare(c,state,'ordinary tick'+state.tick);}
});
