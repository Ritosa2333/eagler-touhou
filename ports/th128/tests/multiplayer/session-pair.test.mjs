// JSON messages cross two actual session/rules/ledger owners; no real network or native World.
import test from 'node:test';import assert from 'node:assert/strict';
import {createMultiplayerRules} from '../../site/runtime/multiplayer-rules.mjs';
import {createMultiplayerSessionOwner} from '../../site/runtime/multiplayer-session.mjs';
const wire=()=>({schema:'th128-multiplayer-rules/1',matchId:'json-pair',epoch:10,mode:'parallel-stage',route:'C2-3',seed:34772,coreSHA256:'1'.repeat(64),resourceSHA256:'2'.repeat(64),inputDelay:2,rollbackWindow:4});
const transfer=(a,b)=>{for(const m of JSON.parse(JSON.stringify(a.drainMessages())))b.receiveRestart(m,m.player);};
async function pair(extraA={},maskB=763){return Promise.all([createMultiplayerSessionOwner({rules:createMultiplayerRules(wire()),allowedMask:763,localPlayer:0,...extraA}),createMultiplayerSessionOwner({rules:createMultiplayerRules(wire()),allowedMask:maskB,localPlayer:1})]);}
async function round(a,b,id){a.requestRestart(id);b.requestRestart(id);transfer(a,b);transfer(b,a);await Promise.all([a.waitRestart(),b.waitRestart()]);transfer(a,b);transfer(b,a);}
test('successive serialized restart rounds derive fresh next targets and commit once per epoch',async()=>{
 const [a,b]=await pair();for(let i=1;i<=6;i++){await round(a,b,'restart-'+i);for(const s of [a,b]){assert.equal(s.rules.epoch,10+i);assert.equal(s.rules.stageId,15);assert.equal(s.rules.seed,34772);assert.equal(s.status().restartCommits,i);assert.equal(s.status().attempt,1);assert.deepEqual(s.consume().inputs.map(x=>x.held),[0,0]);}}
 assert.equal(a.status().outbox,0);assert.equal(b.status().outbox,0);a.close();b.close();
});
test('restart negotiation compares capability mask even when both initial rule fingerprints match',async()=>{
 const [a,b]=await pair({},761);assert.equal(a.status().fingerprint,b.status().fingerprint);const av=a.requestRestart('mask'),bv=b.requestRestart('mask');
 assert.throws(()=>b.receiveRestart(JSON.parse(JSON.stringify(av)),0),/mask/i);assert.throws(()=>a.receiveRestart(JSON.parse(JSON.stringify(bv)),1),/mask/i);
 assert.deepEqual(a.status().votes,[true,false]);assert.deepEqual(b.status().votes,[false,true]);assert.equal(a.rules.epoch,10);assert.equal(b.rules.epoch,10);a.close();b.close();
});
test('a failed preparation can be explicitly retried under a new attempt without adopting late READY',async()=>{
 let fail=true;const [a,b]=await pair({prepareEpoch:async()=>{if(fail){fail=false;throw Error('authored once admission failure');}}});
 a.requestRestart('first');b.requestRestart('first');transfer(a,b);transfer(b,a);await Promise.all([a.waitRestart(),b.waitRestart()]);
 transfer(a,b);const obsolete=b.drainMessages();assert.equal(a.rules.epoch,10);assert.equal(b.rules.epoch,10);for(const m of obsolete)assert.throws(()=>a.receiveRestart(m,1),/attempt|stale/i);
 await round(a,b,'second');assert.equal(a.rules.epoch,11);assert.equal(b.rules.epoch,11);assert.equal(a.status().restartCommits,1);a.close();b.close();
});
