import test from 'node:test';
import assert from 'node:assert/strict';
import {rollbackPolicyForRtt,validRollbackPolicy,MAX_ROLLBACK} from '../web/netplay/rollback-queue.js';

test('TH02 uses the same low-latency rollback policy for two and three players',()=>{
  for(const rtt of [0,20,21,50,100,160,1000]){
    const two=rollbackPolicyForRtt(rtt,true,2),three=rollbackPolicyForRtt(rtt,true,3);
    assert.deepEqual(three,two);
    assert(validRollbackPolicy(two,2));assert(validRollbackPolicy(three,3));
    assert(three.predictionLead<=MAX_ROLLBACK);
    assert(!validRollbackPolicy({...three,schema:three.schema-1},3));
  }
  assert.equal(rollbackPolicyForRtt(100,true,3).inputDelay,1);
  assert.equal(rollbackPolicyForRtt(160,true,3).inputDelay,1);
  assert.equal(rollbackPolicyForRtt(0,true,3).inputDelay,0);
  assert.equal(rollbackPolicyForRtt(160,false,3).inputDelay,0);
  assert.throws(()=>rollbackPolicyForRtt(160,true,4));
});
