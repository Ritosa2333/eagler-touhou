import assert from 'node:assert/strict';
import test from 'node:test';
import {PeerSocket} from '../web/sdl-runtime/peer-socket.mjs';

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
