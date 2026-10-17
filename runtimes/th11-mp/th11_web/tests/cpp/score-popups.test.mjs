import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {animationOracle} from './anm-oracle.mjs';
test('TH11 score popup ring, motion and digit presentation match original callbacks',async()=>{
 const c=await core(),m=await oracle(),o=animationOracle(m),file=o.load('ascii.anm').original,f=c.popup_create(),n=m.allocate(0x78c),player=m.allocate(0x900),position=m.allocate(12);
 m.u32(0x4a8ec0,n);m.u32(0x4a8eb4,player);m.u32(n+16,file);m.reg('ESI',n+24);m.call(0x401fd0);m.reg('EAX',n+24);m.u32(n+0x3c8,file);m.call(0x44ab40,{ecx:196,edx:file});m.u32(0x4c3c10,0);
 let draws=[],checks=0,digits=0;const sprites=m.u32(file+0x118);
 m.replace(0x44fe30,'popup quad geometry boundary',()=>0,2);
 m.replace(0x44f880,'popup draw boundary',()=>{const x=m.f32(n+0x400),y=m.f32(n+0x404);if(Number.isFinite(x))draws.push([(m.u32(n+0x3c4)-sprites)/72,x,y,m.u32(n+0x38c)]);return 0;},1);
 const compare=()=>{const a=Buffer.from(memory(c,c.popup_data(f),13*64)),b=Buffer.from(m.bytes(n+0x44c,13*64));for(let i=0;i<13;++i){a.writeUInt32LE(0,i*64+0x28);b.writeUInt32LE(0,i*64+0x28);}assert.deepEqual(a,b);++checks;};
 try{for(let frame=0;frame<480;++frame){const rate=[1,.5,.99,1.01,1.25][Math.floor(frame/96)],x=Math.fround(Math.sin(frame)*175),y=Math.fround(250+Math.cos(frame)*100);
  if(frame<300&&frame%3===0){for(let j=0;j<frame%5+1;++j){const v=[0,-1,7,90,12345,1234567890][(frame+j)%6],color=[0xffffffff,0xff00ff00,0xffffff00,0xffffff40][j%4];c.popup_add(f,v,x,y,color);m.f32(position,x);m.f32(position+4,y);m.reg('EAX',v);m.call(0x438440,{args:[position,color]});}compare();}
  m.f32(0x4a7948,rate);c.popup_tick(f,rate);m.reg('EAX',n);m.call(0x438020);compare();
  const px=Math.fround(x+frame%180),py=Math.fround(y+frame%100);m.f32(player+0x87c,px);m.f32(player+0x880,py);draws=[];m.call(0x4380d0,{args:[n]});const count=c.popup_glyphs(f,px,py),p=c.popup_glyph_data(f),dv=new DataView(c.memory.buffer),actual=Array.from({length:count},(_,i)=>[dv.getInt32(p+i*16,true),dv.getFloat32(p+i*16+4,true),dv.getFloat32(p+i*16+8,true),dv.getUint32(p+i*16+12,true)]);
  assert.deepEqual(actual,draws,`frame ${frame}`);digits+=count;
 }report('score-popups',{passed:true,checks,digits,scope:'Original 438440/438020/4380d0 ring overwrite, signed digits, lifetime, fractional rates, positions, atlas phases and distance alpha; GPU submission is covered by shared glyph rendering.'});
 }finally{c.popup_delete(f);m.close();}
});
