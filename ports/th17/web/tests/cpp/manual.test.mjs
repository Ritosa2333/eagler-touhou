import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';import {core,oracle,memory,string,root,report,enableNativeMath} from './helpers.mjs';import {resourceLoader,nativePool,prepareBank,nativeInstances,compareAnm} from './anm-oracle.mjs';
test('TH17 original nine-page manual preserves selection trees, page uploads, loading delays, directional turns and return waits',async()=>{
 const c=await core(),m=await oracle({memoryEnd:0x20000000,heapStart:0x8000000,heapLimit:0x1f000000});enableNativeMath(m);const loader=resourceLoader(m),base=m.heap;let frames=0,checks=0,events=[],owner=0;
 m.replace(0x465450,'manual sound output',()=>{events.push(1,m.u32(m.reg('ESP')+4));return 0;},2);
 m.replace(0x442280,'manual page loading boundary',()=>{assert.equal(m.u32(m.reg('ESP')+4),0x4327f0);events.push(3,m.i32(owner+0x24));return 0;},2);
 const previous=m.onImport;m.onImport=e=>{if(e.name==='D3DXLoadSurfaceFromFileInMemory'){events.push(4,m.i32(owner+0x24));m.ret(0,9);}else previous(e);};m.replace(0x473830,'manual texture invalidation',()=>0,1);
 try{for(let scenario=0;scenario<4;scenario++){
  m.heap=base;m.view(m.heap,0x1c90b00).fill(0);const a=c.anm_manager_create(),native=nativePool(m),manual=c.manual_create(a);owner=m.allocate(0x1bc);m.view(owner,0x1bc).fill(0);const age=c.manual_age(manual);m.write(owner+0x10,Buffer.from(memory(c,age,20)));m.f32(owner+0x128,128);m.u32(0x4a0430,0x4b5918);m.f32(0x4b5918,1);m.u32(0x4b76b4,owner);m.u32(0x4b76b8,0);
  try{
   const data=readFileSync(resolve(root,'reference/assets/help.anm')),p=c.allocate(data.length);memory(c,p,data.length).set(data);assert.equal(c.anm_manager_load(a,19,p,data.length),1,string(c,c.anm_manager_error(a)));c.deallocate(p);const bank=prepareBank(m,loader,'help.anm',data,native,19);m.u32(owner+0x12c,bank);const surface=m.u32(m.u32(bank+0x124)+0x18);const vt=m.u32(surface),pixels=m.allocate(16),face=m.allocate(16),fvt=m.allocate(64);m.u32(face,fvt);
   const stub=(at,name,n,fn)=>m.u32(at,m.registerImport({dll:'fixture',name,argc:n,handler:fn}));
   stub(vt+0x24,'texture update',1,()=>0);stub(vt+0x48,'texture surface',3,()=>{m.u32(m.u32(m.reg('ESP')+12),face);return 0;});
   stub(fvt+0x30,'surface description',2,()=>{const out=m.u32(m.reg('ESP')+8);m.view(out,32).fill(0);m.u32(out+28,1);return 0;});
   stub(fvt+0x34,'surface lock',4,()=>{const out=m.u32(m.reg('ESP')+8);events.push(2);m.u32(out,16);m.u32(out+4,pixels);return 0;});stub(fvt+0x38,'surface unlock',1,()=>0);stub(fvt+8,'surface release',1,()=>0);
   const compareQueue=label=>{for(let alt=0;alt<2;alt++){const list=nativeInstances(m,native,alt);assert.equal(c.anm_manager_count(a,alt),list.length,label+' count');for(let i=0;i<list.length;i++){const h=m.u32(list[i]+0x538);assert.equal(c.anm_manager_order(a,alt,i),h,label+' queue');const vm=c.anm_manager_find(a,h);compareAnm(c,m,vm,list[i],label+' script '+(m.u32(list[i]+0x490)&65535));assert.deepEqual(Buffer.from(memory(c,c.anm_vm_translation(vm),12)),Buffer.from(m.bytes(list[i]+0x5e4,12)));checks++;}}};
   const step=(pressed=0,repeated=0)=>{const label=scenario+'/'+frames;m.u32(0x4b323c,pressed);m.u32(0x4b3238,repeated);events=[];assert.equal(m.call(0x432af0,{ecx:owner}),1);assert.equal(c.manual_update(manual,pressed,repeated,1),1,label+': '+string(c,c.manual_error(manual)));assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.manual_events(manual),c.manual_event_count(manual))),events,label+' events');const v=Array.from(new Uint32Array(c.memory.buffer,c.manual_values(manual),14));for(const[i,offset]of[[0,0xc],[1,0x134],[2,0x124],[3,0x120],[4,0xf4]])assert.equal(v[i],m.u32(owner+offset),label+' field '+i);for(let i=0;i<9;i++)assert.equal(v[i+5],m.u32(owner+0xfc+i*4),label+' menu handle');assert.deepEqual(Buffer.from(memory(c,c.manual_cursor(manual),0xd8)),Buffer.from(m.bytes(owner+0x24,0xd8)),label+' cursor');assert.deepEqual(Buffer.from(memory(c,age,20)),Buffer.from(m.bytes(owner+0x10,20)),label+' timer');compareQueue(label+' pre-update');for(let alt=0;alt<2;alt++){assert.equal(c.anm_manager_update(a,alt,1),1,string(c,c.anm_manager_error(a)));m.call(alt?0x475940:0x475830,{ecx:native});}compareQueue(label+' post-update');frames++;return v;};
   const ready=()=>{m.u32(owner+0x134,3);assert.equal(c.manual_ready(manual),1);};
   for(let i=0;i<24;i++)step();for(let i=0;i<12;i++)step(i%2?0:32,i%2?32:0);step(0x80001);for(let i=0;i<7+scenario;i++)step(0x80103);ready();step();for(let i=0;i<22;i++)step();
   // Traverse every page forward and backward; waits cannot be skipped by input.
   for(let direction of [16,32,16])for(let j=0;j<9;j++){
    const before=Array.from(new Uint32Array(c.memory.buffer,c.manual_values(manual),14)),cursor=new DataView(c.memory.buffer).getInt32(c.manual_cursor(manual),true);step(direction);
    if(direction===16&&cursor>0||direction===32&&cursor<8){for(let i=0;i<20;i++)step(0x80001);assert.equal(new Uint32Array(c.memory.buffer,c.manual_values(manual),14)[1],2);for(let i=0;i<scenario+2;i++)step();ready();step();for(let i=0;i<22;i++)step();}
   }
   step(0x80001);for(let i=0;i<22;i++)step();step(16);step(0x102);for(let i=0;i<33;i++)step();assert.equal(new Uint32Array(c.memory.buffer,c.manual_values(manual),14)[2],1);
  }finally{c.manual_delete(manual);c.anm_manager_delete(a);}
 }
 report('manual',{passed:true,scenarios:4,frames,animationChecks:checks,originalFunctions:['0x432840','0x432af0','0x4326b0'],scope:'Actual help ANM animation queues, selection, all nine page turn paths, delayed PNG loading boundary, upload requests, timer/input suppression and close delay. PNG raster/pixels and whole title integration remain separate.'});
 }finally{m.close();}
});
