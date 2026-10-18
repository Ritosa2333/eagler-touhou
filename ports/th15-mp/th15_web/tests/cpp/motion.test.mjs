import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report,enableNativeMath} from './helpers.mjs';
test('TH15 linear, circular, elliptical and wave motion match original coordinate quantization',async()=>{
 const c=await core(),m=await oracle(),p=c.motion_create(),native=m.allocate(68);enableNativeMath(m);m.u32(0x521e34,1);let checks=0;
 try{for(let mode=0;mode<16;mode++)for(let sample=0;sample<256;sample++){
  const bytes=Buffer.alloc(68);for(let i=0;i<16;i++)bytes.writeFloatLE((sample-137)*.375+(i-8)*.625,i*4);bytes.writeFloatLE((sample-127)*.046875,28);bytes.writeFloatLE((sample-123)*.03125,40);bytes.writeFloatLE((sample-93)*.015625,48);bytes.writeUInt32LE(0x34560000|mode,64);memory(c,p,68).set(bytes);m.write(native,bytes);const rate=[.5,1,1.5][sample%3];m.f32(0x4e73e8,rate);
  for(let frame=0;frame<32;frame++){c.motion_step(p,rate);m.call(0x404170,{ecx:native});assert.deepEqual(Buffer.from(memory(c,p,68)),Buffer.from(m.bytes(native,68)),JSON.stringify({mode,sample,frame,rate}));checks++;}
 }report('motion',{passed:true,frames:checks,originalFunctions:['0x404170','0x4074a0','0x4076a0'],scope:'Shared motion modes and hundredth-coordinate floor quantization. Enemy lifecycle, collisions and bullet transforms are separate work.'});
 }finally{c.motion_delete(p);m.close();}
});
test('TH15 per-mode velocity, angle, radius and wave phase integration match original',async()=>{
 const c=await core(),m=await oracle(),p=c.motion_create(),native=m.allocate(68);enableNativeMath(m);m.u32(0x521e34,1);let checks=0;
 try{for(let mode=0;mode<16;mode++)for(let sample=0;sample<256;sample++){
  const bytes=Buffer.alloc(68);for(let i=0;i<16;i++)bytes.writeFloatLE((sample-137)*.0625+(i-8)*.125,i*4);bytes.writeFloatLE((sample-127)*.046875,28);bytes.writeFloatLE((sample-123)*.03125,40);bytes.writeFloatLE((sample-93)*.015625,48);bytes.writeUInt32LE(0x34560000|mode,64);memory(c,p,68).set(bytes);m.write(native,bytes);const rate=[.5,1,1.5][sample%3];m.f32(0x4e73e8,rate);
  for(let frame=0;frame<32;frame++){c.motion_integrate(p,rate);m.call(0x404050,{ecx:native});assert.deepEqual(Buffer.from(memory(c,p,68)),Buffer.from(m.bytes(native,68)),JSON.stringify({mode,sample,frame,rate,integration:true}));c.motion_step(p,rate);m.call(0x404170,{ecx:native});assert.deepEqual(Buffer.from(memory(c,p,68)),Buffer.from(m.bytes(native,68)),JSON.stringify({mode,sample,frame,rate,position:true}));checks+=2;}
 }report('motion-integration',{passed:true,frames:checks/2,checks,originalFunctions:['0x404050','0x404170','0x4075d0'],scope:'Velocity and phase integration followed by shared motion modes. Enemy lifecycle and collision updates are separate work.'});
 }finally{c.motion_delete(p);m.close();}
});
