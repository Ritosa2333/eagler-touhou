import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {writeInputs,signature} from '../web/netplay/th02-state.js';
import {packTouch} from '../web/touch-input.js';

test('touch begin journals its remainder even across a page boundary',async()=>{
  const patch=JSON.parse(await readFile(new URL('../web/local-patch.json',import.meta.url)));
  const heap=new Uint8Array(131072),cb=4096-(patch.entries.touch_remainder%4096)-2;
  const mailbox=cb+patch.entries.mailbox,remainder=cb+patch.entries.touch_remainder;
  heap.set(signature,mailbox);heap[mailbox+22]=1;heap.fill(0x7f,remainder,remainder+12);
  const original=heap.slice(),pages=new Map();
  const mark=(at,size)=>{for(let page=at>>>12;page<=((at+size-1)>>>12);page++)if(!pages.has(page))pages.set(page,heap.slice(page*4096,(page+1)*4096));};
  const touch=packTouch(0,{active:true,begin:true,x:16});
  writeInputs(heap,mailbox,patch,[0,0,touch],3,{},mark);
  assert.notDeepEqual(heap.slice(remainder+8,remainder+12),original.slice(remainder+8,remainder+12));
  for(const [page,bytes] of pages)heap.set(bytes,page*4096);
  assert.deepEqual(heap,original,'rollback restores every host-written page');
});
