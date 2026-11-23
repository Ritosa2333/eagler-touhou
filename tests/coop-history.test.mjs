import test from 'node:test';import assert from 'node:assert/strict';
import {openTh128ReplayPlayback} from '../runtime/replay-playback.mjs';
import {makeCoopRules} from '../runtime/coop-rules.mjs';
import {createConfirmedHistory,readMultiplayerReplay} from '../runtime/confirmed-history.mjs';
import {createTh128SpectatorEncoder,createTh128SpectatorDecoder} from '../runtime/spectator-wire.mjs';
const build={coreSHA256:'a'.repeat(64),resourceSHA256:'3'.repeat(64)},matchId='local-coop-3';
import {openConfirmedTimeline} from '../runtime/confirmed-history.mjs';
for(const players of [2,3])test(`sealed ${players}-seat cooperation replay and native spectator packets retain all seats`,async()=>{
 const rules=makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId,epoch:1,players,route:'EX',difficulty:4,challengeMode:true,inputDelay:0,rollbackWindow:2,seed:482,...build});
 const log=await createConfirmedHistory({rules,allowedMask:763}),wire=await createTh128SpectatorEncoder(log.header,build),viewer=createTh128SpectatorDecoder({build,matchId,expectedRules:rules});
 const old=structuredClone(log.header);assert.equal(old.schema,'th128-coop-history/1');viewer.announce(wire.announce());const head=wire.open().slice(1);assert.equal(head[6],players);assert.equal(await viewer.accept(head),'checkpoint');const retained=viewer.retain();assert.equal(typeof retained.replayBytes,'function');assert.throws(()=>retained.replayBytes(),/sealed/i);
 let prev=Array(players).fill(0);for(let frame=0;frame<4;frame++){const keys=[frame&1?2:0,128,frame%3?1:64].slice(0,players);const row={frame,inputs:keys.map((held,player)=>({player,held,pressed:held&~prev[player],released:prev[player]&~held,predicted:false}))};prev=keys;log.append(row);const pkt=wire.frame(row).slice(1);assert.equal(pkt.byteLength,24+12*players);assert.equal(await viewer.accept(pkt),'frame');assert.deepEqual(viewer.read(frame),row);}
 await viewer.accept(wire.finish('completed').slice(1));log.seal('completed');const bytes=log.replayBytes(),file=JSON.parse(new TextDecoder().decode(bytes));assert.equal(file.schema,'th128-coop-replay/1');
 assert.deepEqual(retained.replayBytes(),bytes);const independent=await openConfirmedTimeline(old,build);assert.throws(()=>independent.replayBytes(),/sealed/i);const opened=await readMultiplayerReplay(bytes,build);assert.deepEqual(opened.read(3),viewer.read(3));assert.deepEqual(viewer.retain().status(),opened.status());
 if(players===3){const scene=await openTh128ReplayPlayback({tape:opened,openWorld:async()=>({engine:{step(){},close(){}}}),render(){}});try{assert.equal(scene.selectSeat(2),true);assert.equal(scene.status().seat,2);assert.equal(scene.status().seatCount,3);assert.throws(()=>scene.selectSeat(3));}finally{scene.close();}}
 for(const variant of [{...file,schema:'th128-mp-replay/1'},{...file,held:file.held.slice(0,-1)},{...file,header:{...file.header,schema:'th128-mp-history/1'}}])await assert.rejects(readMultiplayerReplay(new TextEncoder().encode(JSON.stringify(variant)),build));
 const mismatch=createTh128SpectatorDecoder({build,matchId,expectedRules:makeCoopRules({...old.rules,challengeMode:false})});mismatch.announce(wire.announce());await assert.rejects(mismatch.accept(head));
});
