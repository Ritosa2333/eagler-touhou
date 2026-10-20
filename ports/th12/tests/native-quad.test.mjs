import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {Renderer,nativeQuadCorners,shakePresentation,readCoreAnimationPoses} from '../src/renderer.mjs';
const native=JSON.parse(fs.readFileSync(new URL('./fixtures/native-quad-original.json',import.meta.url)));
const f=Math.fround;
function expected(row){
 const adjustment=row.mode===0?.5:0;
 return [0,1,5,2].map(i=>row.vertices[i].slice(0,2).map(v=>v+adjustment));
}
test('actual mode0 floors its centered origin before screen shift and nearest-even; mode1 never pixel-rounds',()=>{
 assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
 assert.equal(native.wholeGameOracle,false);assert.equal(native.rows.length,108);
 for(const row of native.rows){
  assert.deepEqual(row.rngAfter,row.rngBefore,'original CPU drawing consumes no game RNG');
  assert.equal(row.rngBefore.length,2);
  if(!row.vertices.length)continue; // Native viewport rejects fully outside quads.
  const w=f(row.dimensions[0]*row.scale[0]),h=f(row.dimensions[1]*row.scale[1]);
  const actual=nativeQuadCorners(...row.position,w,h,row.rotation,row.anchor,row.anchor,row.mode,row.offset);
  assert.deepEqual(actual,expected(row),JSON.stringify({...row,vertices:undefined}));
 }
 const zero=native.rows.find(r=>r.mode===0&&r.anchor===0&&r.position[0]===100.5&&r.offset[0]>0&&r.offset[0]<1);
 assert.deepEqual(expected(zero)[0],[98,193]);
 const one=native.rows.find(r=>r.mode===1&&r.anchor===0&&r.position[0]===100.5&&r.rotation===0&&r.offset[0]>0&&r.offset[0]<1);
 assert.deepEqual(expected(one)[0],[98.5,193.5]);
});
test('original mode0/1 primary-alpha guard rejects secondary-color ghosts through the real pose ABI',()=>{
 assert.equal(native.alphaRows.length,4);
 for(const row of native.alphaRows){
  const heap=new Uint8Array(128),view=new DataView(heap.buffer),F=(n,v)=>view.setFloat32(n*4,v,true),U=(n,v)=>view.setUint32(n*4,v,true);
  F(0,244);F(1,160);F(6,1);F(7,1);F(28,1);U(14,row.primary);U(15,row.secondary);U(19,row.mode);U(24,row.flags);
  const core={HEAPU8:heap,_th12_anm_draw:()=>1,_th12_anm_draw_ptr:()=>0,_th12_anm_draw_stride:()=>128,
   _th12_tick:()=>{throw Error('presentation cannot advance game state');}};
  const p=readCoreAnimationPoses(core)[0];
  const r=Object.create(Renderer.prototype);r.atlases={bullet:{sprites:[{w:16,h:14,entry:0}]}};
  let draws=0;r.region=()=>{draws++;};r.state(p.a,p.x,p.y,p.angle,p.scale,p.alpha);
  assert.equal(draws,row.drawCount,`mode${row.mode} primary${row.primary.toString(16)}`);
  assert.equal(p.a.alpha,1,'secondary selection still selects its color');
 }
});
test('the actual Renderer state/region path defers shake until native quad construction without mutating its pose',()=>{
 for(const row of native.rows){
  if(!row.vertices.length)continue;
  const r=Object.create(Renderer.prototype);r.gl={TRIANGLES:4};r.batch=[];
  r.textures=new Map([['bullet:0',{w:64,h:64}]]);
  r.atlases={bullet:{sprites:[{entry:0,x:0,y:0,w:row.dimensions[0],h:row.dimensions[1]}]}};
  r.material=r.texture=r.primitive=()=>{};
  const pose={x:row.position[0],y:row.position[1],drawPriority:18,alpha:1,angle:0,scale:1,
   a:{name:'bullet',sprite:0,x:0,y:0,visible:true,alpha:1,mode:row.mode,rotate:row.mode===1,
    rotation:row.rotation,sx:row.scale[0],sy:row.scale[1],anchorX:row.anchor,anchorY:row.anchor,r:1,g:1,b:1}};
  const before=structuredClone(pose),offset=row.offset.map(v=>Number(v.toPrecision(9))),p=shakePresentation(pose,offset);
  assert.deepEqual([p.x,p.y],row.position);
  r.state(p.a,p.x,p.y,p.angle,p.scale,p.alpha,p.screenShake);
  const corners=[0,1,2,5].map(i=>r.batch.slice(i*10,i*10+2));
  assert.deepEqual(corners,expected(row),`renderer mode${row.mode} anchor${row.anchor} rotation${row.rotation}`);
  assert.deepEqual(pose,before);
 }
});
