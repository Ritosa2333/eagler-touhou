import test from 'node:test';
import assert from 'node:assert/strict';
import {rollbackPolicyForRtt,validRollbackPolicy} from '../web/netplay/rollback-queue.js';

test('TH02 negotiates three-player transit buffering without changing two-player prediction',()=>{
  const two=rollbackPolicyForRtt(160,true,2),three=rollbackPolicyForRtt(160,true,3);
  assert.equal(two.predictionSteps,2);assert.equal(two.inputDelay,3);
  assert.equal(three.predictionSteps,0);assert.equal(three.inputDelay,5);
  assert(validRollbackPolicy(two,2));assert(validRollbackPolicy(three,3));
  assert(!validRollbackPolicy(two,3));assert(!validRollbackPolicy(three,2));
  assert.equal(rollbackPolicyForRtt(0,true,3).inputDelay,0);
  assert.equal(rollbackPolicyForRtt(160,false,3).inputDelay,0);
  assert.throws(()=>rollbackPolicyForRtt(160,true,4));
});
