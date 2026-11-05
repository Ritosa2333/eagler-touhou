import test from 'node:test';
import assert from 'node:assert/strict';
import {readCorePresentationMotion,interpolatePresentation} from '../src/presentation-interpolation.mjs';
function fixture(){
  const HEAPU8=new Uint8Array(512),v=new DataView(HEAPU8.buffer),u=(p,n)=>v.setUint32(p,n,true),f=(p,n)=>v.setFloat32(p,n,true);
  [2,3,123,1,8,4,80,1,24,112,1,64,184,1,40,240,4].forEach((n,i)=>u(4+i*4,n));
  [0xffff0000,2,0].forEach((n,i)=>u(80+i*4,n));[224,416,0].forEach((n,i)=>f(92+i*4,n));
  [8,0xffff0020,0xffff0000,9].forEach((n,i)=>u(112+i*4,n));[1,2,3,224,416,0].forEach((n,i)=>f(128+i*4,n));
  [6,3,0xffffffff,5,0,4].forEach((n,i)=>u(152+i*4,n));
  [0xb0000001,2,3,1,0,4,5,6,7].forEach((n,i)=>u(184+i*4,n));f(220,12.5);
  [8,1,9,2].forEach((n,i)=>u(240+i*4,n));
  return {core:{HEAPU8,_th12_presentation_motion_binary:()=>4},v,u,f};
}
test('packed motion reads high-bit owners, lineage, stage sentinel and laser state',()=>{
  const {core}=fixture(),s=readCorePresentationMotion(core);
  assert.equal(s.version,1);assert.equal(s.frame,123);assert.equal(s.epoch,3);
  assert.deepEqual(s.owners,[{id:0xffff0000,revision:2,kind:'player',position:[224,416,0]}]);
  assert.deepEqual(s.nodes[0],{id:8,owner:0xffff0020,motionOwner:0xffff0000,parent:9,local:[1,2,3],engine:[224,416,0],mask:6,flags:3,stageSlot:-1,meshKind:'spell-ring',lineage:[8,1,9,2]});
  assert.equal(s.lasers[0].age,12.5);assert.equal(s.laserMap.get(0xb0000001).followOwner,6);
});
test('published motion does not retain mutable Wasm views',()=>{
  const {core,f,u}=fixture(),s=readCorePresentationMotion(core);f(92,100);u(240,42);
  assert.equal(s.owners[0].position[0],224);assert.equal(s.nodes[0].lineage[0],8);
});
for(const [name,edit]of [
  ['version',x=>x.u(4,3)],['header pointer',x=>x.core._th12_presentation_motion_binary=()=>500],
  ['stride',x=>x.u(36,23)],['count',x=>x.u(32,65537)],['range',x=>x.u(28,500)],
  ['owner kind',x=>x.u(88,99)],['node kind',x=>x.u(164,99)],['lineage',x=>x.u(172,35)],
  ['finite positions',x=>x.f(92,NaN)],
])test('packed motion rejects invalid '+name,()=>{const x=fixture();edit(x);assert.throws(()=>readCorePresentationMotion(x.core),/展示/);});
test('legacy JSON remains available for older cores',()=>{
  const {core}=fixture(),expected=readCorePresentationMotion(core);
  delete expected.ownerMap;delete expected.nodeMap;delete expected.laserMap;
  const bytes=new TextEncoder().encode(JSON.stringify(expected)),HEAPU8=new Uint8Array(bytes.length+4);HEAPU8.set(bytes,4);
  const state=readCorePresentationMotion({HEAPU8,_th12_presentation_motion:()=>4,_th12_presentation_motion_size:()=>bytes.length});
  assert.deepEqual([...state.nodeMap.values()],expected.nodes);
});
test('repeated native interpolation preserves immutable endpoints and unchanged scalars',()=>{
  const s=readCorePresentationMotion(fixture().core),make=frame=>({epoch:3,frame,stage:7,motion:null,records:[],poses:[{logical:true,id:8,owner:0xffff0020,x:224,y:416,drawPriority:34,clip:2,a:{name:'bullet',sprite:380,mode:1,rotation:0,sx:1,sy:1}}],nativeMotion:{...s,frame}});
  const a=make(122),b=make(123),saved=structuredClone(b);
  const first=interpolatePresentation(a,b,.5);assert.deepEqual(interpolatePresentation(a,b,.5),first);assert.deepEqual(b,saved);
  assert.equal(first.poses[0].a,b.poses[0].a,'unchanged scalar fields reuse the original appearance');
});
