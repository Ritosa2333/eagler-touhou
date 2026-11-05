import test from 'node:test';import assert from 'node:assert/strict';
import {core,oracle,memory,report} from './helpers.mjs';
test('TH11 RNG words and state match original pre-rotation next32 behavior',async()=>{
 const c=await core(),m=await oracle(),p=c.allocate(8),q=m.allocate(8);let checks=0;
 try{for(const seed of [0,1,0x9630,0xffff,0x6553,0x8000,12345,54321])for(const mode of [0,1,2]){
  const b=Buffer.alloc(8);b.writeUInt16LE(seed);b.writeUInt32LE(0xfffffff0,4);memory(c,p,8).set(b);m.write(q,b);
  for(let i=0;i<1024;i++){m.reg('ESI',q);const expected=m.call([0x458bc0,0x458c30,0x458d70][mode],{ecx:q});const actual=c.rng(p,mode)>>>0;assert.equal(actual,mode===0?expected&65535:expected);assert.deepEqual(Buffer.from(memory(c,p,8)),Buffer.from(m.bytes(q,8)));++checks;}
 }report('rng',{passed:true,checks});}finally{c.release(p);m.close();}
});
