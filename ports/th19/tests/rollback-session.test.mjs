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
 const f=fixture();f.session.previousRemote={keys:4|1024|1|16,direct:1,x:30,y:40};
 assert.deepEqual(f.session.predict(0),{keys:1|16,direct:0,x:0,y:0});
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
