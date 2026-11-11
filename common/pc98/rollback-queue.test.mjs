import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {packTouch} from '../../ports/th02/web/touch-input.js';
const coreModule=await WebAssembly.compile(await readFile(new URL('./rollback-core.wasm',import.meta.url)));
for(const game of ['th02','th04']) {
 const {RollbackQueue}=await import(`../../ports/${game}/web/netplay/rollback-queue.js`);
 test(`${game}: delayed input, async completion and departure preserve the confirmed timeline`,()=>{
  const q=new RollbackQueue(0,3,{coreModule,directionPrediction:2,predictionLead:8});q.arm(0);
  for(let frame=0;frame<2;frame++){assert.deepEqual(q.peek(),[0,0,0]);q.commit(q.peek());q.prune();}
  q.capture(8);const speculative=q.peek();assert.deepEqual(speculative,[8,0,0]);
  q.receive(2,4,1);q.receive(2,0,2);q.commit(speculative);assert.equal(q.dirty,2);
  q.rewind(2);assert.deepEqual(q.peek(),[8,4,0]);q.commit(q.peek());q.prune();
  q.capture(0);q.receive(3,0,1);q.receive(3,8,2);q.commit(q.peek());q.prune();
  q.capture(0);q.receive(4,0,1);q.receive(4,8,2);q.commit(q.peek());q.prune();
  q.applyDeparture({frame:5,slots:[2],inputs:[[],[],[]]});
  for(let frame=5;frame<20;frame++){q.capture(0);q.receive(frame,0,1);assert.deepEqual(q.peek(),[0,0,0]);q.commit(q.peek());q.prune();}
  assert.equal(q.confirmed,20);
 });
 test(`${game}: direct touch arrives once and is not predicted`,()=>{
  const q=new RollbackQueue(0,2,{coreModule,directionPrediction:3,predictionLead:8});q.arm(0);
  for(let frame=0;frame<2;frame++)q.commit(q.peek());
  q.capture(0);const input=packTouch(32,{active:true,begin:true,x:123,y:-45});q.receive(2,input,1);assert.deepEqual(q.peek(),[0,input]);q.commit(q.peek());
  q.capture(0);assert.deepEqual(q.peek(),[0,32]);q.commit(q.peek());
 });
}
