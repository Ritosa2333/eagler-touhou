import {test} from 'node:test';
import assert from 'node:assert/strict';
import {createLocalSpectator} from '../browser/pc98-local-spectator.mjs';
import {validateHeader,validateRow,VERSION,stable} from '../../ports/th02/web/replay/tape.js';

test('viewer advances a separate replica using confirmed room history',async()=>{
  const sent=[],applied=[];let channel,stopped=false,errors=[];
  const header={version:VERSION,game:'02',adapter:'confirmed-worker/1',players:2,hostSlot:0,tickMs:10,settings:{},identity:'a'.repeat(64),opening:{}};
  const viewer=createLocalSpectator({game:'02',options:{netplayUrl:'ws://localhost/?room=th02mp-1234&run=4'},validateHeader,validateRow,stable,
    channelFactory:name=>(channel={name,postMessage:m=>sent.push(m),close(){}}),
    createReplica:async()=>({get frame(){return applied.length;},async advance(row){applied.push(row);},async present(){},stop(){stopped=true;}}),onError:e=>errors.push(e)});
  assert.equal(channel.name,'pc98-room-02-th02mp-1234-4');
  const request=sent[0],row={inputs:[32,0],offlineMask:0,proof:'ok',hash:null};
  channel.onmessage({data:{type:'slice',viewer:request.viewer,header,first:0,rows:[row],available:1,end:null}});
  await new Promise(resolve=>setTimeout(resolve,80));
  assert.deepEqual(applied,[row]);assert.equal(viewer.received,1);assert.equal(viewer.frame,1);assert.deepEqual(errors,[]);
  viewer.stop();assert.equal(stopped,true);
});

test('viewer rejects conflicting confirmed history',async()=>{
  let channel,error;
  const header={version:VERSION,game:'02',adapter:'confirmed-worker/1',players:2,hostSlot:0,tickMs:10,settings:{},identity:'a'.repeat(64),opening:{}};
  let request;const viewer=createLocalSpectator({game:'02',options:{netplayUrl:'ws://localhost/?room=r&run=1'},validateHeader,validateRow,stable,
    channelFactory:()=>channel={postMessage:m=>request=m,close(){}},createReplica:async()=>({frame:0,async present(){},stop(){}}),onError:e=>error=e});
  const row={inputs:[32,0],offlineMask:0,proof:'ok',hash:null};
  channel.onmessage({data:{type:'slice',viewer:request.viewer,header,first:0,rows:[row],available:1,end:null}});
  channel.onmessage({data:{type:'slice',viewer:request.viewer,header,first:0,rows:[{...row,proof:'changed'}],available:1,end:null}});
  assert.match(error.message,/重传冲突/);viewer.stop();
});

test('a late viewer restores the latest confirmed checkpoint',async()=>{
  let channel,request,restored=false,frame=0,error;
  const header={version:VERSION,game:'02',adapter:'confirmed-worker/1',players:2,hostSlot:0,tickMs:10,settings:{},identity:'a'.repeat(64),opening:{}};
  const row={inputs:[32,0],offlineMask:0,proof:'ok',hash:null},points=[{frame:0},{frame:2}];
  const viewer=createLocalSpectator({game:'02',options:{netplayUrl:'ws://localhost/?room=r&run=1'},validateHeader,validateRow,stable,
    validatePointIndex:index=>index,readLocalPoint:async(_channel,_viewer,point)=>new Uint8Array([point.frame]),
    channelFactory:()=>channel={postMessage:m=>request=m,close(){}},
    createReplica:async()=>({get frame(){return frame;},async restore(bytes,base){assert.deepEqual([...bytes],[2]);assert.deepEqual([...base],[0]);frame=2;restored=true;return {frame,proof:'ok'};},async advance(){frame++;},async present(){},stop(){}}),onError:e=>error=e});
  channel.onmessage({data:{type:'slice',viewer:request.viewer,header,first:0,rows:[row,row,row],available:3,points,end:null}});
  await new Promise(resolve=>setTimeout(resolve,50));
  assert.equal(restored,true);assert.equal(viewer.frame,3);assert.equal(error,undefined);viewer.stop();
});

test('viewer rewinds independently and can return to live',async()=>{
  let channel,request,created=0,error;
  const header={version:VERSION,game:'02',adapter:'confirmed-worker/1',players:2,hostSlot:0,tickMs:10,settings:{},identity:'a'.repeat(64),opening:{}};
  const row={inputs:[32,0],offlineMask:0,proof:'ok',hash:null};
  const viewer=createLocalSpectator({game:'02',options:{netplayUrl:'ws://localhost/?room=r&run=1'},validateHeader,validateRow,stable,
    channelFactory:()=>channel={postMessage:m=>request=m,close(){}},
    createReplica:async()=>{created++;let frame=0;return {get frame(){return frame;},async advance(){frame++;},async present(){},stop(){}};},onError:e=>error=e});
  channel.onmessage({data:{type:'slice',viewer:request.viewer,header,first:0,rows:Array(20).fill(row),available:20,end:null}});
  await new Promise(resolve=>setTimeout(resolve,100));assert.equal(viewer.frame,20);
  viewer.seek(5);await new Promise(resolve=>setTimeout(resolve,80));assert.equal(viewer.frame,5);assert(viewer.paused);assert.equal(created,2);
  viewer.live();await new Promise(resolve=>setTimeout(resolve,80));assert.equal(viewer.frame,20);assert(viewer.following);assert.equal(error,undefined);viewer.stop();
});
