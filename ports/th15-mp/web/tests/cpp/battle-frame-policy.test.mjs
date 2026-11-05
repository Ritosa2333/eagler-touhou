import test from 'node:test';import assert from 'node:assert/strict';import {core,oracle,report} from './helpers.mjs';
test('TH15 battle frame gates preserve native enemy, bullet and animation pause combinations',async()=>{
 const c=await core(),m=await oracle(),game=m.allocate(0x100),manager=m.allocate(0x141f0e0),bullet=m.allocate(0x1494),node=m.allocate(12),fake=m.allocate(16);m.u32(0x4e9a94,game);m.u32(0x4ca620,0x4e73e8);m.f32(0x4e73e8,1);let enemyCalls=0,motionCalls=0,animationCalls=0,checks=0;
 m.replace(0x426ad0,'observe enemy frame body',()=>{enemyCalls++;return 1;});m.replace(0x419390,'observe ordinary bullet motion body',()=>{motionCalls++;return 0;});m.replace(0x487740,'observe main animation body',()=>{animationCalls++;return 1;});
 try{for(let sample=0;sample<8192;sample++){const flags=((sample&4095)|((Math.imul(sample+1,1999999973)&0x7ffff000)>>>0))>>>0;m.u32(game+0x90,flags);enemyCalls=motionCalls=animationCalls=0;
  m.call(0x426bd0,{ecx:fake});assert.equal(c.battle_frame_policy(flags,0),enemyCalls,'enemy gate '+flags.toString(16));
  m.view(bullet,0x1494).fill(0);m.u32(bullet+0x20,0x200);m.view(bullet+0xc8a,2).set([1,0]);m.u32(bullet+0x1478,1);m.u32(node,bullet);m.u32(node+4,0);m.u32(node+8,0);m.u32(manager+0x6c,node);m.u32(manager+0x40,99);m.call(0x41a5b0,{ecx:manager});const actualMode=m.u32(manager+0x40)===99?0:motionCalls?1:2;assert.equal(c.battle_frame_policy(flags,1),actualMode,'bullet gate '+flags.toString(16));if(actualMode)assert.equal(m.i32(bullet+0x146c),1,'visual age ticks even with disabled motion');
  m.call(0x487b10,{ecx:fake});assert.equal(c.battle_frame_policy(flags,2),animationCalls,'animation gate '+flags.toString(16));checks+=3;
 }
 report('battle-frame-policy',{passed:true,cases:8192,checks,originalFunctions:['0x426bd0','0x41a5b0','0x41a000','0x487b10'],scope:'Unchanged native callback wrappers and full bullet manager traversal compare independent enemy/bullet/animation gates. Body callbacks are observed boundaries. Bullet flags 1/4 skip the complete traversal; bit 0x400 retains visual collection and age while skipping motion; bit 2 alone does not freeze ordinary bullets. Complete stage pause/menu state remains unverified.'});
 }finally{m.close();}
});
