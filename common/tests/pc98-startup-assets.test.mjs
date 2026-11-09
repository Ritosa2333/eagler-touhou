import test from 'node:test';
import assert from 'node:assert/strict';
import {createHash,webcrypto} from 'node:crypto';
if(!globalThis.crypto)globalThis.crypto=webcrypto;
for(const game of ['02','04'])test(`TH${game} startup asset cache rejects corrupted bytes and isolates callers`,async()=>{
  const {assetBytes}=await import(`../../ports/th${game}/web/preload-cache.js`);
  const original=globalThis.fetch,good=new TextEncoder().encode('verified startup bytes '+game),bad=good.slice();bad[0]^=1;
  const hash=createHash('sha256').update(good).digest('hex');let corrupted=true,calls=0;
  globalThis.fetch=async()=>{calls++;return new Response(corrupted?bad:good);};
  try{
    const path=`https://startup-test.invalid/th${game}/verified.bin`;
    await assert.rejects(assetBytes(path,{hash,size:good.length}),/校验失败/);
    corrupted=false;const first=await assetBytes(path,{hash,size:good.length});first[0]^=1;
    assert.deepEqual(await assetBytes(path,{hash,size:good.length}),good);
    assert.equal(calls,2);
  }finally{globalThis.fetch=original;}
});
