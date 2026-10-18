import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report,enableNativeSine} from './helpers.mjs';
test('TH15 vector and per-axis enemy position interpolation match original',async()=>{
 const c=await core(),m=await oracle(),p=c.position_interpolation_create(),native=m.allocate(104),out=m.allocate(12);enableNativeSine(m);m.u32(0x4ca620,0x4e73e8);let checks=0;
 try{for(const axes of [0,1])for(let mode=0;mode<32;mode++)for(let sample=0;sample<64;sample++){
  const bytes=Buffer.alloc(104);for(let i=0;i<15;i++)bytes.writeFloatLE((sample-33)*19.125+(i-8)*33.875,i*4);bytes.writeInt32LE(-1,60);bytes.writeUInt32LE(1,76);bytes.writeInt32LE([0,-1,1,7,19,90][sample%6],80);for(let i=0;i<3;i++)bytes.writeInt32LE((mode+i*sample)%32,84+i*4);bytes.writeInt32LE(mode,96);bytes.writeUInt32LE(0x45678000|axes,100);memory(c,p,104).set(bytes);m.write(native,bytes);const rate=[.5,1,1.5][sample%3];m.f32(0x4e73e8,rate);
  for(let frame=0;frame<45;frame++){const result=c.position_interpolation_step(p,rate);m.call(0x431dd0,{ecx:native,args:[out]});const label=JSON.stringify({axes,mode,sample,frame,rate});assert.deepEqual(Buffer.from(memory(c,result,12)),Buffer.from(m.bytes(out,12)),label+' value');assert.deepEqual(Buffer.from(memory(c,p,104)),Buffer.from(m.bytes(native,104)),label+' state');checks+=2;}
 }report('position-interpolation',{passed:true,frames:checks/2,checks,originalFunction:'0x431dd0',scope:'Enemy position interpolation, vector/per-axis modes, timer and rounding. Commands configuring it and entity lifecycle need separate validation.'});
 }finally{c.position_interpolation_delete(p);m.close();}
});
