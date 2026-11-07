import {test} from 'node:test';
import assert from 'node:assert/strict';
import {createLocalSpectator} from '../browser/pc98-local-spectator.mjs';

test('TH03 normal playback presents each replay step even after scheduling delays',async()=>{
  const header={game:'03',tickMs:5},rows=Array.from({length:60},()=>({})),batches=[];
  let frame=0,presentations=0,stopped=false;
  const channel={ready:true,close(){},postMessage(message){
    if(message.type!=='pull')return;
    queueMicrotask(()=>channel.onmessage?.({data:{type:'slice',viewer:message.viewer,header,first:message.first,
      rows:rows.slice(message.first,message.first+60),available:rows.length,end:{frame:rows.length,reason:'end'}}}));
  }};
  const replica={get frame(){return frame;},get executed(){return frame;},stop(){stopped=true;},
    async advanceBatch(batch){batches.push(batch.length);frame+=batch.length;},async present(){presentations++;}};
  const viewer=createLocalSpectator({game:'03',options:{netplayUrl:'ws://localhost/?room=test&run=1'},
    smoothPresentation:true,channelFactory:()=>channel,createReplica:async()=>replica,
    validateHeader(){},validateRow(){},stable:JSON.stringify});
  try{
    const deadline=Date.now()+5000;
    while(frame<rows.length&&Date.now()<deadline)await new Promise(resolve=>setTimeout(resolve,10));
    assert.equal(frame,rows.length);assert(batches.every(size=>size===1));
    assert(presentations>=rows.length,'normal playback should not discard frames by batching');
  }finally{viewer.stop();}
  assert(stopped);
});
