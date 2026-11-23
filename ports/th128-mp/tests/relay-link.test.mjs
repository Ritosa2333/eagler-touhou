import assert from 'node:assert/strict';
import {test} from 'node:test';
import {openRelayLink} from '../runtime/relay-link.mjs';

class Socket extends EventTarget {
  readyState=0;bufferedAmount=0;sent=[];closeRequests=[];
  constructor(url){super();this.url=url;Socket.last=this;}
  open(){this.readyState=1;this.dispatchEvent(new Event('open'));}
  send(bytes){this.sent.push(bytes);}
  incoming(bytes){this.dispatchEvent(new MessageEvent('message',{data:bytes.buffer}));}
  control(message){this.dispatchEvent(new MessageEvent('message',{data:JSON.stringify(message)}));}
  close(code){this.closeRequests.push(code);this.readyState=3;this.dispatchEvent(new Event('close'));}
}
const endpoint='ws://127.0.0.1:53128/?room=th128mp-53128&run=1&player=0&players=2';
const encode=value=>new TextEncoder().encode(JSON.stringify(value));
const options={url:endpoint,localPlayer:0,WebSocketClass:Socket};

test('author Relay sender envelope supplies the remote seat and async receive order',async()=>{
  let settle;const blocked=new Promise(resolve=>{settle=resolve;}),rows=[];
  const link=openRelayLink({...options,async onMessage(row,seat){if(row.frame===0)await blocked;rows.push([row,seat]);}});
  const socket=Socket.last;socket.open();await link.ready;
  link.send({frame:2});assert.deepEqual(socket.sent[0],encode({frame:2}));
  for(let frame=0;frame<2;frame++)socket.incoming(new Uint8Array([0xe6,1,...encode({frame})]));
  await Promise.resolve();assert.deepEqual(rows,[]);settle();await link.drained();
  assert.deepEqual(rows,[[{frame:0},1],[{frame:1},1]]);
  link.close();assert.equal(socket.readyState,3);
});

test('untagged gameplay requires an explicit two-seat loopback link',async()=>{
  assert.throws(()=>openRelayLink({...options,url:endpoint.replace('127.0.0.1','example.invalid'),loopbackLegacy:true,onMessage(){}}),/loopback/);
  assert.throws(()=>openRelayLink({...options,url:endpoint.replace('players=2','players=3'),onMessage(){}}),/seat/);
  const fail=[],link=openRelayLink({...options,onError:e=>fail.push(e.message),onMessage(){assert.fail('untagged packet escaped the source gate');}});
  const socket=Socket.last;socket.open();await link.ready;socket.incoming(encode({frame:0}));await link.drained();
  assert.deepEqual(fail,['Relay gameplay sender envelope missing']);assert.equal(socket.readyState,3);
  const rows=[],local=openRelayLink({...options,loopbackLegacy:true,onMessage:(row,seat)=>rows.push([row,seat])});
  const next=Socket.last;next.open();await local.ready;next.incoming(encode({frame:0}));await local.drained();
  assert.deepEqual(rows,[[{frame:0},1]]);local.close();
});

test('a room-bound link waits for the author Relay route and keeps host spectator uploads on their own lane',async()=>{
  const address=endpoint+'&member=private-host-id';
  const messages=[],link=openRelayLink({...options,url:address,requireRoute:true,spectatorUpload:true,
    onMessage:(row,seat)=>messages.push([row,seat])});
  const socket=Socket.last;socket.open();await link.ready;
  assert.throws(()=>link.send({frame:0}),/route/);
  assert.throws(()=>link.sendSpectator(new Uint8Array([0xe8,1])),/route/);
  socket.control({type:'route',mode:'relay'});await link.routeReady;
  link.sendSpectator({type:'spectator-session',session:'1234567890abcdef',seed:34772,generation:0});
  const packet=new Uint8Array(49);packet.set([0xe8,0x45,0x48,0x4e,0x50]);link.sendSpectator(packet);
  assert.equal(typeof socket.sent[0],'string');assert.deepEqual(socket.sent[1],packet);
  socket.incoming(new Uint8Array([0xe6,1,...encode({frame:0})]));await link.drained();
  assert.deepEqual(messages,[[{frame:0},1]]);link.close();
  assert.throws(()=>openRelayLink({...options,url:address.replace('player=0','player=1'),localPlayer:1,
    requireRoute:true,spectatorUpload:true,onMessage(){}}),/host/);
});

test('route control never becomes a gameplay message and failed routing closes the branded lane',async()=>{
  const faults=[],link=openRelayLink({...options,url:endpoint+'&member=private-host-id',requireRoute:true,
    onMessage(){assert.fail('unexpected gameplay');},onError:error=>faults.push(error.message)});
  const socket=Socket.last;socket.open();await link.ready;socket.control({type:'route',mode:'failed',reason:'unavailable'});
  await assert.rejects(link.routeReady,/unavailable/);await link.drained();
  assert.equal(socket.readyState,3);assert.equal(faults.length,1);
});
