import test from'node:test';import assert from'node:assert/strict';import fs from'node:fs';
const source=fs.readFileSync(new URL('../../src/practice/Thprac128SectionData.hpp',import.meta.url),'utf8');
const rows=[...source.matchAll(/\{(\d+),(\d+),(\d+),(\d+),(\d+),(\d+)\}/g)].map(m=>m.slice(1).map(Number));
const words=source.slice(source.indexOf('words[]={')).match(/\d+(?=u)/g).map(Number);
test('every generated practice call satisfies the real VM terminated ASCII and zero padding contract',()=>{
 let count=0;const failures=[];
 for(const r of rows){let at=r[4],regions=words[at++];for(let i=0;i<regions;i++){at+=2;const size=words[at++];for(let j=0;j<size;j++){const h=words.slice(at,at+9);at+=9;const raw=words.slice(at,at+h[8]);at+=h[8];if(h[2]!==11)continue;count++;
 const bytes=Buffer.alloc(raw.length*4);raw.forEach((v,k)=>bytes.writeUInt32LE(v,k*4));const len=bytes.readUInt32LE(0),text=bytes.subarray(4,4+len),end=text.indexOf(0);if(end<1||end>=len||text.subarray(0,end).some(v=>v<32||v>126)||text.subarray(end).some(v=>v!==0))failures.push({section:r[0],identity:r[3],offset:h[0],text:bytes.toString('hex')});
 }}}
 assert(count>0);assert.deepEqual(failures,[],JSON.stringify(failures.slice(0,8)));
});
