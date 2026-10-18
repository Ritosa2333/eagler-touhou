import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {oracle,core,memory,report,root,bits} from './helpers.mjs';
import {installAnm,normalizedAnm} from './anm-oracle.mjs';
test('ASCII glyph layout and score popups match original positions, lifetimes, digits and proximity opacity',async()=>{
 const m=await oracle(),c=await core(),raw=readFileSync(resolve(root,'reference/assets/ascii.anm')),data=c.allocate(raw.length),p=c.allocate(12),str=c.allocate(64),f=c.overlay_fixture();memory(c,data,raw.length).set(raw);
 const anm=installAnm(m,raw,1),manager=m.allocate(0xe0b0),pos=m.allocate(12),text=m.allocate(64),players=[m.allocate(0x1ba0),m.allocate(0x1ba0)],arg=i=>m.u32(m.reg('ESP')+4+i*4);let checks=0,drawn=[],views=[];
 m.view(manager,0xe0b0).fill(0);m.u32(manager+0x8288,anm.file);m.call(0x4343e0,{ecx:manager});assert.equal(c.overlay_initialize(f,data,raw.length),1);
 m.replace(0x43a950,'ascii-sprite',()=>{drawn.push(m.bytes(arg(0),676));return 0;},1);
 m.replace(0x401390,'ascii-view',()=>{const side=arg(0);views.push(100+side);m.u32(0x4b3448,0x4b3178+side*0xf0);return 0;},1);
 for(const a of [0x4396a0,0x42ff00])m.replace(a,'ascii-state',()=>0);m.replace(0x42ff40,'ascii-depth',()=>0,2);m.replace(0x401460,'ascii-mode',()=>1);
 function cvm(bytes){return normalizedAnm(bytes,c.overlay_resource(f,0),c.overlay_resource(f,1),c.overlay_resource(f,2));}
 const nvm=bytes=>normalizedAnm(bytes,anm.file,anm.source,anm.sprites);
 function compareVMs(label){for(let n=0;n<2;++n)assert.deepEqual(cvm(memory(c,c.overlay_part(f,n),676)),nvm(m.bytes(manager+n*676,676)),label+' vm'+n);++checks;}
 function compareDraw(label){assert.equal(c.overlay_drawing_count(f),drawn.length,label+' draw count');for(let n=0;n<drawn.length;++n)assert.deepEqual(cvm(memory(c,c.overlay_drawings(f)+n*676,676)),nvm(drawn[n]),label+' draw'+n);assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.overlay_events(f),c.overlay_event_count(f))),views,label+' view');++checks;}
 const putpos=(x,y,z=0)=>{const v=Uint8Array.from(new Uint8Array(new Float32Array([x,y,z]).buffer));memory(c,p,12).set(v);m.write(pos,v);};
 try{
  compareVMs('initialize');
  for(let n=0;n<220;++n){putpos(n%130-65,n%300+.25,.3);const value=[-1,0,15,1780,3456789,2147483647][n%6],side=n%2,color=(0xff000000|n*7919)>>>0;c.overlay_popup(f,side,p,value,color);m.call(0x4346a0,{ecx:manager,args:[side,pos,value,color]});assert.deepEqual(memory(c,c.overlay_part(f,2),11200),m.bytes(manager+0xb240,11200));assert.equal(c.overlay_value(f,0),m.u32(manager+0x8290));++checks;}
  const frame=c.overlay_part(f,4);m.u32(0x4b36d4,0);m.u32(0x4a7ecc,0);m.u32(0x4a7ec4,0);m.f32(0x4a80e0,-144);m.f32(0x4a80e4,0);
  for(let side=0;side<2;++side){m.u32(0x4a7d94+side*56,players[side]);m.u32(0x4b3178+side*240+204,16+side*320);m.u32(0x4b3178+side*240+208,16);}
  for(let n=0;n<100;++n){const rate=n%3===0?.5:1,flags=n%19===2?0x800:0,fields=[n%13===3?1:0,n%7===3?1:0];const v=new DataView(c.memory.buffer);v.setFloat32(frame,rate,true);v.setUint32(frame+8,flags,true);v.setUint32(frame+12,fields[0],true);v.setUint32(frame+16,fields[1],true);m.f32(0x4b36b8,rate);m.u32(0x4a7ec4,flags);m.u32(0x4a7dc4,fields[0]);m.u32(0x4a7dfc,fields[1]);c.overlay_popup_update(f);m.call(0x435b00,{ecx:manager});assert.deepEqual(memory(c,c.overlay_part(f,2),11200),m.bytes(manager+0xb240,11200),'popup frame '+n);++checks;
   for(let side=0;side<2;++side){putpos(n*2-100,side*50+100);m.write(players[side]+0x1b88,m.bytes(pos,12));drawn=[];views=[];c.overlay_popup_draw(f,side,p);m.call(0x401390,{ecx:0x4b3100,args:[side]});m.call(0x4353f0,{ecx:manager,args:[side]});compareDraw('popup '+n+'/'+side);compareVMs('popup draw');}
  }
  for(let n=0;n<270;++n){const s=Buffer.from((n%2?'STAGE 9 12:34\n  12345':'Score: 1234567890  *!')+'\0');putpos(n%20+24,n%100,0);memory(c,str,s.length).set(s);m.write(text,s);const color=(0xff000000|n*10077)>>>0,x=n%2?.75:1,y=n%3?.9:1,view=n%4===0?1:0;c.overlay_text(f,p,str,color,x,y,view);m.u32(manager+0x8268,color);m.f32(manager+0x826c,x);m.f32(manager+0x8270,y);m.u32(manager+0x8274,view);m.call(0x4342a0,{ecx:manager,args:[pos,text]});}
  assert.deepEqual(memory(c,c.overlay_part(f,3),24576),m.bytes(manager+0x2264,24576));assert.equal(c.overlay_value(f,1),256);drawn=[];views=[];c.overlay_text_draw(f);m.call(0x434540,{ecx:manager});compareDraw('text');compareVMs('text');report('ascii',{checks,texts:256,popupCreations:220,popupFrames:100,scope:'Original ascii.anm, complete glyph VMs, queue overflow limit, text layout, popup reuse/lifetime, digit order, distance opacity and ordered draws; final GPU rasterization is outside this test.'});
 }finally{c.overlay_delete(f);for(const a of [data,p,str])c.release(a);m.close();}
});
