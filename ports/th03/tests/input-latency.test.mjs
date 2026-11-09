import {test} from 'node:test';
import assert from 'node:assert/strict';
import {chooseInputPolicy,validInputPolicy} from '../web/netplay/input-policy.js';
import {FrameQueue,neutral} from '../web/frame-queue.js';
import {Lockstep} from '../web/lockstep.js';
import {packTouch,unpackTouch} from '../web/touch-input.js';

test('rollback prioritizes local input until the prediction budget is exhausted',()=>{
  for(const [rtt,delay] of [[0,0],[21,0],[50,0],[100,0],[200,0],[300,1]]){
    const report={rollbackEnabled:true,rttMs:rtt,maxRttMs:rtt};
    const policy=chooseInputPolicy(report,report);
    assert.equal(policy.inputDelay,delay);assert(validInputPolicy(policy));
    assert(policy.predictionLead<=12);
  }
  const report={rollbackEnabled:false,rttMs:100,maxRttMs:100};
  assert.equal(chooseInputPolicy(report,report).inputDelay,0);
});

test('zero-delay rollback captures the local action for the current frame',()=>{
  const queue=new FrameQueue(0,{rollbackEnabled:true,inputDelay:0,predictionLead:4});
  queue.arm(2);
  for(let n=0;n<2;n++)queue.commit(queue.peek());
  const input={...neutral(),actions:['right']};
  assert.equal(queue.capture(input).frame,2);
  assert.deepEqual(queue.peek()[0].actions,['right']);
  queue.commit(queue.peek());
  assert.equal(queue.capture(neutral()).frame,3);
  assert.deepEqual(queue.peek()[0].actions,[]);
});


test('stationary touch prediction keeps the drag without repeating movement or start',()=>{
  for(const unlimited of [false,true]){
    const queue=new FrameQueue(0,{rollbackEnabled:true,inputDelay:0,predictionLead:8});queue.arm(2);
    for(let i=0;i<2;i++)queue.commit(queue.peek());
    queue.capture(neutral());
    const drag={...neutral(),touch:packTouch(0,{active:true,begin:true,unlimited,x:128,y:-32})};
    queue.receive(2,drag);queue.commit(queue.peek());
    queue.capture(neutral());const predicted=queue.peek();
    assert.deepEqual(unpackTouch(predicted[1].touch),{x:0,y:0,active:true,unlimited});
    queue.commit(predicted);
    queue.receive(3,{...drag,touch:packTouch(0,{active:true,unlimited})});
    assert.equal(queue.dirty,null,'an unchanged held drag must not roll back');
    queue.capture(neutral());const held=queue.peek();queue.commit(held);
    queue.receive(4,{...drag,touch:0});assert.equal(queue.dirty,4,'release must correct prediction');
    queue.rewind(4);assert.equal(queue.peek()[1].touch,0);
  }
});

test('actual touch movement still corrects a stationary prediction',()=>{
  const queue=new FrameQueue(0,{rollbackEnabled:true,inputDelay:0,predictionLead:8});queue.arm(2);
  for(let i=0;i<2;i++)queue.commit(queue.peek());
  queue.capture(neutral());queue.receive(2,{...neutral(),touch:packTouch(0,{active:true})});queue.commit(queue.peek());
  queue.capture(neutral());queue.commit(queue.peek());
  queue.receive(3,{...neutral(),touch:packTouch(0,{active:true,x:16})});assert.equal(queue.dirty,3);
});

// A dropped remote input should be hidden by bounded prediction; its eventual
// arrival still corrects history instead of being silently skipped.
test('rollback survives a short missing input burst and repairs changed input',()=>{
  const report={rollbackEnabled:true,rttMs:0,maxRttMs:0},policy=chooseInputPolicy(report,report);
  assert.equal(policy.predictionLead,4);
  const network={localSeat:0,setPacketHandler(){}};
  const sync=new Lockstep({network,rollbackEnabled:true,predictionLead:policy.predictionLead});
  try{
    const q=sync.queue;q.arm(2);for(let i=0;i<2;i++)q.commit(q.peek());
    for(let i=0;i<4;i++){q.capture(neutral());const pair=sync.nextInputs();assert.ok(pair);q.commit(pair);}
    q.capture(neutral());assert.equal(sync.nextInputs(),null);assert.equal(q.waitReason,'prediction-lead');
    q.receive(2,{...neutral(),actions:['right']});assert.equal(q.dirty,2);
    for(let i=3;i<6;i++)q.receive(i,neutral());
    assert.equal(q.confirmed,6);q.rewind(2);assert.deepEqual(q.peek()[1].actions,['right']);
  }finally{sync.wakeChannel?.port1.close();sync.wakeChannel?.port2.close();}
});
