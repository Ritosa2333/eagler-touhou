import {test} from 'node:test';
import assert from 'node:assert/strict';

test('confirmed history is delivered to a local spectator after rollback correction',async()=>{
  const saved={document:globalThis.document,fetch:globalThis.fetch,Worker:globalThis.Worker,BroadcastChannel:globalThis.BroadcastChannel,__eaglerPC98Options:globalThis.__eaglerPC98Options};
  const channels=[];
  globalThis.document={createElement:()=>({})};
  globalThis.fetch=async()=>new Response('runtime');
  globalThis.__eaglerPC98Options=()=>({netplayPlayer:0,netplayUrl:'ws://localhost/?room=th02mp-1234&run=7'});
  globalThis.Worker=class {
    postMessage(message){queueMicrotask(()=>this.onmessage?.({data:{id:message.id,result:message.type==='replaySave'?{frame:0,bytes:new Uint8Array([1])}:{},status:{}}}));}
    terminate(){}
  };
  globalThis.BroadcastChannel=class {constructor(name){this.name=name;this.messages=[];channels.push(this);}postMessage(value){this.messages.push(value);}close(){}};
  try{
    const {createRecorder}=await import('../../ports/th02/web/replay/recorder.js');
    const recorder=createRecorder(null);
    await recorder.boot({settings:{},players:2,hostSlot:0,opening:{}});
    assert.equal(channels[1].name,'pc98-room-02-th02mp-1234-7');
    const row=proof=>({inputs:[32,0],offlineMask:0,proof,hash:null});
    recorder.stage(0,row('predicted'));recorder.rewind(0);recorder.stage(0,row('correct'));
    recorder.confirm(1,true);
    const channel=channels[0];channel.onmessage({data:{type:'pull',viewer:'test',first:0}});
    assert.equal(channel.messages[0].rows.length,0);
    recorder.confirm(1,false);
    await new Promise(resolve=>setTimeout(resolve,15));
    const room=channels[1];room.onmessage({data:{type:'pull',viewer:'test',first:0}});
    assert.deepEqual(room.messages.at(-1).rows,[row('correct')]);
    assert.equal(room.messages.at(-1).available,1);
    await new Promise(resolve=>setTimeout(resolve,20));recorder.close();
  }finally{Object.assign(globalThis,saved);}
});
