import test from 'node:test';
import assert from 'node:assert/strict';
for(const game of ['th02','th04']) test(`${game} portable checkpoint delta restores aligned and unaligned buffers`,async()=>{
  const {encodeCheckpoint,decodeCheckpoint}=await import(`../../ports/${game}/web/replay/portable.js`);
  const view=(size,offset,seed)=>{const a=new Uint8Array(size+offset);for(let i=0;i<a.length;i++)a[i]=(i*31+seed)&255;return a.subarray(offset);};
  const base={buffers:[view(65539,0,4),view(129,1,6),view(79,3,9),view(0,0,0)]};
  const target={version:'test',buffers:[view(65539,0,12),view(129,1,8),view(79,2,5),view(0,0,0)]};
  const encoded=await encodeCheckpoint(target,base),decoded=await decodeCheckpoint(encoded,base);
  assert.deepEqual(decoded.buffers,target.buffers);
  const absolute=await decodeCheckpoint(await encodeCheckpoint(target));
  assert.deepEqual(absolute.buffers,target.buffers);
});
