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
