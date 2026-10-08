import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root),output=path.join(root,'artifacts/custom-geometry-test-runtime');fs.mkdirSync(output,{recursive:true});
const python=path.join(workspace,'tools/emsdk/python/3.13.3_64bit/python.exe'),compiler=path.join(workspace,'tools/emsdk/upstream/emscripten/emcc.py');
const build=spawnSync(python,[compiler,'tests/fixtures/custom-geometry.cpp','src/game/CustomGeometry.cpp','src/game/AnmLogic.cpp','-std=c++20','-O2','-o',path.join(output,'core.mjs'),'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node','-sALLOW_MEMORY_GROWTH=1','-sEXPORTED_FUNCTIONS=["_malloc","_free","_load_bank","_sprite_region","_generate_geometry","_vertices","_metadata"]','-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],{cwd:root,encoding:'utf8',env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(build.status,0,build.stderr||build.stdout);
const create=(await import(pathToFileURL(path.join(output,'core.mjs')).href)).default;
const samples=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/custom-geometry-original.json'),'utf8'));
const rings=JSON.parse(fs.readFileSync(path.join(root,'tests/fixtures/spell-ring-geometry-native.json'),'utf8'));
function generate(c,r,kind){
  const s=r.state,origin=r.origin??s.position.map((n,i)=>n+s.engine[i]+s.offset[i]),nodes=r.nodes??[];
  const f=new Float32Array([...origin,...s.rotation,...s.scale,...s.uvScroll,...s.uvScale,...s.dimensions,...s.uvCorners,...(r.laserPosition??[0,0,0]),r.laserAngle??0,r.curveDrawWidth??r.laserWidth??0,r.headOffset??0,...s.resourceDimensions]);
  const w=new Uint32Array([s.flags,s.primary,s.secondary,s.integers[0],s.integers[1],s.mode,s.layer,s.blend,s.anchorX,s.anchorY,s.sprite,0,0,r.age??0]);
  const n=new Float32Array(nodes.flat());const pointers=[f,w,n].map(x=>{const p=c._malloc(Math.max(4,x.byteLength));c.HEAPU8.set(new Uint8Array(x.buffer),p);return p;});
  const count=c._generate_geometry(kind,...pointers,nodes.length),p=c._vertices(),dv=new DataView(c.HEAPU8.buffer),vertices=[];
  for(let i=0;i<count;i++)vertices.push(Array.from({length:7},(_,j)=>j===4?dv.getUint32(p+i*28+j*4,true):dv.getFloat32(p+i*28+j*4,true)));
  for(const p of pointers)c._free(p);return vertices;
}
function close(actual,expected,label){
  assert.equal(actual.length,expected.length,label);
  for(let i=0;i<expected.length;i++)for(let j=0;j<7;j++){
    const a=actual[i][j],e=expected[i][j];assert.ok(Number.isFinite(e),`${label} original non-finite`);
    if(j===4)assert.equal(a,e,`${label} vertex${i} ARGB`);
    else assert.ok(Math.abs(a-e)<(j<2?0.0001:0.000003),`${label} vertex${i} field${j}: ${a} != ${e}`);
  }
}
test('UFO206 original32-pair arc, repeated UV, fill, four-color IRQ2 and fade vertices',async()=>{
  const c=await create();let total=0;
  for(const r of samples.records.filter(r=>r.kind==='ufo206')){const a=generate(c,r,0);close(a,r.vertices,r.name);total+=a.length;assert.equal(c._metadata(0),5);assert.equal(c._metadata(4),r.state.layer);assert.equal(c._metadata(5),r.state.blend);}
  assert.equal(total,576);
});
test('spell125/126 mode9 draws retail closed rings rather than scaled sprite rectangles',async()=>{
  const c=await create();assert.equal(rings.records.length,16);
  for(const r of rings.records){
    const vertices=generate(c,r,4);close(vertices,r.vertices,r.name);
    assert.equal(vertices.length,r.state.integers[0]*2);
    assert.deepEqual(vertices.at(-2).slice(0,6),vertices[0].slice(0,6));
    assert.deepEqual(vertices.at(-1).slice(0,6),vertices[1].slice(0,6));
    assert.equal(c._metadata(0),5);
  }
});
test('Moving/Timed main flat ANM quads preserve native width, anchor, texture coordinates and headOffset',async()=>{
  const c=await create();const rows=samples.records.filter(r=>r.kind==='straight');assert.equal(rows.length,22);
  for(const r of rows){close(generate(c,r,3),r.draws[0].vertices,r.name);assert.equal(c._metadata(0),4);}
  for(const kind of [0,1]){const heads=rows.filter(r=>r.name.startsWith(`laser${kind}-head`));assert.equal(heads.length,3);assert.equal(heads[0].draws.length,2);assert.equal(heads[1].draws.length,1);assert.equal(heads[2].draws.length,1);assert.deepEqual(heads[0].draws[0].vertices,heads[2].draws[0].vertices);}
});
test('Curve laser native2/4/8/12-node meshes, angle wrap, width and ordinal UV',async()=>{
  const c=await create();const rows=samples.records.filter(r=>r.kind==='curve');assert.equal(rows.length,15);
  for(const r of rows)close(generate(c,r,2),r.draws[0].vertices,r.name);
});
test('geometry is read-only and honors original draw guards and secondary color',async()=>{
  const c=await create(),r=structuredClone(samples.records[1]);
  r.state.flags|=0x10000;r.state.secondary=0x80ab37ff;const a=generate(c,r,0);assert.ok(a.every(v=>v[4]===0x80ab37ff));assert.deepEqual(generate(c,r,0),a);
  r.state.primary=0x00ffffff;assert.deepEqual(generate(c,r,0),[]);r.state.primary=0xffffffff;r.state.flags&=~2;assert.deepEqual(generate(c,r,0),[]);
});
test('C++ raw resource UV records agree with corrected original460610/454b80 bindings',async()=>{
  const c=await create(),raw=fs.readFileSync(path.join(root,'local/retail/bullet.anm')),p=c._malloc(raw.length);c.HEAPU8.set(raw,p);assert.equal(c._load_bank(p,raw.length),1);c._free(p);
  const seen=new Set();for(const r of [...samples.records,...rings.records]){const s=r.state;if(seen.has(s.sprite))continue;seen.add(s.sprite);const at=c._sprite_region(s.sprite),v=new Float32Array(c.HEAPU8.buffer,at,6);assert.deepEqual(Array.from(v),[...s.resourceDimensions,s.uvCorners[0],s.uvCorners[1],s.uvCorners[2],s.uvCorners[5]]);}
  assert.equal(seen.size,4); // One spell-ring sprite is also used by UFO206.
});
