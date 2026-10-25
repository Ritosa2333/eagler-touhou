import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
const source=readFileSync(new URL('../src/netplay/BrowserPeerTransport.cpp',import.meta.url),'utf8');
function embedded(name,args,context){const at=source.indexOf(', '+name+','),start=source.indexOf('{',at),end=source.indexOf('\n});',start);assert(at>=0&&end>start);return runInNewContext('('+args+')=>'+source.slice(start,end+2),context);}
const sockets=[];class Socket{static OPEN=1;static CONNECTING=0;static CLOSING=2;readyState=1;sent=[];constructor(){sockets.push(this);}send(b){this.sent.push(b);}close(){this.readyState=3;}}
const heap=new Uint8Array(1024);heap.set(new TextEncoder().encode('wss://example.test/room'),0);heap.set(new TextEncoder().encode('th06'),100);heap.set(new TextEncoder().encode('observer_123'),120);
const timers=new Map();let timerId=0;
const context={globalThis:{},Module:{eaglerOptions:{}},location:{href:'https://example.test/'},HEAPU8:heap,HEAPU32:new Uint32Array(heap.buffer),TextDecoder,URL,WebSocket:Socket,Uint8Array,ArrayBuffer,Blob,
 setTimeout:fn=>{timers.set(++timerId,fn);return timerId;},clearTimeout:id=>timers.delete(id),setInterval:()=>1,clearInterval(){}};
const connect=embedded('eagler_spectator_connect','relayUrlPtr,spectatorIdPtr,tagPtr,playerCount,stageSpectatorHistory',context);
const take=embedded('eagler_peer_take_spectator_session','words',context);
assert.equal(connect(0,120,100,2,1),1);const state=context.globalThis.__eaglerPeerTransport;
const descriptor=generation=>({type:'spectator-session',session:(9+generation).toString(16).padStart(16,'0'),seed:417+generation,generation});
const receive=(socket,session)=>socket.onmessage({data:JSON.stringify(session)});
state.receiveBinary(new Uint8Array([1]).buffer);receive(state.relay,descriptor(0));assert.equal(state.received.length,0);
assert.equal(take(256),1);assert.deepEqual([...context.HEAPU32.slice(64,67)],[9,0,417]);assert.equal(take(256),0);
state.receiveBinary(new Uint8Array([2]).buffer);receive(state.relay,descriptor(1));assert.equal(state.receivedBytes,0);assert.equal(take(256),1);assert.equal(context.HEAPU32[64],10);
for(let attempt=0;attempt<3;++attempt){const old=state.relay;old.onclose({code:1006,reason:''});assert.equal(state.failed,false);const fn=timers.get(state.reconnectTimer);timers.delete(state.reconnectTimer);fn();assert.notEqual(state.relay,old);receive(state.relay,descriptor(1));old.onmessage({data:new Uint8Array([99]).buffer});assert.equal(state.received.length,0);assert.equal(take(256),1);}
state.relay.onclose({code:1006,reason:'offline'});assert.equal(state.failed,true);assert.equal(state.error,'offline');
assert.equal(connect(0,120,100,2,1),1);const next=context.globalThis.__eaglerPeerTransport;receive(next.relay,descriptor(0));next.relay.onclose({code:1006,reason:''});assert.ok(timers.has(next.reconnectTimer));next.close();assert.equal(timers.size,0);
assert.equal(connect(0,120,100,2,1),1);const terminal=context.globalThis.__eaglerPeerTransport;receive(terminal.relay,descriptor(0));terminal.relay.onclose({code:1011,reason:'invalid source stream'});assert.equal(terminal.failed,true);assert.equal(timers.size,0);
assert.equal(connect(0,120,100,2,1),1);const opening=context.globalThis.__eaglerPeerTransport;
opening.relay.onclose({code:1006,reason:''});assert.equal(opening.failed,false);assert.equal(opening.spectatorSession,undefined);
const retry=timers.get(opening.reconnectTimer);timers.delete(opening.reconnectTimer);retry();receive(opening.relay,descriptor(0));assert.equal(take(256),1);assert.equal(opening.failed,false);opening.close();assert.equal(timers.size,0);
console.log('PASS spectator session queue replacement, exact native identity, opening/same-session recovery, three-attempt limit, stale-socket isolation and user/structural cancellation');
