import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {SharedNetplay} from '../../sdl-runtime/shared-netplay.mjs';

// Run the real shell and embedded sender with controlled sockets/clocks.
// This is a deterministic regression gate, not a network performance test.
const source=readFileSync(new URL('../../../third_party/eagler-common/src/netplay/BrowserPeerTransport.cpp',import.meta.url),'utf8').replaceAll('\r\n','\n');
function embedded(name,args,returnType='int') {
  const at=source.indexOf(`EM_JS(${returnType}, ${name},`);
  assert.ok(at>=0,name);const begin=source.indexOf('{',at)+1,end=source.indexOf('\n});',begin);
  return new Function(...args,'HEAPU8','globalThis','WebSocket',source.slice(begin,end));
}
const send=embedded('eagler_peer_send_spectator',['data','size']);
const status=embedded('eagler_peer_spectator_state',[]);
const stop=embedded('eagler_peer_stop_spectators',[],'void');
const constants={OPEN:1,CLOSING:2};
let now=0;
const originalPerformance=globalThis.performance;
Object.defineProperty(globalThis,'performance',{configurable:true,value:{now:()=>now}});
function fixture({ready=1,queued=0,route='rtc',sendCost=0}={}) {
  const heap=new Uint8Array(1024),sent=[],signals=[],notices=[],closed=[];
  const counts={playerClose:0,worldEnd:0,viewerEnd:0,stop:0};
  const relay={readyState:ready,bufferedAmount:queued,send(bytes){
    sent.push(new Uint8Array(bytes));this.bufferedAmount+=bytes.length;now+=sendCost;
  },close(){throw Error('optional output must not close any player socket');}};
  const state={route,localPlayer:0,closed:false,failed:false,spectatorCount:1,relay,sendSignal:m=>signals.push(m)};
  const g={__eaglerPeerTransport:state};
  const core={HEAPU8:heap,_th09_peer_packet_buffer:()=>0,
    _th09_peer_send_spectator:n=>send(0,n,heap,g,constants),
    _th09_peer_spectator_state:()=>status(heap,g,constants),
    _th09_peer_stop_spectators:()=>{++counts.stop;stop(heap,g,constants);},
    _th09_peer_state:()=>1,_th09_peer_poll:()=>0,
    _th09_peer_close:()=>++counts.playerClose,_th09_network_end:()=>++counts.worldEnd,
    _th09_spectator_end:()=>++counts.viewerEnd};
  const play=new SharedNetplay(core,{onStatus:s=>notices.push(s),onClose:s=>closed.push(s),onResult:()=>{}});
  Object.assign(play,{connected:true,routeReady:true,prepared:true,active:true,side:0,
    options:{netplaySpectatorCount:1},buildBytes:new Uint8Array(12)});
  return {play,core,state,relay,sent,signals,counts,notices,closed};
}
const publish=(f,n)=>f.play.publish(n,0,0,0,0,0,0,0,0);
const queued=f=>f.play.spectatorPending.length-f.play.spectatorHead;
try {
  for(const route of ['rtc','relay']) {
    const f=fixture({queued:1048576,route});
    for(let n=0;n<8193;++n){publish(f,n);f.play.flushSpectators();}
    assert.equal(f.play.connected,true);assert.equal(f.counts.playerClose,0);assert.equal(f.counts.worldEnd,0);
    assert.equal(f.counts.stop,1);assert.equal(queued(f),0);assert.equal(f.play.spectatorOutputStopped,true);
    assert.equal(f.sent.length,1);assert.deepEqual([...f.sent[0]],[0xe8,0x53,0x54,0x4f,0x50,1]);
    assert.equal(f.relay.bufferedAmount,1048582,'only ONE terminal marker may exceed existing backpressure');
    assert.equal(f.signals.length,route==='rtc'?1:0);
    for(let n=8193;n<10000;++n){publish(f,n);f.play.flushSpectators();}
    assert.equal(f.sent.length,1);assert.equal(f.counts.stop,1);
  }
  {
    const f=fixture({ready:3});publish(f,0);f.play.flushSpectators();
    assert.equal(f.counts.stop,1);assert.equal(f.signals.length,1);
    assert.equal(f.play.connected,true);assert.equal(f.counts.worldEnd,0);
    assert.equal(f.sent.length,0);assert.equal(queued(f),0);
  }
  {
    const f=fixture();for(let n=0;n<4096;++n)publish(f,n);
    assert.equal(f.sent.length,0,'publishing cannot repeatedly spend a pump budget');
    f.play.flushSpectators();assert.equal(f.sent.length,32);assert.equal(queued(f),4064);
    while(queued(f)){f.relay.bufferedAmount=0;f.play.flushSpectators();}
    assert.equal(f.play.spectatorSentFrame,4096);assert.equal(f.play.spectatorHead,0);
    f.sent.forEach((bytes,n)=>assert.equal(new DataView(bytes.buffer).getUint32(9,true),n));
  }
  {
    const f=fixture({sendCost:1.1});for(let n=0;n<50;++n)publish(f,n);
    f.play.flushSpectators();assert.equal(f.sent.length,2);assert.equal(queued(f),48);
  }
  {
    const f=fixture();for(let n=0;n<2000;++n)publish(f,n);
    for(let n=0;n<100;++n)f.play.flushSpectators();
    assert.ok(f.relay.bufferedAmount<=65536);assert.ok(queued(f)>0);
    assert.equal(f.play.connected,true);assert.equal(f.counts.stop,0);
    f.relay.bufferedAmount=0;f.play.flushSpectators();assert.ok(f.play.spectatorSentFrame>1394);
  }
  {
    const f=fixture();const metadata=new Uint8Array(40);metadata.set([84,57,84,77,1]);
    f.play.spectatorPending.push(metadata);publish(f,0);f.play.flushSpectators();
    assert.equal(f.sent[0].length,41);assert.equal(f.sent[1].length,47);
    assert.equal(f.play.spectatorSentFrame,1);
  }
  {
    const f=fixture();Object.assign(f.play,{spectator:true,spectatorReceiveAt:now});
    now+=15001;f.play.pump();assert.equal(f.closed.length,1);assert.equal(f.counts.viewerEnd,1);
    assert.equal(f.counts.worldEnd,0);assert.match(f.closed[0],/观战确认帧流中断/);
  }
  console.log('Spectator output: healthy players survive failures, real backpressure, 32-packet/2ms pump, ordered drain, timing-before-input and viewer timeout PASS');
} finally {
  Object.defineProperty(globalThis,'performance',{configurable:true,value:originalPerformance});
}
