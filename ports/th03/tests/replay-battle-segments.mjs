import assert from 'node:assert/strict';


globalThis.document={createElement(){return {append(){},set textContent(value){},set href(value){},set hidden(value){},set title(value){}};}};
globalThis.fetch=async()=>new Response('source');
globalThis.BroadcastChannel=class{postMessage(){} close(){}};
const {createRecorder}=await import('../web/replay/recorder.js');
const {decodeTape}=await import('../web/replay/tape.js');
const produced=[];
const recorder=createRecorder(null,{onBattleReplay:value=>produced.push(value)});
await recorder.boot({settings:{},players:2,hostSlot:0,opening:{}});
const input={actions:[],touch:0,focusEnabled:true,points:false,commands:[]};
const row={inputs:[input,input],offlineMask:0,proof:'',hash:null,scene:1};
recorder.stage(0,row);recorder.stage(1,row);recorder.match({start:1,winner:0});
await new Promise(r=>setTimeout(r,10));assert.equal(produced.length,0);
recorder.confirm(2,false);recorder.match({start:1,winner:0});
recorder.stage(2,row);recorder.stage(3,row);recorder.confirm(4,false);recorder.match({start:3,winner:1});
for(let n=0;n<100&&produced.length<2;n++)await new Promise(r=>setTimeout(r,10));
assert.equal(produced.length,2);
const first=await decodeTape(produced[0].blob),second=await decodeTape(produced[1].blob);
assert.equal(first.matchStart,0);assert.equal(first.end.frame,2);assert.equal(second.matchStart,2);assert.equal(second.end.frame,4);
assert.equal(recorder.snapshot().rows.length,4);assert.deepEqual(produced.map(x=>x.winner),[0,1]);
recorder.close();console.log('TH03 confirmed match replay segmentation PASS');
