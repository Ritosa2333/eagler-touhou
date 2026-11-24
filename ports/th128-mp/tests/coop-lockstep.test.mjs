import assert from 'node:assert/strict';
import test from 'node:test';
import {makeCoopRules} from '../runtime/coop-rules.mjs';
import {createCoopLockstep} from '../runtime/coop-lockstep.mjs';
const rule=(players,window=2,delay=0)=>makeCoopRules({schema:'th128-coop/1',mode:'same-stage-coop',matchId:'room-run12',epoch:1,route:'A',players,difficulty:2,challengeMode:false,seed:927,inputDelay:delay,rollbackWindow:window,coreSHA256:'d'.repeat(64),resourceSHA256:'a'.repeat(64)});
function engine(n){let values=Array(n).fill(0);let frame=0;const owned=new Map();return {checkpoint(){const t={};owned.set(t,[frame,values.slice()]);return t;},release(t){assert.equal(owned.delete(t),true);},restore(t){const [f,v]=owned.get(t);frame=f;values=v.slice();},step(row){assert.equal(row.frame,frame++);assert.equal(row.inputs.length,n);row.inputs.forEach(({held,player},i)=>{assert.equal(i,player);values[i]+=held;});},read:()=>({frame,values,alloc:owned.size}),close(){assert.equal(owned.size,0);}};}
const input=(player,frame)=>[0,512,64,1,0,16,128,2][(player+frame)%8];
for(const n of [2,3])test(`${n} peers correct late edges, publish only confirmed contiguous rows, and replay exactly`,async()=>{
 const rules=rule(n),queue=[],engines=Array.from({length:n},()=>engine(n)),tapes=Array.from({length:n},()=>[]);
 const peers=await Promise.all(engines.map((e,i)=>createCoopLockstep({rules,localSeat:i,engine:e,send:msg=>queue.push([i,msg]),onConfirmed:r=>tapes[i].push(r)})));
 const fan=([from,msg])=>peers.forEach((p,i)=>{if(i!==from)p.receive(msg,from);});
 const flush=()=>queue.splice(0).forEach(fan);
 peers.forEach(p=>p.announce());flush();assert.ok(peers.every(p=>p.status().ready));let late;
 for(let frame=0;frame<10;frame++){
  peers.forEach((p,i)=>p.sample(input(i,frame)));for(const item of queue.splice(0)){if(item[0]===n-1&&frame===3)late=item;else fan(item);}
  peers.forEach(p=>assert.equal(p.advance(),true));if(frame===3){assert.equal(tapes[0].length,3);assert.ok(engines[0].read().alloc>0);}
  if(frame===4){fan(late);peers.forEach(p=>p.flush());}
 }
 assert.deepEqual(tapes[0].map(r=>r.frame),Array.from({length:10},(_,i)=>i));assert.equal(peers[0].status().replayed,2);
 const v=engine(n);tapes[0].forEach(r=>v.step(r));for(let i=0;i<n;i++){assert.deepEqual(engines[i].read(),v.read());assert.deepEqual(tapes[i],tapes[0]);peers[i].close();}
});
test('3P waits for every seat, rejects forged ownership / identity and handles delay-only without prediction',async()=>{
 const rules=rule(3,0,2),sent=[],first=engine(3),p=await createCoopLockstep({rules,localSeat:2,engine:first,send:x=>sent.push(x),onConfirmed(){}});p.announce();const hello=sent[0];
 assert.throws(()=>p.receive({...hello,seat:1},0));assert.throws(()=>p.receive({...hello,seat:0,key:'b'.repeat(64)},0));p.receive({...hello,seat:0},0);assert.equal(p.status().ready,false);p.receive({...hello,seat:1},1);
 p.sample(16);let packet=sent.at(-1);assert.equal(packet.frame,2);assert.equal(p.advance(),true);p.sample(32);assert.equal(p.advance(),true);p.sample(64);assert.equal(p.advance(),false);
 p.receive({...packet,seat:0,keys:128},0);assert.equal(p.advance(),false);p.receive({...packet,seat:1,keys:0},1);assert.equal(p.advance(),true);
 assert.deepEqual(first.read(),{frame:3,values:[128,0,16],alloc:0});assert.throws(()=>p.receive({...packet,seat:0,keys:128,frame:99999},0));p.close();
});
