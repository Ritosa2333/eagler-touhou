import test from 'node:test';import assert from 'node:assert/strict';import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report,enableNativeMath} from './helpers.mjs';import {resourceLoader,nativePool,nativeInstances,prepareBank,compareAnm,retireOriginalAnimations} from './anm-oracle.mjs';
test('TH17 continuous Roaring, otter barrier, break and delayed animal rewards match original frames',async()=>{
 const c=await core(),m=await oracle(),a=c.anm_manager_create(),nm=nativePool(m),loader=resourceLoader(m),native=m.allocate(0x100),text=m.allocate(0x19270),player=m.allocate(0x19100),spell=m.allocate(0x90),bomb=m.allocate(0x80),stage=m.allocate(0x80),hud=m.allocate(0x200),lasers=m.allocate(0x610),laser=m.allocate(0x20),vtable=m.allocate(0x28),handle=m.allocate(4),scratch=m.allocate(4),tickCall=m.allocate(64);enableNativeMath(m);
 for(const [at,p]of [[0x4b7684,native],[0x4b7678,text],[0x4b77d0,player],[0x4b7690,spell],[0x4b7688,bomb],[0x4b76a0,stage],[0x4b76ac,hud],[0x4b76bc,lasers],[0x4a0430,0x4b5918]])m.u32(at,p);m.f32(0x4b5918,1);
 // The original manager owns an idle fallback VM for expired handles.
 m.i32(nm+0xdc+0x28,-1);
 let events=[],f,frames=0,checks=0,scenarios=0,barriers=0,animalRewards=0,completed=0,breaks=0;const sp=()=>m.reg('ESP');
 for(const [at,id,argc]of [[0x4654f0,1,1],[0x465450,2,2]])m.replace(at,'observe continuous Roaring audio '+id,()=>{events.push(id,m.u32(sp()+4));return 0;},argc);
 m.replace(0x434740,'observe continuous Roaring item reward',()=>{const p=m.u32(sp()+8);events.push(3,m.u32(sp()+4),m.u32(p),m.u32(p+4),m.u32(p+8),m.u32(sp()+16),m.u32(sp()+20),m.u32(sp()+24),m.u32(sp()+32));return 0;},8);
 const captureSse=(at,name,argc,fn)=>{const stub=m.registerImport({dll:'fixture',name,argc,handler:fn}),code=Buffer.from([0xf3,0x0f,0x11,0x15,0,0,0,0,0xe9,0,0,0,0]);code.writeUInt32LE(scratch,4);code.writeInt32LE(stub-at-code.length,9);m.write(at,code);};
 captureSse(0x410380,'observe delayed Roaring animals',2,()=>{const p=m.u32(sp()+4);events.push(4,m.u32(sp()+8),m.u32(p),m.u32(p+4),m.u32(p+8),m.u32(scratch));animalRewards++;return 0;});
 captureSse(0x419df0,'observe Roaring bullet cancellation',2,()=>{const p=m.u32(sp()+4);events.push(5,m.u32(p),m.u32(p+4),m.u32(p+8),m.u32(scratch),m.u32(sp()+8));return 0;});
 m.replace(0x435ab0,'observe Roaring laser cancellation',()=>{const p=m.u32(sp()+4);events.push(6,m.u32(p),m.u32(p+4),m.u32(p+8),m.u32(scratch),m.u32(sp()+8),m.u32(sp()+12));return 0;},3);
 const cancel=m.registerImport({dll:'fixture',name:'observe otter laser contact',argc:4,handler:()=>{const p=m.u32(sp()+4);events.push(6,m.u32(p),m.u32(p+4),m.u32(p+8),m.u32(sp()+8),m.u32(sp()+12),m.u32(sp()+16));barriers++;return 0;}});m.u32(vtable+0x24,cancel);m.u32(laser,vtable);m.u32(lasers+0x14,laser);
 // Original player damage-source loop with the same bounded entry/exit as its dedicated test.
 const setup=Buffer.from([0x55,0x89,0xe5,0x83,0xec,0x40,0x53,0x56,0x57,0x89,0xcf,0xb9,0,1,0,0,0xe9,0,0,0,0,0x5f,0x5e,0x5b,0x89,0xec,0x5d,0xc3]);setup.writeInt32LE(0x447ef3-(tickCall+21),17);m.write(tickCall,setup);const leave=Buffer.from([0xe9,0,0,0,0]);leave.writeInt32LE(tickCall+21-0x448047-5,1);m.write(0x448047,leave);
 const load=(name,id)=>{const b=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(b.length);memory(c,p,b.length).set(b);assert.equal(c.anm_manager_load(a,id,p,b.length),1,string(c,c.anm_manager_error(a)));c.deallocate(p);return prepareBank(m,loader,name,b,nm,id);};
 const same=(cp,np,n,label)=>{const actual=Buffer.from(memory(c,cp,n)),expected=Buffer.from(m.bytes(np,n));if(!actual.equals(expected)){let i=0;while(actual[i]===expected[i])i++;assert.fail(label+' byte '+i.toString(16)+' actual='+actual.subarray(Math.max(0,i-4),i+12).toString('hex')+' original='+expected.subarray(Math.max(0,i-4),i+12).toString('hex'));}};
 try{m.u32(text+0x19248,load('ascii.anm',2));assert.equal(c.anm_manager_fallback(a,2),1);const bank=load('beast.anm',30),heap=m.heap;
  for(let scenario=0;scenario<18;scenario++){
   f=c.roaring_create(a,30);m.heap=heap;m.view(native,0x100).fill(0);m.u32(native+12,bank);m.view(0x4b5a60,104).fill(0);m.view(player,0x19100).fill(0);m.write(player+0xf114,Buffer.from(memory(c,c.roaring_field(f,8),256*0x9c)));m.view(spell,0x90).fill(0);m.view(bomb,0x80).fill(0);m.view(stage,0x80).fill(0);m.f32(0x4b5918,1);assert.equal(c.anm_manager_update(a,0,1),1);
   const dv=()=>new DataView(c.memory.buffer),write=(field,off,np,value,float=false)=>{const at=c.roaring_field(f,field)+off;if(float){dv().setFloat32(at,value,true);m.f32(np,value);}else{dv().setUint32(at,value>>>0,true);m.u32(np,value);}};
   write(3,0,0x4b59f8,scenario%3);write(2,0,player+0x610,-90.125,true);write(2,4,player+0x614,420.125,true);write(2,8,player+0x618,.125,true);
   for(let v=0;v<2;v++){const rng=Buffer.alloc(8);rng.writeUInt16LE(1733+scenario*137+v*373);m.write(v?0x4b7660:0x4b7668,rng);memory(c,c.roaring_rng(f,v),8).set(rng);}
   m.call(0x405a70,{ecx:bank,args:[handle,13,0xffffffff,0]});m.u32(native+0x24,m.u32(handle));assert.equal(c.roaring_hud(f),1,string(c,c.roaring_error(f)));
   const check=tag=>{const label=scenario+'/'+tag;same(c.roaring_field(f,0),0x4b5a60,104,label+' state');same(c.roaring_field(f,1),native+0x24,48,label+' owned handles');same(c.roaring_field(f,4),player+0x18e8c,4,label+' player flags');same(c.roaring_field(f,5),player+0x18e78,20,label+' invulnerability');for(const [off,at]of [[0,spell+0x78],[4,spell+0x24],[8,spell+0x7c]])same(c.roaring_field(f,6)+off,at,4,label+' spell');same(c.roaring_field(f,8),player+0xf114,256*0x9c,label+' damage sources');same(c.roaring_field(f,9),player+0xf110,4,label+' damage cursor');same(c.roaring_field(f,10),stage+0x40,4,label+' breaks');same(c.roaring_field(f,11),stage+0x44,4,label+' chain');
    for(let v=0;v<2;v++)same(c.roaring_rng(f,v),v?0x4b7660:0x4b7668,8,label+' RNG');assert.deepEqual(Array.from(new Uint32Array(c.memory.buffer,c.roaring_events(f),c.roaring_event_count(f))),events,label+' events');
    for(let alt=0;alt<2;alt++){const objects=nativeInstances(m,nm,alt);assert.equal(c.anm_manager_count(a,alt),objects.length,label+' ANM count');for(const np of objects){const vm=c.anm_manager_find(a,m.u32(np+0x538));compareAnm(c,m,vm,np,label+' ANM '+np);same(c.anm_vm_visual(vm)+340,np+0x5e4,12,label+' ANM translation');}}checks++;
   };
   const dominant=scenario%4+1,rare=scenario%8,sequence=[dominant>3?1:dominant,dominant>3?2:dominant,dominant>3?3:dominant,4,rare?rare+7:7];
   for(const kind of sequence){events=[];m.call(0x40f980,{ecx:native,args:[kind]});assert.equal(c.roaring_collect(f,kind),1,string(c,c.roaring_error(f)));check('collect '+kind);}
   write(0,72,0x4b5aa8,90);write(0,76,0x4b5aac,90,true);
   for(let frame=0;frame<240;frame++){
    const rate=[1,.5,.25,1.25][scenario%4],active=frame%7<3,hudCollect=frame>=20&&frame<45,freeze=frame>=60&&frame<70,held=frame===100&&scenario%3===0?2:0;
    const flag=dv().getUint32(c.roaring_field(f,0)+100,true);write(0,100,0x4b5ac4,(flag&~0x200)|(freeze?0x200:0));write(12,0,player+0x119c,active?1:0);m.i32(hud+0x1c0,hudCollect?1:0);m.u32(0x4b3448,held);m.f32(0x4b5918,rate);
    const region=scenario%6;const pos=[region%3===0?-100.125:region%3===1?-.125:100.125,region<3?120.125:420.125,.125];for(let k=0;k<3;k++)write(2,k*4,player+0x610+k*4,pos[k],true);
    if(frame===80&&scenario%3===1&&(m.u32(0x4b5ac4)&2)){events=[];m.call(0x40f6d0,{ecx:native,args:[scenario%2]});assert.equal(c.roaring_break(f,scenario%2),1,string(c,c.roaring_error(f)));check('death/manual break');breaks++;for(let i=0;i<5;i++){events=[];m.call(0x40f980,{ecx:native,args:[i%3+1]});assert.equal(c.roaring_collect(f,i%3+1),1);check('reserve '+i);}}
    const wasActive=m.u32(0x4b5ac4)&2;events=[];m.call(0x40e750,{ecx:native});assert.equal(c.roaring_update(f,rate,hudCollect?1:0,held),1,string(c,c.roaring_error(f)));check('frame '+frame);if(wasActive&&!(m.u32(0x4b5ac4)&2))completed++;
    for(let alt=0;alt<2;alt++){assert.equal(c.anm_manager_update(a,alt,rate),1,string(c,c.anm_manager_error(a)));m.call(alt?0x475940:0x475830,{ecx:nm});}m.call(tickCall,{ecx:player});c.roaring_damage_tick(f,rate);check('animations '+frame);frames++;
   }
   c.roaring_delete(f);f=0;retireOriginalAnimations(c,m,a,nm);scenarios++;
  }
  assert.ok(barriers>0);assert.ok(animalRewards>0);assert.ok(completed>0);assert.ok(breaks>0);report('roaring-continuous-frames',{passed:true,scenarios,frames,checks,barriers,animalRewards,completed,breaks,originalFunctions:['0x40e750','0x40ee70','0x40e920','0x40f6d0','0x449c80'],scope:'All Roaring types, inventory HUD hiding, native ANM trees and translations, near-player otter cancellation requests and damage sources, fractional countdown including shot drain and freeze/resume, voluntary/death break, reserve reactivation, end rewards and six launch regions. Item/spirit factories and bullet/laser contact at host boundaries; whole-game replay remains separate.'});
 }finally{if(f)c.roaring_delete(f);c.anm_manager_delete(a);m.close();}
});
