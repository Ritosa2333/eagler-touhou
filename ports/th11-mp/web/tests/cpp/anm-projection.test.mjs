import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {graphicsOracle} from './graphics-oracle.mjs';
test('TH11 billboard and transformed quads match original projected coordinates',async()=>{
 const c=await core(),m=await oracle(),lib=graphicsOracle(m),vm=c.allocate(0x434),camera=c.allocate(188),out=c.allocate(128);
 const nativeVm=m.allocate(0x434),nativeCamera=m.allocate(0x120),manager=m.allocate(0x7bd900),scratch=m.allocate(512);m.u32(0x4c37cc,nativeCamera);
 let state=8675309,checks=0,clipped=0;const random=()=>{state=(Math.imul(state,1664525)+1013904223)>>>0;return state/4294967296;};
 const bits=n=>{const b=Buffer.alloc(4);b.writeFloatLE(n);return b.readUInt32LE();};
 const eq=(a,b,label)=>{assert.equal(a.length,b.length);for(let i=0;i<a.length;i+=4)assert.equal(a.readUInt32LE(i),b.readUInt32LE(i),`${label} word=${i/4} ${a.readFloatLE(i)} vs ${b.readFloatLE(i)}`);};
 try{for(let kind=0;kind<2;++kind)for(let sample=0;sample<2048;++sample){
  const cam=Buffer.alloc(188);for(const off of [0,4,8,12,16,20])cam.writeFloatLE((random()-.5)*4,off);cam.writeFloatLE(-500-random()*500,8);
  m.write(scratch,cam.subarray(0,12));m.write(scratch+16,Buffer.alloc(12));m.f32(scratch+32,0);m.f32(scratch+36,1);m.f32(scratch+40,0);
  m.call(lib.exports.get('D3DXMatrixLookAtLH'),{args:[scratch+64,scratch,scratch+16,scratch+32]});cam.set(m.bytes(scratch+64,64),24);
  m.call(lib.exports.get('D3DXMatrixPerspectiveFovLH'),{args:[scratch+128,...[.5+random(),4/3,1,10000].map(bits)]});cam.set(m.bytes(scratch+128,64),88);
  [16,16,384,448].forEach((v,k)=>cam.writeUInt32LE(v,152+k*4));cam.writeFloatLE(0,168);cam.writeFloatLE(1,172);
  memory(c,camera,188).set(cam);m.write(nativeCamera,cam.subarray(0,12));m.write(nativeCamera+0x30,cam.subarray(12,24));m.write(nativeCamera+0x4c,cam.subarray(24,88));m.write(nativeCamera+0x8c,cam.subarray(88,152));m.write(nativeCamera+0xcc,cam.subarray(152,176));
  const b=Buffer.alloc(0x434);for(const off of [0x24,0x28,0x2c])b.writeFloatLE(sample%4===0?0:(random()-.5)*6,off);
  b.writeFloatLE((random()-.5)*4,0x3c);b.writeFloatLE((random()-.5)*4,0x40);b.writeFloatLE(random()*300,0x4c);b.writeFloatLE(random()*300,0x50);
  for(const base of [0x2b4,0x2f4])for(let i=0;i<16;++i)b.writeFloatLE(i===15?1:(i%5===0?random()*.8:0),base+i*4);
  if(sample%5===0)for(let i=0;i<16;++i)b.writeFloatLE((random()-.5)*2,0x2f4+i*4);
  for(const off of [0x3dc,0x3e0,0x3e4,0x3f4,0x3f8,0x3fc])b.writeFloatLE((random()-.5)*24,off);
  b.writeFloatLE((random()-.5)*600,0x3e8);b.writeFloatLE((random()-.5)*600,0x3ec);b.writeFloatLE((random()-.15)*1800,0x3f0);
  b.writeUInt32LE((3|((sample%4)*4)|((sample%3)<<18)|((Math.floor(sample/3)%3)<<20)|(sample%7===0?0x4000:0))>>>0,0x404);
  memory(c,vm,b.length).set(b);m.write(nativeVm,b);const ok=c.anm_project(vm,camera,out,kind);m.reg('EAX',nativeVm);const expected=m.call(kind?0x450e20:0x450700,{args:kind?[manager]:[]});
  assert.equal(ok,expected===0?1:0,`kind=${kind} sample=${sample} visibility`);if(ok){const projected=Buffer.concat(Array.from({length:4},(_,i)=>Buffer.from(m.bytes(0x4c91d8+i*28,12))));eq(Buffer.from(memory(c,out,48)),projected,`kind=${kind} sample=${sample}`);}else ++clipped;
  eq(Buffer.from(memory(c,vm+0x2f4,64)),Buffer.from(m.bytes(nativeVm+0x2f4,64)),`matrix kind=${kind} sample=${sample}`);assert.equal(new DataView(c.memory.buffer).getUint32(vm+0x404,true),m.u32(nativeVm+0x404));++checks;
 }report('anm-projection',{passed:true,checks,clipped,scope:'billboard and projected quad coordinates, anchors, rotations, world matrix updates and flags; exact float bytes'});
 }finally{m.close();for(const p of [vm,camera,out])c.release(p);}
});
