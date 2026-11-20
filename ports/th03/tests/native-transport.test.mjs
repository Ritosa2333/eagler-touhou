import test from 'node:test';
import assert from 'node:assert/strict';
import {createManagedTransport} from '../web/managed-transport.mjs';

test('signaling socket loss does not end a working RTC match',async()=>{
 const sockets=[];class Socket{constructor(url){this.url=url;this.readyState=1;sockets.push(this);}send(){}close(){this.readyState=3;}}
 class Peer{constructor(){this.connectionState='connected';}close(){} }
 const env={WebSocket:Socket,RTCPeerConnection:Peer},errors=[];
 const transport=createManagedTransport({netplayPlayer:1,netplayPlayerCount:2,netplayUrl:'ws://localhost/?room=th03mp-1111&run=1&member=guest'},['host','guest'],e=>errors.push(e),env);
 const channel=transport.channelFor('host');transport.connect();const signal=sockets[1];
 signal.onmessage({data:JSON.stringify({type:'peers',peers:[],iceServers:[]})});await new Promise(resolve=>setTimeout(resolve,0));
 const peer=env.__th03PeerTransport.peers.get(0),rtc={readyState:'open',bufferedAmount:0,send(){},close(){}};peer.rtc=rtc;signal.onmessage({data:JSON.stringify({type:'route',mode:'rtc'})});
 assert.equal(channel.readyState,'open');signal.onclose({});assert.equal(errors.length,0);assert.equal(channel.readyState,'open');channel.send('{}');transport.close();
});

test('transport failure reports its cause before a generic peer close',()=>{
 const sockets=[];class Socket{constructor(){this.readyState=1;sockets.push(this);}send(){}close(){this.readyState=3;}}
 const errors=[],env={WebSocket:Socket};
 const transport=createManagedTransport({netplayPlayer:1,netplayPlayerCount:2,netplayUrl:'ws://localhost/?run=1&member=guest'},['host','guest'],e=>errors.push(e.message),env);
 const channel=transport.channelFor('host');channel.onclose=()=>errors.push('generic peer close');transport.connect();
 sockets[0].onmessage({data:JSON.stringify({type:'error',error:'relay failure detail'})});
 assert.deepEqual(errors,['relay failure detail']);assert.equal(channel.readyState,'closed');
});

for(const game of ['th01','th02','th03','th04']){
 const {createManagedTransport}=await import(`../../${game}/web/managed-transport.mjs`);
 function fixture(local=0){
  const sockets=[],errors=[],connections=[];
  class Socket{
   constructor(url){this.url=url;this.readyState=1;this.sent=[];sockets.push(this);}
   send(text){this.sent.push(JSON.parse(text));}
   close(){this.readyState=3;}
   receive(message){this.onmessage?.({data:JSON.stringify(message)});}
  }
  class Peer{
   constructor(configuration){this.configuration=configuration;this.connectionState='new';this.offers=[];connections.push(this);}
   createOffer(options){this.offers.push(options);return {type:'offer',sdp:'offer'};}
   setLocalDescription(description){this.localDescription={toJSON:()=>description};}
   setRemoteDescription(description){this.remoteDescription=description;}
   async addIceCandidate(candidate){if(candidate.candidate==='stale')throw new DOMException('Old generation','OperationError');}
   createAnswer(){return {type:'answer',sdp:'answer'};}
   close(){this.connectionState='closed';this.onconnectionstatechange?.();}
  }
  const env={WebSocket:Socket,RTCPeerConnection:Peer};
  const roles=['host','guest'];
  const transport=createManagedTransport({netplayPlayer:local,netplayPlayerCount:2,netplayUrl:'ws://localhost/?run=1&member=guest'},roles,error=>errors.push(error.message),env);
  const channel=transport.channelFor(roles[local^1]);
  transport.connect();
  return {transport,channel,sockets,connections,errors,peer:env['__'+game+'PeerTransport'].peers.get(local^1)};
 }
 const settle=()=>new Promise(resolve=>setImmediate(resolve));
 test(`${game}: honors a server-selected TURN route and continues past a stale ICE candidate`,async()=>{
  const f=fixture(1);
  try {
   const signal=f.sockets[1],iceServers=[{urls:'turn:relay.invalid',username:'test',credential:'test'}];
   signal.receive({type:'peers',peers:[0],iceServers,initialIceRelayOnly:true});
   assert.deepEqual(f.connections[0].configuration,{iceServers,iceTransportPolicy:'relay'});
   signal.receive({type:'signal',from:0,candidate:{candidate:'stale'}});
   signal.receive({type:'signal',from:0,description:{type:'offer',sdp:'offer'}});
   await settle();
   assert.deepEqual(signal.sent.map(m=>m.description?.type??m.type),['answer']);
   assert.deepEqual(f.errors,[]);
  }finally{f.transport.close();}
 });
 test(`${game}: keeps the server's route failure cause`,()=>{
  const f=fixture();
  try {
   f.channel.onclose=()=>f.errors.push('generic close');
   f.sockets[0].receive({type:'route',mode:'failed',reason:'WebRTC connection timed out'});
   assert.deepEqual(f.errors,['WebRTC connection timed out']);
  }finally{f.transport.close();}
 });
 for(const local of [0,1])test(`${game}: restarts failed ICE without replacing seat ${local}'s gameplay channel`,async()=>{
  const f=fixture(local);
  try {
   const signal=f.sockets[1],sent=[];
   signal.receive({type:'peers',peers:[],iceServers:[]});
   const pc=f.connections[0];
   f.peer.rtc={readyState:'open',send:text=>sent.push(text),close(){}};
   signal.receive({type:'route',mode:'rtc'});
   pc.connectionState='failed';pc.onconnectionstatechange();
   await settle();
   if(local===0){
    assert.deepEqual(pc.offers,[{iceRestart:true}]);
    assert.deepEqual(signal.sent.map(m=>m.description?.type??m.type),['offer']);
   }else{
    assert.deepEqual(signal.sent,[{type:'ice-restart-request',to:0}]);
   }
   pc.connectionState='connected';pc.onconnectionstatechange();
   f.channel.send('{}');
   assert.deepEqual(sent,['{}']);assert.equal(f.channel.readyState,'open');assert.deepEqual(f.errors,[]);
  }finally{f.transport.close();}
 });
}
