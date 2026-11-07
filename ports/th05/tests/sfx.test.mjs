import test from 'node:test';
import assert from 'node:assert/strict';
import {createSfx,decodeWave} from '../web/sfx.mjs';
function wav(){const bytes=new Uint8Array(52),view=new DataView(bytes.buffer);const tag=(at,text)=>bytes.set(new TextEncoder().encode(text),at);tag(0,'RIFF');view.setUint32(4,44,true);tag(8,'WAVE');tag(12,'fmt ');view.setUint32(16,16,true);view.setUint16(20,1,true);view.setUint16(22,2,true);view.setUint32(24,44100,true);view.setUint32(28,176400,true);view.setUint16(32,4,true);view.setUint16(34,16,true);tag(36,'data');view.setUint32(40,8,true);view.setInt16(44,16384,true);view.setInt16(46,-16384,true);return bytes;}
function audio(){const sources=[];return {state:'running',destination:{},sources,createBuffer(channels,frames,rate){const data=Array.from({length:channels},()=>new Float32Array(frames));return {channels,frames,rate,getChannelData:channel=>data[channel]};},createBufferSource(){const value={stopped:false,started:false,connect(){},disconnect(){},start(){this.started=true;},stop(){this.stopped=true;}};sources.push(value);return value;}};}
test('original SE preserves stereo PCM and rejects malformed WAV',()=>{
 const context=audio(),buffer=decodeWave(context,wav());assert.equal(buffer.getChannelData(0)[0],.5);assert.equal(buffer.getChannelData(1)[0],-.5);
 assert.throws(()=>decodeWave(context,wav().subarray(0,47)),/Truncated/);
});
test('original SE restarts the same sound immediately without interrupting other effects',()=>{
 const context=audio(),files=new Map(Array.from({length:15},(_,i)=>[`s${String(i+1).padStart(2,'0')}.wav`,wav()])),sfx=createSfx(context,files);
 sfx.trigger(1);sfx.trigger(2);assert.equal(context.sources.length,2);assert.ok(context.sources.every(s=>s.started&&!s.stopped));
 sfx.trigger(1);assert.equal(context.sources[0].stopped,true);assert.equal(context.sources[1].stopped,false);assert.deepEqual(sfx.info().active,[2,1]);
 sfx.clear();assert.ok(context.sources.every(s=>s.stopped));sfx.close();sfx.trigger(3);assert.equal(context.sources.length,3);
});
