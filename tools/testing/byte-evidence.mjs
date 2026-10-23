import assert from 'node:assert/strict';

/** Preserve byte evidence before a fixture's finally block frees live WASM/native memory. */
export function assertBytesEqual(actual,expected,label){
  assert.ok(actual instanceof Uint8Array&&expected instanceof Uint8Array,'Byte comparison requires Uint8Array inputs');
  const count=Math.min(actual.length,expected.length);
  let offset=0;
  while(offset<count&&actual[offset]===expected[offset])offset++;
  if(offset===count&&actual.length===expected.length)return;
  const left=Uint8Array.from(actual),right=Uint8Array.from(expected);
  const start=Math.max(0,offset-8),end=Math.min(Math.max(left.length,right.length),offset+16);
  const difference={offset,actualLength:left.length,expectedLength:right.length,
    actualByte:offset<left.length?left[offset]:null,expectedByte:offset<right.length?right[offset]:null,
    context:{start,actual:Array.from(left.subarray(start,end)),expected:Array.from(right.subarray(start,end))}};
  try{assert.deepEqual(left,right,`${label}: first byte difference at +0x${offset.toString(16)} (${left.length}/${right.length} bytes)`);}
  catch(error){error.byteDifference=difference;throw error;}
}
