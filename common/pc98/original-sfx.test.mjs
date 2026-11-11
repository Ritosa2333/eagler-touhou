import {test} from 'node:test';
import assert from 'node:assert/strict';
import {OriginalSfx} from './original-sfx.mjs';

test('failed original sound setup releases its context, node and worker',async()=>{
  const old={AudioContext:globalThis.AudioContext,AudioWorkletNode:globalThis.AudioWorkletNode,Worker:globalThis.Worker};
  const calls=[];
  globalThis.AudioContext=class {constructor(){this.destination={};this.audioWorklet={addModule:async()=>{}};}async resume(){}async close(){calls.push('close');}};
  globalThis.AudioWorkletNode=class {constructor(){this.port={};}connect(){}disconnect(){calls.push('disconnect');}};
  globalThis.Worker=class {terminate(){calls.push('terminate');}};
  try{
    await assert.rejects(OriginalSfx.create('th02',{find(){throw Error('missing driver');}}),/missing driver/);
    assert.deepEqual(calls,['terminate','disconnect','close']);
  }finally{for(const [key,value] of Object.entries(old)){if(value===undefined)delete globalThis[key];else globalThis[key]=value;}}
});

test('a rejected worklet module closes the newly created audio context',async()=>{
  const old=globalThis.AudioContext;let closed=false;
  globalThis.AudioContext=class {constructor(){this.audioWorklet={addModule:async()=>{throw Error('worklet unavailable');}};}async resume(){}async close(){closed=true;}};
  try{await assert.rejects(OriginalSfx.create('th04',{}),/worklet unavailable/);assert.equal(closed,true);}
  finally{if(old===undefined)delete globalThis.AudioContext;else globalThis.AudioContext=old;}
});

// Autoplay-blocked browsers can keep resume() pending until a later gesture.
test('a pending audio resume does not block startup or cleanup',async()=>{
  const old=globalThis.AudioContext;let closed=false;
  globalThis.AudioContext=class {constructor(){this.audioWorklet={addModule:async()=>{throw Error('worklet unavailable');}};}resume(){return new Promise(()=>{});}async close(){closed=true;}};
  try{
    await Promise.race([assert.rejects(OriginalSfx.create('th03',{}),/worklet unavailable/),new Promise((_,reject)=>setTimeout(()=>reject(Error('startup blocked on resume')),100))]);
    assert.equal(closed,true);
  }finally{if(old===undefined)delete globalThis.AudioContext;else globalThis.AudioContext=old;}
});
