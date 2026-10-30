import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report} from './helpers.mjs';
import {nativePool,resourceLoader,prepareBank,nativeInstances,compareAnm,retireOriginalAnimations} from './anm-oracle.mjs';

test('Original loading logo, footsteps and closing/opening transition panels preserve animation timing and owner cleanup',async()=>{
 const c=await core(),m=await oracle({memoryEnd:0x10000000,heapStart:0x08000000,heapLimit:0x0f000000}),a=c.anm_manager_create(),nm=nativePool(m),loader=resourceLoader(m),out=m.allocate(4);let effects=0,checks=0;
 try{
  const banks={},text=m.allocate(0x19270);m.u32(0x4b7678,text);
  for(const [id,name]of[[2,'ascii.anm'],[8,'effect.anm']]){
   const bytes=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.anm_manager_load(a,id,p,bytes.length),1,string(c,c.anm_manager_error(a)));c.deallocate(p);banks[id]=prepareBank(m,loader,name,bytes,nm,id);if(id===2){c.anm_manager_fallback(a,2);m.u32(text+0x19248,banks[2]);}
  }
  const owner=m.allocate(0x2020);m.u32(0x4b7698,owner);m.u32(owner+12,banks[8]);m.u32(0x4a0430,0x4b5918);m.f32(0x4b5918,1);m.f32(0x524718,1);effects=c.anm_scene_effects_create(a,8);
  m.write(0x4b7660,Buffer.from(memory(c,c.anm_manager_random(a),8)));m.write(0x4b7668,Buffer.from(memory(c,c.anm_manager_environment(a),8)));
  const overlay=c.anm_manager_spawn(a,8,0,-1,0);m.call(0x405a70,{ecx:banks[8],args:[out,0,0xffffffff,0]});assert.equal(overlay,m.u32(out));const vm=c.anm_manager_find(a,overlay),native=m.call(0x476470,{ecx:nm,args:[overlay]});
  assert.equal(c.anm_scene_effects_configure(effects,vm,0),1);m.call(0x406950,{ecx:native});for(const off of[0x5c8,0x5cc,0x5d0,0x5d4])m.u32(native+off,1);
  const interrupt=(handle,label)=>{assert.equal(c.anm_registry_interrupt(c.anm_manager_registry(a),c.anm_manager_find(a,handle),label,0,1),1);m.call(0x476510,{args:[handle,label]});};interrupt(overlay,7);
  const loading=c.anm_manager_spawn(a,2,17,-1,0),translation=c.anm_vm_translation(c.anm_manager_find(a,loading));new Float32Array(c.memory.buffer,translation,3).set([960,784,0]);
  const thunk=m.allocate(21),code=Buffer.from([0xf3,0x0f,0x10,0x0d,0xa8,0x3d,0x4a,0,0xf3,0x0f,0x10,0x15,0x8c,0x3d,0x4a,0,0xe9,0,0,0,0]);code.writeInt32LE(0x41d3d0-thunk-code.length,17);m.write(thunk,code);m.call(thunk);assert.equal(loading,m.u32(text+0x19250));
  for(let frame=0;frame<240;frame++){
   if(frame===70)interrupt(overlay,1);if(frame===180)interrupt(loading,1);
   for(let alt=0;alt<2;alt++){
    const originals=nativeInstances(m,nm,alt);assert.equal(c.anm_manager_count(a,alt),originals.length,'frame '+frame+' pool count');
    for(const np of originals){const handle=m.u32(np+0x538),cp=c.anm_manager_find(a,handle);assert.ok(cp,'native handle '+handle);compareAnm(c,m,cp,np,'frame '+frame+' handle '+handle);checks++;
     if(handle===overlay)for(let i=0;i<5;i++){const panel=c.anm_scene_effects_panel(cp,i);compareAnm(c,m,panel,m.u32(np+0x5bc)+i*0x600,'frame '+frame+' panel '+i);checks++;}
    }
    assert.equal(c.anm_manager_update(a,alt,1),1,string(c,c.anm_manager_error(a)));m.call(alt?0x475940:0x475830,{ecx:nm});
   }
   assert.deepEqual(Buffer.from(memory(c,c.anm_manager_random(a),8)),Buffer.from(m.bytes(0x4b7660,8)),'frame '+frame+' visual RNG');
  }
  assert.equal(c.anm_manager_find(a,loading),0,'Loading controller and child animations have finished');
  assert.equal(c.anm_manager_count(a,2),1,'Native transition controller stays owned by its scene');
  for(let i=0;i<4;i++)assert.equal(c.anm_vm_offset(c.anm_scene_effects_panel(vm,i)),-1,'Opening panel has finished');
  retireOriginalAnimations(c,m,a,nm);
  report('loading-animations',{passed:true,frames:240,checks,ownerCleanup:true,originalFunctions:['0x41d3d0','0x406950','0x406b00','0x4070d0','0x475830','0x475940','0x476680'],scope:'Original ascii.anm loading controller and its white logo/red footstep children, original effect.anm four-panel shutter and scrolling backdrop, complete per-frame animation state, shared visual RNG and scene-owner cleanup. Actual browser pixels and scene entrance integration are checked separately.'});
 }finally{if(effects)c.anm_scene_effects_delete(effects);c.anm_manager_delete(a);m.close();}
});
