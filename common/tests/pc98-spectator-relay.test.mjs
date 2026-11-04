import {test} from 'node:test';
import assert from 'node:assert/strict';
import {createPc98SpectatorPublisher} from '../browser/pc98-spectator-publisher.mjs';
import {createPc98SpectatorChannel} from '../browser/pc98-spectator-channel.mjs';
import {Pc98SpectatorRelay} from '../../services/relay/src/pc98-spectator.mjs';
import {createHash} from 'node:crypto';

test('remote spectator consumes ordered relay history and end',async()=>{
  const relay=new Pc98SpectatorRelay(2),errors=[],received=[];let receiver;
  const host={readyState:1,bufferedAmount:0,send(packet){assert.equal(packet[0],0xe8);const payload=packet.subarray(1);relay.accept(payload);receiver.onmessage({data:payload.slice().buffer});}};
  class Socket{constructor(url){receiver=this;this.url=url;}close(){}}
  const channel=createPc98SpectatorChannel({netplayUrl:'ws://localhost/?room=th02mp-1234&run=1',netplayPlayerCount:2,netplaySpectatorId:'viewer'},e=>errors.push(e),Socket);
  channel.onmessage=({data})=>received.push(data);
  const rows=Array.from({length:61},(_,i)=>({i})),tape={length:61,end:{frame:61},slice:(a,b)=>rows.slice(a,b)};
  const publisher=createPc98SpectatorPublisher({game:'02',players:2},tape,()=>host,e=>errors.push(e));
  for(let i=0;i<100&&!publisher.ended;i++)await new Promise(resolve=>setTimeout(resolve,10));
  await new Promise(resolve=>setTimeout(resolve,30));
  channel.postMessage({type:'pull',viewer:'v',first:0});await new Promise(resolve=>setTimeout(resolve,10));
  assert.equal(received[0].available,61);assert.deepEqual(received[0].rows,rows.slice(0,60));
  channel.postMessage({type:'pull',viewer:'v',first:60});await new Promise(resolve=>setTimeout(resolve,10));
  assert.deepEqual(received[1].rows,rows.slice(60));assert.equal(received[1].end.frame,61);
  assert.deepEqual(errors,[]);publisher.stop();channel.close();
});

test('confirmed checkpoint is retained and verified across the relay',async()=>{
  const relay=new Pc98SpectatorRelay(2),errors=[];let receiver;
  const host={readyState:1,bufferedAmount:0,send(packet){const payload=packet.subarray(1);relay.accept(payload);receiver.onmessage({data:payload.slice().buffer});}};
  class Socket{constructor(){receiver=this;}close(){}}
  const channel=createPc98SpectatorChannel({netplayUrl:'ws://localhost/?room=th02mp-1234&run=1',netplayPlayerCount:2,netplaySpectatorId:'viewer'},e=>errors.push(e),Socket);
  const bytes=new Uint8Array(25000).fill(42),sha=createHash('sha256').update(bytes).digest('hex'),point={frame:0,sha,data:Buffer.from(bytes).toString('base64')};
  const tape={length:0,end:null,slice:()=>[]},publisher=createPc98SpectatorPublisher({game:'02',players:2},tape,()=>host,e=>errors.push(e),()=>[point]);
  for(let i=0;i<100&&relay.lastPoint!==0;i++)await new Promise(resolve=>setTimeout(resolve,10));
  assert.equal(relay.lastPoint,0);const bootstrap=relay.bootstrap();receiver.onmessage({data:Uint8Array.from(bootstrap).buffer});
  await new Promise(resolve=>setTimeout(resolve,30));
  let response;channel.addEventListener('message',event=>response=event.data);channel.postMessage({type:'pull-point',viewer:'v',frame:0,offset:0});
  await new Promise(resolve=>setTimeout(resolve,10));assert.equal(response.sha,sha);assert.equal(Buffer.from(response.payload,'base64').length,24576);
  assert.deepEqual(errors,[]);publisher.stop();channel.close();
});
