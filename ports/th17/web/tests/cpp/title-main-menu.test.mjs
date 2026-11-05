import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report,enableNativeMath} from './helpers.mjs';
import {resourceLoader,nativePool,prepareBank,compareAnm,nativeInstances} from './anm-oracle.mjs';

test('TH17 original title entrance, all ten destinations, locked Extra and returning selections',async()=>{
 const c=await core(),m=await oracle({memoryEnd:0x10000000,heapStart:0x08000000,heapLimit:0x0f000000});
 enableNativeMath(m);m.u32(0x4a0430,0x4b5918);m.f32(0x4b5918,1);m.view(0x4b7660,8).fill(0);
 const nativeManager=nativePool(m),loader=resourceLoader(m),banks=new Map(),assets=new Map();
 for(const [id,name]of [[0,'ascii.anm'],[16,'title.anm'],[17,'title_v.anm']]){const bytes=readFileSync(resolve(root,'reference/assets',name));assets.set(id,bytes);banks.set(id,prepareBank(m,loader,name,bytes,nativeManager,id));}
 const text=m.allocate(0x19270);m.u32(0x4b7678,text);m.u32(text+0x19248,banks.get(0));
 const title=m.allocate(0x5e18),progress=m.allocate(0x2d200);m.u32(0x4b77dc,progress);m.u32(0x4b77f0,title);
 const poolSnapshot=m.bytes(nativeManager,0x1c90b00),rngSnapshot=m.bytes(0x4b7660,8),heap=m.heap,sounds=[];
 m.replace(0x465450,'capture title sound',()=>{sounds.push(m.u32(m.reg('ESP')+4));return 0;},2);
 let checks=0,frames=0,scenarios=0;
 const cases=[...Array.from({length:10},(_,selected)=>({selected,extra:true})),{selected:2,extra:false},{selected:9,extra:false,cancel:true},{selected:3,extra:true,mode:0x20},{selected:2,extra:true,mode:0x10},{selected:6,extra:true,returning:true}];
 try{for(const scenario of cases){
  m.write(nativeManager,poolSnapshot);m.write(0x4b7660,rngSnapshot);m.heap=heap;sounds.length=0;m.view(title,0x5e18).fill(0);m.view(progress,0x2d200).fill(0);if(scenario.extra)m.u32(progress+0x468c,1);
  m.u32(title+0x10,banks.get(16));m.u32(title+0x14,banks.get(17));m.u32(0x4b2b28,2);m.u32(0x4b77f8,8);m.u32(0x4b59c8,scenario.mode??0);m.i32(0x4b5a14,73);m.u32(0x4b5a00,1);
  const manager=c.anm_manager_create();
  for(const[id,bytes]of assets){const p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.anm_manager_load(manager,id,p,bytes.length),1,string(c,c.anm_manager_error(manager)));c.deallocate(p);}c.anm_manager_fallback(manager,0);
  const fixture=c.title_main_create(manager),field=k=>c.title_main_field(fixture,k),dv=()=>new DataView(c.memory.buffer);
  dv().setInt32(field(7)+4,73,true);dv().setUint32(field(7),scenario.mode??0,true);dv().setInt32(field(8),2,true);dv().setInt32(field(8)+4,8,true);
  if(scenario.returning){dv().setUint32(field(4),0,true);dv().setInt32(field(1)+4,14,true);}
  for(const[k,at,n]of [[0,0x24,0xd8],[1,0x18,8],[2,0x20,4],[3,0x2ac,20],[4,0x5cf4,4],[9,0x5cf8,0xd8]])m.write(title+at,memory(c,field(k),n));
  const compare=label=>{
   for(const[k,at,n]of [[0,0x24,0xd8],[1,0x18,8],[2,0x20,4],[3,0x2ac,20],[4,0x5cf4,4],[5,0x2c0,335*4],[6,0x7fc,4],[9,0x5cf8,0xd8]])assert.deepEqual(Buffer.from(memory(c,field(k),n)),Buffer.from(m.bytes(title+at,n)),label+' title field '+k);
   assert.equal(dv().getUint32(field(7),true),m.u32(0x4b59c8),label+' mode');assert.equal(dv().getInt32(field(7)+4,true),m.i32(0x4b5a14),label+' spell');assert.equal(dv().getInt32(field(7)+8,true),m.i32(0x4b5a00),label+' difficulty');
   assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.title_main_sounds(fixture),c.title_main_sound_count(fixture))),sounds,label+' sound requests');
   for(let alternate=0;alternate<2;alternate++){const instances=nativeInstances(m,nativeManager,alternate);assert.equal(c.anm_manager_count(manager,alternate),instances.length,label+' animation count');for(let i=0;i<instances.length;i++){const handle=m.u32(instances[i]+0x538);assert.equal(c.anm_manager_order(manager,alternate,i),handle,label+' animation order');compareAnm(c,m,c.anm_manager_find(manager,handle),instances[i],label+' instance '+handle);checks++;}}
   assert.deepEqual(Buffer.from(memory(c,c.anm_manager_random(manager),8)),Buffer.from(m.bytes(0x4b7660,8)),label+' visual RNG');
  };
  const step=(pressed=0,repeated=0)=>{const label='case '+scenarios+' frame '+frames;assert.equal(c.title_main_step(fixture,pressed,repeated,scenario.extra,1),1,label+': '+string(c,c.title_main_error(fixture)));m.u32(0x4b323c,pressed);m.u32(0x4b3238,repeated);m.call(0x451eb0,{ecx:title});m.call(0x402ab0,{ecx:title+0x2ac});compare(label+' menu');for(let alternate=0;alternate<2;alternate++){assert.equal(c.anm_manager_update(manager,alternate,1),1,string(c,c.anm_manager_error(manager)));m.call(alternate?0x475940:0x475830,{ecx:nativeManager});}compare(label+' animations');frames++;};
  try{
   for(let i=0;i<140;i++)step();
   assert.equal(dv().getInt32(field(2),true),2);
   for(let i=0;i<10&&dv().getInt32(field(0),true)!==scenario.selected;i++)step(0,32);
   assert.equal(dv().getInt32(field(0),true),scenario.selected);
   if(scenario.cancel){step(2);step(2);}else step(1);
   for(let i=0;i<21;i++)step();
   assert.notEqual(dv().getInt32(field(1),true),1,'destination entered');scenarios++;
  }finally{c.title_main_delete(fixture);c.anm_manager_delete(manager);}
 }
 report('title-main-menu',{passed:true,scenarios,frames,animationComparisons:checks,originalFunctions:['0x451eb0','0x450c60','0x450cc0','0x450f70','0x446150','0x44f840'],scope:'Real title ANM scripts and every main-menu destination, entrance timing, locked Extra, cursor history, return paths, sound requests and visual RNG. Destination screens, audio output and complete gameplay require separate validation.'});
 }finally{m.close();}
});
