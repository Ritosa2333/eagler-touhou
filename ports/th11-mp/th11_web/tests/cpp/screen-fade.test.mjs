import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';
test('TH11 type 2/3/5 fades match original timers, opacity and untextured rectangle',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),f=c.fade_create(),pause=m.allocate(0x100);let checks=0,draw;
 m.replace(0x456b70,'fade update registration',()=>0);m.replace(0x456c10,'fade draw registration',()=>0);
 const device=m.allocate(4),vtable=m.allocate(0x180);m.u32(device,vtable);m.u32(0x4c3288,device);m.u32(0x4a8e88,pause);
 const arg=n=>m.u32(m.reg('ESP')+n*4),hook=(off,name,argc,handler)=>m.u32(vtable+off,m.registerImport({dll:'fade',name,argc,handler}));
 hook(0x10c,'combiner',4,()=>0);hook(0xe4,'state',3,()=>0);hook(0x164,'layout',2,()=>0);hook(0x14c,'draw',5,()=>{assert.equal(arg(2),5);assert.equal(arg(3),2);assert.equal(arg(5),20);draw=Buffer.from(m.bytes(arg(4),80));return 0;});
 hook(0xbc,'viewport',2,()=>0);
 try{for(const type of[2,3,5])for(const duration of[0,1,30,-3,117])for(const rate of[0,.375,.99,1,1.01,1.25]){
  const color=0x03125476;c.fade_start(f,type,duration,color,rate);m.f32(0x4a7948,rate);
  const p=m.call(0x4489e0,{args:[type,duration,color,0,10]});
  for(let frame=0;frame<140;++frame){const stopped=frame===139,frozen=frame%11<3;m.u32(0x4c2ef4,stopped?1:0);m.u32(pause+0x60,frozen?4:0);
   const actual=c.fade_tick(f,stopped?1:0,frozen?1:0),expected=m.call(type===3?0x447f10:0x448420,{ecx:p});assert.equal(actual,expected);
   assert.deepEqual(Buffer.from(memory(c,c.fade_data(f,0),12)),Buffer.from(m.bytes(p+48,12)),`${type}/${duration}/${rate}/${frame} timer`);
   assert.deepEqual(Buffer.from(memory(c,c.fade_data(f,1),4)),Buffer.from(m.bytes(p+24,4)),`${type}/${duration}/${rate}/${frame} alpha`);
   assert.equal(c.fade_draw(f),80);m.call(type===5?0x448390:0x4484b0,{ecx:p});assert.deepEqual(Buffer.from(memory(c,c.fade_data(f,2),80)),draw);++checks;if(actual===7)break;
  }
 }report('screen-fade',{passed:true,checks,scope:'Original type 2/3/5 construction, opacity, fractional timer, stopped/paused rules and actual playfield/full-screen draw vertices.'});
 }finally{c.fade_delete(f);m.close();}
});
