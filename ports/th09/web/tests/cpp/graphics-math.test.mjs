import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report,bits,d3dxScalar} from './helpers.mjs';
test('Graphics matrices and projections match original scalar math and cameras',async()=>{
 const m=await oracle(),c=await core(),base=m.allocate(2048),p=c.allocate(2048);d3dxScalar(m);let checks=0,seed=0x15ffadd3;const random=()=>{seed^=seed<<13;seed^=seed>>>17;seed^=seed<<5;return seed>>>0;},real=scale=>(random()%10001-5000)/5000*scale;
 const a=p+256,b=p+512,d=p+768,na=base+256,nb=base+512,nd=base+768;
 function write(at,n,values){const bytes=new Uint8Array(new Float32Array(values).buffer);memory(c,at,bytes.length).set(bytes);m.write(n,bytes);}
 function compare(size,label){assert.deepEqual(memory(c,p,size),m.bytes(base,size),label);++checks;}
 try{for(let i=0;i<2000;++i){
  write(a,na,Array.from({length:16},()=>real(200)));write(b,nb,Array.from({length:16},()=>real(200)));write(d,nd,[real(1),real(1),real(1)]);
  m.call(0x44e823,{args:[base,na]});c.graphics_math_op(0,p,a,b,d);compare(12,'normalize '+i);
  m.call(0x44f126,{args:[base,na,nb]});c.graphics_math_op(2,p,a,b,d);compare(64,'multiply '+i);
  m.call(0x450013,{args:[base,na,nb,nd]});c.graphics_math_op(4,p,a,b,d);compare(64,'look-at '+i);
  const axis=i%3,angle=real(6.28);write(a,na,[axis,angle]);m.call([0x44faf5,0x44fb91,0x44fc2e][axis],{args:[base,bits(angle)]});c.graphics_math_op(3,p,a,b,d);compare(64,'rotation '+i);
  const params=[.01+Math.abs(real(1.5)),.2+Math.abs(real(3)),1+Math.abs(real(10)),500+Math.abs(real(1500))];write(a,na,params);m.call(0x45015b,{args:[base,...params.map(bits)]});c.graphics_math_op(5,p,a,b,d);compare(64,'perspective '+i);
 }
 for(const v of [[.9990545511245728,.0033311061561107635,.04334532096982002],[1,0,0],[0,0,0],[1,2**-12,2**-12]]){write(a,na,v);m.call(0x44e823,{args:[base,na]});c.graphics_math_op(0,p,a,b,d);compare(12,'normalization tolerance '+v);}
 const viewport=p+896,nvp=base+896,camera=p+1024,ncamera=base+1024;const vd=new DataView(c.memory.buffer);[16,16,288,448].forEach((n,i)=>{vd.setUint32(viewport+4*i,n,true);m.u32(nvp+4*i,n);});vd.setFloat32(viewport+16,0,true);vd.setFloat32(viewport+20,1,true);m.f32(nvp+16,0);m.f32(nvp+20,1);
 for(let i=0;i<300;++i){write(a,na,Array.from({length:16},()=>real(2)));write(b,nb,Array.from({length:16},()=>real(2)));write(d,nd,Array.from({length:16},()=>real(2)));write(camera,ncamera,[real(300),real(300),real(300)]);m.call(0x44ebb7,{args:[base,ncamera,nvp,na,nb,nd]});c.graphics_project(p,camera,viewport,a,b,d);compare(12,'project '+i);}
 const device=m.allocate(4),vt=m.allocate(0x100);m.u32(device,vt);m.u32(0x4b3108,device);m.u32(0x4dc550,0);m.u32(vt+0x94,m.registerImport({dll:'graphics',name:'matrix',argc:3,handler:()=>0}));
 for(let i=0;i<300;++i){
  const bg=[real(300),real(300),real(300),real(2),real(2),real(2),0,1,0,0,0,0,real(10),real(10),real(10),.31+Math.abs(real(.5))];write(camera,ncamera,bg);m.view(ncamera,240).fill(0);m.write(ncamera,memory(c,camera,36));m.write(ncamera+0x3c,memory(c,camera+48,16));m.write(ncamera+0xcc,memory(c,viewport,24));
  m.call(0x4306b0,{args:[ncamera]});c.graphics_camera(p,viewport,camera,1);assert.deepEqual(memory(c,p,128),m.bytes(ncamera+0x4c,128),'scene camera '+i);assert.deepEqual(memory(c,p+128,12),m.bytes(ncamera+0x30,12),'camera right '+i);++checks;
  m.call(0x430590,{args:[ncamera]});c.graphics_camera(p,viewport,camera,0);assert.deepEqual(memory(c,p,128),m.bytes(ncamera+0x4c,128),'screen camera '+i);++checks;
 }
 report('graphics-math',{checks,scope:'Original D3DX scalar normalization, matrix multiplication/rotation and original game look-at and perspective routines. Native dispatch is set to its shipped scalar implementation.'});
 }finally{c.release(p);m.close();}
});
