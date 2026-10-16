import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
const source=readFileSync(new URL('../include/eagler/audio/WebSfx.hpp',import.meta.url),'utf8');
const body=source.split('// BEGIN_WEB_SFX_JS')[1].split('// END_WEB_SFX_JS')[0];
const command=new Function('Module','HEAPU8','op','id','pcm','frames','channels','rate','format','value',body);
function context(){
 const nodes=[];
 const node=()=>{const n={connections:[],disconnect(){this.connections=[];},connect(...args){this.connections.push(args);return args[0];}};nodes.push(n);return n;};
 return {state:'running',currentTime:0,destination:{},nodes,
  createBuffer(channels,length,sampleRate){const pcm=Array.from({length:channels},()=>new Float32Array(length));return {length,sampleRate,duration:length/sampleRate,getChannelData:c=>pcm[c]};},
  createGain(){return {...node(),gain:{value:1,setValueAtTime(value){this.value=value;}}};},
  createChannelSplitter:node,createChannelMerger:node,
  createBufferSource(){return {...node(),playbackRate:{value:1},start(when,offset){this.offset=offset;this.starts=(this.starts||0)+1;},stop(){this.stopped=true;}};}
 };
}
const ctx=context(),Module={SDL3:{audioContext:ctx}},heap=new Uint8Array(4096);
new Float32Array(heap.buffer,128,4).set([0.25,-0.5,0.5,-0.25]);
const call=(op,id=1,value=0,frames=2)=>command(Module,heap,op,id,128,frames,2,100,0,value);
assert.equal(call(0),1);
let voice=Module.eaglerSfxOutput.voices.get(1);
assert.deepEqual([...voice.buffer.getChannelData(0)],[0.25,0.5]);
assert.deepEqual([...voice.buffer.getChannelData(1)],[-0.5,-0.25]);
call(5,1,0.4);call(6,1,-0.75);
assert.equal(voice.left.gain.value,0.4);assert.ok(Math.abs(voice.right.gain.value-0.1)<1e-9);
call(2);const old=voice.node;ctx.currentTime=.005;
assert.ok(Math.abs(call(8)-.5)<1e-9);
call(10,0,1);assert.equal(old.stopped,true);assert.equal(voice.node,null);assert.equal(call(9),1);
ctx.currentTime=2;call(10,0,0);assert.equal(voice.node.offset,.005);
const resumed=voice.node;call(10,0,0);assert.equal(voice.node,resumed);
call(7,1,1);assert.equal(voice.node,resumed,'unchanged pitch must not restart output');
call(3);assert.equal(call(9),0);
call(4,1,1);call(2);assert.equal(voice.node.offset,.01);
call(1);assert.equal(call(11),0);assert.equal(voice.merge.connections.length,0);
call(0);call(2);voice=Module.eaglerSfxOutput.voices.get(1);const removed=voice.node;
assert.equal(call(0,1,0,0),0);assert.equal(removed.stopped,true);assert.equal(call(11),0,'failed replacement must retire old output');
call(0);call(2);const previous=Module.eaglerSfxOutput.voices.get(1);
Module.SDL3.audioContext=context();call(0,2);
assert.equal(previous.node,null);assert.equal(call(11,1),0);assert.equal(call(11,2),1);
call(12);assert.equal(Module.eaglerSfxOutput.voices.size,0);
console.log('Web SFX PCM, gain/balance, cursor, suspend, replacement and context lifecycle: PASS');
