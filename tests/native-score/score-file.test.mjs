import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import{profileFixture,expectedCanonical,referenceFile,SIZE,ST,CD}from'./score-fixture.mjs';
const url=new URL('../../site/runtime/native-score-file.mjs',import.meta.url),product=fs.existsSync(url)?await import(url):{};
function api(name){assert.equal(typeof product[name],'function',name+' actual native score codec is missing');return product[name];}
test('independently authored T821 file decodes to memory routes/ST/CD with every field preserved',()=>{
 const decode=api('decodeNativeScoreFile'),file=referenceFile(),copy=file.slice(),p=decode(file);
 assert.deepEqual(p,expectedCanonical());assert.deepEqual(file,copy);assert.equal(p[ST+100],17);assert.equal(p[CD+200],23);
});
test('encode writes real T821 header, corrected block receipts and native file block order',()=>{
 const encode=api('encodeNativeScoreFile'),decode=api('decodeNativeScoreFile'),p=profileFixture(),before=p.slice(),file=encode(p),v=new DataView(file.buffer,file.byteOffset,file.byteLength);
 assert.deepEqual([...file.subarray(0,4)],[84,56,50,49]);assert.equal(v.getUint32(4,true),file.length);
 assert.equal(v.getUint32(12,true),0x100);assert.equal(v.getUint32(16,true),file.length-24);assert.equal(v.getUint32(20,true),52232);
 assert.deepEqual(decode(file),expectedCanonical());assert.deepEqual(p,before);
});
test('decoder owns the original file and returns detached profile bytes',()=>{
 const decode=api('decodeNativeScoreFile'),a=referenceFile(),p=decode(a);a.fill(0);assert.deepEqual(p,expectedCanonical());
 const b=referenceFile(),one=decode(b);one.fill(0);assert.deepEqual(decode(b),expectedCanonical());
});
for(const[name,mutate]of [
 ['magic',f=>f[0]^=1],
 ['declared total size',f=>new DataView(f.buffer).setUint32(4,f.length+1,true)],
 ['version',f=>new DataView(f.buffer).setUint32(12,0x101,true)],
 ['compressed size',f=>new DataView(f.buffer).setUint32(16,f.length,true)],
 ['inflated size',f=>new DataView(f.buffer).setUint32(20,52231,true)]
])test('malformed native score '+name+' is rejected',()=>{
 const decode=api('decodeNativeScoreFile');assert.throws(()=>decode(referenceFile({headerChange:mutate})),/score|header|size|magic|version|container/i);
});
for(const[name,mutate]of [
 ['route checksum',a=>a[30]^=1],
 ['route order',a=>new DataView(a.buffer).setUint32(12,7,true)],
 ['CD identity',a=>a[13152]^=1],
 ['CD size',a=>new DataView(a.buffer).setUint32(13152+8,38011,true)],
 ['ST checksum',a=>a[51164+4]^=1]
])test('native score '+name+' is checked independently of encryption',()=>{
 const decode=api('decodeNativeScoreFile');assert.throws(()=>decode(referenceFile({blockChange:mutate})),/score|checksum|block|route|identity|size/i);
});
test('oversized, wrong-type and truncated native files fail within the fixed profile domain',()=>{
 const decode=api('decodeNativeScoreFile'),encode=api('encodeNativeScoreFile');
 for(const data of [null,[],new Uint8Array(23),referenceFile().slice(0,-1),new Uint8Array(2*1024*1024+25)])assert.throws(()=>decode(data),/score|size|byte|type|header/i);
 for(const p of [new Uint8Array(52231),new Uint8Array(52233),new Uint8Array(SIZE)])assert.throws(()=>encode(p),/score|profile|size|block|identity/i);
});
test('native score payload and checksum mutation never writes back to the source profile',()=>{
 const encode=api('encodeNativeScoreFile'),decode=api('decodeNativeScoreFile'),p=profileFixture();p[CD+250]=89;
 const before=p.slice(),file=encode(p),loaded=decode(file);assert.equal(loaded[CD+250],89);assert.deepEqual(p,before);
 for(const offset of [20,1644+20,ST+100,CD+200])assert.equal(loaded[offset],p[offset]);
});
