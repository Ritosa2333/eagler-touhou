import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,readdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {createHash} from 'node:crypto';
import {oracle,core,root,memory,report} from './helpers.mjs';

test('TH09 score.dat loading and newly saved containers are accepted by the original',async()=>{
    const m=await oracle(),c=await core(),score=c.score_create(),other=c.score_create(),p=c.allocate(1024*1024),out=c.allocate(1024*1024),rng=c.allocate(8);
    let current=readFileSync(resolve(root,'../[th09] 东方花映塚 (日文版)/score.dat')),checks=0;
    const sourceHash=createHash('sha256').update(current).digest('hex');
    m.replace(0x42c5c0,'log',()=>0);
    m.replace(0x42c970,'read-score',()=>{const q=m.allocate(current.length);m.write(q,current);m.u32(m.reg('EDX'),current.length);return q;},1);
    const verify=()=>{
        memory(c,p,current.length).set(current);assert.equal(c.score_decode(other,p,current.length),1);
        const q=m.call(0x421c80,{ecx:0x2020000,limit:100000000});
        const size=c.score_size(other);assert.equal(size,m.u32(q+12));assert.deepEqual(memory(c,c.score_data(other),size),m.bytes(q,size));checks++;
    };
    try{
        verify();assert.equal(c.score_decode(score,p,current.length),1);
        for(const seed of [0,1,0x1234,0xffff]){
            memory(c,rng,8).fill(0);new DataView(c.memory.buffer).setUint16(rng,seed,true);
            const length=c.score_encode(score,rng,out,1024*1024);assert.ok(length>0);current=Buffer.from(memory(c,out,length));verify();
            assert.deepEqual(memory(c,c.score_data(other)+24,c.score_size(other)-24),memory(c,c.score_data(score)+24,c.score_size(score)-24));
        }
        const damaged=Buffer.from(current);damaged[damaged.length-3]^=128;memory(c,p,damaged.length).set(damaged);assert.equal(c.score_decode(other,p,damaged.length),0);
        for(const size of [0,1,23,24,current.length-8])assert.equal(c.score_decode(other,p,size),0);
        report('score-file',{passed:true,originalReaderChecks:checks,sourceSha256:sourceHash,decodedBytes:c.score_size(score),invalidFilesRejected:6});
    }finally{c.score_delete(score);c.score_delete(other);c.release(p);c.release(out);c.release(rng);m.close();}
});

test('TH09 original demonstration replays decode exactly and re-encode byte-for-byte',async()=>{
    const m=await oracle(),c=await core(),r=c.replay_create(),p=c.allocate(1024*1024),out=c.allocate(1024*1024);const results=[];
    try{
        for(const name of readdirSync(resolve(root,'reference/assets')).filter(n=>/^demorpy\d\.rpy$/.test(n))){
            const input=readFileSync(resolve(root,'reference/assets',name));memory(c,p,input.length).set(input);assert.equal(c.replay_decode(r,p,input.length),1,name);
            const source=m.allocate(input.length);m.write(source,input);
            const native=m.call(0x4205e0,{ecx:source,edx:input.length,limit:200000000});assert.ok(native,name);
            const size=c.replay_size(r),expected=m.bytes(native,size),view=new DataView(expected.buffer);
            for(let i=0;i<40;++i){const at=32+i*4,offset=view.getUint32(at,true);if(offset)view.setUint32(at,offset-native,true);}
            assert.deepEqual(memory(c,c.replay_data(r),size),expected,name);
            const length=c.replay_encode(r,out,1024*1024);assert.equal(length,input.length,name);assert.deepEqual(memory(c,out,length),new Uint8Array(input),name+' re-encode');
            for(const short of [0,24,191,input.length-200])assert.equal(c.replay_decode(r,p,short),0);
            const bad=Buffer.from(input);bad[200]^=1;memory(c,p,bad.length).set(bad);assert.equal(c.replay_decode(r,p,bad.length),0);
            results.push({name,encodedBytes:input.length,decodedBytes:size,sha256:createHash('sha256').update(input).digest('hex'),byteExact:true});
        }
        assert.equal(results.length,3);report('replay-file',{passed:true,files:results,invalidFilesRejected:15});
    }finally{c.replay_delete(r);c.release(p);c.release(out);m.close();}
});
