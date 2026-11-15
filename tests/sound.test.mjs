import assert from 'node:assert/strict';
import test from 'node:test';
import {GameSoundQueue} from '../site/runtime/sound.mjs';

test('game sound playback uses prepared ID binding, native volume and bounded host panning',async()=>{
  const nodes=[];
  const node=()=>{const item={connect:()=>{},disconnect:()=>{},start:()=>{item.started=true;},stop:()=>{},gain:{value:0},pan:{value:0}};nodes.push(item);return item;};
  const context={createBufferSource:node,createGain:node,createStereoPanner:node};
  const queue=new GameSoundQueue(context,{});queue.buffers.set(44,{buffer:{},volumeRaw:-2000});
  assert.equal(queue.play(44,96),true);assert.equal(nodes[0].started,true);
  assert.equal(nodes[1].gain.value,.1);assert.equal(nodes[2].pan.value,.5);
  assert.throws(()=>queue.play(999,0));assert.throws(()=>queue.play(44,NaN));
  queue.dispose();assert.throws(()=>queue.play(44,0));queue.dispose();
});

test('native charge sound stop and pan update target the active sound ID',()=>{
  const nodes=[];const node=()=>{const item={connect(){},disconnect(){this.disconnected=true;},start(){},stop(){this.stopped=true;},gain:{value:0},pan:{value:0}};nodes.push(item);return item;};
  const queue=new GameSoundQueue({createBufferSource:node,createGain:node,createStereoPanner:node},{});
  queue.buffers.set(31,{buffer:{},volumeRaw:0,loop:true});queue.buffers.set(32,{buffer:{},volumeRaw:0});
  assert.equal(queue.play(31,0),true);assert.equal(queue.play(32,0),true);
  assert.equal(queue.position(31,-96),true);assert.equal(nodes[2].pan.value,-.5);assert.equal(nodes[5].pan.value,0);
  assert.equal(queue.stop(31),true);assert.equal(nodes[0].stopped,true);assert.equal(nodes[3].stopped,undefined);
  assert.equal(queue.stop(31),false);assert.equal(queue.position(31,20),false);
  assert.throws(()=>queue.position(31,NaN));assert.throws(()=>queue.stop(999));
  queue.dispose();assert.equal(nodes[3].stopped,true);assert.throws(()=>queue.position(31,0));
});
