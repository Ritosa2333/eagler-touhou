import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {runInNewContext} from 'node:vm';

for(const game of ['th02','th04']) test(`${game} preserves touch input while the live frame is already captured`,async()=>{
  const root=new URL(`../../ports/${game}/web/`,import.meta.url);
  const {RollbackQueue}=await import(new URL('netplay/rollback-queue.js',root));
  const q=new RollbackQueue(0,2);q.frame=2;q.arm(2);
  let motion=12,consumed=0,sent=[];
  const room=await readFile(new URL('netplay/room.js',root),'utf8');
  const source=room.slice(room.indexOf('function captureLocalInput('),room.indexOf('function nextInputs('));
  const capture=runInNewContext(`(${source})`,{queue:q,document:{hidden:false},
    player:{setContext(){},alwaysPoint:()=>false,pack:()=>{const value=motion;motion=0;return value;},consume(){consumed++;}},
    runtime:{touchContext:()=>({}),setPointPreferences(){}},focusSettings:()=>true,
    observeInput(){},normalize:x=>x,inputLatch:{sample:()=>0,consume(){}},bits:0,pendingAction:0,inputChangedAt:0,
    performance:{now:()=>0},captureTimes:new Map(),broadcast:packet=>sent.push(packet)});
  assert.equal(capture(),true);assert.equal(sent[0].buttons,12);
  motion=27;assert.equal(capture(),false);assert.equal(motion,27);assert.equal(consumed,1);
  q.frame++;assert.equal(capture(),true);assert.equal(sent[1].buttons,27);assert.equal(consumed,2);
});

for(const game of ['th02','th04']) test(`${game} corrects a late remote input without replacing local history`,async()=>{
  const {RollbackQueue}=await import(new URL(`../../ports/${game}/web/netplay/rollback-queue.js`,import.meta.url));
  const q=new RollbackQueue(0,2);q.frame=2;q.arm(2);
  q.capture(32);q.commit(q.peek());
  q.capture(4);q.commit(q.peek());
  q.receive(2,8,1);assert.equal(q.refreshCorrection(),2);
  q.rewind(2);assert.deepEqual(q.peek(),[32,8]);
  q.commit(q.peek());assert.equal(q.inputs[0].get(3),4);
});
