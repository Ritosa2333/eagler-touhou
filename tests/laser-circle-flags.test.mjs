import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),dir=path.join(root,'artifacts/laser-circle-flags-tests');
fs.mkdirSync(dir,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py'),output=path.join(dir,'core.mjs');
const result=spawnSync(python,[compiler,'tests/fixtures/laser-circle-flags.cpp','src/game/Laser.cpp','-std=c++20','-O2','-o',output,
 '-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sEXPORTED_FUNCTIONS=["_laser_circle_flags"]','-sEXPORTED_RUNTIME_METHODS=["UTF8ToString"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stdout+'\n'+result.stderr);
const create=(await import(pathToFileURL(output))).default,native=JSON.parse(fs.readFileSync(new URL('./fixtures/laser-circle-flags-original.json',import.meta.url)));
test('raw circle flags0..3 keep class-specific point conversion and exact radius boundaries',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const core=await create();
 for(const row of native.records){
  const actual=JSON.parse(core.UTF8ToString(core._laser_circle_flags(row.kind,row.radius,row.flags))),label=`kind${row.kind}/r${row.radius}/flags${row.flags}`;
  assert.deepEqual(actual.points,row.itemSpawns.map(n=>n.position.slice(0,2).map(v=>Number(v.toPrecision(9)))),label+' points');
  assert.equal(actual.returned,row.returned,label+' removed cells');
  assert.equal(actual.nodes.length,row.nodes.length,label+' laser fragments');
  for(let i=0;i<actual.nodes.length;i++)for(const field of ['kind','phase','retirePending','geometry','position']){
   const expected=Array.isArray(row.nodes[i][field])?row.nodes[i][field].map(v=>Number(v.toPrecision(9))):row.nodes[i][field];
   assert.deepEqual(actual.nodes[i][field],expected,label+' '+field);
  }
 }
});
