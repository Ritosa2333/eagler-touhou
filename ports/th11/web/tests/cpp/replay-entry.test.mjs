import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
test('TH11 replay stage entry restores native counters, RNG and player coordinates',async()=>{
 const c=await core(),m=await oracle(),header=c.allocate(144),economy=c.allocate(48),rng=c.allocate(8),motion=c.allocate(160),nh=m.allocate(144),manager=m.allocate(0x2dc),player=m.allocate(0x8d40);
 const globals=[0x4a56e4,0x4a56e8,0x4a56f0,0x4a56f4,0x4a5718,0x4a571c,0x4a5720,0x4a5744,0x4a5748,0x4a574c,0x4a5754,0x4a576c];
 m.u32(0x4a8eb8,manager);m.u32(0x4a8eb4,player);m.i32(manager+16,1);m.replace(0x432cc0,'formation rebuild boundary',()=>0);
 let checks=0,seed=62137;const random=()=>seed=(Math.imul(seed,1664525)+1013904223)>>>0;
 try{for(let sample=0;sample<420;++sample){const h=Buffer.alloc(144),e=Buffer.alloc(48),r=Buffer.alloc(8),st=sample%7+1;
  h.writeUInt16LE(st);h.writeUInt16LE(random()&65535,2);h.writeInt32LE(random()%999999999,12);h.writeInt16LE(random()%1200,16);h.writeInt32LE(random()%400000,20);h.writeInt16LE(sample%10,24);h.writeInt16LE(sample%5,26);h.writeInt32LE(sample%2049-1024,28);
  h.writeInt32LE((random()%46000)-23000,32);h.writeInt32LE(random()%51200,36);h.writeInt32LE(sample%8,40);h.writeInt32LE(0,44);h.writeInt32LE(sample%2,48);h.writeInt32LE(random()%100000,52);h.writeInt32LE(sample%5,56);
  for(let k=0;k<12;++k)e.writeInt32LE([0,0,2500000,0,2,3,1,0,400,100,753,5000000][k],k*4);r.writeUInt32LE(0x1234beef);r.writeUInt32LE(9000,4);
  memory(c,header,144).set(h);memory(c,economy,48).set(e);memory(c,rng,8).set(r);memory(c,motion,160).fill(0);m.write(nh,h);m.write(0x4c2f00,r);globals.forEach((a,i)=>m.i32(a,e.readInt32LE(i*4)));m.u32(manager+0xb4+st*36,nh);m.i32(0x4a5728,st);
  c.replay_restore(header,economy,rng,motion,0);m.call(0x436f30);m.call(0x436da0);
  const dv=new DataView(c.memory.buffer);globals.forEach((a,i)=>assert.equal(dv.getInt32(economy+i*4,true),m.i32(a),`sample${sample} global ${a.toString(16)}`));
  assert.deepEqual(Buffer.from(memory(c,rng,8)),Buffer.from(m.bytes(0x4c2f00,8)));
  for(const [off,np,n]of [[0,0x87c,8],[12,0x888,8],[88,0x8d20,4],[108,0x8bac,4]])assert.deepEqual(Buffer.from(memory(c,motion+off,n)),Buffer.from(m.bytes(player+np,n)));
  ++checks;
 }report('replay-entry',{passed:true,checks,scope:'Native 436f30 and 436da0 stage restore, signed fields/power clamp, RNG and positions. Option creation has its own native oracle.'});
 }finally{for(const p of[header,economy,rng,motion])c.release(p);m.close();}
});
