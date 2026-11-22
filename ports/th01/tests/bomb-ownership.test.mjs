import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {NativeSimulation} from '../web/netplay/runtime-core.js';

const binary=await readFile(new URL('../web/game.wasm',import.meta.url));
async function game(players){
  const core=(await WebAssembly.instantiate(binary,{})).instance.exports;
  const sim=new NativeSimulation(core,{players,count:2,rank:1,lifeCompensation:false,startStage:'normal'},0);
  sim.reset();
  for(let i=0;i<180;i++)sim.step(Array(players).fill(0));
  return {core,sim};
}
const inputs=(players,slots)=>Array.from({length:players},(_,i)=>slots.includes(i)?4096:0);
const bombs=sim=>sim.state().players.map(player=>player.bombs);

test('each player spends only their own bomb in multiplayer',async()=>{
  for(const players of [2,3])for(let slot=0;slot<players;slot++){
    const {sim}=await game(players),initial=bombs(sim),pressed=inputs(players,[slot]);
    sim.step(pressed);sim.step(pressed);
    assert(sim.state().bombActive);
    assert.deepEqual(bombs(sim),initial.map((count,i)=>count-(i===slot?1:0)));
  }
});

test('bomb input during another bomb cannot consume or queue a teammate bomb',async()=>{
  for(const players of [2,3])for(let owner=0;owner<players;owner++)for(let other=0;other<players;other++){
    if(owner===other)continue;
    for(const hold of [false,true]){
      const {sim}=await game(players),start=inputs(players,[owner]),press=inputs(players,[other]);
      sim.step(start);sim.step(start);assert(sim.state().bombActive);
      const before=bombs(sim);let seenEnd=false;
      for(let frame=0;frame<300;frame++){
        sim.step(hold||frame<2?press:inputs(players,[]));
        assert.deepEqual(bombs(sim),before,`owner ${owner} other ${other} frame ${frame}`);
        if(!sim.state().bombActive){seenEnd=true;for(let k=0;k<5;k++)sim.step(hold?press:inputs(players,[]));break;}
      }
      assert(seenEnd,'the first bomb must finish');assert.deepEqual(bombs(sim),before);
      sim.step(inputs(players,[]));sim.step(press);sim.step(press);
      assert(sim.state().bombActive,'a new press should launch after the first bomb ends');
      assert.deepEqual(bombs(sim),before.map((value,i)=>value-(i===other?1:0)));
    }
  }
});

test('simultaneous bomb presses select one owner and remain deterministic after rollback',async()=>{
  for(const players of [2,3]){
    const {core,sim}=await game(players),memory=new Uint8Array(core.memory.buffer),snapshot=memory.slice();
    const frame=sim.frame,previous=sim.previous.slice(),resultFrames=sim.resultFrames;
    const run=()=>{const a=inputs(players,Array.from({length:players},(_,i)=>i));sim.step(a);sim.step(a);return {bombs:bombs(sim),active:sim.state().bombActive,checksum:sim.checksum()};};
    const expected=run();assert.deepEqual(expected.bombs,Array.from({length:players},(_,i)=>i===0?0:1));assert(expected.active);
    memory.set(snapshot);sim.frame=frame;sim.previous=previous;sim.resultFrames=resultFrames;
    assert.deepEqual(run(),expected);
  }
});

test('a player without bombs cannot spend a teammate inventory',async()=>{
  for(const players of [2,3])for(let slot=0;slot<players;slot++){
    const {core,sim}=await game(players),press=inputs(players,[slot]);
    sim.step(press);sim.step(press);assert.equal(bombs(sim)[slot],0);
    core.th_stage_command(14,0,1,0,0,0);
    for(let i=0;i<180;i++)sim.step(inputs(players,[]));
    const before=bombs(sim);assert.equal(before[slot],0);
    sim.step(press);sim.step(press);
    assert.deepEqual(bombs(sim),before);assert.equal(sim.state().bombActive,false);
  }
});

test('an active bomb protects every seat, including after replaying a rollback',async()=>{
  for(const players of [2,3])for(let owner=0;owner<players;owner++){
    const {core,sim}=await game(players);
    const idle=inputs(players,[]);
    sim.step(idle.map((value,slot)=>slot===0?4:value));
    for(let i=0;i<180;i++)sim.step(idle);
    assert.equal(sim.state().phase,1);
    for(let slot=0;slot<players;slot++)core.th_debug_position(slot,120+slot*160);
    const team=()=>sim.state().players.map(p=>({lives:p.lives,ghost:p.ghost}));
    const before=team();
    sim.step(inputs(players,[owner]));sim.step(inputs(players,[owner]));
    assert(sim.state().bombActive);
    const memory=new Uint8Array(core.memory.buffer),snapshot=memory.slice();
    const frame=sim.frame,previous=sim.previous.slice(),resultFrames=sim.resultFrames;
    const hitAll=()=>{
      for(let slot=0;slot<players;slot++)core.th_debug_hit(slot,0);
      sim.step(idle);
      for(let slot=0;slot<players;slot++)core.th_debug_hit(slot,1);
      sim.step(idle);
      return team();
    };
    assert.deepEqual(hitAll(),before,`${players} seats, bomb by ${owner}`);
    const checksum=sim.checksum();
    memory.set(snapshot);sim.frame=frame;sim.previous=previous;sim.resultFrames=resultFrames;
    assert.deepEqual(hitAll(),before);
    assert.deepEqual(sim.checksum(),checksum);
    for(let i=0;i<300&&sim.state().bombActive;i++)sim.step(idle);
    assert.equal(sim.state().bombActive,false);
    const teammate=(owner+1)%players;
    core.th_debug_hit(teammate,1);sim.step(idle);
    assert.equal(sim.state().players[teammate].ghost,true,'damage returns when the bomb ends');
  }
});
