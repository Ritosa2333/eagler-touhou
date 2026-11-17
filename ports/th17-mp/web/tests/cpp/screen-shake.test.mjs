import test from 'node:test';import assert from 'node:assert/strict';import {core,oracle,memory,report} from './helpers.mjs';
test('TH17 camera nudge and bomb shake preserve original timing, pause gates, offsets and RNG',async()=>{
 const c=await core(),m=await oracle(),native=m.allocate(64),game=m.allocate(0x100);m.u32(0x4a0430,0x4b5918);let checks=0,frames=0;
 const same=(cp,np,n,label)=>{assert.deepEqual(Buffer.from(memory(c,cp,n)),Buffer.from(m.bytes(np,n)),label);checks++;};
 try{for(const kind of [1,8])for(let sample=0;sample<128;sample++){
  const a=8+sample%19,b=sample%7+1,d=sample%13+1,e=sample%11+1,p=c.screen_shake_create(kind,a,b,d,e),rng=kind===8?0x4b7668:0x4b7660;
  m.view(native,64).fill(0);for(const [off,value]of [[24,a],[28,b],[32,d],[36,e]])m.i32(native+off,value);m.write(native+44,memory(c,c.screen_shake_field(p,1),20));m.write(rng,memory(c,c.screen_shake_field(p,0),8));m.view(0x4b6198,8).fill(0);m.view(0x4b5f40,8).fill(0);
  for(let frame=0;frame<180;frame++){const rate=[.5,1,1.5][(frame+sample)%3],scale=[1,1.5,2][sample%3],flags=frame%11===0?1<<((frame/11)%9):0,available=frame%13!==0,shutdown=frame===179;m.u32(0x4b76b0,available?game:0);m.u32(game+0x8c,flags);m.u32(0x4b780c,shutdown?1:0);m.f32(0x4b5918,rate);m.f32(0x524718,scale);const original=m.call(kind===8?0x4646c0:0x4644e0,{ecx:native});assert.equal(c.screen_shake_step(p,rate,scale,flags,available?1:0,shutdown?1:0),original);same(c.screen_shake_field(p,0),rng,8,'RNG');same(c.screen_shake_field(p,1),native+44,20,'timer');same(c.screen_shake_field(p,2),0x4b6198,8,'camera offset');same(c.screen_shake_field(p,3),0x4b5f40,8,'screen offset');frames++;}c.screen_shake_delete(p);
 }report('screen-shake',{passed:true,cases:256,frames,checks,originalFunctions:['0x4644e0','0x4646c0'],scope:'TH17 nudge and shake timings, global-rate timers, game/visual RNG selection, pause/game availability/shutdown gates, sign behavior and render-resolution scaling. Scene callback ownership and final image composition are separate integration checks.'});
 }finally{m.close();}
});
