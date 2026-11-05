import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,root,memory,report} from './helpers.mjs';
import {installAnm,normalizedAnm} from './anm-oracle.mjs';
test('All fourteen original endings and staff roll match C++ script and animation execution',async()=>{
 const m=await oracle(),c=await core(),name=c.allocate(128),data=c.allocate(16000000),files=new Map(),native=m.allocate(0x2ab8),arg=i=>m.u32(m.reg('ESP')+4+i*4);
 const source=n=>{n=n.split('/').at(-1);if(!files.has(n))files.set(n,readFileSync(resolve(root,'reference/assets',n)));return files.get(n);};
 const textAnm=installAnm(m,source('text.anm'),0),staffAnm=installAnm(m,source('staff01.anm'),11),manager=m.allocate(0x12900);m.view(manager,0x12900).fill(0);m.u32(manager+0x38,textAnm.file);m.u32(manager+11*0x120+0x38,staffAnm.file);m.u32(0x4dc550,manager);m.u32(0x4b36cc,textAnm.file);m.u32(0x4b36d4,0);m.f32(0x4b36b8,1);
 const thread=m.allocate(256);m.view(thread,256).fill(0);m.u32(thread+0x64,m.u32(0x4a6eec));m.replace(0x47faf0,'ending-CRT-thread',()=>thread);
 const fields=Array.from({length:c.ending_field_count()},(_,i)=>Array.from(new Uint32Array(c.memory.buffer,c.ending_fields()+i*12,3)));
 let events=[],text=[],f=0,checks=0,frames=0,draws=0,lines=0,staffRuns=0;
 const string=p=>{const values=[];for(let n=0;n<2048;++n){const b=m.view(p+n,1)[0];if(!b)return values;values.push(b);}throw Error('string bounds');};
 const event=(a,b=0)=>events.push(a,b);
 m.replace(0x42c970,'ending-read',()=>{const bytes=source(m.string(m.reg('ECX'),128)),p=m.allocate(bytes.length);m.write(p,bytes);return p;},1);
 m.replace(0x43cae0,'ending-picture',()=>{event(0);text.push(...string(arg(1)),0);return 0;},2);
 m.replace(0x43bdc0,'ending-text',()=>{event(3,(arg(1)-native-20)/676);event(4,arg(2)|0);text.push(...string(arg(4)),0);++lines;return 0;});
 m.replace(0x42fd30,'ending-stop-music',()=>{event(1,-1);return 0;});m.replace(0x42fc20,'ending-prepare-music',()=>{event(1,arg(0));return 0;},1);m.replace(0x431930,'ending-play-music',()=>{event(1,arg(0));return 0;},2);m.replace(0x42fe20,'ending-fade-music',()=>{event(2,m.f32(m.reg('ESP')+4));return 0;},1);
 m.replace(0x43ca30,'ending-release-staff',()=>0,1);m.replace(0x43cf90,'ending-load-staff',()=>staffAnm.file,2);
 m.replace(0x43c0d0,'ending-draw-background',()=>{event(5,arg(3)|0);event(6,arg(4)|0);return 0;},7);
 m.replace(0x43ab50,'ending-draw-animation',()=>{event(7,(arg(0)-native-20)/676);return 0;},1);m.replace(0x422960,'ending-draw-cover',()=>{event(8,m.reg('EDX')|0);return 0;});
 const error=()=>{const b=memory(c,c.ending_error(f),512);return new TextDecoder().decode(b.subarray(0,b.indexOf(0)));};
 // The original retains an interrupt-return pointer from text.anm when a
 // subtitle VM is reused for staff01.anm. Normalize that pointer by its owner.
 function normalized(bytes,slot,nativeSide){
  const anm=slot===11?staffAnm:textAnm,bases=nativeSide?[anm.file,anm.source,anm.sprites]:[0,1,2].map(i=>c.ending_resource(f,slot,i));
  const out=normalizedAnm(bytes,...bases),v=new DataView(out.buffer,out.byteOffset,out.byteLength),pointer=new DataView(bytes.buffer,bytes.byteOffset,bytes.byteLength).getUint32(0x234,true);
  if(pointer)for(const [id,a,n] of [[0,textAnm,'text.anm'],[11,staffAnm,'staff01.anm']]){const base=nativeSide?a.source:c.ending_resource(f,id,1);if(base&&pointer>=base&&pointer<base+source(n).length){v.setUint32(0x234,(id<<24)+pointer-base+1,true);break;}}
  return out;
 }
 function compare(label){assert.equal(error(),'');const state=c.ending_part(f,0);for(const [nativeOffset,ownOffset,size] of fields)assert.deepEqual(memory(c,state+ownOffset,size),m.bytes(native+nativeOffset,size),label+' field '+nativeOffset.toString(16));assert.equal(c.ending_value(f,0),m.u32(native+0x2ab4)-m.u32(native+0x2a54),label+' cursor');const vms=c.ending_part(f,1);for(let n=0;n<16;++n){const p=native+20+n*676,slot=m.u32(p+0x204)===staffAnm.file?11:0,anm=slot===11?staffAnm:textAnm;const own=normalized(memory(c,vms+n*676,676),slot,false),ref=normalized(m.bytes(p,676),slot,true);assert.deepEqual(own,ref,label+' ANM '+n+' differences '+Array.from(own,(_,j)=>own[j]===ref[j]?'':j.toString(16)+':'+own[j]+'/'+ref[j]).filter(Boolean).join(','));}assert.deepEqual(Array.from(new Int32Array(c.memory.buffer,c.ending_events(f),c.ending_event_count(f))),events,label+' events');assert.deepEqual(memory(c,c.ending_text(f),c.ending_text_size(f)),Uint8Array.from(text),label+' text');++checks;}
 try{
  for(let character=0;character<14;++character){if(f)c.ending_delete(f);f=c.ending_create();for(const n of ['text.anm','staff01.anm','endstaff.end',`end${String(character).padStart(2,'0')}.end`]){const bytes=source(n);memory(c,name,128).fill(0);memory(c,name,128).set(new TextEncoder().encode(n));memory(c,data,bytes.length).set(bytes);c.ending_file(f,name,data,bytes.length);}
   m.f32(0x4b36b8,1);m.call(0x40ef60,{ecx:native});for(let n=0;n<15;++n){const vm=native+20+n*676;m.call(0x401560,{ecx:textAnm.file,args:[vm,14+n]});m.f32(vm+0x208,64);m.f32(vm+0x20c,400+n*16);m.f32(vm+0x210,0);}m.u32(native+0x2a58,1);const filename=m.allocate(64);m.write(filename,new TextEncoder().encode(`end${String(character).padStart(2,'0')}.end\0`));m.call(0x40e4f0,{ecx:native,args:[filename]});assert.equal(c.ending_initialize(f,character),1,error());events=[];text=[];compare('initialize '+character);
   let completed=false,wasStaff=false;
   for(let frame=0;frame<16000;++frame){const held=frame<180?0:0x100|(frame%11===0?1:0),pressed=frame%17===0?1:0,rate=character%3===0?.5:1;m.f32(0x4b36b8,rate);const input=new Uint8Array(88);new DataView(input.buffer).setUint16(0,held,true);new DataView(input.buffer).setUint16(6,pressed,true);m.write(0x4acf34,input);events=[];text=[];const original=m.call(0x40efe0,{ecx:native,limit:10000000}),actual=c.ending_step(f,held,pressed,rate);assert.equal(error(),'');assert.equal(actual,original,`ending ${character}/${frame} result`);compare(`ending ${character}/${frame}`);++frames;if(!wasStaff&&c.ending_value(f,2)){wasStaff=true;++staffRuns;}if(!actual){assert.equal(c.ending_value(f,1),1);completed=true;break;}if(frame%113===0){events=[];text=[];m.call(0x40e480,{ecx:native});c.ending_draw(f);compare(`draw ${character}/${frame}`);++draws;}}
   assert.ok(completed,'Ending completed '+character);assert.ok(wasStaff,'Staff roll reached '+character);
  }
  report('endings',{checks,frames,draws,lines,endings:14,staffRuns,scope:'All original .end text/scripts and staff01 animations, waits/skips, page timing, background cropping offsets, fades, music commands, text bytes, drawing order. Glyph rasterization, image copy and actual audio remain platform boundaries.'});
 }finally{if(f)c.ending_delete(f);c.release(name);c.release(data);m.close();}
});
