import test from 'node:test';
import assert from 'node:assert/strict';
import * as draw from '../site/runtime/draw.mjs';
test('original Boss ring uses textured 76-vertex strips and rejects invented/oversized geometry',()=>{
  assert.equal(typeof draw.ringStripVertices,'function','Boss ring strip conversion is missing');
  const record={mode:14,bank:5,entry:7,layer:17,geometry:{kind:'anm-ring',vertices:Array.from({length:76},(_,index)=>({x:320+index,y:116,z:0,rhw:1,u:index%2*.25,v:Math.floor(index/2)/37,color:0xffffffff}))}};
  const result=draw.ringStripVertices(record);assert.equal(result.length,74*3*8);assert.deepEqual(Array.from(result.slice(0,4)),[320,116,0,0]);
  assert.deepEqual([result[0],result[8],result[16],result[24],result[32],result[40]],[320,321,322,322,321,323]);
  assert.throws(()=>draw.ringStripVertices({...record,mode:0}),/ring/);
  assert.throws(()=>draw.ringStripVertices({...record,geometry:{...record.geometry,vertices:record.geometry.vertices.slice(1)}}),/ring/);
  assert.throws(()=>draw.ringStripVertices({...record,geometry:{...record.geometry,vertices:record.geometry.vertices.map((vertex,index)=>index?vertex:{...vertex,x:NaN})}}),/ring/);
});
