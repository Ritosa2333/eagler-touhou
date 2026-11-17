import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';

// Execute the production EM_JS boundary; no browser/game or network is started.
const source=readFileSync(new URL('../src/netplay/BrowserPeerTransport.cpp',import.meta.url),'utf8');
const start=source.indexOf('EM_JS(int, eagler_peer_connect,'),begin=source.indexOf('{',start);
const end=source.indexOf('\n});',begin);
assert(start>=0&&end>begin);

function fixture({route='rtc',stageHistory=1}={}){
  const sockets=[];
  class Socket {
    static OPEN=1;static CONNECTING=0;static CLOSING=2;
    constructor(url){this.url=url;this.readyState=1;this.closes=[];sockets.push(this);}
    send(){}
    close(code,reason){this.readyState=3;this.closes.push([code,reason]);this.onclose?.({code,reason});}
  }
  const heap=new Uint8Array(256),global={};
  heap.set(new TextEncoder().encode('wss://fixture.invalid/?room=test&run=1'),0);
  heap.set(new TextEncoder().encode('th13mp'),100);
  const timers={set:0,cleared:0,intervals:0,clearedIntervals:0};
  const environment={globalThis:global,HEAPU8:heap,Module:{eaglerOptions:{}},
    location:{href:'https://fixture.invalid/'},WebSocket:Socket,TextDecoder,URL,
    Uint8Array,ArrayBuffer,Blob,
    setTimeout:()=>++timers.set,clearTimeout(){timers.cleared++;},
    setInterval:()=>++timers.intervals,clearInterval(){timers.clearedIntervals++;}};
  const connect=runInNewContext('(relayUrlPtr,tagPtr,localPlayer,playerCount,stageSpectatorHistory)=>'+source.slice(begin,end+2),environment);
  assert.equal(connect(0,100,0,2,stageHistory),1);
  const state=global.__eaglerPeerTransport;
  const pc={connectionState:'connected',iceConnectionState:'connected',signalingState:'stable',
    sctp:{state:'connected'},close(){this.closed=true;},async getStats(){return new Map();}};
  const dc=kind=>({label:'th13mp-'+kind,readyState:'open',bufferedAmount:0,close(){this.readyState='closed';}});
  const peer={pc,primary:{pc},controlOpen:true,inputOpen:true,inputDc:dc('input'),controlDc:dc('control')};
  state.peers.set(1,peer);
  state.setRoute(route);
  return {state,peer,pc,sockets,timers,Socket};
}

test('seat 0 keeps its relay lane open for spectator history until the run is released',()=>{
  const f=fixture();try{
    assert.equal(f.state.route,'rtc');
    assert.equal(f.state.relay?.readyState,f.Socket.OPEN);
    assert.equal(f.state.signal?.readyState,f.Socket.OPEN);
    const signal=f.state.signal,relay=f.state.relay;
    f.state.releaseRelayPresence();
    assert.equal(f.state.relayReleased,true);
    assert.deepEqual(signal.closes,[[1000,'run complete']]);
    assert.deepEqual(relay.closes,[[1000,'run complete']]);
    assert.equal(f.state.signal,null);
    assert.equal(f.state.relay,null);
  }finally{f.state.close();}
});

test('releasing the run keeps the peer data channels and the gameplay state intact',()=>{
  const f=fixture();try{
    f.state.releaseRelayPresence();
    assert.equal(f.state.disconnected,false);
    assert.equal(f.state.failed,false);
    assert.equal(f.state.closed,false);
    assert.equal(f.peer.inputDc.readyState,'open');
    assert.equal(f.peer.controlDc.readyState,'open');
    assert.notEqual(f.pc.closed,true);
    assert.equal(f.state.peers.size,1);
  }finally{f.state.close();}
});

test('a released seat never reopens signaling and drops queued signals',()=>{
  const f=fixture();try{
    const before=f.sockets.length;
    f.state.releaseRelayPresence();
    f.state.sendSignal({type:'spectator-stop'});
    f.state.openSignal();
    f.state.scheduleSignalReconnect();
    f.state.openRelay();
    assert.equal(f.sockets.length,before);
    assert.equal(f.state.pendingSignals.length,0);
    assert.equal(f.state.signalReconnectTimer,null);
    assert.equal(f.state.rtcHealthTimer,null);
    f.state.releaseRelayPresence();
    assert.equal(f.sockets.length,before);
  }finally{f.state.close();}
});

test('relay-routed gameplay keeps its lane: release is a no-op outside the RTC route',()=>{
  const f=fixture({route:'relay'});try{
    const relay=f.state.relay;
    assert.equal(relay.readyState,f.Socket.OPEN);
    f.state.releaseRelayPresence();
    assert.equal(f.state.relayReleased,false);
    assert.equal(relay.readyState,f.Socket.OPEN);
    assert.equal(f.state.relay,relay);
  }finally{f.state.close();}
});

test('close() after release stays idempotent',()=>{
  const f=fixture();
  f.state.releaseRelayPresence();
  f.state.close();
  assert.equal(f.state.closed,true);
  assert.equal(f.state.peers.size,0);
});
