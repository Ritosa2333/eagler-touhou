import {test} from 'node:test';
import assert from 'node:assert/strict';
import {persistIndependent,terminalStream} from './persist-independent.mjs';
for (let failed=0;failed<3;++failed) test(`stream ${failed} fails without losing other evidence`,async()=>{
  const saved=[];
  await assert.rejects(persistIndependent(['framebuffer','audit','work'].map((name,index)=>[name,async()=>{
    saved.push(name);if(index===failed)throw Error('injected storage failure');
  }])),error=>error instanceof AggregateError && error.errors.length===1);
  assert.deepEqual(saved,['framebuffer','audit','work']);
});
test('failed acknowledgement does not replay an already appended batch',async()=>{
  const cursor={failure:null}, saved=[];
  await assert.rejects(terminalStream(cursor,'work',async()=>{saved.push('batch');throw Error('ack lost');}),/ack lost/);
  await assert.rejects(terminalStream(cursor,'work',async()=>saved.push('duplicate')),/acknowledgement failed/);
  assert.deepEqual(saved,['batch']);
});
