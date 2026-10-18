import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
const timers=[0x464,0x478,...Array.from({length:11},(_,i)=>0x680+i*52),0x8f0];
const functions=new Map([[1,0x40a280],[4,0x40a300],[8,0x40a420],[0x10,0x40a4c0],[0x20,0x40a6d0],[0x40,0x40a5d0],[0x100,0x40a930],[0x20000,0x40aa50],[0x40000,0x40aaf0],[0x800000,0x40aba0],[0x2000000,0x40ac60],[0x8000000,0x40adc0]]);
test('TH11 projectile transforms and frame motion match original state and effects',async()=>{
 const c=await core(),m=await oracle(),f=c.bullet_fixture_create(),p=c.bullet_fixture_data(f,0),rate=c.bullet_fixture_data(f,1),player=c.bullet_fixture_data(f,2),sprite=c.bullet_fixture_data(f,3),q=m.allocate(0x910),np=m.allocate(0x900),ns=m.allocate(0x40);
 m.u32(0x4a8eb4,np);let seed=8702,checks=0,transformChecks=0,motionChecks=0,events=[];
 const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed/4294967296;};
 m.replace(0x44a1e0,'centered transform sound',()=>{events.push([0,m.reg('ESI')|0,0,0]);return 0;});
 m.replace(0x44a260,'positional transform sound',()=>{events.push([1,m.reg('EDI')|0,0,m.f32(m.reg('ESP')+4)]);return 0;},1);
 m.replace(0x40ae90,'transform cancel callback',()=>{events.push([2,0,0,0]);return 0;});
 let emitted=[];m.replace(0x40a1f0,'nested emitter callback',()=>{emitted.push(Buffer.from(m.bytes(m.reg('ESI'),0x214)));events.push([3,0,0,0]);return 0;});
 const initial=example=>{
  const b=Buffer.alloc(0x910);for(let o=0x43c;o<0x910;o+=4)b.writeFloatLE((random()-.5)*8,o);
  b.fill(0,0x4d0,0x680);b.writeInt32LE(0,0x4c8);b.writeInt32LE(example%3===0?-1:example%17,0x4c4);b.writeUInt32LE(0,0x4a8);b.writeUInt32LE(0xa1b2c3d4,0x40c);
  b.writeFloatLE((random()-.5)*600,0x43c);b.writeFloatLE((random()-.25)*700,0x440);
  for(let i=0;i<11;++i){const o=0x680+i*52,cur=example%31-3;b.writeInt32LE(cur-1,o);b.writeInt32LE(cur,o+4);b.writeFloatLE(cur+(example%3)*.25,o+8);b.writeUInt32LE(0x4a7948,o+12);b.writeUInt32LE(1,o+16);b.writeInt32LE(2+example%21,o+40);b.writeInt32LE(example%16,o+44);b.writeInt32LE(example%4,o+48);}
  for(const t of[0x464,0x478,0x8f0]){b.writeInt32LE(-1,t);b.writeInt32LE(0,t+4);b.writeFloatLE(0,t+8);b.writeUInt32LE(0x4a7948,t+12);b.writeUInt32LE(1,t+16);}
  b.writeInt32LE(1+example%20,0x904);b.writeInt32LE(example%18,0x908);b.writeInt32LE(example%4,0x778);
  b.writeUInt32LE(ns,0x3b4);return b;
 };
 const install=(b,example)=>{
  events=[];emitted=[];c.bullet_fixture_events(f,2);m.write(q,b);const a=Buffer.from(b);a.writeUInt32LE(sprite,0x3b4);
  for(const t of timers)if(a.readUInt32LE(t+12)===0x4a7948)a.writeUInt32LE(rate,t+12);memory(c,p,a.length).set(a);
  const r=[1,.5,1.01,1.5,.99,0][example%6];m.f32(0x4a7948,r);new DataView(c.memory.buffer).setFloat32(rate,r,true);
  const pos=Buffer.alloc(12);pos.writeFloatLE(example%5===0?b.readFloatLE(0x43c):(random()-.5)*300);pos.writeFloatLE(example%5===0?b.readFloatLE(0x440):random()*448,4);memory(c,player,12).set(pos);m.write(np+0x87c,pos);
  const w=4+example%41,h=5+example%53;m.f32(ns+0x38,w);m.f32(ns+0x34,h);const cv=new DataView(c.memory.buffer);cv.setFloat32(sprite+16,w,true);cv.setFloat32(sprite+20,h,true);
 };
 const compare=label=>{
  const a=Buffer.from(memory(c,p,0x910)),b=Buffer.from(m.bytes(q,0x910));a.writeUInt32LE(ns,0x3b4);for(const t of timers)if(a.readUInt32LE(t+12)===rate)a.writeUInt32LE(0x4a7948,t+12);
  if(!a.equals(b)){let off=0;while(a[off]===b[off])++off;off&=~3;assert.fail(`${label} +0x${off.toString(16)} cpp=${a.readFloatLE(off)} native=${b.readFloatLE(off)} bits=${a.readUInt32LE(off).toString(16)}/${b.readUInt32LE(off).toString(16)}`);}
  const count=c.bullet_fixture_events(f,0),ep=c.bullet_fixture_data(f,4),v=new DataView(c.memory.buffer),actual=Array.from({length:count},(_,i)=>[v.getInt32(ep+i*16,true),v.getInt32(ep+i*16+4,true),v.getInt32(ep+i*16+8,true),v.getFloat32(ep+i*16+12,true)]);assert.deepEqual(actual,events,label+' events');
  assert.equal(c.bullet_fixture_events(f,1),emitted.length);const em=c.bullet_fixture_data(f,5);for(let i=0;i<emitted.length;++i)assert.deepEqual(Buffer.from(memory(c,em+i*0x214,0x214)),emitted[i],label+' emitter');++checks;
 };
 try{
  const ops=[1,4,8,0x10,0x20,0x40,0x100,0x200,0x400,0x1000,0x2000,0x4000,0x20000,0x40000,0x80000,0x200000,0x400000,0x800000,0x2000000,0x4000000,0x8000000,0x10000000,0x80000000,0x12345];
  for(const op of ops)for(let example=0;example<256;++example){const b=initial(example),i=example%3,o=0x4d0+i*24;
   b.writeInt32LE(i,0x4c8);b.writeFloatLE([-1000,-999,-990,-2.2,0,1.7,990,999,1000][example%9],o);b.writeFloatLE([-1000,-999,-990,-1.5,0,.3,990,999][example%8],o+4);b.writeInt32LE(example%20,o+8);b.writeInt32LE(example%18|(example%2?0x100:0),o+12);b.writeUInt32LE(op,o+16);b.writeInt32LE(example%2,o+20);
   if(op===0x400000)b.writeInt32LE(17,o+8);if(op===0x80000){b.writeUInt32LE(((example%9<<24)|(example%29<<16)|(example%16<<8)|3|(example%2?0x80000000:0))>>>0,o+8);b.writeFloatLE(2.25,o+24);b.writeFloatLE(1.75,o+28);b.writeInt32LE(3,o+32);b.writeInt32LE(128,o+36);}
   if(example%7===0)b.writeUInt32LE(0x100,0x4a8);
   // A following concurrent command tests continuation and barrier handling.
   const end=o+(op===0x80000?48:24);b.writeUInt32LE(0x200,end+16);b.writeInt32LE(31,end+8);b.writeInt32LE(example%2,end+20);
   install(b,example);m.resetThreadFPU();m.call(0x409700,{ecx:q});assert.equal(c.bullet_transform(f),0);compare(`transform ${op.toString(16)}/${example}`);++transformChecks;
  }
  for(const [kind,address]of functions)for(let example=0;example<384;++example){const b=initial(example);b.writeUInt32LE(0xffffffff,0x4a8);if(kind===0x2000000){b.copy(b,0x8f0,0x854,0x868);b.writeInt32LE(b.readInt32LE(0x87c),0x904);}install(b,example);
   for(let frame=0;frame<3;++frame){m.resetThreadFPU();m.reg('EAX',q);m.reg('EDI',q);try{m.call(address,{edx:q});}catch(err){throw Error(`motion ${kind.toString(16)}/${example}/${frame}: ${err.message}`);}assert.equal(c.bullet_motion(f,kind),0);compare(`motion ${kind.toString(16)}/${example}/${frame}`);++motionChecks;}
  }
  report('bullet-state',{passed:true,checks,transformChecks,motionChecks,transformTypes:24,motionHelpers:functions.size,scope:'Full projectile state, transform dispatch, nested emitter payloads and sound/cancel callback order; appearance binding and collision manager excluded'});
 }finally{c.bullet_fixture_delete(f);m.close();}
});
