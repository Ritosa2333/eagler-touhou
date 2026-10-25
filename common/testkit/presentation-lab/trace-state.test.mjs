import test from 'node:test';
import assert from 'node:assert/strict';
import {readNativeTrace} from './trace-state.mjs';

test('declared native traces decode little-endian integers, float vectors and byte blocks at an offset',()=>{
  const raw=Uint8Array.from([99,99,99,99,3,2,1,0,0,0,96,192,0,0,128,62,0,128,255,0,17,0,0,0]);
  const before=raw.slice(),row=readNativeTrace(raw.buffer,4,{wordCount:5,uint32:{first:0,last:4},
    float32:{position:{at:1,length:2}},bytes:{state:{at:12,length:3}}});
  assert.deepEqual(row,{first:0x010203,last:17,position:[-3.5,.25],state:[0,128,255]});
  assert.deepEqual(raw,before);raw.fill(0);assert.deepEqual(row.state,[0,128,255]);assert.deepEqual(row.position,[-3.5,.25]);
});

test('truncated or unaligned native buffers fail before yielding a partial state',()=>{
  assert.throws(()=>readNativeTrace(new ArrayBuffer(12),0,{wordCount:4}));
  assert.throws(()=>readNativeTrace(new ArrayBuffer(16),1,{wordCount:3}),/aligned/);
  for(const wordCount of [0,-1,1.5,NaN])assert.throws(()=>readNativeTrace(new ArrayBuffer(16),0,{wordCount}));
});

test('native trace declarations cannot read outside their frame or overwrite another field',()=>{
  for(const layout of [{wordCount:1,uint32:{frame:1}},{wordCount:2,float32:{player:{at:1,length:2}}},
    {wordCount:2,bytes:{state:{at:7,length:2}}},{wordCount:2,uint32:{frame:0},float32:{frame:{at:0,length:1}}},
    {wordCount:2,uint32:{frame:.5}},{wordCount:2,float32:{position:{at:0,length:0}}}])
    assert.throws(()=>readNativeTrace(new ArrayBuffer(16),0,layout),/Invalid native trace field/);
});
