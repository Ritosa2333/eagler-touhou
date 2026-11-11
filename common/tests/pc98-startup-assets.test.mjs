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

for(const game of ['02','04'])test(`TH${game} hands verified disk bytes to the game worker and rejects corrupt handoffs`,async()=>{
  const {gzipSync}=await import('node:zlib');
  const base=`../../ports/th${game}/web/preload-cache.js`;
  const parent=await import(`${base}?parent-test`),worker=await import(`${base}?worker-test`);
  const originalFetch=globalThis.fetch,originalContent=globalThis.__eaglerPC98Content,originalShared=globalThis.__eaglerPC98StartupAssets;
  const plain=new TextEncoder().encode('disk contents '+game),hash=createHash('sha256').update(plain).digest('hex');let requests=0;
  globalThis.__eaglerPC98Content={};globalThis.fetch=async()=>{requests++;return new Response(gzipSync(plain));};
  try{
    const path=`https://startup-test.invalid/${game}/${game==='02'?'th02-local-jp':'th04-coop-jp'}.hdi.gz`,opts={hash,size:plain.length,gzip:true};
    await parent.assetBytes(path,opts);const handoff=parent.takeManagedStartupAssets();assert.equal(handoff.length,1);assert.equal(parent.takeManagedStartupAssets().length,0);
    globalThis.__eaglerPC98StartupAssets=new Map(handoff);assert.deepEqual(await worker.assetBytes(path,opts),plain);assert.equal(requests,1,'worker should reuse the verified decompressed disk');
    const broken=await import(`${base}?broken-test`);const damaged=plain.slice();damaged[0]^=1;globalThis.__eaglerPC98StartupAssets=new Map([[handoff[0][0],damaged]]);
    assert.deepEqual(await broken.assetBytes(path,opts),plain);assert.equal(requests,2,'corrupt handoff must be discarded and reloaded');
  }finally{globalThis.fetch=originalFetch;globalThis.__eaglerPC98Content=originalContent;globalThis.__eaglerPC98StartupAssets=originalShared;}
});
