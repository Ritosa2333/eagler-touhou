import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {decodeReplay,encodeReplay} from '../../site/runtime/replay.mjs';
import {nativeReplayToFile,OwnedReplayPlayback,OwnedReplayStore} from '../../site/runtime/sound.mjs';
import {readTouchReplay,nativeTouchReplayBlock} from '../../site/runtime/touch-replay.mjs';
import {readThprac128Replay,Thprac128ReplayPlayback} from '../../site/runtime/thprac128-replay.mjs';
import {createReplayIDB} from '../launcher-session/replay-idb.mjs';
const original=new Uint8Array(readFileSync('D:/Codex-th128-owned-20261108/menu-lifecycle-fixes-20261113/case1099-ordinary-original-MSG-legal-touch-RED/run/ordinary-aux-native.bin'));
const source=decodeReplay(nativeReplayToFile(original));
const actualAuxiliary=[[11806,513,1,0],[11807,512,0,1],[11808,544,32,0]];
function trailer(rows){const words=[0x31484354,2,rows.length];for(const row of rows)words.push(row.stage,row.worldFrames,row.events.length,row.auxiliary.length,...row.events.flat(),...row.auxiliary.flat());const b=new Uint8Array(words.length*4),v=new DataView(b.buffer);words.forEach((n,i)=>v.setUint32(i*4,n,true));return b;}
function ordinary(auxiliary=actualAuxiliary,worldFrames=11809){const old=readTouchReplay(source)[0],tail=trailer([{...old,worldFrames,auxiliary}]),prefix=original.subarray(0,36+source.rawPayload.length),native=new Uint8Array(prefix.length+tail.length);native.set(prefix);native.set(tail,prefix.length);return native;}
test('actual11809World/11806main native history preserves original6B inputs and explicit3MSGauxiliary rows',()=>{
 const decoded=decodeReplay(nativeReplayToFile(ordinary())),plan=readTouchReplay(decoded)[0];
 assert.deepEqual(decoded.rawPayload,source.rawPayload);assert.equal(decoded.stages[0].numFrames,11806);
 assert.deepEqual(plan.auxiliary,actualAuxiliary);assert.equal(plan.worldFrames,11809);assert.deepEqual(plan.events,readTouchReplay(source)[0].events);
 const packet=new OwnedReplayPlayback(nativeReplayToFile(ordinary()),9).stagePacket(),base=144+11806*6,v=new DataView(packet.buffer);
 assert.deepEqual(packet.subarray(0,144),source.stages[0].rawHeader);assert.deepEqual(packet.subarray(144,base),source.stages[0].inputRaw);
 assert.deepEqual([0,1,2,3].map(i=>v.getUint32(base+i*4,true)),[0x58554150,11809,3,0]);
 assert.deepEqual(Array.from({length:3},(_,i)=>Array.from({length:4},(_,k)=>v.getUint32(base+16+i*16+k*4,true))),actualAuxiliary);
});
test('ordinary v2USER full strictIDB save close reopen retains complete original inputs and auxiliary bytes',async()=>{
 const native=ordinary(),expected=nativeReplayToFile(native),db=createReplayIDB(),store=new OwnedReplayStore(db);
 const saved=await store.save(12,native,{expectedDigest:null});assert.equal(saved.strictFullReadback,true);assert.equal(saved.strictDurability,true);store.close();
 const reopened=new OwnedReplayStore(db),read=await reopened.export(12);assert.deepEqual(read,expected);assert.deepEqual(readTouchReplay(decodeReplay(read))[0].auxiliary,actualAuxiliary);reopened.close();
});
test('auxiliary mismatched World extent unordered frames and non16bit input words are rejected without accepting partial history',()=>{
 for(const [rows,frames] of [[actualAuxiliary,11808],[[actualAuxiliary[1],actualAuxiliary[0],actualAuxiliary[2]],11809],[[[11806,65536,0,0],actualAuxiliary[1],actualAuxiliary[2]],11809],[[[11809,1,0,0],actualAuxiliary[1],actualAuxiliary[2]],11809]])assert.throws(()=>nativeReplayToFile(ordinary(rows,frames)));
 const legacy=readTouchReplay(source)[0];assert.equal(Object.hasOwn(legacy,'auxiliary'),false);assert.equal(new OwnedReplayPlayback(nativeReplayToFile(original),9).stagePacket().length,144+11806*6);
});
test('practice duplicate TCH1 and PRAC auxiliary owners must exactly agree before binding any native module',()=>{
 const bytes=new Uint8Array(readFileSync('D:/Codex-th128-owned-20261108/menu-lifecycle-fixes-20261113/case1052-actual-practice-aggregate-native-green/run/aggregate.rpy'));
 const decoded=decodeReplay(bytes),metadata=readThprac128Replay(bytes),plans=metadata.stages.map((p,i)=>({stage:decoded.stages[i].stage,worldFrames:p.worldFrames,events:[],auxiliary:p.auxiliary}));
 assert(plans[0].auxiliary.length>0);const block=nativeTouchReplayBlock(trailer(plans),decoded.stages);decoded.userBlocks.splice(decoded.userBlocks.length-1,0,block);
 assert.doesNotThrow(()=>new Thprac128ReplayPlayback(encodeReplay(decoded),9));
 const bad=plans.map(p=>({...p,auxiliary:p.auxiliary.map(r=>r.slice())}));bad[0].auxiliary[0][1]^=1;
 decoded.userBlocks[decoded.userBlocks.length-2]=nativeTouchReplayBlock(trailer(bad),decoded.stages);
 assert.throws(()=>new Thprac128ReplayPlayback(encodeReplay(decoded),9),/auxiliary Replay owners differ/);
});
