import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';import {bits} from './ecl-bytecode.mjs';import {firstDifference} from './shot-oracle.mjs';
test('TH11 destructible enemies receive original cancellation damage and ordered point drops',async()=>{
 const c=await core(),m=await oracle(),p=c.ecl_create(),f=c.em_create(p),n=96,size=0x2678,ce=c.allocate(n*size),ne=m.allocate(n*size),nm=m.allocate(0x100),cp=c.allocate(12),np=m.allocate(12);
 const view=()=>new DataView(c.memory.buffer);m.view(nm,0x100).fill(0);m.u32(0x4a8d7c,nm);let drops=[],checks=0,cases=0,rewards=0;
 m.replace(0x424230,'enemy cancellation item creation boundary',()=>{const s=m.reg('ESP'),a=m.reg('EAX');assert.equal(m.reg('ECX'),8);assert.equal(m.u32(s+4),0xffffffff);assert.equal(m.f32(s+8),Math.fround(-Math.PI/2));assert.equal(m.f32(s+12),Math.fround(.6));drops.push([m.f32(a),m.f32(a+4),m.f32(a+8)]);return 0;},3);
 const adjacent=(x,step)=>{const b=Buffer.alloc(4);b.writeUInt32LE((bits(x)+step)>>>0);return b.readFloatLE();};
 try{for(const beam of [false,true])for(const radius of [0,1.1,16,80,128.125,512])for(const reward of [0,1])for(const center of [[0,224,0],[-192,0,3],[192,448,-3],[.15,99.1,.3]]){
  const initial=Buffer.alloc(n*size),r=Math.fround(radius+16),delta=[0,r,adjacent(r,-1),adjacent(r,1),r*.7071067811865476];
  for(let i=0;i<n;++i){const off=i*size,j=i%12;const position=[[center[0],center[1]],[center[0]+delta[Math.floor(i/12)%5],center[1]],[-194,224],[-193.999,224],[194,224],[193.999,224],[0,-2],[0,-1.999],[0,450],[0,449.999],[center[0]+r*.6,center[1]+r*.8],[center[0]+r*.3,center[1]+r*.4]][j];
   if(beam&&j<6){position[0]=Math.fround(center[0]+[0,radius,-radius,adjacent(radius,-1),adjacent(radius,1),0][j]);position[1]=center[1]+[-1,-1,-1,-1,-1,0][j];}
   for(let k=0;k<3;++k)initial.writeFloatLE(k===2?center[2]:position[k],off+0x1070+k*4);
   initial.writeUInt32LE(i%7?0x800:0,off+0x25bc);initial.writeUInt32LE(i%4,off+0x2540);
   initial.writeInt32LE([0,100000,-1000,2147483600,-2147483600][i%5],off+0x252c);initial.writeInt32LE([0,1234567,-13579,2147483600,-2147483600][i%5],off+0x2538);initial.writeInt32LE([0,999,0x7fffffff,-0x80000000][i%4],off+0x253c);
  }
  memory(c,ce,initial.length).set(initial);m.write(ne,initial);
  for(let i=0;i<n;++i){for(const [base,put]of [[ce,(a,v)=>view().setUint32(a,v,true)],[ne,(a,v)=>m.u32(a,v)]]){const node=base+i*size+0x11a4;put(node,base+i*size);put(node+4,i+1<n?base+(i+1)*size+0x11a4:0);put(node+8,i?base+(i-1)*size+0x11a4:0);}}
  c.em_cancel_first(f,ce+0x11a4);m.u32(nm+0x68,ne+0x11a4);center.forEach((v,i)=>{view().setFloat32(cp+i*4,v,true);m.f32(np+i*4,v);});
  for(let repeat=0;repeat<3;++repeat){drops=[];m.call(beam?0x412b10:0x412990,{args:[np,bits(radius),reward]});assert.equal((beam?c.em_cancel_beam:c.em_cancel)(f,cp,radius,reward),1);
   for(let i=0;i<n;++i){const a=Buffer.from(memory(c,ce+i*size,size)),b=Buffer.from(m.bytes(ne+i*size,size));for(const off of [0x11a4,0x11a8,0x11ac]){a.writeUInt32LE(0,off);b.writeUInt32LE(0,off);}assert.equal(firstDifference(a,b),'',`case ${cases} enemy ${i}`);++checks;}
   const a=c.em_cancel_rewards(f),v=view(),got=Array.from({length:c.em_cancel_reward_count(f)},(_,i)=>Array.from({length:3},(_,j)=>v.getFloat32(a+i*12+j*4,true)));assert.deepEqual(got,drops,`case ${cases} drops`);rewards+=drops.length;++cases;
  }
 }
 report('enemy-cancellation',{passed:true,cases,checks,rewards,scope:'Native 412990 and 412b10 linked-list traversal, inclusive radius and strict beam boundaries, destructible flag, ordinary/seven-times health arithmetic including signed wrap, repeated cancellation and exact ordered type-8 drop requests. Item pool execution is verified separately.'});
 }finally{c.em_cancel_first(f,0);c.em_delete(f);c.ecl_delete(p);c.release(ce);c.release(cp);m.close();}
});
