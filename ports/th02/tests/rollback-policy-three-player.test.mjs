import test from 'node:test';
import assert from 'node:assert/strict';
import {rollbackPolicyForRtt,validRollbackPolicy,MAX_ROLLBACK,RollbackQueue} from '../web/netplay/rollback-queue.js';

test('TH02 uses the same low-latency rollback policy for two and three players',()=>{
  for(const rtt of [0,20,21,50,100,160,1000]){
    const two=rollbackPolicyForRtt(rtt,true,2),three=rollbackPolicyForRtt(rtt,true,3);
    assert.deepEqual(three,two);
    assert(validRollbackPolicy(two,2));assert(validRollbackPolicy(three,3));
    assert(three.predictionLead<=MAX_ROLLBACK);
    assert(!validRollbackPolicy({...three,schema:three.schema-1},3));
  }
  assert.equal(rollbackPolicyForRtt(100,true,3).inputDelay,0);
  assert.equal(rollbackPolicyForRtt(160,true,3).inputDelay,0);
  assert.equal(rollbackPolicyForRtt(280,true,3).inputDelay,1);
  assert.equal(rollbackPolicyForRtt(0,true,3).inputDelay,0);
  assert.equal(rollbackPolicyForRtt(160,false,3).inputDelay,0);
  assert.throws(()=>rollbackPolicyForRtt(160,true,4));
});

test('zero-delay local input remains authoritative through remote corrections',()=>{
  const policy=rollbackPolicyForRtt(120,true,3),q=new RollbackQueue(0,3,{...policy,directionPrediction:6});
  q.arm(0);
  for(let frame=0;frame<2;frame++)q.commit(q.peek());
  const local=q.capture(8);assert.equal(local.frame,2);
  const predicted=q.peek();assert.equal(predicted[0],8);q.commit(predicted);
  q.receive(2,4,1);q.receive(2,0,2);assert.equal(q.dirty,2);
  q.rewind(2);assert.deepEqual(q.peek(),[8,4,0]);q.commit(q.peek());
  assert.equal(q.confirmed,3);
});
