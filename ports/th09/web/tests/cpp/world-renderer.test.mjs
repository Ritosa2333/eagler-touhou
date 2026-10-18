import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report,d3dxScalar} from './helpers.mjs';
test('World sprites and billboards match original transforms and submitted vertices',async()=>{
 const m=await oracle(),c=await core(),f=c.renderer_fixture(),p=Array.from({length:12},(_,i)=>c.renderer_part(f,i)),manager=m.allocate(0x12900),vm=m.allocate(0x2a4),sprite=m.allocate(0x44),device=m.allocate(4),vt=m.allocate(0x160),arg=i=>m.u32(m.reg('ESP')+4+4*i);d3dxScalar(m);
 let dv=new DataView(c.memory.buffer),drawn=[],matrices=new Map(),checks=0,seed=0x21423812;const random=()=>{seed^=seed<<13;seed^=seed>>>17;seed^=seed<<5;return seed>>>0;},real=scale=>(random()%10001-5000)/5000*scale;
 m.view(manager,0x12900).fill(0);m.write(manager+0x12884,[3,0,255,255,255]);m.u32(0x4dc550,manager);m.u32(0x4b3108,device);m.u32(device,vt);m.u32(0x4b3448,0x4b3178);
 for(const [slot,argc] of [[0xf4,3],[0xc8,3],[0x130,2],[0x14c,4],[0xfc,4],[0xa0,2]])m.u32(vt+slot,m.registerImport({dll:'world-draw',name:'state',argc,handler:()=>0}));
 m.u32(vt+0x94,m.registerImport({dll:'world-draw',name:'matrix',argc:3,handler:()=>{matrices.set(arg(1),m.bytes(arg(2),64));return 0;}}));
 const worldVertices=[-128,-128,0,0,0,128,-128,0,1,0,-128,128,0,0,1,128,128,0,1,1],worldBytes=new Uint8Array(new Float32Array(worldVertices).buffer);m.write(manager+0x12894,worldBytes);
 m.u32(vt+0x118,m.registerImport({dll:'world-draw',name:'vertices',argc:4,handler:()=>{drawn.push(...worldBytes);return 0;}}));
 m.replace(0x4396a0,'flush',()=>0);m.replace(0x439730,'quad',()=>{const b=m.bytes(arg(0),112);for(const n of [0,1,2,2,1,3])drawn.push(...b.subarray(n*28,(n+1)*28));return 0;},1);m.replace(0x42ff40,'depth',()=>0,2);
 m.view(0x4dc558,112).fill(0);for(let n=0;n<4;++n){m.f32(0x4dc558+28*n+12,1);m.u32(0x4dc558+28*n+16,0xffffffff);}
 const write=(at,n,values)=>{const b=new Uint8Array(new Float32Array(values).buffer);memory(c,at,b.length).set(b);m.write(n,b);};
 const identity=[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1];
 try{for(let scenario=0;scenario<1800;++scenario){
  dv=new DataView(c.memory.buffer);memory(c,p[0],0x2a4).fill(0);m.view(vm,0x2a4).fill(0);m.u32(vm+0x224,sprite);const flags=15|((scenario%4)<<4)|((scenario%4)<<11)|(scenario%3===0?0x2000:0)|(scenario%7===0?0x20000:0)|(scenario&1?0x8000:0);
  dv.setUint32(p[0]+0x1f8,flags,true);m.u32(vm+0x1f8,flags);dv.setUint32(p[0]+0x1f0,0xff556677,true);m.u32(vm+0x1f0,0xff556677);dv.setUint32(p[0]+0x1f4,0x80408090,true);m.u32(vm+0x1f4,0x80408090);
  write(p[0],vm,[real(2),real(2),real(6)]);write(p[0]+0x18,vm+0x18,[real(2),real(2)]);write(p[0]+0x28,vm+0x28,[16+random()%256,16+random()%256,scenario%9===0?0:real(1),real(1)]);write(p[0]+0x208,vm+0x208,[real(300),real(300),real(900)]);
  for(const off of [0x130,0x170,0x1b0]){const values=identity.slice();values[0]=real(1);values[5]=real(1);write(p[0]+off,vm+off,values);}dv.setUint32(p[1]+4,1,true);m.u32(sprite+4,1);write(p[1]+0x20,sprite+0x20,[.125,.25,.875,.75]);
  [16,16,288,448].forEach((v,i)=>{dv.setUint32(p[2]+i*4,v,true);m.u32(0x4b3178+0xcc+i*4,v);});write(p[2]+16,0x4b3178+0xdc,[0,1]);write(p[3],manager+0x1c,[0,0]);m.u32(manager+4,0);
  const bg=[0,0,1000,0,0,-1,0,-1,0,0,0,0,0,0,0,.5235988];write(p[10],0x4b3178,bg.slice(0,12));write(p[10]+48,0x4b3178+0x3c,bg.slice(12));c.graphics_camera(p[7],p[2],p[10],1);m.call(0x4306b0,{args:[0x4b3178]});
  const kind=scenario%3+5;drawn=[];matrices.clear();const ret=m.call([0x43b140,0x43b0e0,0x43b1a0][kind-5],{ecx:manager,args:[vm]})|0;assert.equal(c.renderer_draw(f,kind),ret,'return '+scenario);dv=new DataView(c.memory.buffer);
  assert.deepEqual(memory(c,p[0],0x200),m.bytes(vm,0x200),'VM matrix/flags '+scenario);assert.deepEqual(memory(c,c.renderer_data(f),c.renderer_size(f)),Uint8Array.from(drawn),'submitted '+scenario+'/'+kind);
  if(kind!==7)assert.deepEqual(memory(c,p[6],112),m.bytes(0x4dc558,112),'quad '+scenario);else{assert.deepEqual(memory(c,p[8],64),matrices.get(256),'world '+scenario);if(matrices.has(16))assert.deepEqual(memory(c,p[9],64),matrices.get(16),'texture '+scenario);}++checks;
 }report('world-renderer',{checks,scope:'Original 3D world matrices, matrix invalidation flags, anchoring, world-to-screen projection, billboards, UV transform caching and vertex payloads. Original scalar D3DX executes; final GPU rasterization is a boundary.'});
 }finally{c.renderer_delete(f);m.close();}
});
