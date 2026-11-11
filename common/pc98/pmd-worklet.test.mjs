import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import vm from 'node:vm';

let Processor, renders=0;
const memory=new WebAssembly.Memory({initial:1});
const exports={memory,pmd_create:()=>1,pmd_input:()=>0,pmd_load:()=>1,
  pmd_render:()=>{renders++;return 1024;},pmd_destroy:()=>{}};
class AudioWorkletProcessor {constructor(){this.port={postMessage(){}};}}
const context={AudioWorkletProcessor,sampleRate:44100,Uint8Array,Float32Array,
  WebAssembly:{Instance:class {constructor(){this.exports=exports;}}},
  registerProcessor:(_name,type)=>{Processor=type;}};
vm.runInNewContext(await readFile(new URL('./pmd-worklet.js',import.meta.url),'utf8'),context);
const processor=new Processor({processorOptions:{module:{}}}),output=[[new Float32Array(128),new Float32Array(128)]];
processor.port.onmessage({data:{type:'load',bytes:new Uint8Array([1])}});
processor.process([],output);processor.process([],output);
assert.equal(renders,0,'prewarming must not advance the song');
processor.port.onmessage({data:{type:'start'}});processor.process([],output);
assert.equal(renders,1,'playback advances after the game starts the song');
processor.port.onmessage({data:{type:'stop'}});
assert.equal(processor.process([],output),false);
console.log('PASS PMD prewarm starts at the beginning');
