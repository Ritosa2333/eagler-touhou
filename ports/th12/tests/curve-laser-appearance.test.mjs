import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),
 folder=path.join(root,'artifacts/curve-laser-appearance-tests');
fs.mkdirSync(folder,{recursive:true});
const temporary=path.join(folder,'compiler-temp');fs.mkdirSync(temporary,{recursive:true});
const python=emscripten(root).python,
 compiler=emscripten(root).compiler,output=path.join(folder,'core.mjs');
const built=spawnSync(python,[compiler,'tests/fixtures/curve-laser-appearance.cpp',...runtimeSources(root),
 '-std=c++20','-O0','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432',
 '-sEXPORTED_FUNCTIONS=["_curve_appearance_setup","_curve_appearance_state","_curve_appearance_vertices","_th12_load_anm","_th12_load_ecl","_malloc","_free"]',
 '-sEXPORTED_RUNTIME_METHODS=["HEAPU8","HEAPU32"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),
 EMCC_TEMP_DIR:temporary,EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default,
 original=JSON.parse(fs.readFileSync(new URL('fixtures/curve-laser-appearance-original.json',import.meta.url)));
function load(c,bytes,method,bank=null){
 const at=c._malloc(bytes.length);c.HEAPU8.set(bytes,at);
 try{assert.equal(bank===null?method(at,bytes.length):method(bank,at,bytes.length),1);}finally{c._free(at);}
}
function passiveMain(){
 const raw=Buffer.alloc(84);raw.write('SCPT');raw.writeUInt16LE(1,4);raw.writeUInt16LE(1,16);
 raw.writeUInt32LE(48,36);raw.write('main',40);raw.write('ECLH',48);
 raw.writeUInt16LE(83,68);raw.writeUInt16LE(20,70);raw[74]=63;raw[75]=1;raw.writeInt32LE(100000,80);return raw;
}
test('actual Curve factory selects color0/6/13 sprites and all84 original CPU strip UV vertices',async()=>{
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(original.gpuExecuted,false);
 const c=await create();load(c,fs.readFileSync(new URL('../local/retail/bullet.anm',import.meta.url)),c._th12_load_anm,0);
 load(c,passiveMain(),c._th12_load_ecl);
 for(const row of original.records){
  assert.deepEqual(row.rngBefore,row.rngAfter,'native cap loop does not consume either RNG');
  c._curve_appearance_setup(row.color);
  const at=c._curve_appearance_state()/4,values=[...c.HEAPU32.subarray(at,at+16)],
   expected=row.draws[0].vertices,label='color'+row.color;
  assert.deepEqual(values.slice(0,6),[row.state.script,row.state.sprite,row.state.layer,row.state.blend,
   row.state.sprite,6],label+' original VM/sprite/texture entry');
  assert.ok(row.callbackTrace.every(call=>call.actualReturnEax===row.state.sprite),label+' actual native callback');
  assert.deepEqual(values.slice(8,14),[row.state.mode,row.state.anchorX,row.state.anchorY,
   row.capState.blend,row.capState.mode,row.capState.anchorY],label+' original Main/Cap constructor draw flags');
  assert.equal(values[7],84,label+'42 pairs');assert.equal(expected.length,84);
  const vertexAt=c._curve_appearance_vertices()+values[6]*28,view=new DataView(c.HEAPU8.buffer);
  for(let i=0;i<84;i++)for(let field=0;field<7;field++){
   const address=vertexAt+i*28+field*4,
    actual=field===4?view.getUint32(address,true):view.getFloat32(address,true);
   if(field<2)assert.ok(Math.abs(actual-expected[i][field])<.0001,`${label} vertex${i} coordinate${field}`);
   else assert.equal(actual,expected[i][field],`${label} vertex${i} CPU field${field}`);
  }
  assert.equal(values[15],6,label+' original cap quad');
  const capAt=c._curve_appearance_vertices()+values[14]*28,capExpected=row.draws[1].vertices;
  for(let i=0;i<6;i++)for(let field=0;field<7;field++){
   const address=capAt+i*28+field*4,
    actual=field===4?view.getUint32(address,true):view.getFloat32(address,true);
   if(field<2)assert.ok(Math.abs(actual-capExpected[i][field])<.0001,`${label} cap vertex${i} coordinate${field}`);
   else assert.equal(actual,capExpected[i][field],`${label} cap vertex${i} CPU field${field}`);
  }
 }
});
