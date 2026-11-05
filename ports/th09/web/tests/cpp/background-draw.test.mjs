import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report,bits} from './helpers.mjs';
test('Background composition matches clears, tint, fog, layer and Boss ordering',async()=>{
 const m=await oracle(),c=await core(),f=c.bg_draw_fixture(),base=c.bg_draw_base(f),bg=c.background_part(base,0),parts=Array.from({length:4},(_,i)=>c.bg_draw_part(f,i)),native=m.allocate(0x65c4),manager=m.allocate(0x12900),device=m.allocate(4),vt=m.allocate(0x160),arg=i=>m.u32(m.reg('ESP')+4+i*4);
 let dv=new DataView(c.memory.buffer),calls=[],animations=[],checks=0;const fields=Array.from({length:c.background_field_count()},(_,i)=>Array.from({length:3},(_,j)=>dv.getUint32(c.background_fields()+i*12+j*4,true))),event=(...a)=>calls.push([...a,...Array(6-a.length).fill(0)].map(n=>n>>>0));
 const index=p=>p<native+0x818?1000+(p-native-0x2c)/0x2a4:2000+(p-native-0xce0)/0x2a4;
 m.u32(0x4dc550,manager);m.u32(0x4b3108,device);m.u32(device,vt);m.u32(vt+0x90,m.registerImport({dll:'background-draw',name:'clear',argc:7,handler:()=>{event(3,arg(3),arg(4));return 0;}}));
 m.replace(0x42ff00,'fog-disable',()=>{event(0,0);return 0;});m.replace(0x42fec0,'fog-enable',()=>{event(0,1);return 0;});m.replace(0x4396a0,'flush',()=>{event(2);return 0;});
 m.replace(0x401390,'viewport',()=>{const side=arg(0);event(1,side);m.u32(0x4b3448,0x4b3178+side*0xf0);return 0;},1);
 m.replace(0x42ff40,'state',()=>{event(4,arg(0),arg(1));return 0;},2);
 m.replace(0x422960,'rectangle',()=>{const p=m.reg('ECX');event(5,...m.readWords(p,4),m.reg('EDX'));return 0;});
 m.replace(0x401640,'overlay',()=>{event(6,index(arg(0)));animations.push(...m.bytes(arg(0),0x2a4));return 0;},1);
 m.replace(0x4026c0,'models',()=>{event(7,arg(0));return 0;},1);m.replace(0x401600,'texture-mode',()=>{event(8,arg(0));return 0;},1);m.replace(0x4013e0,'camera',()=>{event(9);return 0;});
 const custom=m.registerImport({dll:'background-draw',name:'boss-custom',argc:0,handler:()=>{event(10);return 0;}});
 function writeField(off,value){const field=fields.find(a=>a[0]===off);assert.ok(field);dv.setUint32(bg+field[1],value>>>0,true);m.u32(native+off,value);}
 function compare(label){dv=new DataView(c.memory.buffer);assert.equal(c.bg_draw_count(f,0),calls.length,label+' calls');calls.forEach((e,i)=>assert.deepEqual(Array.from({length:6},(_,j)=>dv.getUint32(c.bg_draw_data(f,0)+i*24+j*4,true)),e,label+' call '+i));assert.deepEqual(memory(c,c.bg_draw_data(f,1),c.bg_draw_count(f,1)*0x2a4),Uint8Array.from(animations),label+' animations');for(const off of [0x2c,0xce0,0x642c]){const field=fields.find(a=>a[0]===off);assert.deepEqual(memory(c,bg+field[1],field[2]),m.bytes(native+off,field[2]),label+' state '+off.toString(16));}assert.equal(dv.getUint32(parts[2],true),m.u32(manager),label+' tint');assert.equal(memory(c,parts[3],1)[0],m.u32(manager+4),label+' tint enabled');assert.deepEqual([...memory(c,parts[1],3)],[m.u32(native+0xcd4),m.u32(native+0x6430),m.u32(native+0x6440)],label+' draw flags');++checks;}
 try{for(let i=0;i<768;++i){
  const side=i&1,fog=(i%7)!==0,hasCustom=i%3===0;dv=new DataView(c.memory.buffer);m.view(native,0x65c4).fill(0);m.view(manager,0x12900).fill(0);m.u32(native+12,side);m.u32(0x4b3550,fog?0:4);c.bg_draw_configure(f,side,fog,hasCustom);m.u32(native+0x6404,hasCustom?custom:0);
  for(const field of fields)memory(c,bg+field[1],field[2]).fill(0);
  writeField(0xccc,i%4);writeField(0xcd8,i%9);writeField(0xc7c,[0,0x80402010,0xff112233][i%3]);writeField(0x642c,i%5?0x806040f0:0);writeField(0xc94,bits(150+i%600));const fogField=fields.find(a=>a[0]===0xc94);dv.setFloat32(bg+fogField[1]+4,1800,true);dv.setUint32(bg+fogField[1]+8,0xd020a0f0,true);m.f32(native+0xc98,1800);m.u32(native+0xc9c,0xd020a0f0);
  [i%2,(i>>1)%2,(i>>2)%2].forEach((v,n)=>{memory(c,parts[1]+n,1)[0]=v;m.u32(native+[0xcd4,0x6430,0x6440][n],v);});
  const tint=i%3?0xff2020ff:0x80808080;dv.setUint32(parts[2],tint,true);memory(c,parts[3],1)[0]=i%2;m.u32(manager,tint);m.u32(manager+4,i%2);
  const vx=side?336:16,vy=16;dv.setUint32(parts[0],vx,true);dv.setUint32(parts[0]+4,vy,true);dv.setFloat32(parts[0]+8,-144,true);dv.setFloat32(parts[0]+12,0,true);m.u32(0x4b3178+side*0xf0+0xcc,vx);m.u32(0x4b3178+side*0xf0+0xd0,vy);m.f32(0x4a80e0,-144);m.f32(0x4a80e4,0);
  for(const [off,count] of [[0x2c,3],[0xce0,32]]){const fp=bg+fields.find(a=>a[0]===off)[1];for(let n=0;n<count;++n){const a=fp+n*0x2a4,b=native+off+n*0x2a4;dv.setInt16(a+0x214,(i+n)%4-1,true);m.write(b+0x214,new Uint8Array(new Int16Array([(i+n)%4-1]).buffer));for(let j=0;j<3;++j){const value=(n*21+i*3+j*31)%600-300;dv.setFloat32(a+0x208+j*4,value,true);dv.setFloat32(a+0x288+j*4,value/3,true);m.f32(b+0x208+j*4,value);m.f32(b+0x288+j*4,value/3);}}}
  for(let phase=0;phase<2;++phase){calls=[];animations=[];m.call(phase?0x403680:0x4033e0,{ecx:native});c.bg_draw_run(f,phase);compare(i+'/'+phase);}
 }report('background-draw',{checks,scope:'Both field composition passes, original clears, tint saturation, fog/color/depth changes, all four model layers, overlay placement and Boss background ordering. Model geometry and final draw submission are explicit boundaries covered by separate renderer tests.'});
 }finally{c.bg_draw_delete(f);m.close();}
});
