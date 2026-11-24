import assert from 'node:assert/strict';
import test from 'node:test';
import fs from 'node:fs';
import path from 'node:path';
import {GameSoundQueue} from '../site/runtime/sound.mjs';

const root=path.resolve(import.meta.dirname,'..');
const bindings=JSON.parse(fs.readFileSync(path.join(root,'local/runtime/sound-bindings.json'),'utf8'));
const sounds=path.join(root,'local/generations/8c71c316a4b24e68827f9fc560242050/ja/sounds');

function wavChannels(name){
  const bytes=fs.readFileSync(path.join(sounds,name));
  assert.equal(bytes.toString('ascii',0,4),'RIFF');
  assert.equal(bytes.toString('ascii',8,12),'WAVE');
  let offset=12;
  while(offset+8<=bytes.length){
    const id=bytes.toString('ascii',offset,offset+4),size=bytes.readUInt32LE(offset+4);
    if(id==='fmt ')return bytes.readUInt16LE(offset+10);
    offset+=8+size+(size&1);
  }
  throw new Error(`missing fmt chunk: ${name}`);
}
function node(kind){
  const value={kind,connections:[],disconnected:false,started:0,stopped:false,
    gain:{value:0},buffer:null,loop:false,
    connect(target,output=0,input=0){value.connections.push({target,output,input});return target;},
    disconnect(){value.disconnected=true;},
    start(){++value.started;},
    stop(){value.stopped=true;}};
  return value;
}
function context(nodes){
  return {
    createBufferSource(){const value=node('source');nodes.push(value);return value;},
    createGain(){const value=node('gain');nodes.push(value);return value;},
    createChannelSplitter(channels){const value=node('splitter');value.channels=channels;nodes.push(value);return value;},
    createChannelMerger(channels){const value=node('merger');value.channels=channels;nodes.push(value);return value;},
    decodeAudioData:async()=>({numberOfChannels:1})
  };
}
function queueWith(buffer){
  const nodes=[];const queue=new GameSoundQueue(context(nodes),{});
  queue.buffers.set(1,{buffer,volumeRaw:-1100,loop:true});
  return {queue,nodes};
}

test('every prepared SE WAV remains mono or stereo and production load fails closed otherwise',async()=>{
  const names=new Set(Object.values(bindings.sounds).map(sound=>sound.name));
  assert.equal(Object.keys(bindings.sounds).length,76);
  for(const name of names)assert.ok([1,2].includes(wavChannels(name)),`${name} channel count`);
  const originalFetch=globalThis.fetch;
  globalThis.fetch=async()=>({ok:true,arrayBuffer:async()=>new ArrayBuffer(1)});
  try{
    const queue=new GameSoundQueue({decodeAudioData:async()=>({numberOfChannels:3})},{});
    await assert.rejects(queue.load({sounds:{1:{name:'se_ch00.wav',volumeRaw:-1900}}},'',()=>''),/Unsupported original WAV channel count/);
    assert.equal(queue.buffers.size,0);
    queue.dispose();
  }finally{globalThis.fetch=originalFetch;}
});

test('production mono matrix is center unity, SetPan attenuation and no restart',()=>{
  const {queue,nodes}=queueWith({id:1,numberOfChannels:1});
  assert.equal(queue.play(1,0),true);
  assert.equal(queue.flush(),1);
  const source=nodes.find(value=>value.kind==='source');
  const gains=nodes.filter(value=>value.kind==='gain');
  const merger=nodes.find(value=>value.kind==='merger');
  assert.equal(nodes.filter(value=>value.kind==='splitter').length,0);
  assert.ok(source&&merger);
  assert.equal(source.loop,true);
  assert.equal(source.started,1);
  assert.equal(gains.filter(value=>value.gain.value===1).length,2);
  assert.equal(gains.filter(value=>value.gain.value===Math.pow(10,-1100/2000)).length,1);
  assert.equal(queue.position(1,192),true);
  assert.ok(gains.some(value=>Math.abs(value.gain.value-Math.pow(10,-.5))<1e-12));
  assert.equal(source.started,1);
  assert.equal(queue.position(1,-192),true);
  assert.ok(gains.some(value=>Math.abs(value.gain.value-Math.pow(10,-.5))<1e-12));
  assert.equal(source.started,1);
  assert.equal(nodes.filter(value=>value.kind==='source').length,1);
  source.onended();
  for(const gain of gains)assert.equal(gain.disconnected,true);
  assert.equal(merger.disconnected,true);
  assert.equal(queue.position(1,0),false);
});

test('production same-ID frame requests coalesce to one voice and preserve pending order',()=>{
  const nodes=[];const queue=new GameSoundQueue(context(nodes),{});
  queue.buffers.set(1,{buffer:{id:1,numberOfChannels:1},volumeRaw:0,loop:false});
  queue.buffers.set(2,{buffer:{id:2,numberOfChannels:1},volumeRaw:0,loop:false});
  assert.equal(queue.play(1,0),true);
  assert.equal(queue.play(1,192),true);
  assert.equal(queue.play(2,0),true);
  assert.equal(queue.flush(),2);
  const sources=nodes.filter(value=>value.kind==='source');
  assert.equal(sources.length,2);
  assert.equal(sources[0].buffer.id,1);
  assert.equal(sources[1].buffer.id,2);
  assert.equal(sources[0].started,1);
  assert.equal(sources[1].started,1);
  queue.dispose();
});

test('production stereo matrix preserves authored channels and onended releases graph',()=>{
  const {queue,nodes}=queueWith({id:2,numberOfChannels:2});
  assert.equal(queue.play(1,0),true);
  assert.equal(queue.flush(),1);
  const source=nodes.find(value=>value.kind==='source');
  const splitter=nodes.find(value=>value.kind==='splitter');
  const merger=nodes.find(value=>value.kind==='merger');
  const gains=nodes.filter(value=>value.kind==='gain');
  assert.ok(source&&splitter&&merger);
  assert.equal(splitter.channels,2);
  assert.ok(source.connections.some(edge=>edge.target===splitter));
  assert.equal(splitter.connections.length,2);
  assert.ok(gains.some(value=>value.connections.some(edge=>edge.target===merger&&edge.input===0)));
  assert.ok(gains.some(value=>value.connections.some(edge=>edge.target===merger&&edge.input===1)));
  assert.equal(gains.filter(value=>value.gain.value===1).length,2);
  source.onended();
  assert.equal(splitter.disconnected,true);
  assert.equal(merger.disconnected,true);
  for(const gain of gains)assert.equal(gain.disconnected,true);
  assert.equal(queue.position(1,0),false);
});
