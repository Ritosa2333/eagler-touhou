import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
const binary=await readFile(new URL('../web/game.wasm',import.meta.url));
test('only the dedicated bomb input activates bombs',async()=>{
 for(const players of [2,3])for(let slot=0;slot<players;slot++)for(const sequence of [[12,12,0,12],[4,0,4,0,8,0,8],[4,8,4,8],[7,7]]){
  const e=(await WebAssembly.instantiate(binary,{})).instance.exports;e.th_init(players,2,123,0);e.th_set_netplay_rules(players>1?1:0);e.th_step(4,0,0);for(let i=0;i<30;i++)e.th_step(0,0,0);
  for(const bit of sequence){const args=[0,0,0];args[slot]=bit;e.th_step(...args);e.th_step(...args);}
  assert.equal(e.th_get(14,0),0);assert.equal(e.th_get(32,slot),1);
  e.th_step(0,0,0);const args=[0,0,0];args[slot]=16;e.th_step(...args);e.th_step(...args);
  assert.equal(e.th_get(14,0),1);assert.equal(e.th_get(32,slot),0);
 }
});

 test('single player retains original bomb shortcuts',async()=>{
 for(const sequence of [[4,0,4,0,8,0,8],[4,8,4,8],[7,7]]){
  const e=(await WebAssembly.instantiate(binary,{})).instance.exports;e.th_init(1,2,123,0);e.th_step(4,0,0);for(let i=0;i<30;i++)e.th_step(0,0,0);
  for(const bit of sequence){e.th_step(bit,0,0);e.th_step(bit,0,0)}
  for(let i=0;i<4;i++)e.th_step(0,0,0);assert.equal(e.th_get(14,0),1);assert.equal(e.th_get(32,0),0);
 }
});
