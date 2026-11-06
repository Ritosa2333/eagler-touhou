import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report,enableNativeMath} from './helpers.mjs';import {resourceLoader,nativePool,nativeInstances,prepareBank,compareAnm,retireOriginalAnimations} from './anm-oracle.mjs';

test('TH17 stage entry creates and restores the original animal inventory and active Roaring effects',async()=>{
 const c=await core(),m=await oracle(),a=c.anm_manager_create(),nm=nativePool(m),loader=resourceLoader(m),native=m.allocate(0x100),text=m.allocate(0x19270),player=m.allocate(0x19100),enemy=m.allocate(0x200),spell=m.allocate(0xc0),bomb=m.allocate(0x80),callbacks=[m.allocate(40),m.allocate(40)];enableNativeMath(m);
 for(const[at,p]of[[0x4b7684,native],[0x4b7678,text],[0x4b77d0,player],[0x4b76a0,enemy],[0x4b7690,spell],[0x4b7688,bomb],[0x4a0430,0x4b5918]])m.u32(at,p);m.f32(0x4b5918,1);m.i32(nm+0x104,-1);
 m.replace(0x465450,'stage-entry sound output',()=>0,2);m.replace(0x4654f0,'stage-entry sound output',()=>0,1);
 const load=(name,id)=>{const bytes=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.anm_manager_load(a,id,p,bytes.length),1,string(c,c.anm_manager_error(a)));c.deallocate(p);return prepareBank(m,loader,name,bytes,nm,id);};
 let f=0,cases=0,checks=0,frames=0;const dv=()=>new DataView(c.memory.buffer),same=(cp,np,n,label)=>{assert.deepEqual(Buffer.from(memory(c,cp,n)),Buffer.from(m.bytes(np,n)),label);checks++;};
 try{
  m.u32(text+0x19248,load('ascii.anm',2));assert.equal(c.anm_manager_fallback(a,2),1);const bank=load('beast.anm',30),heap=m.heap;
  for(let animal=0;animal<3;animal++)for(let kind=1;kind<=4;kind++){
   f=c.roaring_create(a,30);m.heap=heap;m.view(native,0x100).fill(0);m.u32(native+4,callbacks[0]);m.u32(native+8,callbacks[1]);m.u32(native+12,bank);m.view(player,0x19100).fill(0);m.view(enemy,0x200).fill(0);m.view(spell,0xc0).fill(0);m.view(bomb,0x80).fill(0);m.f32(0x4b5918,1);
   memory(c,c.roaring_field(f,0),104).set(Buffer.alloc(104));m.view(0x4b5a60,104).fill(0);m.i32(0x4b59f8,animal);dv().setInt32(c.roaring_field(f,3),animal,true);
   for(const[off,value]of[[0,-77.125],[4,410.25],[8,.125]]){dv().setFloat32(c.roaring_field(f,2)+off,value,true);m.f32(player+0x610+off,value);}
   const check=label=>{same(c.roaring_field(f,0),0x4b5a60,104,label+' inventory');same(c.roaring_field(f,1),native+0x24,48,label+' owned effects');for(let v=0;v<2;v++)same(c.roaring_rng(f,v),v?0x4b7660:0x4b7668,8,label+' RNG');for(let alt=0;alt<2;alt++){const list=nativeInstances(m,nm,alt);assert.equal(c.anm_manager_count(a,alt),list.length,label+' animation count');for(const np of list){const vm=c.anm_manager_find(a,m.u32(np+0x538));assert.ok(vm,label+' animation');compareAnm(c,m,vm,np,label+' ANM');same(c.anm_vm_visual(vm)+340,np+0x5e4,12,label+' position');}}};
   m.call(0x40dec0);assert.equal(c.roaring_enter_stage(f),1,string(c,c.roaring_error(f)));check('first stage '+animal+'/'+kind);
   for(const value of kind===4?[1,2,3,4,5]:[kind,kind,kind,4,5]){m.call(0x40f980,{ecx:native,args:[value]});assert.equal(c.roaring_collect(f,value),1,string(c,c.roaring_error(f)));}
   const flags=dv().getUint32(c.roaring_field(f,0)+100,true)|0x201;dv().setUint32(c.roaring_field(f,0)+100,flags,true);m.u32(0x4b5ac4,flags);
   m.call(0x40dec0);assert.equal(c.roaring_enter_stage(f),1,string(c,c.roaring_error(f)));check('active stage '+animal+'/'+kind);
   for(let frame=0;frame<16;frame++){for(let alt=0;alt<2;alt++){assert.equal(c.anm_manager_update(a,alt,1),1,string(c,c.anm_manager_error(a)));m.call(alt?0x475940:0x475830,{ecx:nm});}check('restored animation '+frame);frames++;}
   c.roaring_delete(f);f=0;retireOriginalAnimations(c,m,a,nm);cases++;
  }
  report('roaring-stage-entry',{passed:true,cases,frames,checks,originalFunction:'0x40dec0',scope:'Actual initial animal inventory script 29 and interrupt 34, slot icons, active Roaring timer/animal/mode reconstruction across stage entry for every animal/type, retained state, flags, both random streams and full animation trees. Scene callback enabling is owned by the shared scheduler.'});
 }finally{if(f)c.roaring_delete(f);c.anm_manager_delete(a);m.close();}
});
