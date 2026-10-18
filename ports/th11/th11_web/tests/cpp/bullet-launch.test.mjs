import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
const bits=f=>{const b=Buffer.alloc(4);b.writeFloatLE(f);return b.readUInt32LE();};
test('TH11 bullet launch geometry and RNG match all nine original aiming modes',async()=>{
 const c=await core(),m=await oracle(),ce=c.allocate(0x214),cr=c.allocate(8),cp=c.allocate(12),co=c.allocate(28),ne=m.allocate(0x214),manager=m.allocate(0x100),bullet=m.allocate(0x910),player=m.allocate(0x900);
 m.u32(manager+0x10,bullet);m.u32(0x4a8eb4,player);m.f32(0x4a7948,1);
 // The original has finished launch geometry here; VM binding, sprite data and
 // projectile transform execution belong to subsequent integration tests.
 m.replace(0x409495,'launch geometry boundary',()=>{m.reg('ESP',m.reg('ESP')+32);return 0;},5);
 let seed=81771,checks=0,rejected=0;const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed/4294967296;};
 try{for(const mode of [-1,0,1,2,3,4,5,6,7,8,9])for(let example=0;example<768;++example){
  const e=Buffer.alloc(0x214),r=Buffer.alloc(8),p=Buffer.alloc(12),initial=Buffer.alloc(28);initial.writeFloatLE(91.25,12);initial.writeFloatLE(-23.5,16);
  for(let o=4;o<=0x20;o+=4)e.writeFloatLE((random()-.5)*(o<=12?500:o===0x20?80:7),o);
  const count=1+example%31,layers=example%19,index=example%count,layer=layers?example%layers:0,aim=Math.fround((random()-.5)*6.28);
  e.writeInt16LE(count,0x1f8);e.writeInt16LE(layers,0x1fa);e.writeInt16LE(mode,0x1fc);if(example%7===0)e.writeFloatLE(0,0x20);
  r.writeUInt16LE((random()*65536)|0);r.writeUInt32LE((random()*4294967296)>>>0,4);p.writeFloatLE(e.readFloatLE(4)+(random()-.5)*200);p.writeFloatLE(e.readFloatLE(8)+(random()-.5)*200,4);
  const exclusion=example%4===0?0:example%4===1?-1:example%4===2?900:100000;
  memory(c,ce,e.length).set(e);memory(c,cr,8).set(r);memory(c,cp,12).set(p);memory(c,co,28).set(initial);m.write(ne,e);m.write(0x4c2f00,r);m.write(player+0x87c,p);m.write(bullet,Buffer.alloc(0x910));m.write(bullet+0x448,initial.subarray(12,20));m.f32(manager+0x60,exclusion);
  m.resetThreadFPU();const expected=m.call(0x408f20,{args:[manager,ne,index,layer,bits(aim)]})|0,actual=c.bullet_launch(ce,index,layer,aim,cr,cp,exclusion,co);assert.equal(actual,expected,`result ${mode}/${example}`);
  const a=Buffer.from(memory(c,co,28)),b=Buffer.alloc(28);Buffer.from(m.bytes(bullet+0x43c,20)).copy(b);b.writeUInt32LE(m.u32(bullet+0x454),20);b.writeUInt32LE(m.u32(bullet+0x458),24);
  if(!a.equals(b)){let off=0;while(a[off]===b[off])++off;off&=~3;assert.fail(`launch ${mode}/${example} +${off} cpp=${a.readFloatLE(off)} native=${b.readFloatLE(off)} bits=${a.readUInt32LE(off).toString(16)}/${b.readUInt32LE(off).toString(16)}`);}
  assert.deepEqual(Buffer.from(memory(c,cr,8)),Buffer.from(m.bytes(0x4c2f00,8)),`RNG ${mode}/${example}`);++checks;if(actual===-1)++rejected;
 }report('bullet-launch',{passed:true,checks,rejected,aimModes:9,unknownModes:2,scope:'Original spawn prefix through position, raw-angle velocity, normalized angle, speed, exclusion and RNG; projectile VM and transform integration excluded'});
 }finally{for(const p of[ce,cr,cp,co])c.release(p);m.close();}
});
