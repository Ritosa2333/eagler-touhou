// Two actual protocol owners exchanging packets in authored JS queues. No network/native World.
import test from 'node:test';import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';import {fileURLToPath} from 'node:url';
import {createMultiplayerRules,multiplayerRulesFingerprint,acceptMultiplayerRules} from '../../site/runtime/multiplayer-rules.mjs';
import {createMultiplayerInputLedger} from '../../site/runtime/multiplayer-input-ledger.mjs';
const wire=()=>({schema:'th128-multiplayer-rules/1',matchId:'owned-pair',epoch:7,mode:'parallel-stage',route:'EX',seed:34772,coreSHA256:'1'.repeat(64),resourceSHA256:'2'.repeat(64),inputDelay:2,rollbackWindow:8});
async function pair(){
 const rules=createMultiplayerRules(wire()),fp=await multiplayerRulesFingerprint(rules),accepted=await acceptMultiplayerRules(rules,wire(),fp);
 return Promise.all([createMultiplayerInputLedger({rules,allowedMask:763,localPlayer:0}),createMultiplayerInputLedger({rules:accepted.rules,allowedMask:763,localPlayer:1})]);
}
test('two actual handshaken ledgers converge to the same confirmed input log after reversed delivery',async()=>{
 const [a,b]=await pair(),toA=[],toB=[],keysA=[1,1,16,0,0,0,0,0],keysB=[128,128,0,0,64,0,0,0];
 for(let f=0;f<8;f++){toB.push(a.queueLocal(keysA[f]).packet);toA.push(b.queueLocal(keysB[f]).packet);a.consume();b.consume();}
 for(const p of toA.reverse())a.receive(p,1);for(const p of toB.reverse())b.receive(p,0);
 assert.equal(a.status().dirtyFrame,2);assert.equal(b.status().dirtyFrame,2);
 a.acceptCorrection(a.prepareCorrection());b.acceptCorrection(b.prepareCorrection());
 const ca=a.exportConfirmed(),cb=b.exportConfirmed();assert.deepEqual(ca,cb);assert.equal(ca.frames.length,8);
 assert.deepEqual(ca.frames[4].inputs.map(x=>[x.held,x.pressed,x.released]),[[16,16,1],[0,0,128]]);
 assert.deepEqual(ca.frames[7].inputs.map(x=>[x.held,x.pressed,x.released]),[[0,0,0],[0,0,64]]);
 assert.equal(ca.toFrame,7);assert.equal(ca.completeReplay,false);a.close();b.close();
});
test('foreign epoch traffic cannot alter either live matched owner before or after closure',async()=>{
 const [a,b]=await pair();a.queueLocal(1);b.queueLocal(16);const old=a.status();
 assert.throws(()=>a.receive({matchId:'owned-pair',epoch:6,player:1,frame:2,held:16},1),/epoch/i);
 assert.deepEqual(a.status(),old);a.close();assert.throws(()=>a.receive({matchId:'owned-pair',epoch:7,player:1,frame:2,held:16},1),/closed/i);
 assert.equal(b.status().closed,false);assert.equal(b.consume().frame,0);b.close();
});
test('a read-only confirmed chunk cannot be changed to inject spectator input into either owner',async()=>{
 const [a,b]=await pair();a.consume();b.consume();const snapshot=a.exportConfirmed();
 assert.throws(()=>snapshot.frames.push({frame:1,inputs:[]}));assert.throws(()=>{snapshot.frames[0].inputs[0].held=512;});
 assert.equal(a.exportConfirmed().frames[0].inputs[0].held,0);assert.deepEqual(a.exportConfirmed(),b.exportConfirmed());a.close();b.close();
});
test('complete new protocol source modules parse without importing game or graphics',()=>{
 for(const rel of ['site/runtime/multiplayer-rules.mjs','site/runtime/multiplayer-input-ledger.mjs']){
 const file=fileURLToPath(new URL('../../'+rel,import.meta.url)),run=spawnSync(process.execPath,['--check',file],{encoding:'utf8',windowsHide:true});
 assert.equal(run.status,0,run.stderr);assert.equal(run.stderr,'');assert.equal(run.stdout,'');
 }
});
