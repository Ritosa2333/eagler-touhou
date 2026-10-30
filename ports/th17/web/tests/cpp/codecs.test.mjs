import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,oracle,memory,root,report} from './helpers.mjs';
test('TH17 permutation cipher matches original across block and tail boundaries',async()=>{
 const c=await core(),m=await oracle(),capacity=65536,input=c.allocate(capacity),output=c.allocate(capacity),native=m.allocate(capacity),mark=m.heap;let checks=0;
 try{
  const configs=[[27,55,16,16],[62,155,128,65536],[0x5c,0xe1,0x400,65536],[0x7d,0x3a,0x100,65536],[27,115,256,14336],[18,67,512,15872],[53,121,1024,15360],[3,145,128,25600],[171,220,128,28672],[81,158,256,16384],[193,21,1024,11264],[153,125,128,17408]];
  for(const [key,step,block,limit] of configs)for(const n of [...new Set([0,1,2,3,block/4-1,block/4,block/4+1,block-1,block,block+1,block*2+3,limit-1,limit,limit+1])].filter(x=>x>=0&&x<=capacity))for(const encode of [0,1]){
   const data=Buffer.from(Array.from({length:n},(_,i)=>(i*113+(i>>>7)*37+n)&255));memory(c,input,n).set(data);m.write(native,data);m.heap=mark;
   const expected=m.call(encode?0x401fc0:0x401eb0,{ecx:native,edx:n,args:[key,step,block,limit]});assert.equal(expected,native);
   assert.equal(c.crypt(input,output,n,key,step,block,limit,encode),1);assert.deepEqual(Buffer.from(memory(c,output,n)),Buffer.from(m.bytes(native,n)),JSON.stringify({n,key,step,block,limit,encode}));checks++;
  }
  report('resource-crypt',{passed:true,checks,functions:['0x401eb0','0x401fc0']});
 }finally{m.close();c.deallocate(input);c.deallocate(output);}
});
test('TH17 LZSS decoder matches original on real files and overlapping dictionary copies',async()=>{
 const c=await core(),m=await oracle(),codec=c.codec_create(),capacity=131072,input=c.allocate(capacity),packed=c.allocate(capacity*2),output=c.allocate(capacity),nativeInput=m.allocate(capacity*2),nativeOutput=m.allocate(capacity),mark=m.heap;let checks=0;
 try{
  const files=['demo1.rpy','demo2.rpy','demo3.rpy','st01.ecl','st07.ecl','thbgm.fmt'];
  const samples=[Buffer.alloc(0),Buffer.alloc(1),Buffer.alloc(8193),Buffer.from(Array.from({length:32768},(_,i)=>(i*79+(i>>>4))&255)),...files.map(n=>readFileSync(resolve(root,'reference/assets',n)))];
  for(const data of samples){assert.ok(data.length<=capacity);memory(c,input,data.length).set(data);const n=c.codec_encode(codec,input,data.length,packed,capacity*2);assert.ok(n>=0);const encoded=Buffer.from(memory(c,packed,n));m.write(nativeInput,encoded);m.heap=mark;assert.equal(m.call(0x45ecf0,{ecx:nativeInput,edx:n,args:[nativeOutput,data.length]}),nativeOutput);assert.deepEqual(Buffer.from(m.bytes(nativeOutput,data.length)),data);assert.equal(c.codec_decode(codec,packed,n,output,capacity),data.length);assert.deepEqual(Buffer.from(memory(c,output,data.length)),data);checks++;}
  report('lzss',{passed:true,checks,function:'0x45ecf0',scope:'Native decode including real resource bytes; encode byte identity tested separately'});
 }finally{m.close();c.codec_delete(codec);c.deallocate(input);c.deallocate(packed);c.deallocate(output);}
});
