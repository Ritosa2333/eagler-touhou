import test from 'node:test';
import assert from 'node:assert/strict';
import {endingMusicSignatures,scanEndingMusic} from '../web/netplay/ending-music.js';

test('ending music observes new complete original song headers without repeats',()=>{
  const song=(name,fill)=>({name,bytes:new Uint8Array(64).fill(fill)}),tracks=new Map([[1,song('END1.M',3)],[2,song('ENDING.M',4)],[3,song('STAGE0.M',5)]]);
  const signatures=endingMusicSignatures(tracks),heap=new Uint8Array(0xa1000),base=0x1000;
  heap.set(signatures[0].bytes.subarray(0,31),base+50);
  assert.deepEqual(scanEndingMusic(heap,base,signatures),{found:[],name:null});
  heap.set(signatures[0].bytes,base+50);let scan=scanEndingMusic(heap,base,signatures);
  assert.equal(scan.name,'end1');assert.equal(scanEndingMusic(heap,base,signatures,scan.found).name,null);
  heap.fill(0,base+50,base+82);heap.set(signatures[1].bytes,base+200);
  scan=scanEndingMusic(heap,base,signatures,scan.found);assert.equal(scan.name,'ending');
});
