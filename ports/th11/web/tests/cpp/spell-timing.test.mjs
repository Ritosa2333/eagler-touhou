import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';

test('TH11 spell wall clock, replay checksum and rounding match original sampler',async()=>{
 const c=await core(),m=await oracle(),p=c.spell_timing_create(),spell=m.allocate(0x920),game=m.allocate(0x100),replay=m.allocate(0x300),header=m.allocate(0x90),clock=m.allocate(8);
 m.u32(0x4a8d6c,spell);m.u32(0x4a8e88,game);m.u32(0x4a8eb8,replay);m.i32(0x4a5728,1);m.u32(replay+0x20,header);m.u32(replay+0xd8,header);
 // Only replace the external wall-clock read. Native rounding, encoding and
 // replay checksum routines execute without substitutions.
 const code=Buffer.alloc(7);code[0]=0xdd;code[1]=5;code.writeUInt32LE(clock,2);code[6]=0xc3;m.write(0x446920,code);
 const setClock=t=>{const b=Buffer.alloc(8);b.writeDoubleLE(t);m.write(clock,b);};
 const view=()=>new DataView(c.memory.buffer),value=k=>view().getInt32(c.spell_timing_data(p,k),true);
 let checks=0,seed=13493;const random=()=>seed=(Math.imul(seed,1664525)+1013904223)>>>0;
 try{for(let i=0;i<520;++i){
  const start=i%3===0?0:(random()%10000000)/1000,delta=i<20?[0,.008349999,.00835,.008350001,.0167,.999,1,59.999,60,99.99,100,999,1000,1234.5,.1,.2,.3,.4,.5,.6][i]:random()%1000000/10000;
  const flags=(i%8)<<8,frames=(random()%60000),play=i>=400,encoded=i%2?0x6ae9c24:random()|0;
  m.u32(spell+0x8e0,flags|1);m.u32(spell+0x8f0,0);m.i32(spell+0x8f4,frames);m.u32(game+0x74,+play);m.i32(header+0x3c,encoded);
  view().setUint32(c.spell_timing_data(p,3),0,true);view().setInt32(c.spell_timing_data(p,4),encoded,true);
  setClock(start);m.call(0x40c310);let cf=c.spell_timing_update(p,flags|1,frames,start,+play);assert.equal(cf,m.u32(spell+0x8e0));
  setClock(start+delta);m.u32(spell+0x8e0,cf&~1);m.call(0x40c310);cf=c.spell_timing_update(p,cf&~1,frames,start+delta,+play);
  assert.equal(cf,m.u32(spell+0x8e0),`sample${i} flags`);assert.equal(value(1),m.i32(spell+0x8f8),`sample${i} frames`);assert.equal(value(2),m.i32(spell+0x90c),`sample${i} encoded delta=${delta}`);assert.equal(value(3),m.i32(spell+0x8f0));
  if(!play)assert.equal(view().getInt32(c.spell_timing_data(p,4),true),m.i32(header+0x3c));++checks;
 }report('spell-timing',{passed:true,checks,scope:'Original 40c310 wall-clock sampler with native fmod, rounding, encoding, 20-entry record storage and replay validation; only external clock is controlled.'});
 }finally{c.spell_timing_delete(p);m.close();}
});
