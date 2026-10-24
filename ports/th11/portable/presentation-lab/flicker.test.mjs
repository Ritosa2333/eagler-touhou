import test from 'node:test';
import assert from 'node:assert/strict';
import {analyzeFlicker} from './flicker.mjs';
const record=(key,position,owner='自机')=>({key,ownerId:3,ownerLabel:owner,objectId:1,layer:14,fields:[
  {id:'position',value:position},{id:'script-position',value:[0,0,0]},{id:'child-position',value:[0,0,0]},
]});
const frame=(tick,records)=>({tick,records});
test('continuous motion stays below thresholds',()=>{
  const previous=frame(1,[record('a',[0,0,0])]),current=frame(2,[record('a',[10,0,0])]);
  const samples=[0,.25,.5,.75,1].map(alpha=>({alpha,records:[record('a',[10*alpha,0,0])]}));
  const result=analyzeFlicker({previous,current,samples});
  assert.equal(result.flagged.length,0);assert.equal(result.checked,1);
});
test('a blended teleport is recorded as a large interpolated step',()=>{
  const previous=frame(1,[record('a',[0,0,0])]),current=frame(2,[record('a',[200,0,0])]);
  const samples=[0,.5,1].map(alpha=>({alpha,records:[record('a',[200*alpha,0,0])]}));
  const result=analyzeFlicker({previous,current,samples});
  assert.equal(result.flagged.length,1);
  assert.deepEqual(result.flagged[0].reasons,['large-interpolated-step']);
  assert.equal(result.flagged[0].maxSampleStep,100);
});
test('a stale endpoint is recorded as endpoint mismatch',()=>{
  const previous=frame(1,[record('a',[0,0,0])]),current=frame(2,[record('a',[200,0,0])]);
  const samples=[{alpha:0,records:[record('a',[0,0,0])]},{alpha:.5,records:[record('a',[0,0,0])]},{alpha:1,records:[record('a',[200,0,0])]}];
  const result=analyzeFlicker({previous,current,samples});
  assert.equal(result.flagged.length,1);
  assert.ok(result.flagged[0].reasons.includes('endpoint-mismatch'));
});
test('slow objects are not reported',()=>{
  const previous=frame(1,[record('a',[0,0,0])]),current=frame(2,[record('a',[2,0,0])]);
  const samples=[0,.5,1].map(alpha=>({alpha,records:[record('a',[2*alpha,0,0])]}));
  assert.equal(analyzeFlicker({previous,current,samples}).flagged.length,0);
});
