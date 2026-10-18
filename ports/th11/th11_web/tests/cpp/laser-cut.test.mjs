import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {firstDifference} from './shot-oracle.mjs';
test('TH11 line and infinite laser cuts match original segments and spawns',async()=>{
 const c=await core(),m=await oracle(),f=c.ll_create(),center=c.allocate(12),size=c.allocate(12),q=m.allocate(0xeb0),nc=m.allocate(12),ns=m.allocate(12),manager=m.allocate(0x480),tail=m.allocate(0x440),vt=m.allocate(8);let p=0,stateSize=0;
 m.u32(0x4a8e94,manager);let seed=247185,events=[],emissions=[],checks=0,spawnChecks=0,eventChecks=0;
 const random=()=>((seed=(Math.imul(seed,1664525)+1013904223)>>>0)/4294967296),pos=a=>[m.f32(a),m.f32(a+4),m.f32(a+8)];
 m.replace(0x46a720,'memset',()=>{const sp=m.reg('ESP'),a=m.u32(sp+4);m.view(a,m.u32(sp+12)).fill(m.u32(sp+8)&255);return a;});
 m.replace(0x424230,'cut reward boundary',()=>{events.push([5,m.reg('ECX'),...pos(m.reg('EAX'))]);return 0;},3);
 m.replace(0x455b10,'cut effect boundary',()=>{events.push([4,m.i32(m.reg('ESP')+12),...pos(m.reg('EAX'))]);return 0;},4);
 m.replace(0x433f90,'cut player shot boundary',()=>{events.push([13,0,...pos(m.reg('ECX'))],[14,0,m.f32(0x4a3a64),0,0]);return 0;},2);
 m.replace(0x45fce4,'cut line allocation',()=>m.allocate(m.u32(m.reg('ESP')+4)));
 m.replace(0x424ea0,'cut line constructor boundary',()=>{const a=m.reg('EAX');m.view(a,0xe8c).fill(0);m.u32(a,vt);return a;});
 m.u32(vt+4,m.registerImport({dll:'laser-cut',name:'initialize split line',argc:1,handler:()=>{emissions.push(Buffer.from(m.bytes(m.u32(m.reg('ESP')+4),0x1e4)));events.push([2,0,0,0,0]);return 0;}}));
 const compare=label=>{
  const a=Buffer.from(memory(c,p,stateSize)),b=Buffer.from(m.bytes(q,stateSize));assert.ok(a.equals(b),label+' '+firstDifference(a,b));
  const v=new DataView(c.memory.buffer),ep=c.laser_fixture_data(f,3),n=c.laser_fixture_count(f,0),actual=Array.from({length:n},(_,i)=>[v.getInt32(ep+i*20,true),v.getInt32(ep+i*20+4,true),...Array.from({length:3},(_,j)=>v.getFloat32(ep+i*20+8+j*4,true))]);assert.deepEqual(actual,events,label+' events');eventChecks+=n;
  assert.equal(c.laser_fixture_count(f,1),emissions.length,label+' emission count');const em=c.laser_fixture_data(f,4);for(let i=0;i<emissions.length;++i){const a=Buffer.from(memory(c,em+i*0x1e4,0x1e4)),b=emissions[i];assert.ok(a.equals(b),label+' emission '+firstDifference(a,b));++spawnChecks;}++checks;
 };
 try{
  for(const infinite of[false,true])for(const circle of[false,true])for(let example=0;example<8192;++example){
   p=infinite?c.li_data(f,0):c.laser_fixture_data(f,0);stateSize=infinite?0x648:0x624;
   const b=Buffer.alloc(infinite?0xeb0:0xe8c);for(let i=0x440;i<stateSize;i+=4)b.writeFloatLE((random()-.5)*10,i);
   const length=[0,8,15.9999,16,16.0001,24,32,40,48,64,80,96,100,128,240,500,1000,4096][example%18],angle=[0,.375,Math.PI/2,-Math.PI/2,Math.PI,-.97][example%6];
   const x=Math.fround((random()-.5)*400),y=Math.fround(random()*448);b.writeFloatLE(x,0x3c);b.writeFloatLE(y,0x40);b.writeFloatLE(.125,0x44);b.writeFloatLE(angle,0x54);b.writeFloatLE(length,0x58);b.writeFloatLE(length+37,0x450);b.writeInt32LE(example%7===0?1:0,0x438);b.writeInt16LE(example%16,0x466);
   if(infinite){b.writeFloatLE(length+137,0x460);b.writeFloatLE(32,0x5c);b.writeUInt32LE(example%7===0?8:0,0x490);b.writeInt16LE(4,0x48c);b.writeInt16LE(example%16,0x48e);}
   const hit=Buffer.alloc(12),sz=Buffer.alloc(12),distance=[0,8,16,24,length/2,length-8,length+8,random()*length][example%8],radius=[0,8,16,24,32,100,500,-16][(example>>3)%8];
   hit.writeFloatLE(x+Math.cos(Math.fround(angle))*distance);hit.writeFloatLE(y+Math.sin(Math.fround(angle))*distance,4);sz.writeFloatLE(radius*2);sz.writeFloatLE(radius*2,4);
   events=[];emissions=[];c.laser_fixture_count(f,2);memory(c,p,b.length).set(b);m.write(q,b);memory(c,center,12).set(hit);memory(c,size,12).set(sz);m.write(nc,hit);m.write(ns,sz);
   m.view(manager,0x480).fill(0);m.u32(manager+0x450,tail);m.u32(manager+0x458,0x10000);m.u32(manager+0x474,0x12345);m.resetThreadFPU();
   const reward=(example>>6)&3,skip=(example>>8)&1;
   if(circle){const bits=Buffer.alloc(4);bits.writeFloatLE(radius);m.call(infinite?0x427c00:0x4267e0,{ecx:q,args:[nc,bits.readUInt32LE(),reward,skip]});assert.ok((infinite?c.li_cut_circle:c.laser_cut_circle)(f,center,radius,reward,skip)>=0);}
   else{m.call(infinite?0x427730:0x426230,{ecx:q,args:[nc,ns,reward,skip]});assert.ok((infinite?c.li_cut_rectangle:c.laser_cut_rectangle)(f,center,size,reward,skip)>=0);}
   compare(`${infinite?'infinite':'line'} ${circle?'circle':'rectangle'} ${example}`);
  }
  report('laser-cut',{passed:true,checks,spawnChecks,eventChecks,scope:'Original line 426230/4267e0 and infinite 427730/427c00 rectangle/circle cutting: distinct inclusive/exclusive 16-unit samples, boundary hits, immunity, leading/middle/trailing removal, split spawn parameters, ordered rewards/effects/player-shot callbacks, lengths through 4096. Spawn allocation/initialization, item/effect and shot systems are callback boundaries; manager capacity is not tested here.'});
 }finally{c.ll_delete(f);c.release(center);c.release(size);m.close();}
});
