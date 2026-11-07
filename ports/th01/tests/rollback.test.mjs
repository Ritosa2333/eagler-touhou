import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {FrameQueue,MAX_ROLLBACK} from '../web/netplay/queue.js';
import {NativeSimulation} from '../web/netplay/runtime-core.js';
const binary = await readFile(new URL('../web/game.wasm',import.meta.url));
const makeCore = async () => (await WebAssembly.instantiate(binary,{})).instance.exports;
const settings = {players:2,count:2,rank:1,lifeCompensation:true,startStage:'normal'};

test('later stages keep native results visible through rapid fire',async()=>{
  for(const stage of [1,6,7,9,11,14,16,19]){
    const core=await makeCore(),sim=new NativeSimulation(core,settings,0);sim.reset();
    core.th_stage_command(14,0,stage,0,0,0);core.th_stage_command(15,0,0,0,0,0);
    sim.step([0,0]);assert.equal(core.th_get(0,0),2);
    for(let n=0;n<24;n++)sim.step([n%2?0:4,0]);
    assert.equal(core.th_get(27,0),stage);assert.equal(core.th_get(0,0),2);
    sim.step([4,0]);assert.equal(core.th_get(27,0),stage+1);
  }
});

test('late remote input triggers bounded repair for two and three players',()=>{
  for(const count of [2,3]){
    const q=new FrameQueue(0,1,count,{rollback:true});q.capture(0);q.commit(q.peek());
    for(let f=1;f<=MAX_ROLLBACK;f++){
      q.capture(f&1);for(let slot=2;slot<count;slot++)q.receive(f,0,slot);
      assert(q.peek());q.commit(q.peek());
    }
    q.capture(0);assert.equal(q.peek(),null,'prediction cannot exceed the safety window');
    q.receive(1,1,1);assert.equal(q.dirty,1);q.rewind(1);
    assert.equal(q.peek()[1],1);q.commit(q.peek());assert.equal(q.predicted(1,2),1);
  }
});

test('native rollback snapshot restores exact simulation and result state',async()=>{
  const core=await makeCore(),sim=new NativeSimulation(core,settings,0);sim.reset();
  core.th_stage_command(14,0,6,0,0,0);core.th_stage_command(15,0,0,0,0,0);sim.step([0,0]);
  for(let n=0;n<8;n++)sim.step([0,0]);
  const memory=new Uint8Array(core.memory.buffer),snapshot=memory.slice(),frame=sim.frame,previous=sim.previous.slice(),resultFrames=sim.resultFrames;
  const inputs=Array.from({length:80},(_,i)=>[i%3===0?4:0,i%2]);
  for(const pair of inputs)sim.step(pair);const expected=sim.checksum();
  memory.set(snapshot);sim.frame=frame;sim.previous=previous;sim.resultFrames=resultFrames;
  for(const pair of inputs)sim.step(pair);assert.deepEqual(sim.checksum(),expected);
});
