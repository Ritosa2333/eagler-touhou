import test from "node:test";import assert from "node:assert/strict";
import {core,oracle,memory,report} from "./helpers.mjs";
test("TH15 random sequence and SSE floating conversion match original",async()=>{
 const c=await core(),m=await oracle(),rng=c.rng_create(),native=m.allocate(8),result=m.allocate(4),wrappers=[];
 try{for(const address of [0x403800,0x403840]){const at=m.allocate(32),b=Buffer.alloc(17);b[0]=0xe8;b.writeInt32LE(address-at-5,1);b.set([0xd9,0x1d],5);b.writeUInt32LE(result,7);b[11]=0xa1;b.writeUInt32LE(result,12);b[16]=0xc3;m.write(at,b);wrappers.push(at);}let checks=0;
 for(let i=0;i<4096;i++){const seed=(i*4051+13)&65535,calls=(0xfffffff0+i)>>>0;const v=new DataView(c.memory.buffer,rng,8);v.setUint16(0,seed,true);v.setUint16(2,0x8372,true);v.setUint32(4,calls,true);m.u32(native,seed|0x83720000);m.u32(native+4,calls);for(const kind of [0,1,2,3,1,3,0,2]){const got=c.rng_next(rng,kind)>>>0,expected=m.call(kind===0?0x403630:kind===1?0x4036a0:wrappers[kind-2],{ecx:native});assert.equal(got,kind===0?expected&65535:expected,JSON.stringify({seed,kind,checks}));assert.deepEqual(Buffer.from(memory(c,rng,8)),Buffer.from(m.bytes(native,8)));checks++;}}report("rng",{passed:true,seedSamples:4096,checks,functions:["0x403630","0x4036a0","0x403800","0x403840"]});
 }finally{c.rng_delete(rng);m.close();}
});
