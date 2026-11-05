import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
const timers=[0x14,0x28,...Array.from({length:18},(_,i)=>0x70+i*52),0x424];
test('TH11 line laser transforms, virtual motion and cancellation match original',async()=>{
 const c=await core(),m=await oracle(),f=c.laser_fixture_create(),p=c.laser_fixture_data(f,0),rate=c.laser_fixture_data(f,1),player=c.laser_fixture_data(f,2),q=m.allocate(0xe8c),np=m.allocate(0x900),manager=m.allocate(0x480);
 m.u32(0x4a8eb4,np);m.u32(0x4a8e94,manager);m.u32(manager+0x474,0x123456);let events=[],emissions=[],seed=81732,checks=0,transformChecks=0,motionChecks=0,cancelChecks=0;
 const random=()=>((seed=(Math.imul(seed,1664525)+1013904223)>>>0)/4294967296),position=a=>[m.f32(a),m.f32(a+4),m.f32(a+8)];
 m.replace(0x46a720,'memset',()=>{const sp=m.reg('ESP'),p=m.u32(sp+4);m.view(p,m.u32(sp+12)).fill(m.u32(sp+8)&255);return p;});
 m.replace(0x44a1e0,'centered laser transform sound',()=>{events.push([0,m.reg('ESI')|0,0,0,0]);return 0;});
 m.replace(0x44a260,'positional laser transform sound',()=>{events.push([1,m.reg('EDI')|0,m.f32(m.reg('ESP')+4),0,0]);return 0;},1);
 m.replace(0x424df0,'reflected laser creation boundary',()=>{assert.equal(m.u32(m.reg('ESP')+4),0);emissions.push(Buffer.from(m.bytes(m.reg('EDI'),0x1e4)));events.push([2,0,0,0,0]);return 0;},1);
 m.replace(0x455b10,'laser cancellation effect boundary',()=>{events.push([4,m.i32(m.reg('ESP')+12),...position(m.reg('EAX'))]);return 0;},4);
 m.replace(0x424230,'laser reward boundary',()=>{events.push([5,m.reg('ECX'),...position(m.reg('EAX'))]);return 0;},3);
 const initial=example=>{
  const b=Buffer.alloc(0xe8c);for(let i=0xc;i<0x624;i+=4)b.writeFloatLE((random()-.5)*10,i);
  b.fill(0,0x46c,0x61c);b.writeInt32LE(0,0x418);b.writeUInt32LE(0,0x41c);b.writeInt32LE(example%3?-1:7,0x620);b.writeInt32LE(0,0x438);
  b.writeFloatLE((random()-.5)*600,0x3c);b.writeFloatLE((random()-.25)*700,0x40);b.writeFloatLE(random()*200,0x58);b.writeFloatLE(random()*30,0x5c);b.writeInt16LE(example%16,0x466);
  for(const o of timers){const cur=example%31-3;b.writeInt32LE(cur-1,o);b.writeInt32LE(cur,o+4);b.writeFloatLE(cur+(example%3)*.25,o+8);b.writeUInt32LE(0x4a7948,o+12);b.writeUInt32LE(1,o+16);}
  for(let i=0;i<18;++i){const o=0x70+i*52;b.writeInt32LE(2+example%21,o+40);b.writeInt32LE(example%16,o+44);b.writeInt32LE(example%16,o+48);}
  return b;
 };
 const install=(b,example)=>{
  events=[];emissions=[];c.laser_fixture_count(f,2);m.write(q,b);const a=Buffer.from(b);for(const t of timers)if(a.readUInt32LE(t+12)===0x4a7948)a.writeUInt32LE(rate,t+12);memory(c,p,a.length).set(a);
  const r=[1,.5,1.01,1.5,.99,0][example%6];m.f32(0x4a7948,r);new DataView(c.memory.buffer).setFloat32(rate,r,true);
  const pos=Buffer.alloc(12);pos.writeFloatLE((random()-.5)*300);pos.writeFloatLE(random()*448,4);memory(c,player,12).set(pos);m.write(np+0x87c,pos);
 };
 const compare=label=>{
  const a=Buffer.from(memory(c,p,0x624)),b=Buffer.from(m.bytes(q,0x624));for(const t of timers)if(a.readUInt32LE(t+12)===rate)a.writeUInt32LE(0x4a7948,t+12);
  if(!a.equals(b)){let off=0;while(a[off]===b[off])++off;off&=~3;assert.fail(`${label} +0x${off.toString(16)} cpp=${a.readFloatLE(off)} native=${b.readFloatLE(off)} bits=${a.readUInt32LE(off).toString(16)}/${b.readUInt32LE(off).toString(16)}`);}
  const count=c.laser_fixture_count(f,0),ep=c.laser_fixture_data(f,3),v=new DataView(c.memory.buffer),actual=Array.from({length:count},(_,i)=>[v.getInt32(ep+i*20,true),v.getInt32(ep+i*20+4,true),...Array.from({length:3},(_,j)=>v.getFloat32(ep+i*20+8+j*4,true))]);assert.deepEqual(actual,events,label+' events');
  assert.equal(c.laser_fixture_count(f,1),emissions.length);const em=c.laser_fixture_data(f,4);for(let i=0;i<emissions.length;++i)assert.deepEqual(Buffer.from(memory(c,em+i*0x1e4,0x1e4)),emissions[i],label+' emission');++checks;
 };
 try{
  install(Buffer.alloc(0xe8c),0);m.reg('ESI',q);m.call(0x424830);c.laser_initialize(f);compare('constructor');
  const ops=[1,4,8,0x10,0x20,0x40,0x100,0x200,0x400,0x1000,0x2000,0x4000,0x20000,0x40000,0x200000,0x400000,0x800000,0x80000000,0x12345];
  for(const op of ops)for(let example=0;example<256;++example){const b=initial(example),i=example%3,o=0x46c+i*24;
   b.writeInt32LE(i,0x418);b.writeFloatLE([-1000,-999,-990,-2.2,0,1.7,990,999,1000][example%9],o);b.writeFloatLE([-1000,-999,-990,-1.5,0,.3,990,999][example%8],o+4);b.writeInt32LE(example%20-1,o+8);b.writeInt32LE(example%18,o+12);b.writeUInt32LE(op,o+16);b.writeInt32LE(example%2,o+20);
   if(op===0x400000)b.writeInt32LE(17,o+8);if(example%7===0)b.writeUInt32LE(0x100,0x41c);
   b.writeUInt32LE(0x200,o+40);b.writeInt32LE(31,o+32);b.writeInt32LE(example%2,o+44);
   install(b,example);m.resetThreadFPU();m.call(0x425370,{ecx:q});assert.equal(c.laser_transforms(f),0);compare(`transform ${op.toString(16)}/${example}`);++transformChecks;
  }
  for(const [kind,address]of [[1,0x424790],[4,0x425bd0],[8,0x4247b0],[0x10,0x425ad0],[0x20,0x4247e0],[0x40,0x4247d0],[0x100,0x425850],[0x20000,0x424800],[0x40000,0x424810],[0x800000,0x424820]])for(let example=0;example<384;++example){const b=initial(example);b.writeUInt32LE(0xffffffff,0x41c);install(b,example);
   for(let frame=0;frame<3;++frame){m.resetThreadFPU();m.call(address,{ecx:q});assert.equal(c.laser_motion(f,kind),0);compare(`motion ${kind.toString(16)}/${example}/${frame}`);++motionChecks;}
  }
  for(let example=0;example<2048;++example){const b=initial(example);b.writeFloatLE([0,8,16,16.0001,24,32,32.0001,40,48,64,100,500,1000][example%13],0x58);b.writeInt32LE(example%3===0?1:0,0x438);install(b,example);m.resetThreadFPU();const reward=example&1,skip=(example>>1)&1,expected=m.call(0x426df0,{ecx:q,args:[reward,skip]});assert.equal(c.laser_cancel(f,reward,skip),expected);compare(`cancel ${example}`);++cancelChecks;}
  report('laser-state',{passed:true,checks,transformChecks,motionChecks,cancelChecks,scope:'Original line-laser base constructor, transform dispatcher (excluding animation replacement), all ten virtual motion slots, reflected spawn parameter payloads, cancellation segment positions and reward/effect ordering. ANM, frame, manager, partial cuts and infinite lasers are separate.'});
 }finally{c.laser_fixture_delete(f);m.close();}
});
