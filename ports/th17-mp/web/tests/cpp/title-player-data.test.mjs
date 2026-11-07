import test from 'node:test';import assert from 'node:assert/strict';import{readFileSync}from'node:fs';import{resolve}from'node:path';
import{core,oracle,memory,string,root,report,enableNativeMath}from'./helpers.mjs';import{resourceLoader,nativePool,prepareBank,nativeInstances,compareAnm}from'./anm-oracle.mjs';
const bytes=(b)=>Buffer.from(b.subarray(0,b.indexOf(0)<0?b.length:b.indexOf(0)));
test('TH17 Player Data preserves native three-way entry, nine loadouts, rank and spell pages, bitmap requests and actual animations',async()=>{
 const c=await core(),m=await oracle({memoryEnd:0x20000000,heapStart:0x8000000,heapLimit:0x1f000000});enableNativeMath(m);const loader=resourceLoader(m),base=m.heap;let frames=0,checks=0,textChecks=0,sounds=[],texts=[],pressed=0,repeated=0;
 m.replace(0x465450,'Player Data sound output',()=>{sounds.push(m.i32(m.reg('ESP')+4));return 0;},2);
 m.replace(0x401e40,'keyboard device state',()=>0,1);
 const format=(p,arg)=>{return bytes(m.bytes(p,1024)).toString('latin1').replace(/%%|%[0-9]*[sd]/g,token=>{if(token==='%%')return'%';let out;if(token.at(-1)==='s')out=bytes(m.bytes(m.u32(arg),1024)).toString('latin1');else out=String(m.i32(arg));arg+=4;return out.padStart(Number(token.slice(1,-1)||0),' ');});};
 m.replace(0x475290,'Player Data centered bitmap text',()=>{const sp=m.reg('ESP'),vm=m.u32(sp+8);texts.push({handle:m.u32(vm+0x538),color:m.u32(sp+12),font:m.i32(sp+20),centered:1,bytes:Buffer.from(format(m.u32(sp+28),sp+32),'latin1')});return 0;});
 try{for(let scenario=0;scenario<8;scenario++){
  m.heap=base;m.view(m.heap,0x1c90b00).fill(0);const a=c.anm_manager_create(),native=nativePool(m),owner=m.allocate(0x5e18),ascii=m.allocate(0x19270),scores=m.allocate(0x2d600),t=c.title_main_create(a),data=c.title_data_create(t),raw=c.allocate(10*0x4820+0x4b0);const field=k=>c.title_main_field(t,k),dv=()=>new DataView(c.memory.buffer);
  try{
   m.view(owner,0x5e18).fill(0);m.view(scores,0x2d600).fill(0);m.u32(0x4b7678,ascii);m.u32(0x4b77dc,scores);m.u32(0x4b7660,0);m.u32(0x4b7664,0);m.u32(0x4a0430,0x4b5918);m.f32(0x4b5918,1);m.i32(0x4b5a00,1);m.i32(owner+0x18,10);m.i32(owner+0x1c,1);m.i32(owner+0x5cf4,2);m.i32(owner+0x2c,10);m.i32(owner+0x24,5);m.call(0x402970,{ecx:owner+0x24});m.i32(owner+0x24,scenario%2);m.i32(owner+0x104,5);m.i32(owner+0xfc,1);
   for(const[k,at,n]of[[0,0x24,0xd8],[1,0x18,8],[2,0x20,4],[3,0x2ac,20]])memory(c,field(k),n).set(m.bytes(owner+at,n));dv().setInt32(c.title_data_field(data,0),1,true);dv().setInt32(c.title_data_field(data,0)+8,5,true);
   const records=Buffer.alloc(10*0x4820+0x4b0);for(let loadout=0;loadout<10;loadout++)for(let id=0;id<101;id++){
    const at=loadout*0x4820+0x8d0+id*0x9c;Buffer.from('Spell-'+id).copy(records,at);records.writeInt32LE((id+loadout)%3,at+0x80);records.writeInt32LE((id+loadout)%4,at+0x84);records.writeInt32LE(scenario<2?0:scenario<4?1:id%3?11:0,at+0x88);records.writeInt32LE(7,at+0x8c);
   }memory(c,raw,records.length).set(records);assert.equal(c.title_data_import(data,raw,raw+10*0x4820),1);m.write(scores+8,records.subarray(0,10*0x4820));m.write(scores+0x2d148,records.subarray(10*0x4820));
   for(const[name,id]of[['ascii.anm',0],['text.anm',6],['title.anm',16]]){const b=readFileSync(resolve(root,'reference/assets',name)),p=c.allocate(b.length);memory(c,p,b.length).set(b);assert.equal(c.anm_manager_load(a,id,p,b.length),1);c.deallocate(p);const bank=prepareBank(m,loader,name,b,native,id);if(id===0){m.u32(ascii+0x19248,bank);assert.equal(c.anm_manager_fallback(a,0),1);}else if(id===6)m.u32(0x4b6208,bank);else m.u32(owner+0x10,bank);}
   const compareQueue=label=>{for(let alt=0;alt<2;alt++){const list=nativeInstances(m,native,alt);assert.equal(c.anm_manager_count(a,alt),list.length,label+' count');for(let i=0;i<list.length;i++){const h=m.u32(list[i]+0x538);assert.equal(c.anm_manager_order(a,alt,i),h,label+' queue');compareAnm(c,m,c.anm_manager_find(a,h),list[i],label+' VM '+h);checks++;}}};
   const step=(p=0,r=0)=>{
    pressed=p;repeated=r;const label=scenario+'/'+frames,screen=m.i32(owner+0x18);m.u32(0x4b323c,p);m.u32(0x4b3238,r);sounds=[];texts=[];assert.equal(m.call(screen===10?0x457b40:0x457e40,{ecx:owner}),1);assert.equal(c.title_data_update(data,p,r,0),1,label+' '+string(c,c.title_data_error(data,0)));
    assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.title_data_events(data),c.title_data_event_count(data))),sounds,label+' sounds');assert.equal(c.title_data_text_count(data),texts.length,label+' text count');for(let i=0;i<texts.length;i++){for(let k=0;k<4;k++)assert.equal(c.title_data_text_value(data,i,k)>>>0,[texts[i].handle,texts[i].color,texts[i].font,texts[i].centered][k]>>>0,label+' text field');assert.deepEqual(bytes(memory(c,c.title_data_text_bytes(data,i),1024)),texts[i].bytes,label+' text bytes '+i);textChecks++;}
    for(const[k,at,n]of[[0,0x24,0xd8],[1,0x18,8],[2,0x20,4],[3,0x2ac,20],[5,0x2c0,335*4]])assert.deepEqual(Buffer.from(memory(c,field(k),n)),Buffer.from(m.bytes(owner+at,n)),label+' state '+k);
    if(screen===11){for(const[k,at,n]of[[0,0xfc,0xd8],[1,0x1d4,0xd8],[2,0x5df0,4]])assert.deepEqual(Buffer.from(memory(c,c.title_data_field(data,k),n)),Buffer.from(m.bytes(owner+at,n)),label+' data field '+k);}
    compareQueue(label+' pre-update');m.call(0x402ab0,{ecx:owner+0x2ac});c.title_tick(t,1);for(let alt=0;alt<2;alt++){assert.equal(c.anm_manager_update(a,alt,1),1,string(c,c.anm_manager_error(a)));m.call(alt?0x475940:0x475830,{ecx:native});}compareQueue(label+' post-update');frames++;return m.i32(owner+0x18);
   };
   for(let i=0;i<10;i++)step();step(16);step(32);step(1);for(let i=0;i<27;i++)step();
   for(let i=0;i<9;i++){step(128);if(scenario%2)step(1);step(32);step(32);if(scenario%2)for(let n=0;n<4;n++)step(1);step(16);step(64);}
   step(0x102);for(let i=0;i<10;i++)if(step()!==11)break;step();step(0x102);for(let i=0;i<10;i++)if(step()!==10)break;
  }finally{c.deallocate(raw);c.title_data_delete(data);c.title_main_delete(t);c.anm_manager_delete(a);}
 }
 report('title-player-data',{passed:true,scenarios:8,frames,animationChecks:checks,textChecks,originalFunctions:['0x457b40','0x457e40','0x4587f0'],scope:'Actual original title/text/ascii animation queues and cursor history; three-way entry and return, score versus spell pages, all nine loadouts and five ranks, no/mixed/all seen spells and exact CP932 centered bitmap requests. Raster hardware, physical keyboard and audio output are isolated boundaries.'});
 }finally{m.close();}
});
