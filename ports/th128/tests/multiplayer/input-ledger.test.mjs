import test from 'node:test';import assert from 'node:assert/strict';import fs from 'node:fs';
import {createMultiplayerRules} from '../../site/runtime/multiplayer-rules.mjs';
const url=new URL('../../site/runtime/multiplayer-input-ledger.mjs',import.meta.url),product=fs.existsSync(url)?await import(url):{};
function rules(changes={}){return createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId:'owned-ledger',epoch:1,mode:'parallel-stage',route:'A1-3',seed:34772,coreSHA256:'1'.repeat(64),resourceSHA256:'2'.repeat(64),inputDelay:0,rollbackWindow:4,...changes});}
async function make(options={}){assert.equal(typeof product.createMultiplayerInputLedger,'function','Actual rules-consuming input ledger missing');return product.createMultiplayerInputLedger({rules:rules(),localPlayer:0,allowedMask:763,...options});}
const packet=(player,frame,held,extra={})=>({matchId:'owned-ledger',epoch:1,player,frame,held,...extra});
const both=(ledger,frame,a,b=0)=>{ledger.receive(packet(0,frame,a),0);ledger.receive(packet(1,frame,b),1);};
const edges=f=>f.inputs.map(x=>[x.held,x.pressed,x.released,x.predicted]);
test('two players preserve independent native held/pressed/released edges',async()=>{
 const l=await make();both(l,0,1,128);assert.deepEqual(edges(l.consume()),[[1,1,0,false],[128,128,0,false]]);
 both(l,1,1,128);assert.deepEqual(edges(l.consume()),[[1,0,0,false],[128,0,0,false]]);
 both(l,2,0,64);assert.deepEqual(edges(l.consume()),[[0,0,1,false],[64,64,128,false]]);l.close();
});
test('local delay queues the future packet after deterministic neutral pre-roll',async()=>{
 const l=await make({rules:rules({inputDelay:2}),localPlayer:1});assert.equal(l.status().confirmedThrough,1);
 const queued=l.queueLocal(1);assert.deepEqual(queued.packet,packet(1,2,1));assert.equal(queued.receipt.duplicate,false);
 assert.deepEqual(edges(l.consume()),[[0,0,0,false],[0,0,0,false]]);assert.deepEqual(edges(l.consume()),[[0,0,0,false],[0,0,0,false]]);
 l.receive(packet(0,2,16),0);assert.deepEqual(edges(l.consume()),[[16,16,0,false],[1,1,0,false]]);l.close();
});
test('exact duplicate is idempotent while conflicting duplicate cannot overwrite the stored input',async()=>{
 const l=await make();const p=packet(0,0,1);l.receive(p,0);const status=l.status();assert.equal(l.receive({...p},0).duplicate,true);assert.deepEqual(l.status(),status);
 assert.throws(()=>l.receive({...p,held:2},0),/conflict/i);l.receive(packet(1,0,0),1);assert.equal(l.consume().inputs[0].held,1);l.close();
});
test('out of order packets advance only the common contiguous confirmation prefix',async()=>{
 const l=await make();both(l,2,16);assert.equal(l.status().confirmedThrough,-1);both(l,0,0);assert.equal(l.status().confirmedThrough,0);
 l.receive(packet(0,1,1),0);assert.equal(l.status().confirmedThrough,0);l.receive(packet(1,1,0),1);assert.equal(l.status().confirmedThrough,2);l.close();
});
test('prediction holds the previous input without producing another pressed edge',async()=>{
 const l=await make();both(l,0,1);l.consume();const a=l.consume(),b=l.consume();
 assert.deepEqual(edges(a),[[1,0,0,true],[0,0,0,true]]);assert.deepEqual(edges(b),[[1,0,0,true],[0,0,0,true]]);l.close();
});
test('late authoritative input requests the earliest correction and rebuilds following predicted release edges',async()=>{
 const l=await make();both(l,0,1);l.consume();l.consume();l.consume();
 l.receive(packet(0,2,16),0);l.receive(packet(0,1,0),0);assert.equal(l.status().dirtyFrame,1);assert.throws(()=>l.consume(),/correction/i);
 const c=l.prepareCorrection();assert.equal(c.fromFrame,1);assert.equal(c.toFrame,2);assert.deepEqual(c.frames.map(edges),[[[0,0,1,false],[0,0,0,true]],[[16,16,0,false],[0,0,0,true]]]);
 l.acceptCorrection(c);assert.equal(l.status().dirtyFrame,null);assert.equal(l.consume().inputs[0].held,16);l.close();
});
test('new authority invalidates a pending correction receipt and a foreign ledger cannot ACK it',async()=>{
 const l=await make(),other=await make();both(l,0,0);l.consume();l.consume();l.receive(packet(0,1,1),0);const old=l.prepareCorrection();
 l.receive(packet(1,1,16),1);assert.throws(()=>l.acceptCorrection(old),/stale/i);assert.throws(()=>other.acceptCorrection(old),/owner|receipt/i);
 l.acceptCorrection(l.prepareCorrection());assert.equal(l.status().dirtyFrame,null);other.close();l.close();
});
test('an equal late prediction confirms the stream without scheduling unnecessary correction',async()=>{
 const l=await make();both(l,0,1);l.consume();l.consume();both(l,1,1);assert.equal(l.status().dirtyFrame,null);assert.equal(l.status().confirmedThrough,1);assert.equal(l.prepareCorrection(),null);l.close();
});
test('rollback window stalls speculation and zero window requires both actual players',async()=>{
 const l=await make({rules:rules({rollbackWindow:2})});l.consume();l.consume();assert.throws(()=>l.consume(),/window|stall/i);assert.equal(l.status().nextFrame,2);l.close();
 const z=await make({rules:rules({rollbackWindow:0})});assert.throws(()=>z.consume(),/window|stall/i);both(z,0,16);assert.equal(z.consume().frame,0);assert.throws(()=>z.consume(),/window|stall/i);z.close();
});
test('wire identity, source slot, integer frames and capability masks fail closed without mutation',async()=>{
 const l=await make({allowedMask:761}),before=l.status();
 for(const p of [packet(0,0,2),packet(0,-1,1),packet(2,0,1),packet(0,0,256),packet(0,0,1,{epoch:2}),packet(0,0,1,{matchId:'foreign'}),packet(0,0,1,{frame:1.1}),packet(0,0,65536),packet(0,0,1,{extra:true})])assert.throws(()=>l.receive(p,0));
 assert.throws(()=>l.receive(packet(0,0,1),1),/source/i);assert.deepEqual(l.status(),before);l.close();
});
test('wire accessors are rejected without invoking getters and accepted inputs are detached',async()=>{
 const l=await make(),p=packet(0,0,1);Object.defineProperty(p,'held',{enumerable:true,get(){assert.fail('Packet getter executed');}});
 assert.throws(()=>l.receive(p,0),/data|property/i);const owned=packet(0,0,16);l.receive(owned,0);owned.held=128;l.receive(packet(1,0,0),1);assert.equal(l.consume().inputs[0].held,16);l.close();
});
test('rules must come from the real validated factory and allowed mask cannot invent native bits',async()=>{
 assert.equal(typeof product.createMultiplayerInputLedger,'function');
 await assert.rejects(make({rules:{...rules()}}),/canonical|rules/i);await assert.rejects(make({allowedMask:256}),/mask/i);await assert.rejects(make({localPlayer:2}),/player/i);
});
test('future receive window is bounded and a peer cannot allocate arbitrary far future input',async()=>{
 const l=await make();assert.throws(()=>l.receive(packet(0,61,1),0),/future/i);assert.equal(l.status().storedPackets,0);l.receive(packet(0,60,1),0);assert.equal(l.status().storedPackets,1);l.close();
});
test('confirmed export is a detached bounded readonly chunk for Replay or spectators',async()=>{
 const l=await make();both(l,0,1);l.consume();l.consume();const c=l.exportConfirmed();
 assert.equal(c.fromFrame,0);assert.equal(c.toFrame,0);assert.equal(c.frames.length,1);assert.equal(c.completeReplay,false);assert.equal(c.durable,false);assert(Object.isFrozen(c));assert(Object.isFrozen(c.frames[0].inputs[0]));assert.throws(()=>{c.frames[0].inputs[0].held=7;});
 both(l,1,0);l.acceptCorrection(l.prepareCorrection());const later=l.exportConfirmed();assert.equal(later.toFrame,1);assert.equal(c.toFrame,0);l.close();
});
test('dirty input cannot be exported as confirmed Replay before correction acceptance',async()=>{
 const l=await make();both(l,0,0);l.consume();l.consume();both(l,1,16);assert.throws(()=>l.exportConfirmed(),/correction/i);l.acceptCorrection(l.prepareCorrection());assert.equal(l.exportConfirmed().frames[1].inputs[0].held,16);l.close();
});
test('retention releases old packets and used frames over many steps while preserving boundary held edges',async()=>{
 const l=await make({rules:rules({rollbackWindow:3})});
 for(let frame=0;frame<300;frame++){both(l,frame,frame<299?1:0);const row=l.consume();if(frame===299)assert.deepEqual([row.inputs[0].pressed,row.inputs[0].released],[0,1]);assert(l.status().storedPackets<=6);assert(l.status().usedFrames<=3);}
 assert.equal(l.status().floorFrame,297);assert.throws(()=>l.receive(packet(0,1,1),0),/retained|old/i);assert.equal(l.exportConfirmed().frames.length,3);l.close();
});
test('close rejects late input and consumption and clears retained owned buffers',async()=>{
 const l=await make();both(l,0,1);l.consume();l.close();l.close();assert.equal(l.status().closed,true);assert.equal(l.status().storedPackets,0);assert.equal(l.status().usedFrames,0);
 assert.throws(()=>l.receive(packet(0,1,1),0),/closed/i);assert.throws(()=>l.consume(),/closed/i);assert.throws(()=>l.exportConfirmed(),/closed/i);
});
test('synthetic deterministic consumers converge after ledger correction without claiming native World rollback',async()=>{
 const predicted=await make({rules:rules({rollbackWindow:8})}),exact=await make({rules:rules({rollbackWindow:8})});
 const stream=[1,1,0,16,16,0],step=(v,f)=>v+f.inputs[0].held*3+f.inputs[0].pressed*5-f.inputs[0].released*2;
 let a=0,b=0;const saved=[0];for(let frame=0;frame<stream.length;frame++){if(frame===0)both(predicted,frame,stream[frame]);a=step(a,predicted.consume());saved.push(a);both(exact,frame,stream[frame]);b=step(b,exact.consume());}
 for(let frame=1;frame<stream.length;frame++)both(predicted,frame,stream[frame]);const plan=predicted.prepareCorrection();a=saved[plan.fromFrame];for(const row of plan.frames)a=step(a,row);predicted.acceptCorrection(plan);
 assert.equal(a,b);assert.equal(predicted.status().confirmedThrough,5);predicted.close();exact.close();
});
