import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const directory=path.join(root,'artifacts/laser-extensions');fs.mkdirSync(directory,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe');
const compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const output=path.join(directory,'core.mjs');
const names=['extensions_begin','extensions_tick','extensions_missing','extensions_snapshot'];
const built=spawnSync(python,[compiler,'tests/fixtures/laser-extensions.cpp','src/game/Laser.cpp',
 '-std=c++20','-O2','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(names.map(x=>'_'+x)),'-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],
 {cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),
 EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(built.status,0,built.stdout+'\n'+built.stderr);
const createFixture=(await import(pathToFileURL(output))).default;
const original=JSON.parse(fs.readFileSync(new URL('./fixtures/laser-extensions-original.json',import.meta.url)));
const close=(a,b,label)=>assert.ok(Math.abs(a-b)<0.00005,`${label}: ${a} vs native ${b}`);
function begin(core,input){core._extensions_begin(input.kind,...input.position,input.angle,input.length,input.speed,input.c,input.flags,input.changedSpeed,input.extension);}
function nodes(core){const pointer=core._extensions_snapshot(),count=new Float32Array(core.HEAPU8.buffer,pointer,1)[0];return Array.from({length:count},(_,i)=>[...new Float32Array(core.HEAPU8.buffer,pointer+4+i*26*4,26)]);}
function compare(core,frame,label){
 const rows=nodes(core);assert.equal(rows.length,frame.count,label+' live count');
 for(let i=0;i<rows.length;i++){
  const a=rows[i],b=frame.nodes[i],raw=Buffer.from(b.parameterHex,'hex');
  assert.deepEqual(a.slice(0,4),[b.id,b.kind,b.phase,b.age],label+':'+i+' identity/state');
  [...b.position,...b.velocity,...b.geometry].forEach((value,j)=>close(a[j+4],value,label+':'+i+' geometry'+j));
  assert.deepEqual(a.slice(15,17),[b.active,b.cursor],label+':'+i+' extensions');
  assert.deepEqual(a.slice(17,20),[b.bounce[0],b.bounce[2],b.bounce[3]],label+':'+i+' inherited bounce');
  const offset={0:0x38,1:0x60,2:0x30}[b.kind];assert.equal(a[20],raw.readInt32LE(offset),label+':'+i+' mutated record.c');
  assert.equal(a[21],(b.animationFlags[0]>>>5)&7,label+':'+i+' Main blend');
  for(let j=0;j<3;j++)close(a[23+j],raw.readFloatLE(j*4),label+':'+i+' stored origin'+j);
 }
 assert.equal(core._extensions_missing(),0,label+' missing');
}
test('original moving-laser mirrors create native child order and delay a child behind the old tail',async()=>{
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 const core=await createFixture();let frames=0;
 for(const row of original.records.filter(r=>r.input.extension===0x100)){
  begin(core,row.input);
  for(const frame of row.frames){if(frame.frame>=0)core._extensions_tick();compare(core,frame,JSON.stringify(row.input)+' tick'+frame.frame);frames++;}
 }
 assert.equal(frames,320);
});
test('original0x10000000 switches only Main blend on all three laser classes',async()=>{
 const core=await createFixture();let frames=0;
 for(const row of original.records.filter(r=>r.input.extension===0x10000000)){
  begin(core,row.input);
  for(const frame of row.frames){if(frame.frame>=0)core._extensions_tick();compare(core,frame,JSON.stringify(row.input)+' tick'+frame.frame);for(const node of nodes(core))assert.equal(node[22],1,'Cap keeps birth blend');frames++;}
 }
 assert.equal(frames,30);
});
