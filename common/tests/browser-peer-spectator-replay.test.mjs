import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {SpectatorReplayTransfer} from '../contracts/spectator-replay.mjs';
const source=readFileSync(new URL('../src/netplay/BrowserPeerTransport.cpp',import.meta.url),'utf8');
function embedded(name,args,context){const at=source.indexOf(', '+name+','),start=source.indexOf('{',at),end=source.indexOf('\n});',start);assert(at>=0&&end>start);return runInNewContext('('+args+')=>'+source.slice(start,end+2),context);}
const packets=[],timers=[];let now=0;
class Socket {static OPEN=1;static CONNECTING=0;static CLOSING=2;readyState=1;bufferedAmount=0;send(bytes){packets.push(bytes);}close(){this.readyState=3;}}
const heap=new Uint8Array(100000),state={retainSpectatorHistory:true,localPlayer:0,spectatorSession:{session:'0000000000000031'},relay:new Socket()};
const context={globalThis:{__eaglerPeerTransport:state},Module:{eaglerOptions:{}},HEAPU8:heap,HEAPU32:new Uint32Array(heap.buffer),WebSocket:Socket,
 URL,TextDecoder,Uint8Array,ArrayBuffer,Blob,location:{href:'https://example.test/'},Date:{now:()=>now},
 setTimeout:fn=>{timers.push(fn);return timers.length;},clearTimeout(){},setInterval:()=>1,clearInterval(){}};
const share=embedded('eagler_peer_share_replay','data,size,extended',context);
heap.fill(7,256,70256);state.relay.bufferedAmount=65536;share(256,70000,1);assert.equal(packets.length,0);
state.relay.bufferedAmount=0;
while(timers.length){now+=16;timers.shift()();}assert.equal(packets.length,3);
const file=new SpectatorReplayTransfer(state.spectatorSession.session);
for(const packet of packets)assert.equal(file.accept(packet),true);
assert.equal(file.complete,true);assert.equal(file.bytes.length,70000);assert.ok(file.bytes.every(b=>b===7));
const connect=embedded('eagler_spectator_connect','relayUrlPtr,spectatorIdPtr,tagPtr,playerCount,stageSpectatorHistory',context);
heap.set(new TextEncoder().encode('wss://example.test/room\0'),0);heap.set(new TextEncoder().encode('th06\0'),100);heap.set(new TextEncoder().encode('observer_123\0'),120);
assert.equal(connect(0,120,100,2,1),1);const receiver=context.globalThis.__eaglerPeerTransport;
receiver.relay.onmessage({data:JSON.stringify({type:'spectator-session',session:state.spectatorSession.session,seed:417,generation:0})});
for(const packet of file.chunks())receiver.relay.onmessage({data:packet.buffer});
assert.deepEqual(receiver.spectatorReplay.bytes,file.bytes);assert.match(receiver.spectatorReplay.name,/\.rpyx$/);
assert.equal(receiver.received.length,0);assert.equal(receiver.failed,false);
receiver.relay.onmessage({data:JSON.stringify({type:'spectator-session',session:'0000000000000032',seed:418,generation:1})});
for(const packet of file.chunks())receiver.relay.onmessage({data:packet.buffer});
assert.equal(receiver.spectatorReplay,null);assert.equal(receiver.replayReceiving,null);assert.equal(receiver.failed,false);
console.log('PASS actual embedded native Replay sender, bounded upload wait, Relay codec, isolated receive and stale-run rejection');
