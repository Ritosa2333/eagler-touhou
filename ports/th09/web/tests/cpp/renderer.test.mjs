import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report} from './helpers.mjs';
test('CPU sprite vertices, clipping, tint, blend and depth match original draw paths',async()=>{
 const m=await oracle(),c=await core(),f=c.renderer_fixture(),p=Array.from({length:7},(_,i)=>c.renderer_part(f,i)),manager=m.allocate(0x12900),vm=m.allocate(0x2a4),sprite=m.allocate(0x44),device=m.allocate(4),vt=m.allocate(0x160),arg=i=>m.u32(m.reg('ESP')+4+4*i);
 let dv=new DataView(c.memory.buffer),drawn=[],checks=0,submitted=0,blend=5,depth=1,seed=0x81263714;const random=()=>{seed^=seed<<13;seed^=seed>>>17;seed^=seed<<5;return seed>>>0;},real=(scale=1)=>(random()%10001-5000)/5000*scale;
 m.view(manager,0x12900).fill(0);m.write(manager+0x12884,[3,0,0xff,0xff]);m.u32(0x4dc550,manager);m.u32(0x4b3108,device);m.u32(device,vt);m.u32(0x4b3448,0x4b3178);
 m.u32(vt+0xf4,m.registerImport({dll:'renderer',name:'texture',argc:3,handler:()=>0}));m.u32(vt+0xc8,m.registerImport({dll:'renderer',name:'state',argc:3,handler:()=>{assert.equal(arg(1),20);blend=arg(2)===2?1:5;return 0;}}));
 m.replace(0x4396a0,'flush-boundary',()=>0);m.replace(0x439730,'quad-boundary',()=>{const b=m.bytes(arg(0),112);for(const n of [0,1,2,2,1,3])drawn.push(...b.subarray(n*28,(n+1)*28));return 0;},1);
 m.replace(0x42ff40,'depth-boundary',()=>{assert.equal(arg(0),14);depth=arg(1);return 0;},2);
 m.view(0x4dc558,112).fill(0);for(let n=0;n<4;++n){m.f32(0x4dc558+n*28+12,1);m.u32(0x4dc558+n*28+16,0xffffffff);}
 function u(off,value){dv.setUint32(p[0]+off,value,true);m.u32(vm+off,value);}function q(off,value){dv.setFloat32(p[0]+off,value,true);m.f32(vm+off,value);}function s(off,value){dv.setFloat32(p[1]+off,value,true);m.f32(sprite+off,value);}
 try{for(let scenario=0;scenario<9000;++scenario){
  dv=new DataView(c.memory.buffer);memory(c,p[0],0x2a4).fill(0);m.view(vm,0x2a4).fill(0);m.u32(vm+0x224,sprite);const flags=(scenario%47===0?0:3)|((scenario%4)<<4)|((scenario%4)<<11)|(scenario%3===0?0x2000:0)|(scenario%7===0?0x20000:0);u(0x1f8,flags);u(0x1f0,scenario%31===0?random()&0xffffff:random());u(0x1f4,random());
  q(8,scenario%3===0?0:real(6.28));q(0x18,real(3));q(0x1c,real(3));q(0x28,random()%385);q(0x2c,random()%385);q(0x208,scenario%9===0?Math.floor(real(600))+.5:real(1000));q(0x20c,real(750));q(0x210,real(1));q(0x30,real(1));q(0x34,real(1));
  const tex=scenario%13+1;dv.setUint32(p[1]+4,tex,true);m.u32(sprite+4,tex);s(0x20,real(1));s(0x24,real(1));s(0x28,real(1));s(0x2c,real(1));
  const view=[scenario%3*160,16,scenario%5===0?640:288,448];view.forEach((v,i)=>{dv.setUint32(p[2]+i*4,v,true);m.u32(0x4b3178+0xcc+i*4,v);});const shake=[real(3),real(3)];shake.forEach((v,i)=>{dv.setFloat32(p[3]+i*4,v,true);m.f32(manager+0x1c+i*4,v);});const tint=random();dv.setUint32(p[4],tint,true);m.u32(manager,tint);memory(c,p[5],1)[0]=scenario&1;m.u32(manager+4,scenario&1);
  const kind=scenario%5;drawn=[];const original=m.call([0x43a950,0x43aa50,0x43ab50,0x43ad40,0x43afe0][kind],{ecx:manager,args:[vm]})|0;assert.equal(c.renderer_draw(f,kind),original,'return '+scenario);dv=new DataView(c.memory.buffer);
  assert.deepEqual(memory(c,p[6],112),m.bytes(0x4dc558,112),'quad '+scenario+'/'+kind);assert.deepEqual(memory(c,c.renderer_data(f),c.renderer_size(f)),Uint8Array.from(drawn),'submit '+scenario);assert.equal(c.renderer_state(f,0),m.u32(manager+0x12880),'texture');assert.equal(c.renderer_state(f,1),blend,'blend');assert.equal(c.renderer_state(f,2),depth,'depth');assert.equal(c.renderer_state(f,3),m.bytes(manager+0x12884,1)[0]);assert.equal(c.renderer_state(f,4),m.bytes(manager+0x12887,1)[0]);++checks;if(drawn.length)++submitted;
 }report('sprite-renderer',{checks,submitted,paths:5,scope:'Original rounded/unrounded/rotated/mirrored CPU sprite geometry, pixel centering, viewport clipping, UVs, tint, blend/depth state and ordered triangle payloads. GPU submission is a recorded boundary.'});
 }finally{c.renderer_delete(f);m.close();}
});
