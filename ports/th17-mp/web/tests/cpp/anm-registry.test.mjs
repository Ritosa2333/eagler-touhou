import test from 'node:test';import assert from 'node:assert/strict';import {core,oracle,memory,string,report} from './helpers.mjs';
function nativePool(m){const manager=m.allocate(0x1c90b00),sentinel=manager+0x18600ec;m.u32(0x509a20,manager);m.u32(sentinel+4,manager+0x6ec+0x600);for(let i=0;i<0x3fff;i++){const vm=manager+0x6ec+i*0x618,node=vm+0x600;m.u32(node,vm);m.u32(node+4,i+1<0x3fff?node+0x618:0);m.u32(node+8,i?node-0x618:sentinel);m.u32(vm+0x614,i);m.u32(vm+0x28,0xffffffff);}return manager;}
function order(m,manager,alternate){const result=[];for(let node=m.u32(manager+(alternate?0x6e4:0x6dc));node;node=m.u32(node+4))result.push(m.u32(m.u32(node)+0x538));return result;}
function nativeAttach(m,child,parent){const old=m.u32(parent+0x58c),node=child+0x578;m.u32(node,child);m.u32(node+4,old);m.u32(node+8,parent+0x588);if(old)m.u32(old+8,node);m.u32(parent+0x58c,node);m.u32(child+0x5b0,m.u32(parent+0x5b0)||parent);m.u32(child+0x5b4,parent);}
test('TH17 animation registry matches original pool slots, handle generations, overflow, queue ordering and reuse',async()=>{
 const c=await core(),m=await oracle(),registry=c.anm_registry_create(),manager=nativePool(m),out=m.allocate(4),instances=[];let checks=0,allocations=0;
 const allocate=(ordering,i)=>{if(i===128){new DataView(c.memory.buffer).setUint32(c.anm_registry_generation(registry),0x3ffff,true);m.u32(manager+0x1c90a44,0x3ffff);}const native=m.call(0x476a60)>>>0,vm=c.anm_registry_allocate(registry);assert.equal(c.anm_registry_slot(registry,vm),m.u32(native+0x53c),'pool slot '+i);m.u32(native+0x28,0xffffffff);const handle=c.anm_registry_submit(registry,vm,ordering);m.call([0x475c80,0x475d30,0x475dc0,0x475e70][(ordering&4?2:0)+(ordering&2?1:0)],{args:[out,native]});assert.equal(handle,m.u32(out),'generation '+i);assert.equal(c.anm_registry_find(registry,handle),vm);assert.equal(m.call(0x476470,{ecx:manager,args:[handle]}),native);allocations++;checks+=4;return{native,vm,handle};};
 try{for(let i=0;i<16386;i++)instances.push(allocate((i%4)*2,i));for(let alternate=0;alternate<2;alternate++){const expected=order(m,manager,alternate);assert.equal(c.anm_registry_count(registry,alternate),expected.length);assert.deepEqual(Array.from({length:expected.length},(_,i)=>c.anm_registry_order(registry,alternate,i)),expected);checks+=2;}
  const released=[];for(let i=0;i<instances.length;i++)if(i%37===0||i>=16383){const instance=instances[i];assert.equal(c.anm_registry_release(registry,instance.vm,0),1);m.call(0x475f00,{ecx:manager,args:[instance.native]});assert.equal(c.anm_registry_find(registry,instance.handle),0,'stale C++ handle');assert.equal(m.call(0x476470,{ecx:manager,args:[instance.handle]}),0,'stale native handle');released.push(instance);checks+=3;}
  for(let i=0;i<released.length+3;i++)allocate(i%2?2:4,16386+i);for(let alternate=0;alternate<2;alternate++){const expected=order(m,manager,alternate);assert.equal(c.anm_registry_count(registry,alternate),expected.length);assert.deepEqual(Array.from({length:expected.length},(_,i)=>c.anm_registry_order(registry,alternate,i)),expected);checks+=2;}assert.equal(string(c,c.anm_registry_error(registry)),'');report('anm-registry-pool',{passed:true,allocations,checks,poolCapacity:16383,originalFunctions:['0x476a60','0x475c80','0x475d30','0x475dc0','0x475e70','0x476470','0x475f00'],scope:'Primary animation pool allocation, four registration orders, generation wrap, heap overflow, stale-handle rejection and free-slot reuse. Script binding/execution and GPU drawing are separate integration requirements.'});}
 finally{c.anm_registry_delete(registry);m.close();}
});
test('TH17 recycled overflow nodes start with fresh VM state and reject retired handles',async()=>{
 const c=await core(),registry=c.anm_registry_create();
 try{
  for(let i=0;i<16383;i++)c.anm_registry_allocate(registry);
  const vm=c.anm_registry_allocate(registry),handle=c.anm_registry_submit(registry,vm,2);
  new DataView(c.memory.buffer).setFloat32(c.anm_vm_translation(vm),127.5,true);
  assert.equal(c.anm_registry_release(registry,vm,0),1);
  const reused=c.anm_registry_allocate(registry);
  assert.equal(reused,vm,'Overflow allocation is reused');
  assert.equal(new DataView(c.memory.buffer).getFloat32(c.anm_vm_translation(reused),true),0,'A recycled overflow VM retains constructor semantics');
  const next=c.anm_registry_submit(registry,reused,4);
  assert.notEqual(next,handle);assert.equal(c.anm_registry_find(registry,handle),0);assert.equal(c.anm_registry_find(registry,next),reused);
 }finally{c.anm_registry_delete(registry);}
});
test('TH17 overflow handle index retains native queue precedence after generation wrap and deletion',async()=>{
 const c=await core(),m=await oracle(),registry=c.anm_registry_create(),manager=nativePool(m),out=m.allocate(4),instances=[];
 try{
  // Exhaust the regular slots without registering them. All following VMs
  // must use the shared overflow slot, including duplicate generations.
  for(let i=0;i<16383;i++){c.anm_registry_allocate(registry);m.call(0x476a60);}
  for(const ordering of [4,0,2,6,0]){
   new DataView(c.memory.buffer).setUint32(c.anm_registry_generation(registry),0x23455,true);m.u32(manager+0x1c90a44,0x23455);
   const vm=c.anm_registry_allocate(registry),native=m.call(0x476a60)>>>0;m.u32(native+0x28,0xffffffff);
   const handle=c.anm_registry_submit(registry,vm,ordering)>>>0;m.call([0x475c80,0x475d30,0x475dc0,0x475e70][(ordering&4?2:0)+(ordering&2?1:0)],{args:[out,native]});
   assert.equal(handle,m.u32(out));instances.push({vm,native,handle});
  }
  const handle=instances[0].handle;
  const compare=()=>{const native=m.call(0x476470,{ecx:manager,args:[handle]})>>>0,expected=instances.find(x=>x.native===native)?.vm||0;assert.equal(c.anm_registry_find(registry,handle),expected);};
  compare();
  for(const index of [1,2,4,3,0]){const instance=instances[index];assert.equal(c.anm_registry_release(registry,instance.vm,0),1);m.call(0x475f00,{ecx:manager,args:[instance.native]});compare();}
  assert.equal(c.anm_registry_find(registry,handle),0);
  report('anm-registry-overflow-collision',{passed:true,overflowInstances:5,checks:12,originalFunctions:['0x476470','0x475f00'],scope:'Duplicate overflow generations across front/back and both groups; exact native first-match semantics after each deletion.'});
 }finally{c.anm_registry_delete(registry);m.close();}
});
test('TH17 animation registry matches original parent/child pause, interrupts, deferred deletion and layer queues',async()=>{
 const c=await core(),m=await oracle(),registry=c.anm_registry_create(),manager=nativePool(m),out=m.allocate(4),instances=[];m.u32(0x4a0430,0x4b5918);m.f32(0x4b5918,1);let checks=0,drawOutput=[];m.replace(0x46f8c0,'observe draw selection',()=>{drawOutput.push(m.u32(m.u32(m.reg('ESP')+4)+0x538));return 0;},1);
 const compareOrder=()=>{for(let alternate=0;alternate<2;alternate++){const expected=order(m,manager,alternate);assert.equal(c.anm_registry_count(registry,alternate),expected.length);assert.deepEqual(Array.from({length:expected.length},(_,i)=>c.anm_registry_order(registry,alternate,i)),expected);checks+=2;}};
 try{for(let i=0;i<96;i++){const native=m.call(0x476a60)>>>0,vm=c.anm_registry_allocate(registry),ordering=(i%4)*2,visual=c.anm_vm_visual(vm),layer=[0,3,17,24,29,36,38,42][i%8];m.u32(native+0x28,0xffffffff);m.u32(native+0x530,0x70006);m.u32(native+0x534,0x8000);m.u32(native+0x18,layer);const view=new DataView(c.memory.buffer);view.setUint32(visual,0x70006,true);view.setUint32(visual+4,0x8000,true);view.setInt32(visual+8,layer,true);const handle=c.anm_registry_submit(registry,vm,ordering);m.call([0x475c80,0x475d30,0x475dc0,0x475e70][i%4],{args:[out,native]});assert.equal(handle,m.u32(out));instances.push({native,vm,handle,visual});if(i%8&&i%8!==7){const parent=instances[i-(i%3?1:2)];assert.equal(c.anm_registry_attach(registry,vm,parent.vm),1);nativeAttach(m,native,parent.native);}}
  for(const root of [instances[0],instances[8],instances[16]])for(const paused of [1,0]){assert.equal(c.anm_registry_pause(registry,root.vm,paused),1);m.u32(out,root.handle);m.call(paused?0x476820:0x4767e0,{ecx:out});for(const instance of instances)assert.equal(new DataView(c.memory.buffer).getUint32(instance.visual,true),m.u32(instance.native+0x530));checks+=instances.length;}
  for(const root of [instances[0],instances[8],instances[16]]){assert.equal(c.anm_registry_interrupt(registry,root.vm,37,0,1),1);m.call(0x476510,{args:[root.handle,37]});for(const instance of instances)assert.equal(c.anm_vm_pending_interrupt(instance.vm),m.i32(instance.native+0x490));checks+=instances.length;}
  for(let alternate=0;alternate<2;alternate++){assert.equal(c.anm_registry_update(registry,alternate,1),1,string(c,c.anm_registry_error(registry)));m.call(alternate?0x475940:0x475830,{ecx:manager});const first=alternate?36:0,last=alternate?43:36;for(let layer=first;layer<last;layer++){const expected=[];drawOutput=expected;m.call(0x475ba0,{ecx:manager,args:[layer]});assert.equal(c.anm_registry_layer_count(registry,layer),expected.length,'layer count '+layer);assert.deepEqual(Array.from({length:expected.length},(_,i)=>c.anm_registry_layer_handle(registry,layer,i)),expected,'layer order '+layer);checks+=2;}}
  for(const root of [instances[0],instances[32],instances[64]]){new DataView(c.memory.buffer).setUint32(root.visual+4,0x8020,true);m.u32(root.native+0x534,0x8020);}for(let alternate=0;alternate<2;alternate++){assert.equal(c.anm_registry_update(registry,alternate,1),1,string(c,c.anm_registry_error(registry)));m.call(alternate?0x475940:0x475830,{ecx:manager});compareOrder();}
  assert.equal(c.anm_registry_retire(registry,instances[8].vm),1);m.call(0x476620,{args:[instances[8].handle]});for(let alternate=0;alternate<2;alternate++){assert.equal(c.anm_registry_update(registry,alternate,1),1);m.call(alternate?0x475940:0x475830,{ecx:manager});}compareOrder();for(let i=0;i<24;i++){const native=m.call(0x476a60)>>>0,vm=c.anm_registry_allocate(registry);assert.equal(c.anm_registry_slot(registry,vm),m.u32(native+0x53c),'deletion reuse '+i);checks++;}report('anm-registry-lifecycle',{passed:true,checks,initialInstances:96,originalFunctions:['0x4767e0','0x476820','0x476510','0x475830','0x475940','0x475ba0','0x475a60','0x475f00','0x476620'],scope:'Parent/child tree order, recursive visibility pause, immediate-child interrupt propagation, two update groups, layer remapping and queues, recursive retirement and deferred destruction and subsequent reuse. Instances intentionally have stopped script cursors; real ANM binding and child spawning need their own integration verification.'});}
 finally{c.anm_registry_delete(registry);m.close();}
});
