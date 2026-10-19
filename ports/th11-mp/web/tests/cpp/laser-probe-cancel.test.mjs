import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {firstDifference} from './shot-oracle.mjs';
test('TH11 laser probes and infinite full cancellation match original',async()=>{
 const c=await core(),m=await oracle(),f=c.ll_create(),p=c.li_data(f,0),center=c.allocate(12),q=m.allocate(0xeb0),nc=m.allocate(12);let seed=172413,events=[],probeChecks=0,cancelChecks=0,eventChecks=0;
 const random=()=>((seed=(Math.imul(seed,1664525)+1013904223)>>>0)/4294967296),position=a=>[m.f32(a),m.f32(a+4),m.f32(a+8)];
 m.replace(0x424230,'full cancel reward boundary',()=>{events.push([5,8,...position(m.reg('EAX'))]);return 0;},3);
 m.replace(0x455b10,'full cancel effect boundary',()=>{events.push([4,m.i32(m.reg('ESP')+12),...position(m.reg('EAX'))]);return 0;},4);
 const manager=m.allocate(0x480);m.u32(0x4a8e94,manager);m.u32(manager+0x474,1);
 try{
  for(let sample=0;sample<16384;++sample){
   const b=Buffer.alloc(0xeb0),hit=Buffer.alloc(12),radius=[0,1,8,16,32,-1][sample%6],angle=[0,Math.PI/2,-Math.PI/2,Math.PI,.375,-.97][sample%6];
   const length=[0,8,16,16.0001,24,32,64,100,240,500,1000,4096][sample%12],width=[0,2,8,32,64,-1][(sample>>3)%6],x=Math.fround((random()-.5)*600),y=Math.fround((random()-.1)*600);
   b.writeFloatLE(x,0x3c);b.writeFloatLE(y,0x40);b.writeFloatLE(.125,0x44);b.writeFloatLE(angle,0x54);b.writeFloatLE(length,0x58);b.writeFloatLE(width,0x5c);b.writeUInt32LE(sample%7===0?8:0,0x490);b.writeInt16LE(sample%16,0x48e);
   const dx=[-radius,0,length/2,length,length+radius,random()*(length+100)-50][(sample>>2)%6],dy=[0,width/2,-width/2,width/2+radius,-width/2-radius,(random()-.5)*100][(sample>>5)%6];
   hit.writeFloatLE(x+dx*Math.cos(Math.fround(angle))-dy*Math.sin(Math.fround(angle)));hit.writeFloatLE(y+dx*Math.sin(Math.fround(angle))+dy*Math.cos(Math.fround(angle)),4);
   memory(c,p,b.length).set(b);m.write(q,b);memory(c,center,12).set(hit);m.write(nc,hit);const bits=Buffer.alloc(4);bits.writeFloatLE(radius);m.resetThreadFPU();
   for(const address of[0x426fd0,0x4284f0]){assert.equal(c.laser_probe(p,center,radius),m.call(address,{ecx:q,args:[nc,bits.readUInt32LE()]}),`probe ${sample} ${address.toString(16)}`);++probeChecks;}
   if(sample<4096){
    events=[];c.laser_fixture_count(f,2);const reward=(sample>>4)&1,skip=(sample>>8)&1;
    assert.equal(c.li_cancel(f,reward,skip),m.call(0x428270,{ecx:q,args:[reward,skip]}),`cancel ${sample} count`);
    const a=Buffer.from(memory(c,p,0xeb0)),expected=Buffer.from(m.bytes(q,0xeb0));assert.ok(a.equals(expected),`cancel ${sample} `+firstDifference(a,expected));
    const ep=c.laser_fixture_data(f,3),n=c.laser_fixture_count(f,0),v=new DataView(c.memory.buffer),actual=Array.from({length:n},(_,i)=>[v.getInt32(ep+i*20,true),v.getInt32(ep+i*20+4,true),...Array.from({length:3},(_,j)=>v.getFloat32(ep+i*20+8+j*4,true))]);assert.deepEqual(actual,events,`cancel ${sample} events`);eventChecks+=n;++cancelChecks;
   }
  }
  report('laser-probe-cancel',{passed:true,probeChecks,cancelChecks,eventChecks,scope:'Original 426fd0/4284f0 rotated probes including boundaries and negative radii; 428270 full infinite cancellation including protection, strict sampling, offscreen effect/reward gating and state changes. Effect/item creation remains a callback boundary.'});
 }finally{c.ll_delete(f);c.release(center);m.close();}
});
