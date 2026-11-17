import test from 'node:test';
import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
test('TH17 random state and results match the original for every 16-bit seed',async()=>{
 const c=await core(),m=await oracle(),r=c.rng_create(),native=m.allocate(8),result=m.allocate(4),wrappers=[];
 try{
  for(const address of [0x402840,0x402880]){const at=m.allocate(32),b=Buffer.alloc(17);b[0]=0xe8;b.writeInt32LE(address-at-5,1);b.set([0xd9,0x1d],5);b.writeUInt32LE(result,7);b[11]=0xa1;b.writeUInt32LE(result,12);b[16]=0xc3;m.write(at,b);wrappers.push(at);}
  let checks=0;for(let seed=0;seed<65536;seed++){
   const calls=(0xfffffff0+seed)>>>0,v=new DataView(c.memory.buffer,r,8);v.setUint16(0,seed,true);v.setUint16(2,0x8372,true);v.setUint32(4,calls,true);m.u32(native,seed|0x83720000);m.u32(native+4,calls);
   for(const kind of (seed%16===0?[0,1,2,3,1,3,0,2]:[0,1])){
    const expected=m.call(kind===0?0x402740:kind===1?0x4027b0:wrappers[kind-2],{ecx:native});
    assert.equal(c.rng_next(r,kind)>>>0,kind===0?expected&65535:expected,JSON.stringify({seed,kind,checks}));
    assert.deepEqual(Buffer.from(memory(c,r,8)),Buffer.from(m.bytes(native,8)));checks++;
   }
  }
  report('rng',{passed:true,seedSamples:65536,checks,functions:['0x402740','0x4027b0','0x402840','0x402880']});
 }finally{c.rng_delete(r);m.close();}
});
