import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
import {firstDifference} from './shot-oracle.mjs';

const ticks=[0,0x428610,0x418170,0x418200,0x418240,0x418290,0x418460,0x418520,0x4188c0,0x4189b0,0x4189d0,0x418cf0,0x418d20];
test('TH11 marked enemies and segmented boss damage/collision match original callbacks',async()=>{
 const c=await core(),m=await oracle(),f=c.ecb_create(),ep=c.ecb_data(f,0,0),vp=c.ecb_data(f,2,0);
 const enemy=m.allocate(0x163c),vm=m.allocate(0x434),manager=m.allocate(128),linked=Array.from({length:8},()=>m.allocate(0x2678));
 m.u32(0x4a8d7c,manager);m.u32(manager+0x68,linked[0]+0x103c+0x168);
 let calls=[],damage=0,checks=0,cases=0,seed=98123;
 const rand=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed;};
 const arg=n=>m.u32(m.reg('ESP')+4*n),pos=p=>[m.f32(p),m.f32(p+4),m.f32(p+8)];
 m.replace(0x4561e0,'arc ANM lookup boundary',()=>vm,1);
 m.replace(0x4347f0,'arc shot collision boundary',()=>{calls.push([2,damage,...pos(arg(1)),m.f32(arg(2)),m.f32(arg(2)+4),0]);return damage;},2);
 m.replace(0x432520,'arc player collision boundary',()=>{calls.push([3,0,...pos(m.reg('EAX')),m.f32(m.reg('ESP')+4),m.f32(m.reg('ESP')+8),m.f32(m.reg('ESP')+12)]);return 0;},3);
 const actualCalls=()=>{const p=c.ecb_data(f,3,0),dv=new DataView(c.memory.buffer);return Array.from({length:c.ecb_count(f,0)},(_,i)=>{const a=p+i*32;return[dv.getInt32(a,true),dv.getInt32(a+4,true),...Array.from({length:6},(_,j)=>dv.getFloat32(a+8+j*4,true))];});};
 try{
  for(const kind of [2,3,4])for(let n=0;n<150;++n){
   for(let j=0;j<8;++j){const b=Buffer.alloc(0x163c);for(let o=0;o<b.length;o+=4)b.writeUInt32LE(rand(),o);
    const p=c.ecb_data(f,1,j),saved=Buffer.from(memory(c,p+0x168,12));memory(c,p,b.length).set(b);memory(c,p+0x168,12).set(saved);
    b.writeUInt32LE(linked[j],0x168);b.writeUInt32LE(j===7?0:linked[j+1]+0x103c+0x168,0x16c);b.writeUInt32LE(0,0x170);m.write(linked[j]+0x103c,b);
   }
   assert.equal(c.ecb_run(f,kind,0),m.call(ticks[kind],{ecx:enemy})|0);
   for(let j=0;j<8;++j){const a=Buffer.from(memory(c,c.ecb_data(f,1,j),0x163c)),b=Buffer.from(m.bytes(linked[j]+0x103c,0x163c));a.fill(0,0x168,0x174);b.fill(0,0x168,0x174);assert.equal(firstDifference(a,b),'',`marked ${kind}/${n}/${j}`);++checks;}++cases;
  }
  for(const kind of [13,14])for(let n=0;n<240;++n){
   const e=Buffer.alloc(0x163c),v=Buffer.alloc(0x434);for(let j=0;j<2;++j)e.writeFloatLE((rand()/2**32-.5)*400,0x34+j*4);
   e.writeUInt32LE(42,0xe0);e.writeInt32LE([0,1,700,23999,30000,-15][n%6],0x14f0);e.writeInt32LE([1,24000,999999][n%3],0x14f4);
   v.writeFloatLE((rand()/2**32-.5)*12,0x24);v.writeFloatLE((rand()/2**32-.5)*24,0x2c);v.writeFloatLE([0,16,127.25,256.1][n%4],0x40);v.writeUInt32LE(rand(),0x404);
   memory(c,ep,e.length).set(e);m.write(enemy,e);memory(c,vp,v.length).set(v);m.write(vm,v);damage=[0,1,2,4,8,30,200,-1,2147483647][n%9];calls=[];
   const native=m.call(kind===13?0x417ef0:0x418050,{ecx:enemy})|0;assert.equal(c.ecb_run(f,kind,damage),native,`arc result ${kind}/${n}`);
   assert.deepEqual(actualCalls(),calls,`arc ordered geometry ${kind}/${n}`);assert.equal(firstDifference(Buffer.from(memory(c,vp,v.length)),Buffer.from(m.bytes(vm,v.length))),'',`arc animation ${kind}/${n}`);checks+=calls.length+2;++cases;
  }
  report('enemy-callbacks-geometry',{passed:true,cases,checks,scope:'Original marked-enemy callbacks 2/3/4 with signed overflow; original arc damage and collision positions, order, per-segment dimensions, health scaling and damage caps. Shot and player collision calls are recorded external boundaries.'});
 }finally{c.ecb_delete(f);m.close();}
});
