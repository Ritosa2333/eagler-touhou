import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {graphicsOracle} from './graphics-oracle.mjs';
test('TH11 C++ camera math matches its original D3DX9_37 scalar routines',async()=>{
 const c=await core(),m=await oracle(),lib=graphicsOracle(m),p=c.allocate(1024),q=m.allocate(1024);let checks=0,state=1934823;
 const random=()=>{state=(Math.imul(state,1664525)+1013904223)>>>0;return state/4294967296;};
 const bits=f=>{const b=Buffer.alloc(4);b.writeFloatLE(f);return b.readUInt32LE();};
 const write=(off,values)=>{const b=Buffer.from(new Float32Array(values).buffer);memory(c,p+off,b.length).set(b);m.write(q+off,b);};
 const compare=(n,context)=>{const a=Buffer.from(memory(c,p,n)),b=Buffer.from(m.bytes(q,n));for(let i=0;i<n;i+=4)assert.equal(a.readUInt32LE(i),b.readUInt32LE(i),`${context} word=${i/4}: ${a.readFloatLE(i)} vs ${b.readFloatLE(i)}`);++checks;};
 try{
  for(let i=0;i<1024;++i){
   for(const off of [256,384,512])write(off,Array.from({length:16},()=>Math.fround((random()-.5)*100)));
   m.call(lib.exports.get('D3DXMatrixMultiply'),{args:[q,q+256,q+384]});c.graphics_math(1,p,p+256,p+384,p+512);compare(64,'matrix '+i);
   m.call(lib.exports.get('D3DXVec3Normalize'),{args:[q,q+256]});c.graphics_math(0,p,p+256,p+384,p+512);compare(12,'normalize '+i);
   m.call(lib.exports.get('D3DXMatrixLookAtLH'),{args:[q,q+256,q+384,q+512]});c.graphics_math(3,p,p+256,p+384,p+512);compare(64,'look-at '+i);
   const axis=i%3,angle=Math.fround((random()-.5)*12);write(256,[axis,angle]);m.call(lib.exports.get(['D3DXMatrixRotationX','D3DXMatrixRotationY','D3DXMatrixRotationZ'][axis]),{args:[q,bits(angle)]});c.graphics_math(2,p,p+256,p+384,p+512);compare(64,'rotation '+i);
   const params=[.2+random()*2,.2+random()*3,.2+random()*10,1000+random()*1000];write(256,params);m.call(lib.exports.get('D3DXMatrixPerspectiveFovLH'),{args:[q,...params.map(bits)]});c.graphics_math(4,p,p+256,p+384,p+512);compare(64,'perspective '+i);
   for(const off of [256,384,512])write(off,Array.from({length:16},()=>Math.fround((random()-.5)*2)));write(640,[random()*200,random()*200,random()*200]);
   const vp=Buffer.alloc(24);[16,16,384,448].forEach((v,k)=>vp.writeUInt32LE(v,k*4));vp.writeFloatLE(.1,16);vp.writeFloatLE(.99,20);memory(c,p+768,24).set(vp);m.write(q+768,vp);
   m.call(lib.exports.get('D3DXVec3Project'),{args:[q,q+640,q+768,q+256,q+384,q+512]});c.graphics_project(p,p+640,p+768,p+256,p+384,p+512);compare(12,'projection '+i);
  }
  for(const v of [[0,0,0],[1,0,0],[1,2**-12,2**-12],[.99999994,0,0],[1.00000012,0,0],[1e-30,0,0]]){write(256,v);m.call(lib.exports.get('D3DXVec3Normalize'),{args:[q,q+256]});c.graphics_math(0,p,p+256,p+384,p+512);compare(12,'normalize boundary '+v);}
  report('graphics-math',{passed:true,checks,reference:'Microsoft D3DX9_37 shipped scalar dispatch; 53-bit FPU; no runtime DLL dependency'});
 }finally{m.close();c.release(p);}
});
