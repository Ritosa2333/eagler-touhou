import test from 'node:test';
import assert from 'node:assert/strict';
import {PmdPlayer} from '../pc98/pmd-player.mjs';

test('prewarmed PMD source is reused once and evicted sources are stopped',async()=>{
  const previous=globalThis.AudioWorkletNode,nodes=[];
  class Node {
    constructor(context){this.context=context;this.messages=[];this.port={postMessage:data=>{this.messages.push(data.type);if(data.type==='load')queueMicrotask(()=>this.port.onmessage({data:{type:'loaded',ok:true}}));}};nodes.push(this);}
    connect(){} disconnect(){this.disconnected=true;}
  }
  globalThis.AudioWorkletNode=Node;
  try{
    const player=new PmdPlayer({currentTime:0},{}),a=new Uint8Array([1]),b=new Uint8Array([2]),c=new Uint8Array([3]);
    player.prewarm(a);player.prewarm(a);player.prewarm(b);
    const source=await player.createSource(a);assert.equal(nodes.length,2,'same song shares preparation');
    await player.createSource(a);assert.equal(nodes.length,3,'a playing source is never reused');
    player.prewarm(a);player.prewarm(c);await new Promise(resolve=>setTimeout(resolve,10));
    assert(nodes[1].messages.includes('stop'),'evicted prepared source is released');
    source.stop(0);player.dispose();await new Promise(resolve=>setTimeout(resolve,10));
    assert(nodes[0].messages.includes('stop'));assert(nodes[3].messages.includes('stop'));assert(nodes[4].messages.includes('stop'));
  }finally{globalThis.AudioWorkletNode=previous;}
});
