import assert from 'node:assert/strict';
import test from 'node:test';
import * as drawModule from '../site/runtime/draw.mjs';

test('world composite uses authored ANM layers without changing simulation records',()=>{
  assert.equal(typeof drawModule.orderDrawLayers,'function','world ANM layer ordering is missing');
  const records=[{id:1,layer:16},{id:2,layer:8},{id:3,layer:0},{id:4,layer:8}];
  assert.deepEqual(drawModule.orderDrawLayers(records).map(record=>record.id),[3,2,4,1]);
  assert.deepEqual(records.map(record=>record.id),[1,2,3,4]);
  for(const value of [NaN,-1,256,1.5])assert.throws(()=>drawModule.orderDrawLayers([{layer:value}]));
});
import { argbColor, decodeDrawBuffer, quadVertices, DRAW_STRIDE } from '../site/runtime/draw.mjs';
import { FrameStepper } from '../site/runtime/timing.mjs';

test('explicit top-left and centered rectangles retain authored pixel geometry', () => {
  assert.deepEqual(quadVertices({x:10,y:20,width:5,height:7,sx:1,sy:1,anchorX:1,anchorY:1,mode:0}),
    [[10,20],[15,20],[15,27],[10,27]]);
  assert.deepEqual(quadVertices({x:10,y:20,width:5,height:7,sx:1,sy:1,anchorX:0,anchorY:0,mode:0}),
    [[7,16],[12,16],[12,23],[7,23]]);
});

test('rotated mode preserves fractional geometry, unsupported modes stop', () => {
  const points=quadVertices({x:0,y:0,width:2,height:4,sx:1,sy:1,anchorX:1,anchorY:1,mode:1,rotation:Math.PI/2});
  assert.ok(Math.abs(points[1][0])<0.000001);
  assert.ok(Math.abs(points[1][1]-2)<0.000001);
  assert.throws(()=>quadVertices({mode:31}),/mode/);
  assert.deepEqual(quadVertices({mode:1,rx:0.5,ry:0.75,x:0,y:0,width:2,height:4,sx:1,sy:1}),
    quadVertices({mode:1,rx:0,ry:0,x:0,y:0,width:2,height:4,sx:1,sy:1}),
    'native mode1 45fad0 uses only Z rotation; X/Y are not a substitute 3D mode');
});

test('ARGB conversion does not exchange red and blue', () => {
  assert.deepEqual(argbColor(0x80402010),[64/255,32/255,16/255,128/255]);
});

test('nonfinite rotation and projected coordinates stop before GPU submission',()=>{
  assert.throws(()=>quadVertices({mode:1,rotation:NaN,x:0,y:0,width:2,height:4,sx:1,sy:1}),/finite/i);
  assert.throws(()=>quadVertices({mode:1,rotation:Infinity,x:0,y:0,width:2,height:4,sx:1,sy:1}),/finite/i);
});

test('WASM draw ABI rejects truncated memory and decodes explicit field offsets', () => {
  const buffer=new ArrayBuffer(DRAW_STRIDE);const view=new DataView(buffer);
  view.setUint32(0,19,true);view.setInt32(8,24,true);view.setInt32(12,64,true);
  view.setInt32(16,1,true);view.setUint32(48,0xffffffff,true);view.setFloat32(56,320,true);
  view.setFloat32(60,480,true);view.setFloat32(64,12.5,true);
  const [record]=decodeDrawBuffer(buffer,0,1,DRAW_STRIDE);
  assert.equal(record.id,19);assert.equal(record.bank,24);assert.equal(record.entry,1);
  assert.equal(record.sprite,64);assert.equal(record.width,320);assert.equal(record.x,12.5);
  assert.throws(()=>decodeDrawBuffer(buffer,0,2,DRAW_STRIDE));
  assert.throws(()=>decodeDrawBuffer(buffer,0,1,DRAW_STRIDE-4));
});

test('240 Hz presentation does not multiply 60 Hz game ticks', () => {
  const clock=new FrameStepper();let steps=0;
  for(let index=0;index<=240;index++) steps+=clock.advance(index*1000/240,true).steps;
  assert.equal(steps,60);
});

test('losing focus clears backlog and a long stall explicitly requests pause', () => {
  const clock=new FrameStepper();clock.advance(0,true);
  assert.equal(clock.advance(1000,true).stalled,true);
  clock.advance(2000,false);
  assert.equal(clock.advance(2001,true).steps,0);
  assert.equal(clock.advance(2018,true).steps,1);
});

test('a hit-effect100ms frame spike catches up without dropping game ticks or forcing a pause',()=>{
  const clock=new FrameStepper();clock.advance(0,true);
  const spike=clock.advance(100,true);assert.equal(spike.stalled,false);assert.equal(spike.steps,4);
  const catchup=clock.advance(100+1000/60,true);assert.equal(catchup.stalled,false);assert.equal(catchup.steps,3);
  assert.equal(clock.advance(100+2000/60,true).steps,1);
});
