import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
const source=readFileSync(new URL('../src/netplay/BrowserPeerTransport.cpp',import.meta.url),'utf8');
const extract=(name,args)=>{const begin=source.indexOf('EM_JS(',source.indexOf('EM_JS('));const marker=source.indexOf(', '+name+',',begin);assert(marker>=0);const start=source.indexOf('{',marker),end=source.indexOf('\n});',start);assert(end>start);return '('+args+')=>'+source.slice(start,end+2);};
const stopSource=extract('eagler_peer_stop_spectators','graceful');
for(const graceful of [false,true]){
 const sent=[],state={route:'rtc',localPlayer:0,spectatorCount:1,closed:false,relay:{readyState:1,send:bytes=>sent.push(['relay',Array.from(bytes)])},sendSignal:value=>sent.push(['signal',value])};
 const run=runInNewContext(stopSource,{WebSocket:{OPEN:1},globalThis:{__eaglerPeerTransport:state},Uint8Array});
 // Final data is already queued on this same relay stream.
 state.relay.send(new Uint8Array([11,20,30]));run(+graceful);run(+graceful);
 assert.deepEqual(sent[0],['relay',[11,20,30]]);
 assert.equal(sent.filter(([kind])=>kind==='signal').length,graceful?0:1);
 assert.deepEqual(sent.at(-1),['relay',[0xe8,0x53,0x54,0x4f,0x50,1]]);
 assert.equal(state.closed,false);assert.equal(state.spectatorStopped,true);assert.equal(state.spectatorCount,0);
}
const heap=new Uint8Array(16),state={failed:true,received:Array.from({length:257},(_,i)=>new Uint8Array([i&255,i>>>8])),receivedHead:0};
const context={HEAPU8:heap,globalThis:{__eaglerPeerTransport:state}};
const size=runInNewContext(extract('eagler_peer_poll_size',''),context),copy=runInNewContext(extract('eagler_peer_poll_copy','out,capacity'),context);
for(let i=0;i<257;i++){assert.equal(size(),2);assert.equal(copy(0,2),2);assert.equal(heap[0]+heap[1]*256,i);}
assert.equal(size(),0);
console.log('PASS spectator graceful STOP ordered on relay only; default failure signaling unchanged; 257 queued packets drain after transport EOF');

