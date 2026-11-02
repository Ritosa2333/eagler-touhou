import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const directory=path.join(root,'artifacts/curve-collision-boundary');fs.mkdirSync(path.join(directory,'tmp'),{recursive:true});
const python=emscripten(root).python;
const compiler=emscripten(root).compiler;
const output=path.join(directory,'core.mjs');
const names=['width_begin','width_tick','width_query_count','width_query','width_grazes'];
const built=spawnSync(python,[compiler,'tests/fixtures/laser-collision-width.cpp','src/game/Laser.cpp','src/game/LaserCollision.cpp',
 '-std=c++20','-O2','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(x=>'_'+x)),'-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),
 EMSDK_PYTHON:python,EMCC_TEMP_DIR:path.join(directory,'tmp'),PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const createFixture=(await import(pathToFileURL(output))).default;
const native=JSON.parse(fs.readFileSync(new URL('./fixtures/curve-collision-boundary-original.json',import.meta.url)));
function run(core,row){
 const i=row.input;core._width_begin(i.kind,...i.position,i.angle,i.length,i.speed,i.width,...i.playerPosition,i.playerWidth);core._width_tick();
 const expected=row.events.filter(e=>e.kind==='lineCollision');assert.equal(core._width_query_count(),expected.length,JSON.stringify(i));
 for(let j=0;j<expected.length;j++){
  const got=[...new Float32Array(core.HEAPU8.buffer,core._width_query(j),7)],want=[...expected[j].position,...expected[j].angleTransverseLength];
  want.forEach((value,index)=>assert.ok(Math.abs(got[index]-value)<0.00004,JSON.stringify({input:i,query:j,index,got:got[index],value})));
  assert.equal(got[6],expected[j].nativeReturn,JSON.stringify(i)+' result');
 }
 assert.equal(core._width_grazes(),row.events.filter(e=>e.kind==='grazeAward').length,JSON.stringify(i)+' graze award');
}
test('Curve queries include accumulated16 and preserve zero/low-speed skips',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const core=await createFixture();for(const row of native.records)run(core,row);
 assert.equal(native.records.length,12);assert.equal(native.threshold,16);assert.equal(native.records.reduce((sum,row)=>sum+row.events.filter(e=>e.kind==='lineCollision').length,0),48);
});
