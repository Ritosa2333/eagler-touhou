import test from 'node:test';
import assert from 'node:assert/strict';
for (const game of ['02','04']) test(`TH${game} keeps an established direct game running if signaling disconnects`,async()=>{
 const {createManagedTransport}=await import(`../../ports/th${game}/web/managed-transport.mjs`);
 const sockets=[];class Socket {constructor(url){this.url=url;this.readyState=1;this.sent=[];sockets.push(this);}send(value){this.sent.push(value);}close(){this.readyState=3;this.onclose?.();}}
 const environment={WebSocket:Socket},failures=[],roles=['host','guest'];
 const transport=createManagedTransport({netplayPlayer:0,netplayPlayerCount:2,netplayUrl:'ws://example.test/?member=member&run=1'},roles,e=>failures.push(e),environment);
 const channel=transport.channelFor('guest');let opened=0;channel.onopen=()=>opened++;
 transport.connect();const status=environment[`__th${game}PeerTransport`],peer=status.peers.get(1),sent=[];
 peer.rtc={readyState:'open',bufferedAmount:0,send:value=>sent.push(value),close(){}};
 sockets[1].onmessage({data:JSON.stringify({type:'route',mode:'rtc'})});
 assert.equal(opened,1);sockets[1].close();sockets[0].close();
 assert.equal(channel.readyState,'open');assert.equal(status.disconnected,false);assert.deepEqual(failures,[]);
 channel.send('{"type":"input","frame":1}');assert.equal(sent.length,1);
 await new Promise(resolve=>setTimeout(resolve,1100));assert.equal(sockets.length,3,'signaling reconnects while RTC stays active');
 sockets[2].onmessage({data:JSON.stringify({type:'peers',peers:[1],route:'rtc'})});assert.equal(peer.rtc.readyState,'open');assert.equal(channel.readyState,'open');assert.deepEqual(failures,[]);transport.close();
});

for (const game of ['02','04']) test(`TH${game} keeps relayed gameplay running while signaling reconnects`,async()=>{
 const {createManagedTransport}=await import(`../../ports/th${game}/web/managed-transport.mjs`);
 const sockets=[];class Socket {constructor(){this.readyState=1;this.sent=[];sockets.push(this);}send(value){this.sent.push(value);}close(){this.readyState=3;this.onclose?.();}}
 const environment={WebSocket:Socket},failures=[];
 const transport=createManagedTransport({netplayPlayer:0,netplayPlayerCount:2,netplayUrl:'ws://example.test/?member=member&run=1'},['host','guest'],e=>failures.push(e),environment);
 const channel=transport.channelFor('guest');transport.connect();sockets[1].onmessage({data:JSON.stringify({type:'route',mode:'relay'})});
 sockets[1].close();channel.send('{"type":"input","frame":1}');assert.equal(sockets[0].sent.length,1);assert.equal(channel.readyState,'open');
 await new Promise(resolve=>setTimeout(resolve,1100));assert.equal(sockets.length,3);sockets[2].onmessage({data:JSON.stringify({type:'peers',peers:[1],route:'relay'})});assert.deepEqual(failures,[]);assert.equal(channel.readyState,'open');transport.close();
});
