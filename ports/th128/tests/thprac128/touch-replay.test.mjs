import test from'node:test';import assert from'node:assert/strict';
import{readFileSync}from'node:fs';import{decodeReplay,encodeReplay}from'../../site/runtime/replay.mjs';
import{nativeTouchReplayBlock,readTouchReplay,validTouchMotion}from'../../site/runtime/touch-replay.mjs';
function sample(events=[[0,1,-576,51200],[1,0,0,0]]){const rawHeader=new Uint8Array(144),h=new DataView(rawHeader.buffer);h.setUint16(0,3,true);h.setUint32(4,2,true);const stage={rawHeader,numFrames:2,stage:3},raw=new Uint8Array(24+events.length*16),v=new DataView(raw.buffer);[0x31484354,1,1,3,2,events.length,...events.flat()].forEach((n,i)=>v.setInt32(i*4,n,true));return{stage,raw};}
test('native touch trailer becomes aligned modern USER with exact signed command history',()=>{
 const{raw,stage}=sample(),block=nativeTouchReplayBlock(raw,[stage]);assert.equal(block.kind,0x31484354);assert.equal(block.raw.length%4,0);assert.equal(new DataView(block.raw.buffer).getUint32(0,true),0x52455355);
 assert.deepEqual(readTouchReplay({userBlocks:[block],stages:[stage]}),[{stage:3,worldFrames:2,events:[[0,1,-576,51200],[1,0,0,0]]}]);
});
test('legacy unlimited and swept unlimited stay distinct across RPY encoding',()=>{
 const events=[[0,2,-23552,4096],[1,4,23552,55296]],{stage,raw}=sample(events),block=nativeTouchReplayBlock(raw,[stage]);
 assert.deepEqual(readTouchReplay({userBlocks:[block],stages:[stage]})[0].events,events);
});
test('malformed touch frame order, raw domain and nonzero USER padding are rejected',()=>{
 for(const events of [[[1,1,0,51200],[0,0,0,0]],[[0,2,999999,51200]],[[2,0,0,0]],[[0,4,0,0]]]){const{raw,stage}=sample(events);assert.throws(()=>nativeTouchReplayBlock(raw,[stage]));}
 const{raw,stage}=sample(),b=nativeTouchReplayBlock(raw,[stage]);b.data[b.data.length-1]=1;assert.throws(()=>readTouchReplay({userBlocks:[b],stages:[stage]}));
});
test('legacy replay needs no touch API while duplicate and truncated touch trailers fail',()=>{
 assert.equal(nativeTouchReplayBlock(new Uint8Array(),[]),null);assert.equal(readTouchReplay({userBlocks:[],stages:[]}),null);
 const{raw,stage}=sample(),b=nativeTouchReplayBlock(raw,[stage]);assert.throws(()=>readTouchReplay({userBlocks:[b,b],stages:[stage]}));assert.throws(()=>nativeTouchReplayBlock(raw.subarray(0,raw.length-1),[stage]));
 assert.equal(validTouchMotion(2,-23552,4096),true);assert.equal(validTouchMotion(3,-32767,32767),true);assert.equal(validTouchMotion(1,0,0),false);
});
test('actual native touch RPY preserves original payload and both ordinary USER blocks before touch and PRAC',{skip:!process.env.TH128_TOUCH_REPLAY_SAMPLE&&'Set TH128_TOUCH_REPLAY_SAMPLE to run the archived native recording check'},()=>{
 const source=new Uint8Array(readFileSync(process.env.TH128_TOUCH_REPLAY_SAMPLE)),decoded=decodeReplay(source),original=decoded.rawPayload.slice();
 const block=(kind,data)=>{const raw=new Uint8Array(12),v=new DataView(raw.buffer);v.setUint32(0,0x52455355,true);v.setUint32(4,12+data.length,true);v.setUint32(8,kind,true);return{kind,headerRaw:raw,data:Uint8Array.from(data)};};
 const pair=[block(0,[1,2,3,0]),block(1,[255,7,0,0])];decoded.userBlocks.unshift(...pair);const read=decodeReplay(encodeReplay(decoded));
 assert.deepEqual(read.rawPayload,original);assert.deepEqual(read.userBlocks.map(b=>b.kind),[0,1,0x31484354,0x43415250]);for(let i=0;i<2;++i){assert.deepEqual(read.userBlocks[i].headerRaw,pair[i].headerRaw);assert.deepEqual(read.userBlocks[i].data,pair[i].data);}assert.equal(readTouchReplay(read)[0].worldFrames,11);
 decoded.userBlocks.push(decoded.userBlocks.at(-1));assert.throws(()=>encodeReplay(decoded));
});
