import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';

// Execute the production EM_JS boundary; no browser/game or network is started.
const source=readFileSync(new URL('../src/netplay/BrowserPeerTransport.cpp',import.meta.url),'utf8');
const start=source.indexOf('EM_JS(int, eagler_peer_connect,'),begin=source.indexOf('{',start);
const end=source.indexOf('\n});',begin);
assert(start>=0&&end>begin);

function fixture(){
  class Socket {
    static OPEN=1;static CONNECTING=0;static CLOSING=2;
    readyState=1;send(){}close(){this.readyState=3;}
  }
  const heap=new Uint8Array(256),global={};
  heap.set(new TextEncoder().encode('wss://fixture.invalid/?room=test&run=1'),0);
  heap.set(new TextEncoder().encode('th12'),100);
  const environment={globalThis:global,HEAPU8:heap,Module:{eaglerOptions:{}},
    location:{href:'https://fixture.invalid/'},WebSocket:Socket,TextDecoder,URL,
    Uint8Array,ArrayBuffer,Blob,setTimeout:()=>1,clearTimeout(){},setInterval:()=>1,clearInterval(){}};
  const connect=runInNewContext('(relayUrlPtr,tagPtr,localPlayer,playerCount,stageSpectatorHistory)=>'+source.slice(begin,end+2),environment);
  assert.equal(connect(0,100,0,2,0),1);
  const state=global.__eaglerPeerTransport;
  const pc={connectionState:'connected',iceConnectionState:'connected',signalingState:'stable',
    sctp:{state:'connected'},close(){},async getStats(){return new Map();}};
  const peer={pc,primary:{pc},controlOpen:true,inputOpen:true};state.peers.set(1,peer);
  let received=0;state.receivePeerData=()=>received++;
  state.setRoute('rtc');
  const channel=kind=>({label:'th12-'+kind,readyState:'open',bufferedAmount:17,close(){this.readyState='closed';}});
  return {state,peer,pc,channel,received:()=>received};
}

for(const kind of ['control','input'])for(const event of ['error','close','open','message']){
  test(`replaced ${kind} channel's delayed ${event} cannot mutate its replacement`,()=>{
    const f=fixture();try{
      const old=f.channel(kind),current=f.channel(kind);
      f.state.setupChannel(1,old,kind);f.state.setupChannel(1,current,kind);
      if(event==='open'){
        current.readyState='connecting';f.peer[kind+'Open']=false;
        old.onopen();assert.equal(f.peer[kind+'Open'],false);
      }else if(event==='message'){
        old.onmessage({data:new Uint8Array([1,2,3]).buffer});assert.equal(f.received(),0);
      }else{
        if(event==='close')old.readyState='closed';
        old['on'+event]({error:{name:'RTCError',message:'old channel failed',errorDetail:'sctp-failure',sctpCauseCode:12}});
        assert.equal(f.peer[kind+'Open'],true);
      }
      assert.equal(f.state.disconnected,false);
      assert.equal(f.peer[kind+'Dc'],current);assert.equal(f.peer.primary[kind+'Dc'],current);
    }finally{f.state.close();}
  });
}

test('adaptive selection still owns the acceptance of late primary packets',()=>{
  const f=fixture();try{
    const primary=f.channel('control'),packets=[];
    f.state.setupChannel(1,primary,'control');
    f.peer.adaptive={receivePrimary:(...args)=>packets.push(args),close(){}};
    f.peer.pc={close(){}};f.peer.controlDc=f.channel('control');
    const data=new Uint8Array([4,5]).buffer;primary.onmessage({data});
    assert.equal(packets.length,1);assert.equal(packets[0][0],'control');assert.equal(packets[0][1],data);
    assert.equal(f.received(),0);assert.equal(f.state.disconnected,false);
  }finally{f.state.close();}
});

for(const kind of ['control','input'])test(`current ${kind} close still ends the connection`,()=>{
  const f=fixture();try{
    const dc=f.channel(kind);f.state.setupChannel(1,dc,kind);
    dc.readyState='closed';dc.onclose();
    assert.equal(f.peer[kind+'Open'],false);
    assert.equal(f.state.disconnected,true);assert.match(f.state.error,new RegExp(kind+' channel closed'));
    const diagnostic=f.state.channelEvents?.at(-1);assert.ok(diagnostic);
    assert.equal(diagnostic.peer,1);assert.equal(diagnostic.kind,kind);assert.equal(diagnostic.event,'close');
    assert.equal(diagnostic.channel.readyState,'closed');
  }finally{f.state.close();}
});

for(const kind of ['control','input'])test(`current ${kind} failure preserves RTC diagnostics and ends the connection`,()=>{
  const f=fixture();try{
    const dc=f.channel(kind);f.state.setupChannel(1,dc,kind);
    dc.onerror({error:{name:'RTCError',message:'SCTP transport failed',errorDetail:'sctp-failure',
      sctpCauseCode:12,receivedAlert:40,sentAlert:80}});
    assert.equal(f.state.disconnected,true);assert.match(f.state.error,new RegExp(kind+' channel error'));
    const diagnostic=f.state.channelEvents?.at(-1);assert.ok(diagnostic);
    assert.equal(diagnostic.peer,1);assert.equal(diagnostic.kind,kind);assert.equal(diagnostic.event,'error');
    assert.equal(diagnostic.error.name,'RTCError');assert.equal(diagnostic.error.message,'SCTP transport failed');
    assert.equal(diagnostic.error.errorDetail,'sctp-failure');assert.equal(diagnostic.error.sctpCauseCode,12);
    assert.equal(diagnostic.error.receivedAlert,40);assert.equal(diagnostic.error.sentAlert,80);
    assert.equal(diagnostic.channel.label,'th12-'+kind);assert.equal(diagnostic.channel.readyState,'open');
    assert.equal(diagnostic.channel.bufferedAmount,17);
    assert.equal(diagnostic.connection.connectionState,'connected');
    assert.equal(diagnostic.connection.iceConnectionState,'connected');
    assert.equal(diagnostic.connection.signalingState,'stable');assert.equal(diagnostic.connection.sctpState,'connected');
  }finally{f.state.close();}
});

test('channel diagnostics retain at most the last eight events',()=>{
  const f=fixture();try{
    const dc=f.channel('control');f.state.setupChannel(1,dc,'control');
    // Keep the transport alive to exercise collection independently of terminal
    // disconnect policy; the production callback and recorder both still run.
    f.state.fail=()=>{};
    for(let i=0;i<12;++i)dc.onerror({error:{name:'RTCError',message:'failure-'+i,errorDetail:'sctp-failure',sctpCauseCode:i}});
    assert.equal(f.state.channelEvents?.length,8);
    assert.equal(f.state.channelEvents[0].error.message,'failure-4');
    assert.equal(f.state.channelEvents.at(-1).error.message,'failure-11');
  }finally{f.state.close();}
});
