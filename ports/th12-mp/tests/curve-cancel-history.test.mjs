import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {runtimeSources} from '../scripts/runtime-sources.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),dir=path.join(root,'artifacts/curve-cancel-history-tests'),temp=path.join(dir,'tmp');
fs.mkdirSync(temp,{recursive:true});
const python=emscripten(root).python,compiler=emscripten(root).compiler,output=path.join(dir,'core.mjs');
const built=spawnSync(python,[compiler,'tests/fixtures/curve-cancel-history.cpp',...runtimeSources(root),
 '-std=c++20','-O0','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sALLOW_MEMORY_GROWTH=1','-sINITIAL_MEMORY=33554432',
 '-sEXPORTED_FUNCTIONS=["_curve_history_setup","_curve_history_input","_curve_history_clear","_curve_history_update","_curve_history_primary","_curve_history_snapshot","_th12_load_ecl","_th12_load_anm","_malloc","_free"]',
 '-sEXPORTED_RUNTIME_METHODS=["HEAPU8","UTF8ToString"]'],{cwd:root,encoding:'utf8',env:{...process.env,
 EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,EMCC_TEMP_DIR:temp,
 PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const create=(await import(pathToFileURL(output))).default,native=JSON.parse(fs.readFileSync(new URL('fixtures/curve-cancel-history-original.json',import.meta.url)));
function load(c,bytes,method,...prefix){
 const at=c._malloc(bytes.length);c.HEAPU8.set(bytes,at);
 try{assert.equal(c[method](...prefix,at,bytes.length),1);}finally{c._free(at);}
}
const snap=c=>JSON.parse(c.UTF8ToString(c._curve_history_snapshot()));
const f32=v=>Array.isArray(v)?v.map(f32):Math.fround(v);
function compare(actual,expected,label){
 for(const field of ['laserPhase','retirePending','protection'])assert.equal(actual[field],expected[field],label+' '+field);
 for(const field of ['position','velocity','angleWidthSpeed','grazeAge','nodes'])
  assert.deepEqual(f32(actual[field]),f32(expected[field]),label+' '+field);
 assert.deepEqual(actual.gameSeedCounter,expected.gameSeedCounter,label+' actual ANM RNG');
 assert.deepEqual(f32(actual.pointRequests),f32(expected.pointRequests.map(p=>p.position)),label+' actual star-point order');
 assert.equal(actual.primaryEffects.length,expected.primaryEffects.length,label+' actual effect factories');
 for(let i=0;i<actual.primaryEffects.length;i++){
  const a=actual.primaryEffects[i],b=expected.primaryEffects[i];
  for(const field of ['script','clock','layer'])assert.equal(a[field],b[field],label+' effect'+i+' '+field);
  for(const field of ['engine','rotation'])assert.deepEqual(f32(a[field]),f32(b[field]),label+' effect'+i+' '+field);
 }
}
test('original Curve circle/rectangle cancellation retains exact history, star points and ANM RNG through the next update',async()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(native.records.length,30);
 for(const row of native.records){
  const c=await create();load(c,fs.readFileSync(new URL('../local/retail/bullet.anm',import.meta.url)),'_th12_load_anm',0);
  const ecl=Buffer.from(row.syntheticEclHex,'hex'),header=ecl.readUInt32LE(36);
  ecl.fill(0,40,header);ecl.write('main',40);load(c,ecl,'_th12_load_ecl');c._curve_history_setup(row.seed);
  const nodes=new Float32Array(row.frames[0].nodes.flat()),at=c._malloc(nodes.byteLength);
  c.HEAPU8.set(new Uint8Array(nodes.buffer),at);c._curve_history_input(at,nodes.length/5);c._free(at);
  const label=row.method+'/'+row.pattern+'/'+row.variant;
  compare(snap(c),row.frames[0],label+' before');
  assert.equal(c._curve_history_clear(Number(row.method==='rectangle'),row.convert,row.force),row.returned,label+' removed-node return');
  compare(snap(c),row.frames[1],label+' clear');c._curve_history_update();compare(snap(c),row.frames[2],label+' update');
  c._curve_history_primary();compare(snap(c),row.frames[3],label+' Primary27');
  if(row.returned){
   const s=snap(c);assert.deepEqual(f32(s.nodes[0].slice(0,3)),f32(s.position),label+' head XYZ synchronizes');
   assert.deepEqual(f32(s.nodes[0].slice(3)),f32([s.angleWidthSpeed[0],s.angleWidthSpeed[3]]),label+' head uses current angle/speed');
  }
 }
});
