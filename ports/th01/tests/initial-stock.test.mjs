import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {NativeSimulation} from '../web/netplay/runtime-core.js';
const binary=await readFile(new URL('../web/game.wasm',import.meta.url));
test('managed multiplayer starts each player with two spare lives and two bombs',async()=>{
 for(const players of [2,3])for(const count of [1,2,4,6,8,9]){
  const core=(await WebAssembly.instantiate(binary,{})).instance.exports;
  const sim=new NativeSimulation(core,{players,count,rank:1,lifeCompensation:false,initialSpares:2,initialBombs:2,startStage:'normal'},0);
  sim.reset();for(let frame=0;frame<200;frame++)sim.step(Array(players).fill(0));
  assert.deepEqual(sim.state().players.map(p=>[p.lives,p.bombs]),Array.from({length:players},()=>[2,2]));
  for(const slot of [0,players-1]){
   const input=Array(players).fill(0);input[slot]=4096;sim.step(input);sim.step(input);
   assert.equal(sim.state().players[slot].bombs,1);
   while(sim.state().bombActive)sim.step(Array(players).fill(0));
  }
  sim.reset();assert.deepEqual(sim.state().players.map(p=>[p.lives,p.bombs]),Array.from({length:players},()=>[2,2]));
 }
});
