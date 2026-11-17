import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';

test('Shared screen effects preserve native overlapping callbacks, pause and end-of-draw resets',async()=>{
 const c=await core(),m=await oracle(),game=m.allocate(0x100);let frames=0,draws=0,checks=0;
 m.u32(0x4a0430,0x4b5918);
 m.replace(0x46d0a0,'screen-effects fixture flush boundary',()=>0);
 const same=(p,n,label)=>{assert.deepEqual(Buffer.from(memory(c,p,8)),Buffer.from(m.bytes(n,8)),label);checks++;};
 try{
  for(let sample=0;sample<48;sample++){
   const f=c.screen_effects_create(),effects=[];
   for(const [field,address]of [[0,0x4b7668],[1,0x4b7660]])m.write(address,memory(c,c.screen_effects_field(f,field),8));
   m.view(0x4b6198,8).fill(0);m.view(0x4b5f40,8).fill(0);
   const check=label=>{for(const [field,address]of [[0,0x4b7668],[1,0x4b7660],[2,0x4b6198],[3,0x4b5f40]])same(c.screen_effects_field(f,field),address,label+' field '+field);assert.equal(c.screen_effects_count(f),effects.filter(e=>e.active).length,label+' callback count');checks++;};
   try{
    for(let frame=0;frame<180;frame++){
     if([0,2,10,13,70].includes(frame)){
      const kind=frame===2||frame===13?1:8,spec=kind===1?[8,6,6,0]:[3+sample%5,3+sample%3,40+sample%13,10],initial=c.screen_shake_create(kind,...spec),p=m.allocate(64);
      m.view(p,64).fill(0);for(let i=0;i<4;i++)m.i32(p+24+i*4,spec[i]);m.write(p+44,memory(c,c.screen_shake_field(initial,1),20));c.screen_shake_delete(initial);
      assert.equal(c.screen_effects_request(f,kind,...spec),1);effects.push({p,active:true,fn:kind===8?0x4646c0:0x4644e0});
     }
     const rate=[.5,1,1.5][sample%3],scale=[1,1.5,2][sample%3],flags=frame%17===0?1<<((frame+sample)%7):0,available=frame%19!==0,shutdown=frame===179;
     m.u32(0x4b76b0,available?game:0);m.u32(game+0x8c,flags);m.u32(0x4b780c,shutdown?1:0);m.f32(0x4b5918,rate);m.f32(0x524718,scale);
     for(const e of effects.slice().reverse())if(e.active&&m.call(e.fn,{ecx:e.p})===7)e.active=false;
     assert.ok(c.screen_effects_step(f,rate,scale,flags,available?1:0,shutdown?1:0)>=0);check(sample+'/'+frame+' update');frames++;
     // Also cover bounded catch-up: several updates can precede the next draw.
     if(frame%4!==1){assert.equal(m.call(0x4431a0),1);c.screen_effects_finish_draw(f);check(sample+'/'+frame+' draw reset');draws++;}
    }
    c.screen_effects_clear(f);assert.equal(c.screen_effects_count(f),0);for(const field of [2,3])assert.deepEqual(Array.from(memory(c,c.screen_effects_field(f,field),8)),Array(8).fill(0));
   }finally{c.screen_effects_delete(f);}
  }
  report('screen-effects-frame',{passed:true,cases:48,frames,draws,checks,originalFunctions:['0x4644e0','0x4646c0','0x4431a0'],scope:'Shared production screen-effect owner against original callbacks with overlapping bomb shakes and ECL/Reimu nudges, reverse equal-priority registration, both RNG streams, pause/unavailable/shutdown gates, three rates/scales, skipped draws and native per-draw camera reset. Actual camera binding and rasterized output are covered separately by the SDL application integration test.'});
 }finally{m.close();}
});
