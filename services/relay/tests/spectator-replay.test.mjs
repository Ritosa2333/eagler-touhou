import test from 'node:test';
import assert from 'node:assert/strict';
import {SpectatorReplayTransfer,parseSpectatorReplayChunk} from '../src/spectator-replay.mjs';
const session='0000000000000031';
function chunk(offset,total,length=3,id=session){const b=new Uint8Array(20+length),v=new DataView(b.buffer);b.set([0xe9,1,1,0]);v.setUint32(4,parseInt(id.slice(8),16),true);v.setUint32(8,parseInt(id.slice(0,8),16),true);v.setUint32(12,total,true);v.setUint32(16,offset,true);b.fill(7,20);return b;}
test('complete native Replay transfers preserve bytes and extension',()=>{
 const file=new SpectatorReplayTransfer(session);assert.equal(file.accept(chunk(0,6)),true);assert.equal(file.complete,false);
 assert.equal(file.accept(chunk(3,6)),true);assert.equal(file.complete,true);assert.equal(file.extended,true);
 const copy=new SpectatorReplayTransfer(session);for(const packet of file.chunks())assert.equal(copy.accept(packet),true);
 assert.deepEqual(copy.bytes,file.bytes);assert.equal(file.accept(chunk(0,6)),false);
});
test('stale generations, gaps, duplicates and excessive allocations cannot replace a Replay',()=>{
 const file=new SpectatorReplayTransfer(session);
 for(const packet of [chunk(3,6),chunk(0,6,3,'0000000000000032'),chunk(0,8*1024*1024+1)])assert.equal(file.accept(packet),false);
 assert.equal(file.bytes,null);assert.equal(file.accept(chunk(0,6)),true);
 for(const packet of [chunk(0,6),chunk(3,7),chunk(5,6,1)])assert.equal(file.accept(packet),false);
 assert.equal(file.received,3);assert.equal(file.accept(chunk(3,6)),true);
 assert.equal(parseSpectatorReplayChunk(new Uint8Array(20)),null);
});
