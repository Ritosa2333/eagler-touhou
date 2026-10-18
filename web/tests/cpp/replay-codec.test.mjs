import test from 'node:test';import assert from 'node:assert/strict';import{readFileSync}from'node:fs';import{resolve}from'node:path';
import {core,oracle,memory,report,root} from './helpers.mjs';
test('TH11 replay compression matches the original encoder',async()=>{
 const c=await core(),m=await oracle(),input=c.allocate(131072),out=c.allocate(262148),ni=m.allocate(131072),length=m.allocate(4),heap=m.heap,r=c.replay_create();let samples=0,seed=27361;
 const random=()=>seed=(Math.imul(seed,1664525)+1013904223)>>>0;
 const compare=b=>{memory(c,input,b.length).set(b);m.heap=heap;m.write(ni,b);const original=m.call(0x4423a0,{args:[ni,b.length,length],limit:100000000}),n=c.codec_encode(input,b.length,out,262148);assert.equal(n,m.u32(length));assert.deepEqual(Buffer.from(memory(c,out,n)),Buffer.from(m.bytes(original,n)),`sample ${samples} size ${b.length}`);++samples;};
 try{for(const n of[0,1,2,3,17,18,19,63,64,65,511,8191,8192,8193,65535])for(let kind=0;kind<4;++kind){const b=Buffer.alloc(n);for(let i=0;i<n;++i)b[i]=kind===0?0:kind===1?i%19:kind===2?random()>>>24:(i%256<200?65:random()>>>24);compare(b);}
  for(let i=0;i<4;++i){const b=readFileSync(resolve(root,`reference/assets/demo${i}.rpy`));memory(c,input,b.length).set(b);assert.equal(c.replay_open(r,input,b.length),1);compare(Buffer.from(memory(c,c.replay_data(r),c.replay_size(r))));}
  report('replay-codec',{passed:true,samples,scope:'Native 4423a0 byte-for-byte compression: empty/tiny, dictionary wrap, repeated/random data and all four decoded demos.'});
 }finally{for(const p of[input,out])c.release(p);c.replay_delete(r);m.close();}
});
