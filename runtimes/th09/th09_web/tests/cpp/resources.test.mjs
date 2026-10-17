import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve,dirname} from 'node:path';
import {createHash} from 'node:crypto';
import {oracle,core,root,memory,report} from './helpers.mjs';

test('TH09 LZSS encoding, decoding and persistent history match original',async()=>{
    const m=await oracle(),c=await core(),codec=c.codec_create(),p=c.allocate(40000),q=c.allocate(90000);
    const source=m.allocate(40000),dest=m.allocate(90000),length=m.allocate(4);let checks=0;
    try{
        for(const size of [0,1,2,3,17,18,19,31,255,8191,8192,8193,18000,35000])for(const pattern of [0,1,2]){
            const input=Uint8Array.from({length:size},(_,i)=>pattern===0?65:pattern===1?(i*71+19)&255:((i*i*13)^(i>>>4))&255);
            m.write(source,input);memory(c,p,size).set(input);
            const packed=m.call(0x434020,{ecx:source,edx:size,args:[length],limit:100000000});
            const n=c.codec_encode(codec,p,size,q,90000);assert.equal(n,m.u32(length),`size ${size}/${pattern}`);
            assert.deepEqual(memory(c,q,n),m.bytes(packed,n),`encode ${size}/${pattern}`);
            assert.deepEqual(memory(c,c.codec_dictionary(codec),8192),m.bytes(0x4cc430,8192));
            const history=Uint8Array.from({length:8192},(_,i)=>(i*13)&255);memory(c,c.codec_dictionary(codec),8192).set(history);m.write(0x4cc430,history);
            assert.equal(c.codec_decode(codec,q,n,p,40000),size);
            m.call(0x433aa0,{ecx:packed,edx:n,args:[dest,size],limit:100000000});
            assert.deepEqual(memory(c,p,size),input);assert.deepEqual(memory(c,p,size),m.bytes(dest,size));
            assert.deepEqual(memory(c,c.codec_dictionary(codec),8192),m.bytes(0x4cc430,8192));checks++;
        }
        report('lzss',{passed:true,checks,encoding:'byte exact',historyBytes:8192});
    }finally{c.codec_delete(codec);c.release(p);c.release(q);m.close();}
});

test('TH09 crypt variants preserve block limits, short inputs and odd tails',async()=>{
    const m=await oracle(),c=await core(),p=c.allocate(40000),q=c.allocate(40000),source=m.allocate(40000);let checks=0;
    try{
        for(const [key,step,block,limit] of [[27,55,12,1024],[62,155,128,1024],[193,81,1024,1024],[3,25,1024,1024],[18,52,1024,1024],[58,205,256,3072]])
        for(const size of [0,1,2,3,11,12,31,127,128,129,255,1001,1023,1024,1025,5119,5120,5121,9001,33000])for(const encrypt of [0,1]){
            const input=Uint8Array.from({length:size},(_,i)=>(i*113+(i>>>8))&255);m.write(source,input);memory(c,p,size).set(input);
            const expected=m.call(encrypt?0x42c370:0x42c180,{ecx:source,edx:size,args:[key,step,block,limit],limit:20000000});
            assert.equal(c.crypt(p,q,size,key,step,block,limit,encrypt),1);assert.deepEqual(memory(c,q,size),m.bytes(expected,size),`${size}/${block}/${encrypt}`);checks++;
        }
        report('resource-crypt',{passed:true,checks});
    }finally{c.release(p);c.release(q);m.close();}
});

