import test from 'node:test';
import assert from 'node:assert/strict';
import {oracle,core,memory,report,bits} from './helpers.mjs';
test('Screen fades, flashes and shakes preserve original timing, RNG and rectangles',async()=>{
 const m=await oracle(),c=await core(),f=c.screen_fixture(),request=c.allocate(28),native=m.allocate(56),manager=m.allocate(64);
 const r=c.screen_part(f,0),context=c.screen_part(f,1),offsets=c.screen_part(f,2),fields=Array.from({length:10},(_,i)=>c.screen_field(i));
 let full=false,rectangles=[],checks=0,draws=0;
 m.replace(0x4396a0,'screen-flush',()=>0);m.replace(0x401390,'screen-view',()=>{full=true;return 0;},1);
 m.replace(0x422960,'screen-rectangle',()=>{rectangles.push([...m.readWords(m.reg('ECX'),4),m.reg('EDX'),Number(full)]);return 0;});
 m.u32(0x4dc550,manager);
 const word=(at,value)=>new DataView(c.memory.buffer).setUint32(at,value>>>0,true);
 const fn=[0x4222c0,0x422660,0x422470,0x4225b0,0x422470,0x4224f0,0x4224f0,0x4227a0],draw=[0x422b70,0,0x422bd0,0x422cd0,0x422b70,0x422c10,0x422c50,0];
 try{for(let scenario=0;scenario<72;++scenario){
  const kind=scenario%8,side=scenario%2,rate=[1,.5,.99][Math.floor(scenario/8)%3],duration=kind===7?16:19,color=kind===1?16:kind===3?3:kind===7?12:0xabcdef,secondary=kind===3?0x80214365:kind===7?6:0;
  const values=[kind,duration,color,secondary,kind===7?7:0,35,side];memory(c,request,28).set(new Uint8Array(new Int32Array(values).buffer));const handle=c.screen_create(f,request),p=c.screen_effect(f,handle);
  m.view(native,56).fill(0);m.u32(native,side);m.u32(native+4,kind);m.u32(native+24,duration);m.u32(native+28,color);m.u32(native+32,secondary);m.u32(native+36,values[4]);m.call(0x401500,{ecx:native+44,args:[0]});
  m.u32(0x4ace0c,0x7351+scenario);m.u32(0x4ace10,0);memory(c,r,8).set(m.bytes(0x4ace0c,8));memory(c,offsets,32).fill(0);m.view(manager+28,8).fill(0);for(let s=0;s<3;++s)m.view(0x4b3260+s*0xf0,8).fill(0);
  for(let frame=0;frame<145;++frame){
   const frozen=frame%13===8?1:0,flags=frame%17===12?0x1800:0,paused=frame%9===5,gameOver=frame%11===6,transition=scenario>=48&&frame>8?2:0,active=frame+2;
   word(context+fields[3],bits(rate));memory(c,context+fields[3]+4,1)[0]=scenario%3===1?1:0;word(context+fields[4],flags);for(let s=0;s<3;++s)word(context+fields[5]+s*4,s===side?frozen:0);word(context+fields[6],active);word(context+fields[7],transition);memory(c,context+fields[8],1)[0]=Number(paused);memory(c,context+fields[9],1)[0]=Number(gameOver);
   m.f32(0x4b36b8,rate);m.u32(0x4b36d4,scenario%3===1?32:0);m.u32(0x4a7ec4,flags);for(let s=0;s<3;++s)m.u32(0x4a7dc4+s*56,s===side?frozen:0);m.u32(0x4ac884,transition);m.u32(0x4a80d8,active);m.write(0x4a7ecc,[Number(paused),Number(gameOver)]);
   if((kind===5||kind===6)&&frame===70){memory(c,p+fields[2],1)[0]=1;m.u32(native+40,1);c.timer_reset(p+fields[0],0);m.call(0x401500,{ecx:native+44,args:[0]});}
   const live=c.screen_step(f,handle),expected=m.call(fn[kind],{ecx:native});assert.equal(live,expected,`alive ${scenario}/${frame}`);
   assert.deepEqual(memory(c,p+fields[0],12),m.bytes(native+44,12),'time');assert.equal(new DataView(c.memory.buffer).getInt32(p+fields[1],true),m.i32(native+20),'opacity '+scenario+'/'+frame);assert.equal(new DataView(c.memory.buffer).getUint32(p+8,true),m.u32(native+28),'flash repeats');assert.deepEqual(memory(c,r,8),m.bytes(0x4ace0c,8),'shake RNG');
   for(let s=0;s<4;++s)assert.deepEqual(memory(c,offsets+s*8,8),m.bytes(s===3?manager+28:0x4b3260+s*0xf0,8),'shake offset');++checks;
   if(draw[kind]){rectangles=[];full=false;c.screen_draw(f,handle);m.call(draw[kind],{ecx:native});assert.deepEqual(Array.from(new Uint32Array(c.memory.buffer,c.screen_rectangles(f),c.screen_rectangle_count(f)*6)),rectangles.flat(),'screen rectangles');++draws;}
   if(!live)break;
  }
 }report('screen-effects',{checks,draws,scenarios:72,scope:'All eight original screen effect modes, timing and pause/transition gates, opacity, repeat/release phases, shake RNG and camera offsets, and exact rectangle payloads. Final rasterization is a platform boundary.'});
 }finally{c.screen_delete(f);c.release(request);m.close();}
});
