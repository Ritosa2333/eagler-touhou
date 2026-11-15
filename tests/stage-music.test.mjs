import assert from 'node:assert/strict';
import test from 'node:test';
import * as audio from '../site/runtime/sound.mjs';
const track={name:'th128_00.wav',file:'music/th128_00.wav',channels:2,sampleRate:44100,loopStart:13.070294784580499,loopEnd:84.62331065759638};
function fixture(){
 const nodes=[];
 const context={decodeAudioData:async()=>({duration:track.loopEnd}),createBufferSource:()=>{
  const node={connect:()=>{},disconnect:()=>{node.disconnected=true;},start:()=>{node.started=true;},stop:()=>{node.stopped=true;}};
  nodes.push(node);return node;
 }};return {context,nodes};
}
test('actual stage music uses th128_00 and native loop bounds, stops on disposal',async()=>{
 assert.equal(typeof audio.StageMusic,'function','actual stage BGM lifecycle missing');
 const {context,nodes}=fixture(),oldFetch=globalThis.fetch,requests=[];
 globalThis.fetch=async url=>{requests.push(url);return {ok:true,arrayBuffer:async()=>new ArrayBuffer(44)};};
 const music=new audio.StageMusic(context,{});
 try{
  await music.load([track],'http://127.0.0.1/assets/',(base,relative)=>new URL(relative,base).href,{});
  assert.deepEqual(requests,['http://127.0.0.1/assets/music/th128_00.wav']);assert.equal(nodes.length,0);
  music.start();assert.equal(nodes[0].started,true);assert.equal(nodes[0].loop,true);
  assert.equal(nodes[0].loopStart,track.loopStart);assert.equal(nodes[0].loopEnd,track.loopEnd);
  assert.equal(music.snapshot().track,track.name);assert.equal(music.snapshot().playing,true);
  assert.throws(()=>music.start(),/started/);music.dispose();music.dispose();
  assert.equal(nodes[0].stopped,true);assert.equal(nodes[0].disconnected,true);assert.throws(()=>music.start(),/disposed/);
  const missing=new audio.StageMusic(context,{});
  await assert.rejects(()=>missing.load([],'',()=>'',{}),/stage|metadata/);
 }finally{globalThis.fetch=oldFetch;music.dispose();}
});
test('BGM aborted decoding cannot publish after resource switch',async()=>{
 assert.equal(typeof audio.StageMusic,'function','actual stage BGM lifecycle missing');
 const {context,nodes}=fixture(),controller=new AbortController(),oldFetch=globalThis.fetch;
 let decode;context.decodeAudioData=()=>new Promise(resolve=>{decode=resolve;});
 globalThis.fetch=async()=>({ok:true,arrayBuffer:async()=>new ArrayBuffer(44)});
 const music=new audio.StageMusic(context,{});
 try{
  const pending=music.load([track],'http://127.0.0.1/assets/',(base,relative)=>new URL(relative,base).href,{signal:controller.signal});
  await new Promise(resolve=>setImmediate(resolve));controller.abort();music.dispose();decode({duration:track.loopEnd});
  await assert.rejects(pending,/abort|disposed/i);assert.equal(nodes.length,0);assert.equal(music.snapshot().playing,false);
  const invalid=new audio.StageMusic(context,{});
  await assert.rejects(()=>invalid.load([{...track,loopStart:-1}],'',()=>'',{}),/loop|metadata/);
 }finally{globalThis.fetch=oldFetch;music.dispose();}
});
test('authored MSG19 switches to a preloaded original Boss track without a second decode during gameplay',async()=>{
 const {context,nodes}=fixture(),oldFetch=globalThis.fetch,requests=[];
 const boss={...track,name:'th128_02.wav',file:'music/th128_02.wav'};
 globalThis.fetch=async url=>{requests.push(url);return {ok:true,arrayBuffer:async()=>new ArrayBuffer(44)};};
 const music=new audio.StageMusic(context,{});
 try{
  await music.load([track,boss],'http://127.0.0.1/assets/',(base,relative)=>new URL(relative,base).href,{tracks:['th128_00.wav','th128_02.wav']});
  music.start();music.switchTo('th128_02.wav');assert.equal(nodes[0].stopped,true);assert.equal(nodes[1].started,true);
  assert.equal(music.snapshot().track,'th128_02.wav');assert.equal(requests.length,2);music.switchTo('th128_02.wav');assert.equal(nodes.length,2);
  assert.throws(()=>music.switchTo('th128_06.wav'),/preload|track/i);
 }finally{globalThis.fetch=oldFetch;music.dispose();}
});
