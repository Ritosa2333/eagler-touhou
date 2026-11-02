import {emscripten} from '../scripts/toolchain.mjs';
import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import path from 'node:path';
import {spawnSync} from 'node:child_process';
import {pathToFileURL} from 'node:url';
import {Renderer,shakePresentation} from '../src/renderer.mjs';
const root=path.resolve(import.meta.dirname,'..'),workspace=path.dirname(root);
const python=emscripten(root).python;
const compiler=emscripten(root).compiler;
const directory=path.join(root,'artifacts/sanae-b-leaf-draw');fs.mkdirSync(directory,{recursive:true});
const output=path.join(directory,'core.mjs');
const functions=['begin','tick','draw','vertices','metadata','seed','calls','heading'];
const result=spawnSync(python,[compiler,'tests/fixtures/sanae-b-leaf-draw.cpp','src/game/CustomGeometry.cpp',
 '-std=c++20','-O2','-o',output,'-sMODULARIZE=1','-sEXPORT_ES6=1','-sENVIRONMENT=node',
 '-sEXPORTED_FUNCTIONS='+JSON.stringify(functions.map(name=>'_'+name)),
 '-sEXPORTED_RUNTIME_METHODS=["HEAPU8"]'],{cwd:root,encoding:'utf8',
 env:{...process.env,EM_CACHE:path.join(root,'artifacts/emscripten-cache'),EMSDK_PYTHON:python,
 PATH:path.dirname(python)+path.delimiter+process.env.PATH}});
assert.equal(result.status,0,result.stdout+'\n'+result.stderr);
const create=(await import(pathToFileURL(output))).default;
const original=JSON.parse(fs.readFileSync(new URL('fixtures/sanae-b-leaf-draw-native.json',import.meta.url)));
test('Sanae B leaf retains native screen-space motion and submits272 exact diffuse LineStrip vertices',async()=>{
 assert.equal(original.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 let vertices=0,draws=0;
 for(const scenario of original.scenarios){
  const c=await create();c._begin(scenario.seed,...scenario.position.slice(0,2));
  for(let tick=-1;tick<scenario.frames.length;tick++){
   if(tick>=0)c._tick();
   const frame=tick<0?scenario.birth:scenario.frames[tick],rng=tick<0?frame.rng:frame.rngAfter;
   assert.deepEqual([c._seed(),c._calls()],[rng.seed,rng.calls],'native leaf RNG '+tick);
   const node=frame.nodes.find(n=>n.type===5),count=c._draw(),pointer=c._vertices();
   if(!node){assert.equal(count,0,'removed leaf '+tick);continue;}
   assert.equal(c._heading(),Math.fround(node.effect.heading),'leaf heading '+tick);
   const native=scenario.draws.find(d=>d.tick===tick);assert.ok(native);
   assert.equal(count,native.vertices.length);
   assert.deepEqual(Array.from({length:5},(_,i)=>c._metadata(i)),[3,-1,13,0,26]);
   assert.deepEqual([native.deviceState.renderStates['19'],native.deviceState.renderStates['20']],[5,6],
    'native SRCALPHA/INVSRCALPHA blend');
   const view=new DataView(c.HEAPU8.buffer);
   for(let i=0;i<count;i++){
    const at=pointer+i*28;
    const actual=[view.getFloat32(at,true),view.getFloat32(at+4,true),view.getFloat32(at+8,true),
     view.getFloat32(at+12,true),view.getUint32(at+16,true)];
    assert.deepEqual(actual,native.vertices[i].map((v,axis)=>axis===4?v:Math.fround(v)),
     'native exact leaf vertex '+tick+'/'+i);
   }
   vertices+=count;draws++;
   const before=[c._seed(),c._calls()];c._draw();assert.deepEqual([c._seed(),c._calls()],before,'draw consumes no RNG');
  }
 }
 assert.equal(draws,32);assert.equal(vertices,272);
});
test('renderer preserves leaf LineStrip boundaries, native blend and already screen-space coordinates',()=>{
 const records=[],renderer=Object.create(Renderer.prototype);
 renderer.gl={LINE_STRIP:3,TRIANGLES:4};renderer.solid={};renderer.batch=[];
 renderer.material=blend=>records.push(['blend',blend]);renderer.texture=()=>{};
 renderer.primitive=primitive=>records.push(['primitive',primitive]);
 renderer.flush=()=>{if(renderer.batch.length)records.push(['vertices',renderer.batch.splice(0)]);};
 const vertices=[{x:224.8671875,y:439,z:0,rhw:1,color:0xff40ffff,u:0,v:0},
                 {x:218.22520446777344,y:432.7709045410156,z:0,rhw:1,color:0xef40ffdf,u:0,v:0}];
 const pose={mesh:true,primitive:3,untextured:true,drawPriority:26,vertices,a:{blend:0,layer:13}};
 assert.equal(shakePresentation(pose,[3,-2]),pose,'original45d5d0 omits Camera offsets');
 renderer.mesh(pose);renderer.mesh(pose);
 assert.deepEqual(records.filter(r=>r[0]==='blend').map(r=>r[1]),[0,0]);
 assert.deepEqual(records.filter(r=>r[0]==='primitive').map(r=>r[1]),[3,3]);
 const calls=records.filter(r=>r[0]==='vertices');assert.equal(calls.length,2,'separate native draw submissions');
 for(const call of calls)assert.deepEqual([call[1][0],call[1][1],call[1][10],call[1][11]],
  vertices.flatMap(v=>[v.x,v.y]),'no midpoint/angle reconstruction');
});
