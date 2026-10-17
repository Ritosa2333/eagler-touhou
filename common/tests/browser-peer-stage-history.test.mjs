import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
const source=readFileSync(new URL('../src/netplay/BrowserPeerTransport.cpp',import.meta.url),'utf8');
function embedded(name,args,context){
 const at=source.indexOf(', '+name+','),start=source.indexOf('{',at),end=source.indexOf('\n});',start);
 assert(at>=0&&end>start);return runInNewContext('('+args+')=>'+source.slice(start,end+2),context);
}
class Socket{
 static OPEN=1;static CONNECTING=0;static CLOSING=2;readyState=1;
 send(){}close(){this.readyState=3;}
}
function sandbox(){
 const heap=new Uint8Array(512);heap.set(new TextEncoder().encode('wss://example.test/room'),0);heap.set(new TextEncoder().encode('th06'),100);heap.set(new TextEncoder().encode('observer_123'),120);
 const global={};return {globalThis:global,Module:{eaglerOptions:{}},location:{href:'https://example.test/'},HEAPU8:heap,TextDecoder,TextEncoder,
   URL,WebSocket:Socket,Uint8Array,ArrayBuffer,Blob,setTimeout:()=>1,clearTimeout(){},setInterval:()=>1,clearInterval(){}};
}
for(const [seat,retain,open] of [[0,true,true],[0,false,false],[1,true,false]]){
 const context=sandbox(),connect=embedded('eagler_peer_connect','relayUrlPtr,tagPtr,localPlayer,playerCount,stageSpectatorHistory',context);
 assert.equal(connect(0,100,seat,2,+retain),1);const state=context.globalThis.__eaglerPeerTransport;
 state.updateRtcPath=async()=>{};state.setRoute('rtc');assert.equal(state.relay.readyState===1,open);
 assert.equal(state.spectatorCount,0,'retaining confirmed history must not invent a viewer');
}
for(const staged of [false,true]){
 const context=sandbox(),connect=embedded('eagler_spectator_connect','relayUrlPtr,spectatorIdPtr,tagPtr,playerCount,stageSpectatorHistory',context);
 assert.equal(connect(0,120,100,2,+staged),1);const state=context.globalThis.__eaglerPeerTransport;
 assert.equal(state.maxReceivedPackets,staged?131072:16384);
 const bytes=new Uint8Array(48).buffer;
 for(let i=0;i<20000;++i)state.receiveBinary(bytes);
 assert.equal(state.failed,!staged);
 if(staged){
  assert.equal(state.receivedBytes,20000*48);state.maxReceivedBytes=state.receivedBytes;
  const copy=embedded('eagler_peer_poll_copy','out,capacity',context);
  assert.equal(copy(200,48),48);assert.equal(state.receivedBytes,19999*48);
  state.receiveBinary(bytes);assert.equal(state.failed,false);
  state.receiveBinary(bytes);assert.equal(state.failed,true,'byte capacity remains bounded after raising the stage packet count');
 }
}
console.log('PASS P1 retains an empty-viewer RTC spectator lane only for complete stage history; long-stage receive queue stays byte/packet bounded; legacy queue unchanged');
