import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),dir=path.join(root,'artifacts/timed-fragment-viewport-tests');
const temp=path.join(dir,"tmp");fs.mkdirSync(temp,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler,output=path.join(dir,'core.mjs');
const result=spawnSync(python,[compiler,'tests/fixtures/timed-fragment-viewport.cpp','src/game/Laser.cpp','-std=c++20','-O2','-o',output,
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_timed_fragment_viewport"]','-sEXPORTED_RUNTIME_METHODS=["UTF8ToString"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EMCC_TEMP_DIR:temp,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stdout+'\n'+result.stderr);
const create=(await import(pathToFileURL(output))).default,native=JSON.parse(fs.readFileSync(new URL('./fixtures/timed-fragment-viewport-original.json',import.meta.url)));
test('original Timed fragments reject starts outside the32px viewport margin, including boundary equality',async()=>{
 assert.equal(native.rows.length,19);const core=await create();
 for(const row of native.rows){
  // ECL300 rounds the constructor position to hundredths. Feed that measured
  // starting position to this Laser-only fixture rather than bypassing it.
  const axis=Number(row.axis==='y'),origin=row.nodes[0].position[axis];
  const actual=JSON.parse(core.UTF8ToString(core._timed_fragment_viewport(axis,origin)));
  assert.equal(actual.returned,row.returned,JSON.stringify(row));assert.equal(actual.nodes.length,row.nodes.length,`${row.axis}/${row.target}`);
  for(let i=0;i<row.nodes.length;i++){
   assert.equal(actual.nodes[i].kind,row.nodes[i].kind);
   for(const key of ['position','geometry'])for(let k=0;k<row.nodes[i][key].length;k++)assert.ok(Math.abs(actual.nodes[i][key][k]-row.nodes[i][key][k])<=4e-5,`${row.axis}/${row.target}/${i}/${key}${k}`);
  }
 }
});
