import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
test('TH11 quad vertices and screen UVs match original CPU drawing routines',async()=>{
 const c=await core(),m=await oracle(),p=c.allocate(0x434),out=c.allocate(48),uv=c.allocate(8),q=m.allocate(0x434),nativeOut=m.allocate(48),nativeUv=m.allocate(8);let checks=0,state=123456789;
 const random=()=>{state=(Math.imul(state,1664525)+1013904223)>>>0;return state/4294967296;};
 const offsets=[0x2c,0x3c,0x40,0x4c,0x50,0x3dc,0x3e0,0x3e4,0x3e8,0x3ec,0x3f0,0x3f4,0x3f8,0x3fc];
 try{for(let mode=0;mode<4;++mode)for(let ax=0;ax<3;++ax)for(let ay=0;ay<3;++ay)for(let n=0;n<64;++n){
  const b=Buffer.alloc(0x434);for(const off of offsets)b.writeFloatLE((random()-.5)*(off===0x2c?6.28:off===0x3c||off===0x40?5:640),off);b.writeUInt32LE((mode<<22)|(ax<<18)|(ay<<20),0x404);
  memory(c,p,b.length).set(b);m.write(q,b);memory(c,out,48).fill(0);m.view(nativeOut,48).fill(0);assert.equal(c.anm_quad(p,out),1);m.reg('EAX',q);m.call(0x452010,{ecx:nativeOut});
  const a=Buffer.from(memory(c,out,48)),expected=Buffer.from(m.bytes(nativeOut,48));for(let i=0;i<12;++i)assert.equal(a.readUInt32LE(i*4),expected.readUInt32LE(i*4),`mode=${mode} anchor=${ax},${ay} sample=${n} coordinate=${i}: ${a.readFloatLE(i*4)} vs ${expected.readFloatLE(i*4)}`);++checks;
  for(let i=0;i<4;++i){c.anm_screen_uv(out+i*12,uv);m.reg('EAX',nativeOut+i*12);m.call(0x44b470,{ecx:nativeUv});assert.deepEqual(Buffer.from(memory(c,uv,8)),Buffer.from(m.bytes(nativeUv,8)));++checks;}
 }report('anm-geometry',{passed:true,checks,scope:'quad modes 0-3, nine anchor combinations, random scale/rotation/position, screen UV'});
 }finally{m.close();c.release(p);c.release(out);c.release(uv);}
});
test('TH11 ripple initialization and animated vertices match original',async()=>{
 const c=await core(),m=await oracle(),p=c.allocate(0x434),data=c.allocate(0x4b0),rng=c.allocate(8),q=m.allocate(0x434),heap=m.heap;let checks=0;
 try{for(const seed of [1,12345,65535]){
  m.heap=heap;const b=Buffer.alloc(0x434);b.writeFloatLE(111.317,0x3e8);b.writeFloatLE(-91.751,0x3ec);b.writeFloatLE(.17,0x3f0);b.writeFloatLE(-61.91,0x3dc);b.writeFloatLE(23.73,0x3e0);b.writeFloatLE(.04,0x3e4);b.writeUInt32LE(0x89aabbcc,0x374);
  memory(c,p,b.length).set(b);m.write(q,b);memory(c,data,0x4b0).fill(0);memory(c,rng,8).fill(0);new DataView(c.memory.buffer).setUint16(rng,seed,true);m.view(0x4c2f00,8).fill(0);m.u32(0x4c2f00,seed);
  m.view(heap,0x4b0).fill(0);c.anm_ripple_init(p,data,rng);m.reg('EBX',q);m.call(0x4520f0);const native=m.u32(q+0x400);
  for(let frame=0;frame<240;++frame){
   const a=Buffer.from(memory(c,data,0x4b0)),b2=Buffer.from(m.bytes(native,0x4b0));for(let i=0;i<a.length;i+=4)assert.equal(a.readUInt32LE(i),b2.readUInt32LE(i),`seed=${seed} frame=${frame} word=${i/4} ${a.readFloatLE(i)} vs ${b2.readFloatLE(i)}`);
   assert.deepEqual(Buffer.from(memory(c,rng,8)),Buffer.from(m.bytes(0x4c2f00,8)));++checks;
   c.anm_ripple_tick(p);m.call(0x452420,{ecx:q});
  }
 }report('anm-ripple',{passed:true,checks});
 }finally{m.close();c.release(p);c.release(data);c.release(rng);}
});
test('TH11 ring and arc vertices match original animation updates',async()=>{
 const c=await core(),m=await oracle(),p=c.allocate(0x434),out=c.allocate(4096),q=m.allocate(0x434),nativeOut=m.allocate(4096),code=m.allocate(16);let checks=0,state=987654321;
 const random=()=>{state=(Math.imul(state,1664525)+1013904223)>>>0;return state/4294967296;};
 const ins=Buffer.alloc(16);ins.writeUInt16LE(8,2);ins.writeInt16LE(32000,4);ins.writeInt16LE(-1,8);ins.writeUInt16LE(8,10);m.write(code,ins);m.f32(0x4a7948,1);
 try{for(const mode of [9,13])for(const count of [3,7,16,33])for(let n=0;n<64;++n){
  const b=Buffer.alloc(0x434);for(const off of [0x24,0x2c,0x3c,0x40,0x54,0x58,0x70,0x78,0x3dc,0x3e0,0x3e4,0x3e8,0x3ec,0x3f0,0x3f4,0x3f8,0x3fc])b.writeFloatLE((random()-.5)*(off===0x24||off===0x2c?6.28:128),off);
  b.writeFloatLE(random(),0x54);b.writeFloatLE(random(),0x58);
  b.writeUInt32LE((mode<<22)|(n&1?0x8000:0),0x404);b.writeUInt32LE(0x89abcdef,0x374);b.writeUInt32LE(0x76543210,0x378);b.writeInt32LE(count,0x3b4);b.writeInt32LE(n%5,0x3b8);
  memory(c,p,b.length).set(b);m.write(q,b);m.u32(q+0x400,nativeOut);m.u32(q+0x3a4,code);m.u32(q+0x3a8,code);m.u32(q+0x68,0x4a7948);memory(c,out,4096).fill(0);m.view(nativeOut,4096).fill(0);
  assert.equal(c.anm_ring(p,out,128),1);assert.equal(m.call(0x44b4b0,{args:[q]}),0);
  const a=Buffer.from(memory(c,out,count*56)),b2=Buffer.from(m.bytes(nativeOut,count*56));
  for(let i=0;i<a.length;i+=4)assert.equal(a.readUInt32LE(i),b2.readUInt32LE(i),`mode=${mode} count=${count} sample=${n} word=${i/4} ${a.readFloatLE(i)} vs ${b2.readFloatLE(i)}`);++checks;
 }report('anm-ring',{passed:true,checks});
 }finally{m.close();c.release(p);c.release(out);}
});
