import test from 'node:test';
import assert from 'node:assert/strict';
import {assertBytesEqual} from './byte-evidence.mjs';

test('native byte mismatch remains stable after fixture cleanup overwrites the original memory',()=>{
  const memory=new Uint8Array([1,2,3,4]),expected=new Uint8Array([1,2,9,4]);
  let failure;
  try{assertBytesEqual(memory,expected,'frame 12 / timer');}
  catch(error){failure=error;}
  assert.ok(failure);memory.fill(0);expected.fill(255);
  assert.deepEqual(failure.actual,new Uint8Array([1,2,3,4]));
  assert.deepEqual(failure.expected,new Uint8Array([1,2,9,4]));
  assert.equal(failure.byteDifference.offset,2);assert.equal(failure.byteDifference.actualByte,3);
  assert.equal(failure.byteDifference.expectedByte,9);assert.match(failure.message,/frame 12.*\+0x2/);
});

test('length-only differences and views with byte offsets identify the first missing byte',()=>{
  const memory=new Uint8Array([99,1,2,3,99]);
  assert.doesNotThrow(()=>assertBytesEqual(memory.subarray(1,4),new Uint8Array([1,2,3]),'same'));
  assert.throws(()=>assertBytesEqual(memory.subarray(1,3),new Uint8Array([1,2,3]),'short'),error=>{
    assert.equal(error.byteDifference.offset,2);assert.equal(error.byteDifference.actualByte,null);
    assert.equal(error.byteDifference.expectedByte,3);return true;
  });
});
