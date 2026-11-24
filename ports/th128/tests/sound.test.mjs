import assert from 'node:assert/strict';
import test from 'node:test';
import {GameSoundQueue} from '../site/runtime/sound.mjs';

function audioFixture(){
  const nodes=[];
  const make=kind=>{const item={kind,connect(){},disconnect(){this.disconnected=true;},start(){this.started=true;},stop(){this.stopped=true;},gain:{value:0}};nodes.push(item);return item;};
  return{nodes,context:{createBufferSource:()=>make('source'),createGain:()=>make('gain'),createChannelMerger:()=>make('merger'),createChannelSplitter:()=>make('splitter')}};
}
test('game sound requests play after frame flush with native volume and bounded panning',()=>{
  const {nodes,context}=audioFixture(),queue=new GameSoundQueue(context,{});
  queue.buffers.set(44,{buffer:{numberOfChannels:1},volumeRaw:-2000});
  assert.equal(queue.play(44,96),true);assert.equal(nodes.length,0,'enqueue must not present before the frame drain');
  assert.equal(queue.flush(),1);
  const channel=queue.channels.get(44);assert.equal(channel.source.started,true);assert.equal(channel.gain.gain.value,.1);
  assert.equal(channel.pan.left.gain.value,Math.pow(10,-.25));assert.equal(channel.pan.right.gain.value,1);
  assert.throws(()=>queue.play(999,0));assert.throws(()=>queue.play(44,NaN));
  queue.dispose();assert.throws(()=>queue.play(44,0));queue.dispose();
});
test('native charge sound stop and pan update target the active sound ID',()=>{
  const {context}=audioFixture(),queue=new GameSoundQueue(context,{});
  queue.buffers.set(31,{buffer:{numberOfChannels:1},volumeRaw:0,loop:true});
  queue.buffers.set(32,{buffer:{numberOfChannels:1},volumeRaw:0});
  assert.equal(queue.play(31,0),true);assert.equal(queue.play(32,0),true);assert.equal(queue.flush(),2);
  const first=queue.channels.get(31),second=queue.channels.get(32),firstSource=first.source,secondSource=second.source;
  assert.equal(queue.position(31,-96),true);assert.equal(first.pan.left.gain.value,1);assert.equal(first.pan.right.gain.value,Math.pow(10,-.25));
  assert.equal(second.pan.left.gain.value,1);assert.equal(second.pan.right.gain.value,1);assert.equal(first.source,firstSource);
  assert.equal(queue.stop(31),true);assert.equal(firstSource.stopped,true);assert.equal(secondSource.stopped,undefined);
  assert.equal(queue.stop(31),false);assert.equal(queue.position(31,20),false);
  assert.throws(()=>queue.position(31,NaN));assert.throws(()=>queue.stop(999));
  queue.dispose();assert.equal(secondSource.stopped,true);assert.throws(()=>queue.position(31,0));
});
