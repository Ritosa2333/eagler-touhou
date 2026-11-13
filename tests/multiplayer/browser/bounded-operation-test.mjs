import {test} from 'node:test';
import assert from 'node:assert/strict';
import {boundedOperation} from './bounded-operation.mjs';
test('successful and rejected operations preserve their outcome',async()=>{
  assert.equal(await boundedOperation('ok',async()=>42,100),42);
  await assert.rejects(boundedOperation('fail',async()=>{throw Error('injected');},100),/injected/);
});
test('hung work fails promptly and late continuations observe expiry',async()=>{
  let release, expired;
  const p=boundedOperation('hung',async isExpired=>{await new Promise(r=>{release=r;});expired=isExpired();},10);
  await assert.rejects(p,/hung timed out/);release();await new Promise(r=>setImmediate(r));assert.equal(expired,true);
});
