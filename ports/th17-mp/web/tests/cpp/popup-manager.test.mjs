import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,string,root,report,enableNativeMath} from './helpers.mjs';
import {resourceLoader,nativePool,prepareBank} from './anm-oracle.mjs';

test('TH17 floating numbers preserve native rotation, lifetimes, glyph order, alpha and bonus text',async()=>{
 const c=await core(),m=await oracle({memoryEnd:0x10000000,heapStart:0x08000000,heapLimit:0x0f000000});
 const a=c.anm_manager_create(),p=c.popup_create(a),position=c.allocate(12),player=c.allocate(12),owner=m.allocate(0x1038),np=m.allocate(12),pl=m.allocate(0x1000),text=m.allocate(0x19270);
 enableNativeMath(m);let expected=[],texts=[],checks=0,glyphs=0;
 try{
  const bytes=readFileSync(resolve(root,'reference/assets/ascii.anm')),input=c.allocate(bytes.length);
  memory(c,input,bytes.length).set(bytes);assert.equal(c.anm_manager_load(a,2,input,bytes.length),1);c.deallocate(input);
  assert.equal(c.popup_initialize(p,2),1,string(c,c.popup_error(p)));
  const nm=nativePool(m),loader=resourceLoader(m),bank=prepareBank(m,loader,'ascii.anm',bytes,nm,2);
  m.u32(0x4b77e0,owner);m.u32(0x4b77d0,pl);m.u32(0x4b7678,text);m.u32(0x4b5cdc,4);
  m.call(0x409510,{ecx:owner+0x18});m.u32(owner+0x34,2);m.call(0x466390,{ecx:bank,args:[owner+0x18,259]});m.u32(owner+0x54c,0x384000);
  m.replace(0x46d260,'floating number geometry boundary',()=>0,5);
  m.replace(0x46c8b0,'floating number GPU submission boundary',()=>{
   const vm=m.u32(m.reg('ESP')+4);
   expected.push({sprite:m.i32(vm+0x20),position:Buffer.from(m.bytes(vm+0x5e4,12)),color:m.u32(vm+0x520)});return 0;
  },2);
  m.replace(0x4082b0,'bonus ASCII queue boundary',()=>{
   const sp=m.reg('ESP'),format=m.string(m.u32(sp+12));
   const value=format==='BONUS %.1f'?'BONUS '+Buffer.from(m.bytes(sp+16,8)).readDoubleLE(0).toFixed(1):format==='%d'?String(m.i32(sp+16)):format;
   texts.push({position:Buffer.from(m.bytes(m.u32(sp+8),12)),style:Buffer.from(m.bytes(text+0x19214,4)),value});return 0;
  });
  for(let frame=0;frame<420;frame++){
   if(frame%2===0){
    const xyz=[Math.fround((frame%23-11)*20),Math.fround((frame%19-2)*16),Math.fround(frame%7/8)];
    xyz.forEach((v,i)=>{new DataView(c.memory.buffer).setFloat32(position+i*4,v,true);m.f32(np+i*4,v);});
    const amount=[0,-1,17,2015,2147483647,921053][frame%6],color=(0xf08020a0+frame*17)>>>0;
    c.popup_number(p,position,amount,color);m.call(0x450b70,{args:[np,amount,color]});
    assert.equal(c.popup_cursor(p),m.u32(owner+0x10));checks++;
   }
   // Exercise the five separately animated bonus slots, including fading and
   // both text forms, without substituting their update/draw implementation.
   if(frame%37===0){
    const index=13+(Math.floor(frame/37)%5),entry=Buffer.alloc(72),at=index*72;
    entry.writeFloatLE((frame%11-5)*31,12);entry.writeFloatLE(frame%99,16);
    entry.writeUInt32LE((0xc07711aa+frame*31)>>>0,28);entry.writeUInt32LE(1,48);entry[60]=1;
    entry.writeInt32LE(frame%3===0?-1:frame*512,64);entry.writeFloatLE(Math.fround(frame*.135),68);
    memory(c,c.popup_entries(p)+at,72).set(entry);m.write(owner+0x618+at,entry);
   }
   const rate=[1,.5,1.2,.25,.995][frame%5];m.f32(0x4b5918,rate);
   c.popup_update(p,rate);m.call(0x4505a0,{ecx:owner});
   assert.deepEqual(Buffer.from(memory(c,c.popup_entries(p),18*72)),Buffer.from(m.bytes(owner+0x618,18*72)),`entries ${frame}`);checks++;
   const xyz=[(frame%7-3)*63,(frame%9)*35,0];xyz.forEach((v,i)=>{new DataView(c.memory.buffer).setFloat32(player+i*4,v,true);m.f32(pl+0x610+i*4,v);});
   expected=[];texts=[];m.call(0x450700,{ecx:owner});assert.equal(c.popup_draw(p,player),1,string(c,c.popup_error(p)));
   assert.equal(c.popup_glyph_count(p),expected.length,`glyph count ${frame}`);
   expected.forEach((e,i)=>{const v=c.anm_vm_visual(c.popup_glyph(p,i)),dv=new DataView(c.memory.buffer);
    assert.equal(dv.getInt32(v+12,true),e.sprite,`sprite ${frame}/${i}`);
    assert.equal(dv.getUint32(v+96,true),e.color,`alpha ${frame}/${i}`);
    assert.deepEqual(Buffer.from(memory(c,v+340,12)),e.position,`position ${frame}/${i}`);glyphs++;checks+=3;
   });
   assert.equal(c.popup_text_count(p),texts.length,`bonus count ${frame}`);
   texts.forEach((e,i)=>{assert.equal(string(c,c.popup_text_value(p,i)),e.value);
    assert.deepEqual(Buffer.from(memory(c,c.popup_text_position(p,i),12)),e.position);
    assert.deepEqual(Buffer.from(memory(c,c.popup_text_style(p,i),4)),e.style);checks+=3;
   });
  }
  report('popup-manager',{passed:true,frames:420,glyphs,checks,originalFunctions:['0x450b70','0x4505a0','0x450700'],scope:'Native ordinary number allocation/ring and all eighteen entries frame by frame, including variable rate motion, timer rounding, bonus fading, per-digit sprite order/position/alpha and both bonus text forms. Bonus slot producers and GPU geometry are outside this comparison.'});
 }finally{c.popup_delete(p);c.anm_manager_delete(a);c.deallocate(position);c.deallocate(player);m.close();}
});
