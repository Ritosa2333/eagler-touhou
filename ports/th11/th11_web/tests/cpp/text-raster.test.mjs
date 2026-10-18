import test from 'node:test';import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';import {resolve} from 'node:path';
import {core,oracle,memory,root,report} from './helpers.mjs';
import {FontOracle,fontParams} from '../../scripts/native/font-oracle.mjs';

test('TH11 measured glyphs and text image match original GDI drawing and alpha processing',async()=>{
 const c=await core(),m=await oracle(),f=c.text_raster_create(),gdi=new FontOracle(1024,65),style=c.allocate(32),string=c.allocate(1024),native=m.allocate(1024*65*2),text=m.allocate(1024),rect=m.allocate(16),texture=m.allocate(4),vtable=m.allocate(128);
 const fonts=[[32,400,'ＭＳ ゴシック'],[32,600,'ＭＳ 明朝'],[15,700,'ＭＳ ゴシック'],[15,700,'ＭＳ 明朝']],decoder=new TextDecoder('shift_jis');let selected=101,drawColor=0,checks=0;
 const arg=n=>m.u32(m.reg('ESP')+n*4),dv=()=>new DataView(c.memory.buffer),put=(o,v)=>dv().setInt32(style+o,v,true);
 for(const [i,name]of ['font0.bin','font1.bin','font2.bin','font3.bin','cp932.bin','blend4444.bin'].entries()){const bytes=readFileSync(resolve(root,'assets/sdl-native/fonts',name)),p=c.allocate(bytes.length);memory(c,p,bytes.length).set(bytes);assert.equal(c.text_raster_load(f,i,p,bytes.length),1);c.release(p);}
 m.u32(texture,vtable);m.u32(vtable+0x48,m.registerImport({dll:'texture',name:'GetSurface',argc:3,handler:()=>{m.u32(arg(3),0);return 0;}}));
 for(const e of m.importMap.values()){
  if(e.name==='SelectObject'){e.argc=2;e.handler=()=>{const old=selected;selected=arg(2);if(selected>=101&&selected<=104){const [h,w,face]=fonts[selected-101];gdi.select(fontParams(h,w),face);}return old;};}
  if(e.name==='SetBkMode'){e.argc=2;e.handler=()=>1;}
  if(e.name==='SetTextColor'){e.argc=2;e.handler=()=>{drawColor=arg(2);return 0;};}
  if(e.name==='TextOutA'){e.argc=5;e.handler=()=>{const raw=m.bytes(native,1024*65*2);gdi.pixels.set(new Uint16Array(raw.buffer,raw.byteOffset,1024*65));gdi.draw(decoder.decode(m.bytes(arg(4),arg(5))),drawColor,arg(2)|0,arg(3)|0);m.write(native,new Uint8Array(gdi.pixels.buffer));return 1;};}
 }
 m.replace(0x46a720,'memset',()=>{m.view(arg(1),arg(3)).fill(arg(2)&255);return arg(1);});
 // Capture the exact source pixels; native D3DX filtering is checked separately.
 m.replace(0x45f9e6,'text surface upload',()=>0,10);
 for(const [a,v]of [[0x4a58c0,26],[0x4a58c4,1024],[0x4a58c8,64],[0x4a58d0,2048],[0x4a58cc,1024*64*2],[0x4a58e0,native],[0x4a58d4,1],[0x4c2eec,101],[0x4c2ee8,102],[0x4c0ee4,103],[0x4a8ed4,104]])m.u32(a,v);
 [0,0,384,16].forEach((v,i)=>m.i32(rect+i*4,v));
 const strings=[Buffer.from('Spell 0123456789: .%!?'),Buffer.from([0x92,0x6e,0x97,0xec,0x93,0x61,0x81,0x75,0x96,0xb2,0x91,0x7a,0x81,0x76]),Buffer.from('iWjgA / slash _'),Buffer.from([0xb1,0xb2,0xb3,0xde,0xdf])];
 try{
  for(let sample=0;sample<160;++sample){const bytes=strings[sample%4],font=(sample>>2)%4,height=sample%3===0?16:17,offset=sample%5===0?-3:sample%19,color=(sample*199969+0x1234)&0xffffff,outline=(sample*78901)&0xffffff,spacing=sample%7===0?12:0,plain=sample%6===0;
   memory(c,string,bytes.length+1).set(Buffer.concat([bytes,Buffer.from([0])]));m.write(text,Buffer.concat([bytes,Buffer.from([0])]));
   for(const [o,v]of [[0,offset],[4,height],[8,font],[12,color],[16,outline],[20,spacing],[24,+plain]])put(o,v);
   assert.equal(c.text_raster_draw(f,string,style),1);
   m.reg('EAX',font);m.reg('EBP',0);m.reg('ESI',0);if(plain)m.call(0x444890,{args:[rect,offset,height,color,text,texture]});else m.call(0x444610,{ecx:text,edx:offset,args:[rect,height,color,outline,texture,spacing]});
   const actual=Buffer.from(memory(c,c.text_raster_data(f),1024*40*2)),expected=Buffer.from(m.bytes(native,actual.length));
   if(!actual.equals(expected)){let at=0;while(actual[at]===expected[at])++at;assert.fail(`sample ${sample} font ${font} plain ${plain} at (${(at>>1)%1024},${Math.floor(at/2048)}): cpp=${actual[at]} original=${expected[at]}`);}++checks;
  }
  report('text-raster',{passed:true,checks,fonts:4,glyphs:37456,scope:'Original 444610/444890 including real GDI TextOutA, shadow, fixed spacing, negative offset, packed alpha and 443860 edge bleed. D3DX image filtering has a separate check.'});
 }finally{gdi.close();c.text_raster_delete(f);m.close();}
});
