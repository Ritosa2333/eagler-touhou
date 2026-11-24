import test from'node:test';import assert from'node:assert/strict';import fs from'node:fs';
import{decodeReplay,encodeReplay}from'../../site/runtime/replay.mjs';import{OwnedReplayStore}from'../../site/runtime/sound.mjs';import{createThprac128Session}from'../../site/runtime/thprac128.mjs';import{createReplayIDB,replayFile}from'../launcher-session/replay-idb.mjs';
const url=new URL('../../site/runtime/thprac128-replay.mjs',import.meta.url),p=fs.existsSync(url)?await import(url):{};const method=n=>{assert.equal(typeof p[n],'function','Actual practice Replay '+n+' missing');return p[n];};
const identity={coreSHA256:'1'.repeat(64),resourceSHA256:'2'.repeat(64)},session=createThprac128Session({section:10102,score:0,motivation:100000,perfect_freeze:30000,ice_area:0});
function file(){const d=decodeReplay(replayFile()),v=new DataView(d.stages[0].rawHeader.buffer);d.stages[0].rawHeader[0]=1;v.setUint32(128,100000,true);v.setUint32(132,30000,true);return encodeReplay(d);}
test('actual original RPY payload and vanilla USER blocks survive a source-correct appended PRAC JSON block',()=>{
 const original=file(),prior=decodeReplay(original),result=method('attachThprac128Replay')(original,session,identity),decoded=decodeReplay(result),metadata=method('readThprac128Replay')(result,identity);
 assert.deepEqual(decoded.rawPayload,prior.rawPayload);assert.equal(decoded.userBlocks.length,prior.userBlocks.length+1);assert.equal(decoded.userBlocks.at(-1).kind,0x43415250);assert.equal(String.fromCharCode(...decoded.userBlocks.at(-1).raw.slice(0,4)),'USER');assert.equal(decoded.userBlocks.at(-1).raw.length%4,0);assert.equal(decoded.userBlocks.at(-1).data.at(-1),0);assert.deepEqual(metadata.session.params,session.params);assert.equal(metadata.playLock,true);assert.equal(metadata.sourceAuthentication,false);
 for(let i=0;i<prior.userBlocks.length;i++)assert.deepEqual(decoded.userBlocks[i].raw,prior.userBlocks[i].raw);
});
test('metadata-free ordinary files remain ordinary, wrong identity and duplicate/corrupt PRAC are refused',()=>{
 assert.equal(method('readThprac128Replay')(file(),identity),null);const output=method('attachThprac128Replay')(file(),session,identity);assert.throws(()=>method('attachThprac128Replay')(output,session,identity),/duplicate|already|PRAC/i);assert.throws(()=>method('readThprac128Replay')(output,{...identity,coreSHA256:'3'.repeat(64)}),/identity|core|revision/i);const changed=output.slice();changed[changed.length-1]=7;assert.throws(()=>method('readThprac128Replay')(changed,identity),/padding|PRAC|JSON|trailing/i);
});
test('practice stage/rank/initial fields must agree with recorded RPY rather than metadata labels',()=>{
 method('attachThprac128Replay');for(const bad of[createThprac128Session({stage:2,section:10302}),createThprac128Session({},0),createThprac128Session({motivation:123})])assert.throws(()=>p.attachThprac128Replay(file(),bad,identity),/stage|rank|initial|resource/i);
});
test('actual 25-slot strict-CAS store saves and reopens full practice RPY metadata with no ordinary score changes',async()=>{
 const idb=createReplayIDB(),store=new OwnedReplayStore(idb),bytes=method('attachThprac128Replay')(file(),session,identity);assert.equal(typeof store.saveThprac,'function','Actual practice file store consumer missing');
 const saved=await store.saveThprac(4,bytes,{expectedDigest:null,isCurrent:()=>true});assert.equal(saved.strictFullReadback,true);assert.equal(saved.strictDurability,true);assert.equal(saved.scoreProfileImported,false);assert.deepEqual(await store.export(4),bytes);assert.equal((await store.observe())[4].source,'thprac-rpy');assert.equal((await store.observe()).filter(Boolean).length,1);
 await assert.rejects(store.saveThprac(4,bytes,{expectedDigest:'0'.repeat(64),isCurrent:()=>true}),/digest|conflict/i);store.close();
});
test('closed/cancelled practice save owner cannot mutate an actual synthetic transaction',async()=>{
 const idb=createReplayIDB(),store=new OwnedReplayStore(idb),bytes=method('attachThprac128Replay')(file(),session,identity);assert.equal(typeof store.saveThprac,'function');await assert.rejects(store.saveThprac(1,bytes,{expectedDigest:null,isCurrent:()=>false}),/cancel|owner|abort/i);assert.equal(idb.counts.writes,0);store.close();
});
test('practice playback restores the same session and uses practice-specific native bind with playLock',()=>{
 const bytes=method('attachThprac128Replay')(file(),session,identity);assert.equal(typeof p.Thprac128ReplayPlayback,'function','Actual practice playback consumer missing');const owner=new p.Thprac128ReplayPlayback(bytes,4,identity),selection=owner.firstSelection();assert.deepEqual(selection.thprac.params,session.params);assert.equal(selection.stage,1);assert.equal(selection.seed,34772);assert.equal(owner.playLock,true);
 const calls=[],m={HEAPU8:new Uint8Array(1024),_malloc:()=>32,_free:x=>calls.push(['free',x]),UTF8ToString:x=>x,_th128_world_error:()=>'',_th128_world_thprac_replay_playback_load:(ptr,n)=>{calls.push(['bind',ptr,n]);return 1;},_th128_world_replay_playback_state:()=>JSON.stringify({active:true,stage:1,rank:1,generation:9,frames:2,consumed:0,complete:false,auxiliaryKnown:true,auxiliaryLive:false}),_th128_world_replay_playback_cancel:()=>{calls.push(['cancel']);return 1;}};
 owner.bindNativeStage(m);assert.equal(calls[0][0],'bind');assert.equal(owner.snapshot().consumed,0);const next=owner.restartSelected();assert.deepEqual(next.firstSelection().thprac.params,session.params);owner.cancel();assert.equal(calls.at(-1)[0],'cancel');assert.throws(()=>owner.stagePacket(),/closed|owner|cancel/i);
});
