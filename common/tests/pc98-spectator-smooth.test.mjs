import {test} from 'node:test';
import assert from 'node:assert/strict';
import {createLocalSpectator,spectatorNextDelay} from '../browser/pc98-local-spectator.mjs';

for(const game of ['01','02','03','04'])test(`TH${game} normal playback presents each replay step even after scheduling delays`,async()=>{
  const header={game,tickMs:5},rows=Array.from({length:60},()=>({})),batches=[];
  let frame=0,presentations=0,stopped=false;
  const channel={ready:true,close(){},postMessage(message){
    if(message.type!=='pull')return;
    queueMicrotask(()=>channel.onmessage?.({data:{type:'slice',viewer:message.viewer,header,first:message.first,
      rows:rows.slice(message.first,message.first+60),available:rows.length,end:{frame:rows.length,reason:'end'}}}));
  }};
  const replica={get frame(){return frame;},get executed(){return frame;},stop(){stopped=true;},
    async advanceBatch(batch){batches.push(batch.length);frame+=batch.length;},async present(){presentations++;}};
  const viewer=createLocalSpectator({game,options:{netplayUrl:'ws://localhost/?room=test&run=1'},
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

test('viewer starts before a growing history is fully downloaded',async()=>{
  let created=false;
  const channel={ready:true,close(){},postMessage(message){
    if(message.type==='pull'&&message.first===0)queueMicrotask(()=>channel.onmessage?.({data:{type:'slice',viewer:message.viewer,
      header:{game:'01',tickMs:16},first:0,rows:[{}],available:100}}));
  }};
  const viewer=createLocalSpectator({game:'01',options:{netplayUrl:'ws://localhost/?room=test&run=1'},channelFactory:()=>channel,
    createReplica:async()=>{created=true;return {frame:0,stop(){},async present(){},async advance(){}};},
    validateHeader(){},validateRow(){},stable:JSON.stringify});
  try{await new Promise(resolve=>setTimeout(resolve,30));assert(created,'playback should start while later history is still downloading');}
  finally{viewer.stop();}
});

test('pause waits for an in-flight replica batch before reporting a settled position',async()=>{
  let frame=0,release,entered;
  const started=new Promise(resolve=>entered=resolve),pending=new Promise(resolve=>release=resolve);
  const channel={ready:true,close(){},postMessage(message){if(message.type==='pull'&&message.first===0)queueMicrotask(()=>channel.onmessage?.({data:{type:'slice',viewer:message.viewer,header:{game:'01',tickMs:16},first:0,rows:Array.from({length:60},()=>({})),available:60}}));}};
  const viewer=createLocalSpectator({game:'01',options:{netplayUrl:'ws://localhost/?room=test&run=1'},channelFactory:()=>channel,
    createReplica:async(_,{onFrame})=>({get frame(){return frame;},stop(){},async present(){onFrame({});},async advanceBatch(batch){entered();await pending;frame+=batch.length;}}),
    validateHeader(){},validateRow(){},stable:JSON.stringify});
  try{await started;viewer.pause(true);assert.equal(viewer.seeking,true);release();
    const deadline=Date.now()+1000;while(viewer.seeking&&Date.now()<deadline)await new Promise(resolve=>setTimeout(resolve,5));
    assert.equal(viewer.seeking,false);const settled=viewer.frame;await new Promise(resolve=>setTimeout(resolve,40));assert.equal(viewer.frame,settled);
  }finally{release();viewer.stop();}
});


test('spectator subtracts execution time from the next frame wait',()=>{
  assert.equal(spectatorNextDelay(0,20,14,true),6);
  assert.equal(spectatorNextDelay(0,20,21,true),0);
  assert.equal(spectatorNextDelay(1,20,2,true),0);
  assert.equal(spectatorNextDelay(0,20,21,false),8);
});
