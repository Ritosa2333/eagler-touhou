import assert from 'node:assert/strict';
import test from 'node:test';
import {SoundPlayer} from '../src/sound-player.mjs';

function wav({bits,channels,rate,samples,junk=[]}){
  const sampleBytes=bits/8,dataBytes=samples.length*sampleBytes;
  const junkBytes=junk.length?8+junk.length+(junk.length&1):0;
  const bytes=new Uint8Array(12+24+junkBytes+8+dataBytes+(dataBytes&1));
  const view=new DataView(bytes.buffer);
  const tag=(offset,value)=>{for(let i=0;i<4;i++)bytes[offset+i]=value.charCodeAt(i);};
  tag(0,'RIFF');view.setUint32(4,bytes.length-8,true);tag(8,'WAVE');
  tag(12,'fmt ');view.setUint32(16,16,true);view.setUint16(20,1,true);
  view.setUint16(22,channels,true);view.setUint32(24,rate,true);
  view.setUint32(28,rate*channels*sampleBytes,true);
  view.setUint16(32,channels*sampleBytes,true);view.setUint16(34,bits,true);
  let offset=36;
  if(junk.length){tag(offset,'JUNK');view.setUint32(offset+4,junk.length,true);bytes.set(junk,offset+8);offset+=junkBytes;}
  tag(offset,'data');view.setUint32(offset+4,dataBytes,true);offset+=8;
  for(let i=0;i<samples.length;i++){
    if(bits===8)bytes[offset+i]=samples[i];
    else view.setInt16(offset+i*2,samples[i],true);
  }
  return bytes;
}

function player(bytes){
  const context={
    createBuffer(channels,length,sampleRate){
      const data=Array.from({length:channels},()=>new Float32Array(length));
      return {numberOfChannels:channels,length,sampleRate,getChannelData:channel=>data[channel]};
    },
  };
  return new SoundPlayer({context,index:[{id:10,file:'sample.wav',volume:0}],readBytes:()=>bytes});
}

test('8-bit mono WAV retains its sample rate and respects a typed-array slice',async()=>{
  const source=wav({bits:8,channels:1,rate:22050,samples:[0,128,255]});
  const wrapped=new Uint8Array(source.length+8);wrapped.set(source,5);
  const buffer=await player(wrapped.subarray(5,5+source.length)).load('sample.wav');
  assert.equal(buffer.sampleRate,22050);
  assert.equal(buffer.numberOfChannels,1);
  assert.deepEqual([...buffer.getChannelData(0)],[-1,0,127/128]);
});

test('16-bit stereo WAV reads past an odd-sized JUNK chunk without mixing channels',async()=>{
  const source=wav({bits:16,channels:2,rate:44100,
    samples:[-32768,32767,16384,-16384],junk:[1,2,3]});
  const buffer=await player(source.buffer).load('sample.wav');
  assert.equal(buffer.sampleRate,44100);
  assert.equal(buffer.numberOfChannels,2);
  assert.deepEqual([...buffer.getChannelData(0)],[-1,0.5]);
  assert.deepEqual([...buffer.getChannelData(1)],[32767/32768,-0.5]);
});

test('incomplete WAV data is rejected before creating an audio buffer',async()=>{
  const source=wav({bits:8,channels:1,rate:22050,samples:[0,128,255]});
  await assert.rejects(player(source.subarray(0,-1)).load('sample.wav'),/WAV/);
});
