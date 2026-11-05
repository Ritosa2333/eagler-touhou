import assert from 'node:assert/strict';
import test from 'node:test';
import {PeerSocket} from '../web/sdl-runtime/peer-socket.mjs';
import {Netplay} from '../web/sdl-runtime/netplay.mjs';

async function captureIceConfiguration({player=0,fallback=[],serverMessage={}}={}){
 const OldSocket=globalThis.WebSocket,OldPeer=globalThis.RTCPeerConnection,sockets=[],configurations=[];
 class Socket {static OPEN=1;constructor(url){this.url=String(url);this.readyState=1;this.sent=[];sockets.push(this);}send(value){this.sent.push(JSON.parse(value));}close(){this.readyState=3;}}
 class Peer {
  constructor(configuration){configurations.push(configuration);}
  createDataChannel(){return {readyState:'connecting',close(){this.readyState='closed';}};}
  async createOffer(){return {type:'offer',sdp:'local-offer'};}
  async createAnswer(){return {type:'answer',sdp:'local-answer'};}
  async setLocalDescription(value){this.localDescription=value;}
  async setRemoteDescription(value){this.remoteDescription=value;}
  async addIceCandidate(){}
  close(){}
 }
 globalThis.WebSocket=Socket;globalThis.RTCPeerConnection=Peer;let socket;
 try{
  socket=new PeerSocket(`ws://127.0.0.1:18142/?room=th19mp-1234&player=${player}&players=2&run=1`,{netplayIceServers:fallback,netplayPreferTurn:true});
  let createdBeforeJoin;
  if(player===0)await socket.control({type:'peers',peers:[1],...serverMessage});
  else{
   await socket.control({type:'peers',peers:[],...serverMessage});createdBeforeJoin=configurations.length;
   await socket.control({type:'peer-join',player:0});
   await socket.control({type:'signal',from:0,description:{type:'offer',sdp:'remote-offer'}});
  }
  return {configuration:configurations[0],createdBeforeJoin,answerType:socket.pc?.localDescription?.type};
 }finally{socket?.close();globalThis.WebSocket=OldSocket;globalThis.RTCPeerConnection=OldPeer;}
}

const launcherIceServers=[{urls:'stun:stun.cloudflare.com:3478'}];
const relayIceServers=[
 {urls:'stun:stun.relay.example:3478'},
 {urls:['turn:turn.relay.example:3478?transport=udp','turns:turn.relay.example:5349?transport=tcp'],username:'room-user',credential:'room-secret',credentialType:'password'},
];

test('server ICE servers replace Launcher fallback for both offer and peer-join answer',async t=>{
 for(const player of [0,1])await t.test(player===0?'offer side':'answer side waits for peer-join',async()=>{
  const result=await captureIceConfiguration({player,fallback:launcherIceServers,serverMessage:{iceServers:relayIceServers}});
  assert.deepEqual(result.configuration.iceServers,relayIceServers);
  assert.equal(result.configuration.iceTransportPolicy,'relay');
  if(player===1){assert.equal(result.createdBeforeJoin,0);assert.equal(result.answerType,'answer');}
 });
});

test('an empty server ICE list overrides Launcher fallback',async()=>{
 const result=await captureIceConfiguration({fallback:launcherIceServers,serverMessage:{iceServers:[]}});
 assert.deepEqual(result.configuration.iceServers,[]);
});

test('missing or invalid server ICE data preserves Launcher fallback',async t=>{
 for(const scenario of [
  {name:'missing on offer side',player:0,serverMessage:{}},
  {name:'invalid on peer-join answer side',player:1,serverMessage:{iceServers:{urls:'turn:invalid.example'}}},
 ])await t.test(scenario.name,async()=>{
  const result=await captureIceConfiguration({...scenario,fallback:launcherIceServers});
  assert.deepEqual(result.configuration.iceServers,launcherIceServers);
  if(scenario.player===1){assert.equal(result.createdBeforeJoin,0);assert.equal(result.answerType,'answer');}
 });
});

test('RTC must pass the shared route barrier before delivering game input',async()=>{
 const OldSocket=globalThis.WebSocket,OldPeer=globalThis.RTCPeerConnection,sockets=[];
 class Socket {static OPEN=1;constructor(url){this.url=String(url);this.readyState=1;this.sent=[];sockets.push(this);}send(value){this.sent.push(JSON.parse(value));}close(){this.readyState=3;}}
 const channel={readyState:'connecting',bufferedAmount:0,send(){},close(){this.readyState='closed';}};
 class Peer {createDataChannel(){return channel;}async createOffer(){return {type:'offer',sdp:'test-offer'};}async setLocalDescription(value){this.localDescription=value;}async setRemoteDescription(value){this.remoteDescription=value;}async addIceCandidate(){}close(){}}
 globalThis.WebSocket=Socket;globalThis.RTCPeerConnection=Peer;
 let peer;
 try{
  peer=new PeerSocket('ws://127.0.0.1:18142/?room=th19mp-1234&player=0&players=2&run=1');
  const received=[];peer.onmessage=event=>received.push(event.data);
  await peer.control({type:'peers',peers:[1],iceServers:[]});
  assert.ok(sockets[0].url.includes('signal=1'));
  assert.equal(sockets[0].sent[0].description.type,'offer');
  channel.readyState='open';channel.onopen();
  assert.equal(peer.readyState,0);
  const input=new Uint8Array([1,2,3]).buffer;channel.onmessage({data:input});assert.deepEqual(received,[]);
  await peer.control({type:'route',mode:'rtc'});
  assert.equal(peer.readyState,1);assert.equal(JSON.parse(received[0]).mode,'rtc');assert.equal(received[1],input);
  assert.throws(()=>peer.select('relay'),/conflict|冲突/);
 }finally{peer?.close();globalThis.WebSocket=OldSocket;globalThis.RTCPeerConnection=OldPeer;}
});
test('a server-selected Relay cannot silently bypass the client fallback preference',()=>{
 const OldSocket=globalThis.WebSocket;
 class Socket {static OPEN=1;constructor(){this.readyState=1;}close(){}send(){}}
 globalThis.WebSocket=Socket;let peer;
 try{peer=new PeerSocket('ws://127.0.0.1:18142/?room=th19mp-1234&player=1&run=1');assert.throws(()=>peer.select('relay'),/未启用/);}
 finally{peer?.close();globalThis.WebSocket=OldSocket;}
});
test('explicit fallback opens both streams before the server route barrier',async()=>{
 const OldSocket=globalThis.WebSocket,sockets=[];
 class Socket {static OPEN=1;constructor(url){this.url=String(url);this.readyState=1;sockets.push(this);}close(){}send(){}}
 globalThis.WebSocket=Socket;let peer;
 try{
  peer=new PeerSocket('ws://127.0.0.1:18142/?room=th19mp-1234&player=1&run=1',{netplayAllowWebsocketFallback:true});
  assert.equal(sockets.length,2);
  assert.ok(sockets[0].url.includes('signal=1'));
  assert.equal(new URL(sockets[1].url).searchParams.has('signal'),false);
  sockets[1].onopen();assert.equal(peer.readyState,0);
  await peer.control({type:'route',mode:'relay'});assert.equal(peer.readyState,1);
 }finally{peer?.close();globalThis.WebSocket=OldSocket;}
});

test('launcher peer exposes the live TH19 RTC connection across the route barrier',async()=>{
 const keys=['WebSocket','RTCPeerConnection','fetch','location','__th19PeerTransport','__eaglerNetplayTransport',
  '__eaglerNetplayPath','__eaglerNetplayLanActive','__eaglerNetplayLanFrame','__eaglerNetplayLanConfirmed',
  '__eaglerNetplayLanRollback','__eaglerNetplayLanResimulated','__eaglerNetplayInputDelayFrames'];
 const saved=new Map(keys.map(key=>[key,Object.getOwnPropertyDescriptor(globalThis,key)]));
 const channel={readyState:'connecting',bufferedAmount:0,send(){},close(){this.readyState='closed';}};
 const stats=new Map([['connection',{}]]);
 class Socket {static OPEN=1;constructor(url){this.url=String(url);this.readyState=1;this.sent=[];}send(value){this.sent.push(value);}close(){this.readyState=3;}}
 class Peer {
  createDataChannel(){return channel;}
  async createOffer(){return {type:'offer',sdp:'test-offer'};}
  async setLocalDescription(value){this.localDescription=value;}
  async setRemoteDescription(value){this.remoteDescription=value;}
  async addIceCandidate(){}
  async getStats(){return stats;}
  close(){}
 }
 for(const [key,value] of Object.entries({
  WebSocket:Socket,RTCPeerConnection:Peer,location:{href:'http://localhost/'},
  fetch:async()=>({ok:true,json:async()=>({game:'th19',protocol:'eagler-touhou/1',buildId:'a'.repeat(64)})}),
 }))Object.defineProperty(globalThis,key,{configurable:true,writable:true,value});
 let game;
 try{
  game=new Netplay({_th19_network_end(){}}, {onStatus(){},onClose(){},onResult(){}});
  await game.connect({netplayUrl:'ws://127.0.0.1:18142/?room=th19mp-1234&player=0&players=2&run=1',
   netplayPlayer:0,netplayPlayerCount:2,netplayLoadouts:[{character:0},{character:1}],netplayDifficulty:1});
  const transport=globalThis.__th19PeerTransport,exposed=transport.peers.get(1);
  const signal=game.socket.signal;
  signal.onmessage({data:JSON.stringify({type:'peers',peers:[1],iceServers:[]})});
  await game.socket.serial;
  assert.ok(exposed.pc instanceof Peer);
  assert.equal(await exposed.pc.getStats(),stats);
  assert.equal(exposed.inputOpen,false);
  assert.equal(exposed.controlOpen,false);

  channel.readyState='open';channel.onopen();
  assert.equal(exposed.inputOpen,false);
  signal.onmessage({data:JSON.stringify({type:'route',mode:'rtc'})});
  await game.socket.serial;
  assert.equal(game.socket.route,'rtc');
  assert.equal(exposed.inputOpen,true);
  assert.equal(exposed.controlOpen,true);

  game.prepared=true;game.peerReady=true;game.activate();
  assert.equal(globalThis.__th19PeerTransport.peers.get(1),exposed);
  channel.readyState='closed';channel.onclose();
  assert.equal(exposed.inputOpen,false);
  assert.equal(exposed.controlOpen,false);
 }finally{
  game?.close();
  for(const [key,descriptor] of saved){if(descriptor)Object.defineProperty(globalThis,key,descriptor);else delete globalThis[key];}
 }
});
