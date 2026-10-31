import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const python=emscripten(root).python;
const compiler=emscripten(root).compiler;
const directory=path.join(root,'artifacts/stage-background-effect');fs.mkdirSync(directory,{recursive:true});
const output=path.join(directory,'core.mjs');
const functions=['load','reset','tick','spell','columns','rows','mode','state_fields','grid_vertices','draw','vertices','metadata','malloc','free'];
const result=spawnSync(python,[compiler,'tests/fixtures/stage-background-effect.cpp','src/game/StageBackgroundEffect.cpp',
 '-std=c++20','-O2','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(functions.map(name=>'_'+name)),
 '-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],{cwd:root,encoding:'utf8',
 env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,
 PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stdout+'\n'+result.stderr);
const create=(await import(pathToFileURL(output))).default;
const original=JSON.parse(fs.readFileSync(new URL('fixtures/stage-background-effect-native.json',import.meta.url)));
const raw=fs.readFileSync(new URL('../local/retail/stage05.std',import.meta.url));
async function ready(){const c=await create(),pointer=c._malloc(raw.length);c.HEAPU8.set(raw,pointer);assert.equal(c._load(pointer,raw.length),1);c._free(pointer);c._reset();return c;}
function vertices(c,pointer,count){const view=new DataView(c.HEAPU8.buffer);return Array.from({length:count},(_,i)=>Array.from({length:7},(_,axis)=>axis===4?view.getUint32(pointer+i*28+axis*4,true):view.getFloat32(pointer+i*28+axis*4,true)));}
test('stage05 STD17 wave state and833 grid vertices match actual original Stage12 updates',async()=>{
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const c=await ready();let total=0,draws=0;
 for(let tick=0;tick<=2800;tick++){
  c._tick();const native=original.samples.find(sample=>sample.frame===tick);if(!native)continue;
  assert.deepEqual([c._columns(),c._rows()],[...native.dimensions]);assert.equal(c._mode(),native.effectFields[5]);
  const fields=new Float32Array(c.HEAPU8.buffer,c._state_fields(),5);
  assert.deepEqual([...fields],[...native.effectFields.slice(0,2),...native.effectFields.slice(3,5),native.callbackCount].map(Math.fround));
  assert.deepEqual(vertices(c,c._grid_vertices(),native.vertices.length),native.vertices.map(row=>row.map((v,axis)=>axis===4?v:Math.fround(v))),'original grid '+tick);
  total+=native.vertices.length;
  for(let column=0;column<native.draws.length;column++){
   const count=c._draw(column),draw=native.draws[column];
   assert.deepEqual(Array.from({length:6},(_,field)=>c._metadata(field)),[5,8,28,0,14,2]);
   assert.deepEqual(vertices(c,c._vertices(),count),draw.vertices.map(row=>row.map((v,axis)=>axis===4?v:Math.fround(v))),'original strip '+tick+'/'+column);
   draws++;
  }
 }
 assert.equal(total,833);assert.equal(draws,112);
});
test('active spell pauses the existing stage05 wave without rebuilding or advancing its angles',async()=>{
 const c=await ready();for(let tick=0;tick<=2564;tick++)c._tick();
 const before=vertices(c,c._grid_vertices(),119),fields=[...new Float32Array(c.HEAPU8.buffer,c._state_fields(),4)];
 c._spell(1);for(let tick=0;tick<20;tick++)c._tick();
 assert.deepEqual(vertices(c,c._grid_vertices(),119),before);
 assert.deepEqual([...new Float32Array(c.HEAPU8.buffer,c._state_fields(),4)],fields);
 c._spell(0);c._tick();assert.notDeepEqual(vertices(c,c._grid_vertices(),119),before);
});
