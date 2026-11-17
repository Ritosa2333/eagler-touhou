import {test} from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {FrameQueue,MAX_ROLLBACK} from '../web/netplay/queue.js';
const rollbackModule=await WebAssembly.compile(await readFile(new URL('../../../common/pc98/rollback-core.wasm',import.meta.url)));
import {NativeSimulation} from '../web/netplay/runtime-core.js';
import {AUDITED_CORE,replicaBinary} from '../web/replay/native-state.js';
import {createHash} from 'node:crypto';
const binary = await readFile(new URL('../web/game.wasm',import.meta.url));
const makeCore = async () => (await WebAssembly.instantiate(binary,{})).instance.exports;
const settings = {players:2,count:2,rank:1,lifeCompensation:true,startStage:'normal'};

test('shipped native core is accepted by the spectator replica',async()=>{
  assert.equal(createHash('sha256').update(binary).digest('hex'),AUDITED_CORE);
  const replica=(await WebAssembly.instantiate(await replicaBinary(binary),{})).instance.exports;
  assert(replica.__replay_stack instanceof WebAssembly.Global);
  assert.equal(replica.memory.buffer.byteLength,4194304);
});

test('rescued players immediately move and retain their action pose',async()=>{
  for(const slot of [0,1]){
    const core=await makeCore();core.th_init(2,2,1,0);core.th_step(4,0,0);
    core.th_debug_position(0,150);core.th_debug_position(1,300);
    core.th_debug_hit(slot,1);core.th_step(0,0,0);assert.equal(core.th_get(33,slot),1);
    core.th_debug_position(slot,slot===0?295:155);core.th_step(0,0,0);
    assert.equal(core.th_get(33,slot),0);assert.equal(core.th_get(129,slot),0);
    const before=core.th_get(30,slot);core.th_step(slot===0?2:0,slot===1?2:0,0);
    assert(core.th_get(30,slot)>before);assert.equal(core.th_get(40,slot),core.th_get(30,slot));
    assert.notEqual(core.th_get(39,slot),64,'moving pose must not be replaced by the idle pose');
  }
});

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

test('both final bosses reach the native ending in multiplayer',async()=>{
  for(const startStage of ['m20','j20']){
    const core=await makeCore(),sim=new NativeSimulation(core,{...settings,startStage,qa:true},0);sim.reset();
    for(let i=0;i<220;i++)sim.step([4,0]);
    for(let i=0;i<3000&&core.th_get(50,0)!==10;i++){
      if(i%15===0)core.th_stage_command(20,0,99999,0,0,0);
      sim.step([0,0]);
    }
    assert.equal(core.th_get(50,0),10,`${startStage} should enter the ending`);
    const age=core.th_get(106,0);for(let i=0;i<10;i++)sim.step([0,0]);
    assert(core.th_get(106,0)>age,'ending must continue advancing');
  }
});

test('late remote input triggers bounded repair for two and three players',()=>{
  for(const count of [2,3]){
    const q=new FrameQueue(0,1,count,{rollback:true,coreModule:rollbackModule});q.capture(0);q.commit(q.peek());
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
