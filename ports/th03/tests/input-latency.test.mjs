import {test} from 'node:test';
import assert from 'node:assert/strict';
import {chooseInputPolicy,validInputPolicy} from '../web/netplay/input-policy.js';
import {FrameQueue,neutral} from '../web/frame-queue.js';

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
