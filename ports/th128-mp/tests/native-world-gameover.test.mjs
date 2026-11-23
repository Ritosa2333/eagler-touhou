import assert from 'node:assert/strict';
import test from 'node:test';
import {bindNativeRollbackWorld} from '../runtime/native-world.mjs';

const row=(frame,...held)=>({frame,inputs:[0,1].map(player=>({player,held:held[player]??0,pressed:0,released:0,predicted:false}))});

function fixture(seat,program){
  // Keep the entire mock native cursor/pause/input in the checkpoint heap;
  // counts outside it represent attempted calls, not game state.
  const memory=new Uint8Array(32),view=new DataView(memory.buffer);
  const calls={input:0,tick:0};
  const module={HEAPU8:memory,UTF8ToString:text=>text,_th128_world_error:()=>'',_th128_world_pause_active:()=>memory[10],
    _th128_world_presentation_state:()=>JSON.stringify({frame:view.getUint32(0,true),normalRecord:{bound:false},playerState:1}),
    _th128_world_input(held){calls.input++;view.setUint16(8,held,true);return 1;},
    _th128_world_tick(){
      calls.tick++;const frame=view.getUint32(0,true),selected=program(frame,view.getUint16(8,true));
      if(selected===1||selected==='moved')view.setUint32(0,frame+1,true);
      if(selected===2||selected==='moved')memory[10]=1;
      return selected==='moved'||selected==='no-pause'?2:selected;
    }};
  const owner=bindNativeRollbackWorld({module,seat,checkpointSlots:2,maxCheckpointBytes:64,holdGameOver:true});
  return {owner,calls,state:()=>JSON.parse(module._th128_world_presentation_state())};
}

test('natural GameOver freezes only one native World while the other consumes the shared logical timeline',()=>{
  const a=fixture(0,frame=>frame===1?2:1),b=fixture(1,()=>1);
  for(let i=0;i<4;i++)for(const seat of [a,b])seat.owner.step(row(i));
  assert.deepEqual([a.state().frame,b.state().frame],[1,4]);
  assert.deepEqual(a.owner.terminal(),{reason:'game-over',frame:1});assert.equal(b.owner.terminal(),null);
  assert.deepEqual(a.calls,{input:2,tick:2});assert.deepEqual(b.calls,{input:4,tick:4});
  assert.throws(()=>a.owner.step(row(5)),/frame or seat/);
  a.owner.close();b.owner.close();
});

test('a predicted GameOver is removed by restoring its checkpoint and correcting the input',()=>{
  const {owner,state,calls}=fixture(0,(_frame,held)=>held===64?2:1);
  owner.step(row(0));const before=owner.checkpoint();
  owner.step(row(1,64));assert.equal(state().frame,1);assert.equal(owner.terminal().frame,1);
  const after=owner.checkpoint();owner.step(row(2));
  owner.restore(after);assert.equal(owner.terminal().frame,1);owner.step(row(2));
  owner.restore(before);assert.equal(owner.terminal(),null);
  owner.step(row(1,128));owner.step(row(2));assert.equal(state().frame,3);assert.equal(calls.tick,4);
  owner.release(before);owner.release(after);owner.close();
});

test('result zero, a missing pause and a moving native cursor do not get treated as successful GameOver',()=>{
  for(const code of [0,'no-pause','moved']){
    const {owner}=fixture(0,()=>code);
    assert.throws(()=>owner.step(row(0)),/non-playing boundary/);assert.equal(owner.terminal(),null);owner.close();
  }
});
