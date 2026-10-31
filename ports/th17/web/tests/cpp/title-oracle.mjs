import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,string,root,enableNativeMath} from './helpers.mjs';
import {resourceLoader,nativePool,prepareBank,compareAnm,nativeInstances} from './anm-oracle.mjs';

// Original menu controller and real animation scripts. Only graphics allocation
// and sound output are fixtures; no menu, score-unlock or animation logic is stubbed.
export async function titleOracle(){
 const c=await core(),m=await oracle({memoryEnd:0x10000000,heapStart:0x08000000,heapLimit:0x0f000000});
 enableNativeMath(m);m.u32(0x4a0430,0x4b5918);m.f32(0x4b5918,1);m.view(0x4b7660,8).fill(0);
 const nativeManager=nativePool(m),loader=resourceLoader(m),banks=new Map(),assets=new Map();
 for(const [id,name]of [[0,'ascii.anm'],[16,'title.anm'],[17,'title_v.anm']]){const bytes=readFileSync(resolve(root,'reference/assets',name));assets.set(id,bytes);banks.set(id,prepareBank(m,loader,name,bytes,nativeManager,id));}
 const text=m.allocate(0x19270);m.u32(0x4b7678,text);m.u32(text+0x19248,banks.get(0));
 const title=m.allocate(0x5e18),progress=m.allocate(0x2d200);m.u32(0x4b77dc,progress);m.u32(0x4b77f0,title);
 const poolSnapshot=m.bytes(nativeManager,0x1c90b00),rngSnapshot=m.bytes(0x4b7660,8),heap=m.heap,sounds=[],events=[];let checkpoint=0,pendingStart=false;
 m.replace(0x465450,'capture title sound',()=>{sounds.push(m.u32(m.reg('ESP')+4));return 0;},2);
 m.replace(0x442140,'music output boundary',()=>{events.push(1);return 0;});
 m.replace(0x41d3d0,'loading display output boundary',()=>{events.push(2);return 0;});
 m.replace(0x41b8c0,'transition effect output boundary',()=>{events.push(3);const out=m.u32(m.reg('ESP')+4);m.u32(out,0);return out;},1);
 m.replace(0x401e40,'physical keyboard input boundary',()=>{const at=m.reg('ECX');m.view(at,256).fill(0);if(checkpoint)m.view(at+0x30+checkpoint,1)[0]=128;return 0;});
 const startHook=m.cpu.hook_add(m.uc.HOOK_MEM_WRITE,()=>{pendingStart=true;},null,0x4b61d4,0x4b61d7);m.hooks.push(startHook);
 let frames=0,animationChecks=0;
 return {c,m,get frames(){return frames;},get animationChecks(){return animationChecks;},close(){m.close();},scenario(options={}){
  m.write(nativeManager,poolSnapshot);m.write(0x4b7660,rngSnapshot);m.heap=heap;sounds.length=0;events.length=0;checkpoint=options.checkpoint??0;m.view(title,0x5e18).fill(0);m.view(progress,0x2d200).fill(0);
  m.u32(title+0x10,banks.get(16));m.u32(title+0x14,banks.get(17));m.u32(0x4b2b28,options.difficulty??1);m.u32(0x4b77f8,options.character??0);m.u32(0x4b77f4,options.animal??0);m.u32(0x4b2b38,0);m.u32(0x4b77ec,options.reason??1);
  m.u32(0x4b59c8,0);m.i32(0x4b5a14,-1);m.u32(0x4b5a00,1);m.u32(0x4b59f4,0);m.u32(0x4b59f8,0);m.u32(0x4b59dc,1);m.u32(0x4b59cc,0);
  const manager=c.anm_manager_create();
  for(const[id,bytes]of assets){const p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.anm_manager_load(manager,id,p,bytes.length),1,string(c,c.anm_manager_error(manager)));c.deallocate(p);}c.anm_manager_fallback(manager,0);
  const fixture=c.title_main_create(manager),field=k=>c.title_main_field(fixture,k),dv=()=>new DataView(c.memory.buffer),get=(k,offset=0)=>dv().getInt32(field(k)+offset,true),set=(k,value,offset=0)=>dv().setInt32(field(k)+offset,value,true);
  set(8,options.difficulty??1);set(8,options.character??0,4);set(8,options.animal??0,8);set(11,options.reason??1);
  c.title_checkpoint(fixture,checkpoint);
  for(let variant=0;variant<9;variant++)for(let difficulty=0;difficulty<5;difficulty++){const value=options.clear?.(variant,difficulty)??1;set(10,value,(variant*5+difficulty)*4);m.i32(progress+0x468c+variant*0x4820+difficulty*4,value);}
  for(let variant=0;variant<9;variant++)for(let difficulty=0;difficulty<5;difficulty++)for(let stage=0;stage<8;stage++){const value=options.stageAvailable?.(variant,difficulty,stage)??1;memory(c,field(10)+180+(variant*5+difficulty)*8+stage,1)[0]=value;m.view(progress+0x46ad+variant*0x4820+difficulty*64+stage*8,1)[0]=value;}
  for(const[k,at,n]of [[0,0x24,0xd8],[1,0x18,8],[2,0x20,4],[3,0x2ac,20],[4,0x5cf4,4],[9,0x5cf8,0xd8]])m.write(title+at,memory(c,field(k),n));
  const compare=label=>{
   for(const[k,at,n]of [[0,0x24,0xd8],[1,0x18,8],[2,0x20,4],[3,0x2ac,20],[4,0x5cf4,4],[5,0x2c0,335*4],[6,0x7fc,4],[9,0x5cf8,0xd8]])assert.deepEqual(Buffer.from(memory(c,field(k),n)),Buffer.from(m.bytes(title+at,n)),label+' title field '+k);
   for(const[co,at]of [[0,0x4b59c8],[4,0x4b5a14],[8,0x4b5a00],[12,0x4b59f4],[16,0x4b59f8],[20,0x4b59dc],[24,0x4b59cc]])assert.equal(get(7,co),m.i32(at),label+' selection '+co);
   for(const[co,at]of [[0,0x4b2b28],[4,0x4b77f8],[8,0x4b77f4],[12,0x4b2b38]])assert.equal(get(8,co),m.i32(at),label+' persistent selection '+co);
   assert.equal(get(11),m.i32(0x4b77ec),label+' return reason');
   assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.title_main_sounds(fixture),c.title_main_sound_count(fixture))),sounds,label+' sound requests');
   assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.title_start_events(fixture),c.title_start_event_count(fixture))),events,label+' start services');
   for(let alternate=0;alternate<2;alternate++){const instances=nativeInstances(m,nativeManager,alternate);assert.equal(c.anm_manager_count(manager,alternate),instances.length,label+' animation count');for(let i=0;i<instances.length;i++){const handle=m.u32(instances[i]+0x538);assert.equal(c.anm_manager_order(manager,alternate,i),handle,label+' animation order');compareAnm(c,m,c.anm_manager_find(manager,handle),instances[i],label+' instance '+handle);animationChecks++;}}
   assert.deepEqual(Buffer.from(memory(c,c.anm_manager_random(manager),8)),Buffer.from(m.bytes(0x4b7660,8)),label+' visual RNG');
  };
  const step=(pressed=0,repeated=0)=>{
   const label=options.label+' frame '+frames,screen=get(1),address=new Map([[1,0x451eb0],[5,0x455500],[6,0x4559b0],[7,0x456250],[8,0x456900]]).get(screen);assert.ok(address,'unsupported oracle title screen '+screen);
   m.u32(0x4b323c,pressed);m.u32(0x4b3238,repeated);m.call(address,{ecx:title});if(pendingStart){events.push(4,m.i32(0x4b59dc));pendingStart=false;}assert.equal(c.title_selection_step(fixture,pressed,repeated),1,label+': '+string(c,c.title_selection_error(fixture)));compare(label+' menu');
   m.call(0x402ab0,{ecx:title+0x2ac});c.title_tick(fixture,1);
   for(let alternate=0;alternate<2;alternate++){assert.equal(c.anm_manager_update(manager,alternate,1),1,string(c,c.anm_manager_error(manager)));m.call(alternate?0x475940:0x475830,{ecx:nativeManager});}compare(label+' animations');frames++;return get(1);
  };
  return {get,set,step,field,fixture,manager,title,progress,ready(){for(let i=0;get(2)!==2&&i<150;i++)step();assert.equal(get(2),2,'menu ready');},transition(){const screen=get(1);for(let i=0;get(1)===screen&&i<50;i++)step();assert.notEqual(get(1),screen,'menu transitioned');},close(){c.title_main_delete(fixture);c.anm_manager_delete(manager);}};
 }};
}
