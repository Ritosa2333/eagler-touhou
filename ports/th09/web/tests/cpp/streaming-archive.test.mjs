import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {resolve} from 'node:path';
import {core,memory,report,root,target} from './helpers.mjs';
test('Every PBGZ entry decodes identically when spread across bounded preload steps',async()=>{
 const c=await core(),archive=c.archive_create(),bytes=readFileSync(resolve(root,target.executable,'..','th09.dat')),input=c.allocate(bytes.length),packed=c.allocate(16000000),output=c.allocate(16000000),expected=c.allocate(16000000),sizep=c.allocate(4);memory(c,input,bytes.length).set(bytes);assert.equal(c.archive_open(archive,input,bytes.length),1);let files=0,steps=0,total=0;
 try{for(let n=0;n<c.archive_count(archive);++n){const name=c.archive_name(archive,n),length=c.archive_packed(archive,name,packed,sizep),size=new DataView(c.memory.buffer).getUint32(sizep,true),decoder=c.lzss_stream_create(),budget=n%3===0?131072:n%3===1?65536:16384;assert.ok(length);assert.equal(c.archive_read(archive,name,expected,16000000,0),size);
  try{let previous=0;while(!c.lzss_stream_done(decoder)){assert.equal(c.lzss_stream_step(decoder,packed,length,output,size,budget),1);const written=c.lzss_stream_size(decoder);assert.ok(written-previous<=budget+17);previous=written;++steps;}assert.equal(c.lzss_stream_size(decoder),size);assert.deepEqual(memory(c,output,size),memory(c,expected,size),'entry '+n);++files;total+=size;}finally{c.lzss_stream_delete(decoder);}}
  report('streaming-archive',{files,steps,bytes:total,budgets:[16384,65536,131072],scope:'All real archive files decoded from a fresh dictionary match the synchronous reader, whose asset hashes were verified against the original executable.'});
 }finally{c.archive_delete(archive);}
});
