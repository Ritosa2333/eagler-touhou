import assert from 'node:assert/strict';
import test from 'node:test';
import {RollbackSession} from '../web/sdl-runtime/rollback-session.mjs';

function fixture(){
 let world=0,serial=0;const snapshots=new Map(),commits=[];
 const core={save(){snapshots.set(++serial,world);return serial;},load(id){assert.ok(snapshots.has(id));world=snapshots.get(id);},release(id){snapshots.delete(id);},canPredict(){return true;},step(frame,left,right){world+=left.keys+right.keys;return {world,sounds:[frame]};}};
 return {core,snapshots,commits,value:()=>world,session:new RollbackSession(core,{window:8,commit:(frame,effects)=>commits.push({frame,world:effects.world})})};
}
const input=keys=>({keys,direct:0,x:0,y:0});
test('late remote input restores the native checkpoint and commits corrected frames once',()=>{
 const f=fixture(),s=f.session;
 assert.equal(s.advance(input(1)),true);
 assert.equal(s.advance(input(2)),true);
 assert.equal(f.commits.length,0);
 s.receive(0,input(4));s.receive(1,input(8));s.reconcile();
 assert.equal(f.value(),15);
 assert.deepEqual(f.commits.map(row=>row.frame),[0,1]);
 assert.equal(s.rollbacks,1);
 s.reconcile();assert.equal(f.commits.length,2);
});
test('prediction does not repeat a Bomb, pause edge or direct displacement',()=>{
 const {session:s}=fixture();
 s.receive(0,{keys:4|1024|1|16,direct:1,x:30,y:40});
 assert.ok(s.advance(input(0))&&s.advance(input(0)));
 assert.deepEqual(s.records.get(1).remote,input(1|16));
});
test('out-of-order input is exact only at its own frame until the prefix catches up',()=>{
 const {session:s,commits}=fixture();
 s.receive(0,input(16));s.receive(2,input(32));
 for(let i=0;i<4;i++)assert.ok(s.advance(input(0)));
 assert.deepEqual([1,2,3].map(f=>s.records.get(f).remote.keys),[16,32,16]);
 s.receive(1,input(16));s.reconcile();
 assert.equal(s.resimulated,1);
 assert.equal(s.records.get(3).remote.keys,32);
 s.receive(3,input(32));s.reconcile();
 assert.deepEqual(commits.map(x=>x.world),[16,32,64,96]);
 assert.equal(s.rollbacks,1);
});
test('repeated short taps release speculatively; a confirmed long hold cancels the estimate',()=>{
 const {session:s}=fixture(),words=[16,16,0,32,32,0,16,16,0,17];
 for(let f=0;f<words.length;f++){s.receive(f,input(words[f]));assert.ok(s.advance(input(0)));}
 for(let f=10;f<=13;f++)assert.ok(s.advance(input(0)));
 assert.deepEqual([10,11,12,13].map(f=>s.records.get(f).remote.keys),[17,17,1,1]);
 for(let f=10;f<=12;f++)s.receive(f,input(17));
 s.reconcile();
 assert.equal(s.records.get(13).remote.keys,17);
 assert.equal(s.resimulated,2);
});
test('conflicting input and nonfinite touch data are rejected',()=>{
 const s=fixture().session;s.receive(0,input(1));
 assert.throws(()=>s.receive(0,input(2)),/Conflicting/);
 assert.throws(()=>s.receive(1,{...input(0),x:NaN}),/Invalid input/);
 assert.throws(()=>s.receive(301,input(0)),/Invalid input frame/);
});
test('unconfirmed resource transitions do not advance simulation',()=>{
 const f=fixture();f.core.canPredict=()=>false;
 assert.equal(f.session.advance(input(1)),false);assert.equal(f.value(),0);
 f.session.receive(0,input(2));assert.equal(f.session.advance(input(1)),true);
 assert.equal(f.value(),3);
});
