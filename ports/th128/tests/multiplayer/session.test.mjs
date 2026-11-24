import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';
import {createMultiplayerRules} from '../../site/runtime/multiplayer-rules.mjs';
const url=new URL('../../site/runtime/multiplayer-session.mjs',import.meta.url),product=fs.existsSync(url)?await import(url):{};
const wire=(epoch=1)=>({schema:'th128-multiplayer-rules/1',matchId:'owned-restart',epoch,mode:'parallel-stage',route:'EX',seed:34772,coreSHA256:'1'.repeat(64),resourceSHA256:'2'.repeat(64),inputDelay:0,rollbackWindow:4});
async function make(player=0,extra={}){assert.equal(typeof product.createMultiplayerSessionOwner,'function','Actual rules+ledger restart session owner missing');return product.createMultiplayerSessionOwner({rules:createMultiplayerRules(wire()),localPlayer:player,allowedMask:763,...extra});}
async function pair(extra={}){return Promise.all([make(0,extra.a),make(1,extra.b)]);}
function send(from,to){const messages=from.drainMessages();for(const m of messages)to.receiveRestart(m,m.player);return messages;}
async function votes(a,b){a.requestRestart('r1');b.requestRestart('r1');send(a,b);send(b,a);await Promise.all([a.waitRestart(),b.waitRestart()]);}
async function commit(a,b){await votes(a,b);send(a,b);send(b,a);}
const input=(player,frame,held,epoch=1)=>({matchId:'owned-restart',epoch,player,frame,held});
function hold(s){s.queueLocal(1);s.receiveInput(input(1,0,16),1);return s.consume();}
function deferred(){let resolve,reject;const promise=new Promise((a,b)=>{resolve=a;reject=b;});return{promise,resolve,reject};}
test('real session starts with the current immutable rules and independent actual input ledger',async()=>{
 const a=await make();assert.equal(a.rules.epoch,1);assert(Object.isFrozen(a.rules));assert.deepEqual(hold(a).inputs.map(x=>x.held),[1,16]);a.close();
});
test('one explicit local vote never implies peer consent or resets the active input owner',async()=>{
 const a=await make();hold(a);const vote=a.requestRestart('r1');assert.equal(vote.kind,'vote');assert.equal(a.status().epoch,1);assert.deepEqual(a.status().votes,[true,false]);
 assert.equal(a.consume().inputs[0].held,1);assert.equal(a.drainMessages().length,1);a.close();
});
test('two votes prepare real candidates but neither commits before the other READY',async()=>{
 const [a,b]=await pair();await votes(a,b);assert.equal(a.status().epoch,1);assert.equal(b.status().epoch,1);assert.deepEqual(a.status().ready,[true,false]);assert.throws(()=>a.consume(),/restart|prepar/i);
 send(a,b);assert.equal(b.status().epoch,2);assert.equal(a.status().epoch,1);send(b,a);assert.equal(a.status().epoch,2);a.close();b.close();
});
test('paired commit keeps stage seed and configuration while replacing old held input with a fresh ledger',async()=>{
 const [a,b]=await pair();hold(a);b.queueLocal(16);b.receiveInput(input(0,0,1),0);b.consume();await commit(a,b);
 for(const s of [a,b]){assert.equal(s.rules.epoch,2);assert.equal(s.rules.stageId,16);assert.equal(s.rules.difficulty,4);assert.equal(s.rules.seed,34772);assert.equal(s.rules.inputDelay,0);assert.deepEqual(s.consume().inputs.map(x=>x.held),[0,0]);assert.equal(s.status().restartCommits,1);s.close();}
});
test('duplicate vote and ready traffic is idempotent including after the matching epoch commits',async()=>{
 const [a,b]=await pair();const av=a.requestRestart('r1'),bv=b.requestRestart('r1');assert.equal(a.requestRestart('r1'),av);a.drainMessages();b.drainMessages();
 a.receiveRestart(bv,1);assert.equal(a.receiveRestart(bv,1).duplicate,true);b.receiveRestart(av,0);await Promise.all([a.waitRestart(),b.waitRestart()]);
 const ar=a.drainMessages()[0],br=b.drainMessages()[0];a.receiveRestart(br,1);b.receiveRestart(ar,0);
 assert.equal(a.receiveRestart(bv,1).duplicate,true);assert.equal(a.receiveRestart(br,1).duplicate,true);assert.equal(a.status().epoch,2);assert.equal(a.status().restartCommits,1);a.close();b.close();
});
test('foreign source conflicting request and unsupported mask cannot change the live vote set',async()=>{
 const a=await make();const v=a.requestRestart('r1'),before=a.status();
 for(const message of [{...v,player:1,requestId:'other'},{...v,player:1,allowedMask:761},{...v,player:1,toEpoch:3},{...v,player:1,extra:true},{...v,player:1,fingerprint:'0'.repeat(64)}])assert.throws(()=>a.receiveRestart(message,1));
 assert.throws(()=>a.receiveRestart({...v,player:1},0),/source/i);assert.deepEqual(a.status(),before);a.close();
});
test('an out of order READY cannot stand in for an explicit remote vote',async()=>{
 const [a,b]=await pair();const av=a.requestRestart('r1'),bv=b.requestRestart('r1');a.drainMessages();b.drainMessages();b.receiveRestart(av,0);await b.waitRestart();
 const br=b.drainMessages()[0];a.receiveRestart(br,1);assert.deepEqual(a.status().votes,[true,false]);assert.equal(a.status().epoch,1);
 a.receiveRestart(bv,1);await a.waitRestart();const ar=a.drainMessages()[0];assert.equal(a.status().epoch,2);b.receiveRestart(ar,0);assert.equal(b.status().epoch,2);a.close();b.close();
});
test('failed local host preparation sends abort and preserves both old real ledgers',async()=>{
 const [a,b]=await pair({a:{prepareEpoch:async()=>{throw Error('authored host admission refused');}}});hold(a);await votes(a,b);
 assert.equal(a.status().epoch,1);assert.match(a.status().lastError,/admission refused/);send(a,b);assert.equal(b.status().epoch,1);assert.equal(b.status().phase,'running');
 assert.equal(a.consume().inputs[0].held,1);a.close();b.close();
});
test('cancelled asynchronous preparation aborts its owner and cannot later adopt a candidate',async()=>{
 const gate=deferred();let signal;const [a,b]=await pair({a:{prepareEpoch:async p=>{signal=p.signal;await gate.promise;}}});
 a.requestRestart('r1');b.requestRestart('r1');send(a,b);send(b,a);a.cancelRestart();assert.equal(signal.aborted,true);gate.resolve();await a.waitRestart();send(a,b);
 assert.equal(a.status().epoch,1);assert.equal(a.status().phase,'running');assert.equal(b.status().epoch,1);a.close();b.close();
});
test('close during pending preparation rejects late completions input and messages',async()=>{
 const gate=deferred();let signal;const [a,b]=await pair({a:{prepareEpoch:async p=>{signal=p.signal;await gate.promise;}}});
 a.requestRestart('r1');b.requestRestart('r1');send(a,b);send(b,a);a.close();assert.equal(signal.aborted,true);gate.resolve();await a.waitRestart();
 assert.equal(a.status().closed,true);assert.equal(a.status().epoch,1);assert.throws(()=>a.receiveInput(input(1,1,16),1),/closed/i);assert.throws(()=>a.requestRestart('r2'),/closed/i);b.close();
});
test('prepared local READY cannot be revoked by a late local cancellation',async()=>{
 const [a,b]=await pair();await votes(a,b);assert.throws(()=>a.cancelRestart(),/ready|revoke/i);assert.equal(a.status().epoch,1);send(a,b);send(b,a);assert.equal(a.status().epoch,2);a.close();b.close();
});
test('abort raises attempt so stale votes cannot join a replacement request',async()=>{
 const [a,b]=await pair();const old=a.requestRestart('r1');send(a,b);a.cancelRestart();send(a,b);assert.equal(a.status().attempt,2);assert.equal(b.status().attempt,2);
 a.requestRestart('r2');b.requestRestart('r2');assert.throws(()=>b.receiveRestart(old,0),/attempt|stale/i);send(a,b);send(b,a);await Promise.all([a.waitRestart(),b.waitRestart()]);send(a,b);send(b,a);assert.equal(a.status().epoch,2);a.close();b.close();
});
test('old epoch input and an old correction receipt cannot affect the new ledger',async()=>{
 const [a,b]=await pair();a.consume();a.receiveInput(input(0,0,1),0);const old=a.prepareCorrection();await commit(a,b);
 assert.throws(()=>a.receiveInput(input(1,0,0),1),/epoch/i);assert.throws(()=>a.acceptCorrection(old),/owner|receipt/i);assert.equal(a.consume().inputs[0].held,0);a.close();b.close();
});
test('epoch overflow and a bounded undrained outbox fail without mutating the current ledger',async()=>{
 const end=await make(0,{rules:createMultiplayerRules(wire(2147483647))});assert.throws(()=>end.requestRestart('r1'),/epoch|exhaust/i);assert.equal(end.rules.epoch,2147483647);end.close();
 const a=await make();for(let i=0;i<3;i++){a.requestRestart('r'+i);a.cancelRestart();}const before=a.status();assert.throws(()=>a.requestRestart('overflow'),/outbox/i);assert.deepEqual(a.status(),before);assert.equal(a.drainMessages().length,6);assert.equal(a.requestRestart('after-drain').kind,'vote');a.close();
});
test('restart message accessors are rejected without executing caller getters',async()=>{
 const a=await make(),v={...a.requestRestart('r1'),player:1};Object.defineProperty(v,'requestId',{enumerable:true,get(){assert.fail('getter executed');}});
 assert.throws(()=>a.receiveRestart(v,1),/data|property/i);a.close();
});
