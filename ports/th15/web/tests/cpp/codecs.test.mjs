import test from "node:test";
import assert from "node:assert/strict";
import {readFileSync} from "node:fs";
import {resolve} from "node:path";
import {core,oracle,memory,root,report,sha} from "./helpers.mjs";
test("TH15 crypt matches original encode/decode across block and limit boundaries",async()=>{
 const c=await core(),m=await oracle(),params=JSON.parse(readFileSync(resolve(root,"reference/archive-ciphers.json"))).entries;
 const input=c.allocate(65536),out=c.allocate(65536),native=m.allocate(65536),heap=m.heap;let checks=0;
 try{for(const p of [...params,{key:0x5c,step:0xe1,block:0x400,limit:0x800},{key:0x7d,step:0x3a,block:0x100,limit:0x200}])for(const n of [...new Set([0,1,2,3,p.block/4-1,p.block/4,p.block/4+1,p.block-1,p.block,p.block+1,p.block*2-1,p.limit-1,p.limit,p.limit+1,p.limit+19])]){
  const bytes=Buffer.from(Array.from({length:n},(_,i)=>(i*73+(i>>>3)*17+51)&255));
  for(const encode of [0,1]){memory(c,input,n).set(bytes);assert.equal(c.crypt(input,out,n,p.key,p.step,p.block,p.limit,encode),1);m.heap=heap;m.write(native,bytes);m.call(encode?0x402ca0:0x402b80,{ecx:native,edx:n,args:[p.key,p.step,p.block,p.limit],limit:10000000});assert.deepEqual(Buffer.from(memory(c,out,n)),Buffer.from(m.bytes(native,n)),JSON.stringify({p,n,encode}));checks++;}
 }report("crypt",{passed:true,checks,originalFunctions:["0x402b80","0x402ca0"]});}finally{c.release(input);c.release(out);m.close();}
});
test("TH15 LZSS encoder bytes and decoder output match original",async()=>{
 const c=await core(),m=await oracle(),codec=c.codec_create(),p=c.allocate(262144),packed=c.allocate(524288),out=c.allocate(262144),native=m.allocate(262144),dest=m.allocate(262144),length=m.allocate(4),heap=m.heap;let checks=0;const records=[];
 try{const samples=[Buffer.from([17]),Buffer.from("touhou15 touhou15 touhou15 touhou15"),Buffer.alloc(16384),Buffer.from(Array.from({length:16385},(_,i)=>(i*7^(i>>>3))&255)),... ["default.ecl","st01.ecl","st06.ecl","st07bs.ecl","pl00.sht","st01a.msg"].map(n=>readFileSync(resolve(root,"reference/assets",n)))];
  for(const bytes of samples){memory(c,p,bytes.length).set(bytes);const size=c.codec_encode(codec,p,bytes.length,packed,524288);m.heap=heap;m.write(native,bytes);const original=m.call(0x46d1b0,{ecx:native,edx:bytes.length,args:[length],limit:1000000000}),originalSize=m.u32(length);assert.equal(size,originalSize);assert.deepEqual(Buffer.from(memory(c,packed,size)),Buffer.from(m.bytes(original,originalSize)),"encoder "+bytes.length);
   assert.equal(c.codec_decode(codec,packed,size,out,bytes.length),bytes.length);assert.deepEqual(Buffer.from(memory(c,out,bytes.length)),bytes);m.call(0x46d590,{ecx:original,edx:originalSize,args:[dest,bytes.length],limit:1000000000});assert.deepEqual(Buffer.from(m.bytes(dest,bytes.length)),bytes);
   const stream=c.stream_create();for(let steps=0;!c.stream_done(stream);steps++){assert.ok(steps<bytes.length+100);assert.equal(c.stream_step(stream,packed,size,out,bytes.length,127),1);}assert.equal(c.stream_size(stream),bytes.length);assert.deepEqual(Buffer.from(memory(c,out,bytes.length)),bytes);c.stream_delete(stream);checks++;records.push({bytes:bytes.length,packed:size,sha256:sha(bytes)});
  }report("lzss",{passed:true,checks,originalFunctions:["0x46d1b0","0x46d590"],samples:records});
 }finally{c.codec_delete(codec);for(const ptr of [p,packed,out])c.release(ptr);m.close();}
});
