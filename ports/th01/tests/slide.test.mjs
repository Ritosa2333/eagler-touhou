import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
const binary=await readFile(new URL('../web/game.wasm',import.meta.url));
const make=async()=>(await WebAssembly.instantiate(binary,{})).instance.exports;
test('slides cannot be cancelled by reversing, releasing or shooting',async()=>{
 for(const players of [1,2,3])for(let slot=0;slot<players;slot++)for(const dir of [1,2])for(const input of [0,dir===1?2:1,4,(dir===1?2:1)|4]){
  const e=await make();e.th_init(players,2,123,0);e.th_set_netplay_rules(players>1?1:0);e.th_debug_position(slot,300);
  const step=bits=>{const args=[0,0,0];args[slot]=bits;e.th_step(...args)};
  step(dir);step(dir|8);const start=e.th_get(30,slot);assert.equal(e.th_get(36,slot),2);
  for(let frame=1;frame<=12;frame++){step(input);assert.equal(e.th_get(36,slot),2);assert.equal(e.th_get(30,slot),start+(dir===1?-6:6)*frame);}
  step(0);assert.notEqual(e.th_get(36,slot),2);const end=e.th_get(30,slot);step(dir===1?2:1);assert.equal(e.th_get(30,slot),end+(dir===1?4:-4));
 }
});
test('replaying reverse input during slide produces identical rollback state',async()=>{
 const e=await make();e.th_init(2,2,123,0);e.th_set_netplay_rules(1);e.th_debug_position(0,300);e.th_step(2,0,0);e.th_step(10,0,0);
 const heap=new Uint8Array(e.memory.buffer),snapshot=heap.slice(),run=()=>{let frames=[];for(let i=0;i<14;i++){e.th_step(1,0,0);frames.push([e.th_get(30,0),e.th_get(36,0),e.th_net_checksum()])}return frames};
 const expected=run();heap.set(snapshot);assert.deepEqual(run(),expected);
});
