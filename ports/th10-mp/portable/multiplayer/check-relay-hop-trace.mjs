// ROOT runs: node --test portable/multiplayer/check-relay-hop-trace.mjs
// Exercises the actual observer, trace and Input impairment in a controlled
// realm. No browser, relay, network, GPU or native gameplay is run here.
// Recording socket/timer/queue boundaries prove diagnostic semantics only.
import test,{after} from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {createContext,runInContext} from 'node:vm';
import {createTransportTrace,transportTraceRowSchema,transportTraceRelayHopRowSchema}
 from './transport-trace.mjs';

const files=['browser-evidence.cjs','transport-trace.mjs','websocket-input-impairment.cjs',
 'check-relay-hop-trace.mjs'];
const inventory=()=>Object.fromEntries(files.map(path=>[path,
 createHash('sha256').update(readFileSync(new URL(path,import.meta.url))).digest('hex')]));
const before=inventory();
after(()=>assert.deepEqual(inventory(),before,'Relay-hop diagnostic sources changed during validation'));
const browserSource=readFileSync(new URL('browser-evidence.cjs',import.meta.url),'utf8');
const impairmentSource=readFileSync(new URL('websocket-input-impairment.cjs',import.meta.url),'utf8');
const realSendBefore=globalThis.WebSocket?.prototype.send;
const mainSchema=[
 'atMs','samplePerformanceMs','sampleDateEpochMs','datePerformanceSkewMs',
 'net.canStart','net.nextFrame','net.lastSimulatedFrame','net.confirmedThroughAllRemotes',
 'smooth.correcting','smooth.predictionFrontier','smooth.missingInputs',
 'smooth.intervalScaleMilli','smooth.localCaptureAtNext',
 'native.schema','native.networkEnabled','native.open','native.error','native.sent',
 'native.received','native.ignored','native.repairs','native.latestCapture',
 'native.acknowledgedLocalThroughAllRemotes','native.confirmedThroughAllRemotes',
 'native.retiring','native.generation','native.mode','native.bufferedBytes',
 'stateAvailable','receivedQueuePackets','route','recovering','disconnected',
 'peerCount','omittedPeerRows','peers'];
const hopSchema=[
 'atMs','samplePerformanceMs','relayAvailable','socketId','player','route','readyState',
 'browser.binaryMessagesReceived','browser.lastBinaryReceivePerformanceMs',
 'shim.matched','shim.scheduled','shim.sent','shim.pendingJobs','shim.queuedBytes',
 'shim.maxTimerOverrunMs','shim.maxDeliveredDelayMs','shim.canceled','shim.dropped',
 'shim.overflow','shim.sendErrors','shim.scheduleErrors'];
const objectRow=(schema,row)=>Object.fromEntries(schema.map((name,i)=>[name,row[i]]));
const plain=value=>JSON.parse(JSON.stringify(value));

function emptyShim(){
 return {stats:{schema:'th10-websocket-input-impairment/1',transportMode:'relay',
  matched:0,scheduled:0,sent:0,pendingJobs:0,queuedBytes:0,maxTimerOverrunMs:0,
  maxDeliveredDelayMs:0,canceled:0,dropped:0,overflow:0,sendErrors:0,scheduleErrors:0}};
}
function browserHarness(options={},install=true){
 let at=100,state=null,timerId=0;
 const reads={performance:0,date:0,transport:0,evidence:0,shim:0};
 const faults={clock:false,transport:false,hop:false,listener:false};
 const sent=[],timers=new Map(),returnValue={originalSendReturn:true};
 class Socket{
  static OPEN=1;
  constructor(url){
   this.url=String(url);this.readyState=1;this.bufferedAmount=0;
   this.listeners=new Map();this.onmessage=null;this.failSend=false;
  }
  addEventListener(type,fn){
   if(faults.listener&&type==='message')throw Error('intentional listener failure');
   if(!this.listeners.has(type))this.listeners.set(type,[]);
   this.listeners.get(type).push(fn);
  }
  emit(data){
   const event={type:'message',data};
   for(const fn of this.listeners.get('message')??[])fn(event);
   if(typeof this.onmessage==='function')this.onmessage(event);
  }
  send(data){
   if(this.failSend)throw Error('intentional original send failure');
   sent.push({socket:this,data,at});return returnValue;
  }
 }
 class PeerConnection{
  createOffer(){return Promise.resolve();}
  createAnswer(){return Promise.resolve();}
  setLocalDescription(){return Promise.resolve();}
  setRemoteDescription(){return Promise.resolve();}
 }
 const window={screenX:0,screenY:0,addEventListener(){}};window.top=window;
 const perf={timeOrigin:1700000000000,now(){
  ++reads.performance;if(faults.clock)throw Error('intentional clock failure');return at;
 }};
 class ClockDate extends Date{
  static now(){++reads.date;return perf.timeOrigin+at;}
 }
 const realm=createContext({WebSocket:Socket,RTCPeerConnection:PeerConnection,window,
  performance:perf,Date:ClockDate,URL,Map,ArrayBuffer,Uint8Array,DataView,
  document:{visibilityState:'visible',hidden:false,hasFocus:()=>true,addEventListener(){}},
  screen:{width:1920,height:1080,availWidth:1920,availHeight:1080},
  innerWidth:960,innerHeight:720,outerWidth:960,outerHeight:720,devicePixelRatio:1,
  setTimeout(){throw Error('observer or trace must not install a timer');},
  setInterval(){throw Error('observer or trace must not install an interval');}});
 Object.defineProperty(realm,'__eaglerPeerTransport',{configurable:true,get(){
  ++reads.transport;if(faults.transport)throw Error('intentional transport getter failure');return state;
 }});
 runInContext(browserSource,realm);
 const h={realm,Socket,reads,faults,sent,timers,returnValue,shim:emptyShim(),evidence:null,
  net:[1,1,42,41,30,0,0,0,0,0,0],
  native:[1,1,1,0,777,11,0,0,42,40,30,0,9,2,0],
  smooth:[1,0,0,0,41,12,1000,0,0,1],
  get at(){return at;},set at(value){at=value;},
  get state(){return state;},set state(value){state=value;},
  socket(query='player=0&players=2'){return new realm.WebSocket('ws://diagnostic.invalid/?'+query);},
  select(socket){
   state={tag:'th10',route:'relay',relay:socket,signal:null,localPlayer:0,playerCount:2,
    peers:new Map(),received:[],receivedHead:0,closed:false,failed:false,disconnected:false,
    isRecovering:()=>false};
   return state;
  },
  installShim(){
   runInContext(impairmentSource,realm);
   h.shim=realm.installWebSocketInputImpairment({oneWayMs:50,jitterMs:0},{
    WebSocket:realm.WebSocket,transport:()=>state,now:()=>at,
    later(fn,ms){const id=++timerId;timers.set(id,{fn,due:at+ms});return id;},
    cancel(id){timers.delete(id);}});
   return h.shim;
  },
  fireNext(time){
   const next=[...timers].sort((a,b)=>a[1].due-b[1].due||a[0]-b[0])[0];
   assert.ok(next,'one existing impairment timer');assert.ok(time>=next[1].due);
   at=time;timers.delete(next[0]);next[1].fn();
  }};
 if(install)h.evidence=realm.installTh10BrowserEvidence(options);
 h.api={netStatus:()=>h.net,transportStatus:()=>h.native};
 return h;
}
// Tests using the imported trace are synchronous and restore every global in
// finally. The browser observer continues to run in its own controlled realm.
function withTraceGlobals(h,fn){
 const descriptors={
  performance:{value:h.realm.performance,writable:true},
  Date:{value:h.realm.Date,writable:true},
  __eaglerPeerTransport:{get:()=>h.realm.__eaglerPeerTransport},
  __th10BrowserEvidence:{get(){
   ++h.reads.evidence;if(h.faults.hop)throw Error('forbidden hop observer read');return h.evidence;
  }},
  perfImpairment:{get(){
   ++h.reads.shim;if(h.faults.hop)throw Error('forbidden hop shim read');return h.shim;
  }},
  setTimeout:{value:()=>{throw Error('trace must not install a timer');},writable:true},
  setInterval:{value:()=>{throw Error('trace must not install an interval');},writable:true}
 };
 const originals=new Map(Object.keys(descriptors).map(key=>[key,Object.getOwnPropertyDescriptor(globalThis,key)]));
 try{
  for(const [key,value]of Object.entries(descriptors))
   Object.defineProperty(globalThis,key,{configurable:true,...value});
  return fn();
 }finally{
  for(const [key,value]of originals)
   if(value)Object.defineProperty(globalThis,key,value);else delete globalThis[key];
 }
}
function activeHarness(){
 const h=browserHarness({relayHopTrace:true});h.select(h.socket());return h;
}
function trace(h,options={}){
 return createTransportTrace(h.api,{enabled:true,relayHopTrace:true,
  startedAtPerformanceMs:100,...options});
}
function assertPaired(report){
 assert.equal(report.rows.length,report.relayHop.rows.length);
 for(let i=0;i<report.rows.length;++i){
  assert.equal(report.rows[i].length,36);assert.equal(report.relayHop.rows[i].length,21);
  assert.equal(report.rows[i][0],report.relayHop.rows[i][0]);
  assert.equal(report.rows[i][1],report.relayHop.rows[i][1]);
 }
}
// Independent production-shaped Input: only the existing real impairment
// classifies these bytes. The observer/trace never decode this test packet.
function inputPacket(frame){
 const bytes=new Uint8Array(56);bytes.set([0x45,0x41,0x4e,0x50,4,1,0,2]);
 const view=new DataView(bytes.buffer);
 view.setUint32(16,frame,true);view.setUint32(20,0xffffffff,true);
 view.setUint32(24,frame,true);view.setUint32(28,0xffffffff,true);
 view.setUint32(32,frame,true);view.setUint32(36,frame,true);bytes[42]=1;return bytes;
}

test('schemas and imports preserve the original 36-column transport contract',t=>{
 assert.equal(globalThis.WebSocket?.prototype.send,realSendBefore);
 assert.deepEqual(transportTraceRowSchema,mainSchema);
 assert.deepEqual(transportTraceRelayHopRowSchema,hopSchema);
 assert.ok(Object.isFrozen(transportTraceRowSchema));
 assert.ok(Object.isFrozen(transportTraceRelayHopRowSchema));
 t.diagnostic(JSON.stringify({kind:'relay-hop-source-inventory',sources:before}));
});

test('default-OFF browser hop does no binary clock, transport, payload or send counting',()=>{
 const h=browserHarness(),socket=h.socket();h.select(socket);
 let payloadReads=0,gameMessages=0;
 const binary=new Proxy({},{get(){++payloadReads;throw Error('payload must stay opaque');}});
 socket.onmessage=()=>++gameMessages;
 const beforeReads={...h.reads},beforeErrors=h.evidence.read().errors.length;
 h.faults.clock=h.faults.transport=true;
 assert.equal(h.evidence.readRelayHop(),null);
 assert.doesNotThrow(()=>socket.emit(binary));
 assert.equal(socket.send(binary),h.returnValue);
 assert.equal(h.evidence.relayHopTraceEnabled,false);
 assert.deepEqual(h.reads,beforeReads);assert.equal(payloadReads,0);
 assert.equal(gameMessages,1);assert.equal(h.sent.length,1);
 assert.equal(h.evidence.read().errors.length,beforeErrors);
 assert.equal(socket.listeners.get('message').length,1);
 assert.equal(h.timers.size,0);
});

test('disabled trace touches no API/global; enabled trace with hop OFF never reads hop globals',()=>{
 const h=activeHarness();h.faults.clock=h.faults.transport=h.faults.hop=true;
 const poison=new Proxy({},{get(){throw Error('disabled trace API touched');}});
 withTraceGlobals(h,()=>{
  const beforeReads={...h.reads};
  const disabled=createTransportTrace(poison,{enabled:false,
   get relayHopTrace(){throw Error('disabled trace option touched');}});
  assert.equal(disabled.sample(0),false);assert.equal(disabled.finish(),null);
  assert.deepEqual(h.reads,beforeReads);
  h.faults.clock=h.faults.transport=false;
  const off=trace(h,{relayHopTrace:false});
  assert.equal(off.sample(0,h.net,h.smooth),true);
  const report=off.finish();assert.equal(report.relayHop,null);assert.equal(report.complete,true);
  assert.equal(h.reads.evidence,0);assert.equal(h.reads.shim,0);
 });
});

test('one existing message listener counts only current gameplay socket binary events',()=>{
 const h=browserHarness({relayHopTrace:true});
 const absent=h.evidence.readRelayHop();
 assert.equal(absent.valid,true);assert.equal(absent.available,false);
 assert.equal(absent.binaryMessagesReceived,null);assert.equal(absent.socketId,null);
 const current=h.socket(),other=h.socket('player=1&players=2');
 const signal=h.socket('player=0&players=2&signal=1'),lobby=h.socket('player=0&players=2&lobby=1');
 h.select(current);let payloadReads=0,gameMessages=0;
 const binary=new Proxy({},{get(){++payloadReads;throw Error('payload read');}});
 current.onmessage=()=>++gameMessages;
 h.at=101;current.emit(new ArrayBuffer(3));h.at=102;current.emit(new Uint8Array(4));
 h.at=103;current.emit(binary);
 current.emit('{"type":"peers"}');current.emit(null);current.emit(undefined);current.emit(3);
 other.emit(binary);signal.emit(binary);lobby.emit(binary);
 const first=h.evidence.readRelayHop();
 assert.equal(first.socketId,1);assert.equal(first.player,0);
 assert.equal(first.binaryMessagesReceived,3);assert.equal(first.lastBinaryReceivePerformanceMs,103);
 assert.equal(payloadReads,0);assert.equal(gameMessages,7);
 assert.equal(current.listeners.get('message').length,1);assert.equal(h.timers.size,0);
 assert.equal(h.evidence.read().connections.filter(v=>v.event==='message').length,1);
 assert.ok(h.evidence.read().connections.every(v=>!('binaryMessagesReceived'in v)));
 // Replacing the socket exposes a new zero, never a sum across sockets.
 h.state.relay=other;const second=h.evidence.readRelayHop();
 assert.equal(second.socketId,2);assert.equal(second.player,1);
 assert.equal(second.binaryMessagesReceived,0);assert.equal(second.lastBinaryReceivePerformanceMs,null);
 h.at=110;current.emit(binary);other.emit(binary);
 assert.equal(h.evidence.readRelayHop().binaryMessagesReceived,1);
 h.state.relay=current;
 assert.equal(h.evidence.readRelayHop().binaryMessagesReceived,3);
 // Browser arrival is visible even if the product will ignore a closed owner.
 h.state.closed=true;h.at=111;current.emit(binary);
 assert.equal(h.evidence.readRelayHop().binaryMessagesReceived,4);
 assert.equal(h.evidence.readRelayHop().lastBinaryReceivePerformanceMs,111);
});

test('installation options are explicit booleans and cannot silently change an installed observer',()=>{
 for(const bad of [null,0,1,'true',{},[]]){
  const h=browserHarness({},false);
  assert.throws(()=>h.realm.installTh10BrowserEvidence({relayHopTrace:bad}),/boolean/);
  assert.equal(h.realm.WebSocket,h.Socket);
 }
 const off=browserHarness();assert.equal(off.realm.installTh10BrowserEvidence(),off.evidence);
 assert.throws(()=>off.realm.installTh10BrowserEvidence({relayHopTrace:true}),/fixed/);
 const on=activeHarness();
 assert.equal(on.realm.installTh10BrowserEvidence({relayHopTrace:true}),on.evidence);
 assert.throws(()=>on.realm.installTh10BrowserEvidence({relayHopTrace:false}),/fixed/);
 withTraceGlobals(on,()=>{
  for(const bad of [null,0,1,'true'])
   assert.throws(()=>trace(on,{relayHopTrace:bad}),/boolean/);
 });
});

test('unobserved/over-limit sockets and failed listener/clock reads stay explicitly invalid',()=>{
 const unobserved=browserHarness({relayHopTrace:true});
 unobserved.select(new unobserved.Socket('ws://diagnostic.invalid/?player=0&players=2'));
 let result=unobserved.evidence.readRelayHop();
 assert.equal(result.valid,false);assert.equal(result.binaryMessagesReceived,null);
 const bounded=browserHarness({relayHopTrace:true});
 for(let i=0;i<16;++i)bounded.socket();
 bounded.select(bounded.socket());result=bounded.evidence.readRelayHop();
 assert.equal(result.valid,false);assert.equal(bounded.evidence.read().droppedSockets,1);
 assert.equal(bounded.evidence.read().connections.length,16);
 const listener=browserHarness({relayHopTrace:true});listener.faults.listener=true;
 listener.select(listener.socket());assert.equal(listener.evidence.readRelayHop().valid,false);
 const clock=activeHarness();let gameMessages=0;
 clock.state.relay.onmessage=()=>++gameMessages;
 clock.faults.clock=true;assert.doesNotThrow(()=>clock.state.relay.emit(new ArrayBuffer(1)));
 clock.faults.clock=false;clock.at=120;
 for(let i=0;i<20;++i)clock.state.relay.emit(new ArrayBuffer(1));
 assert.equal(gameMessages,21);assert.equal(clock.evidence.readRelayHop().valid,false);
 assert.equal(clock.evidence.read().errors.filter(v=>v.where==='relay-hop-message').length,1);
 const backwards=activeHarness();backwards.state.relay.emit(new ArrayBuffer(1));backwards.at=99;
 backwards.state.relay.emit(new ArrayBuffer(1));
 assert.equal(backwards.evidence.readRelayHop().valid,false);
 const getter=activeHarness(),savedRelay=getter.state.relay;
 let injectedGetterHits=0;
 // A normal transport property avoids Node VM's contextified-global getter
 // exception handling while exercising the real observer's read catch.
 Object.defineProperty(getter.state,'relay',{configurable:true,get(){
  ++injectedGetterHits;throw Error('intentional transport relay getter failure');
 }});
 assert.equal(getter.evidence.readRelayHop().valid,false);
 assert.equal(injectedGetterHits,1);
 Object.defineProperty(getter.state,'relay',{configurable:true,value:savedRelay});
 assert.equal(getter.evidence.readRelayHop().valid,false);
});

test('real impairment acceptance, actual underlying send, browser receive and native poll stay separate',()=>{
 const h=browserHarness({relayHopTrace:true});h.installShim();
 const socket=h.socket();h.select(socket);socket.onmessage=event=>h.state.received.push(event.data);
 withTraceGlobals(h,()=>{
  const sample=trace(h);
  assert.equal(socket.send(inputPacket(7)),undefined);
  assert.equal(h.shim.stats.matched,1);assert.equal(h.shim.stats.sent,0);
  assert.equal(h.sent.length,0);assert.equal(h.timers.size,1);
  assert.equal(sample.sample(0,h.net,h.smooth),true);
  h.fireNext(173);assert.equal(h.sent.length,1);assert.equal(h.shim.stats.sent,1);
  assert.equal(sample.sample(73,h.net,h.smooth),true);
  h.at=181;socket.emit(new ArrayBuffer(6));h.at=210;
  assert.equal(sample.sample(110,h.net,h.smooth),true);
  h.state.receivedHead=1;++h.native[5];h.at=240;
  assert.equal(sample.sample(140,h.net,h.smooth),true);
  h.at=250;socket.send(inputPacket(8));socket.failSend=true;h.fireNext(310);
  assert.equal(sample.sample(210,h.net,h.smooth),true);
  const report=sample.finish(),rows=report.relayHop.rows.map(row=>objectRow(hopSchema,row));
  assert.equal(report.complete,true);assert.equal(report.relayHop.complete,true);assertPaired(report);
  assert.equal(rows[0]['shim.matched'],1);assert.equal(rows[0]['shim.sent'],0);
  assert.equal(rows[0]['shim.pendingJobs'],1);assert.equal(rows[0]['shim.queuedBytes'],56);
  assert.equal(rows[1]['shim.sent'],1);assert.equal(rows[1]['shim.pendingJobs'],0);
  assert.equal(rows[1]['shim.maxTimerOverrunMs'],23);assert.equal(rows[1]['shim.maxDeliveredDelayMs'],73);
  assert.equal(rows[1]['browser.binaryMessagesReceived'],0);
  assert.equal(rows[2]['browser.binaryMessagesReceived'],1);
  assert.equal(rows[2]['browser.lastBinaryReceivePerformanceMs'],181);
  assert.equal(objectRow(mainSchema,report.rows[2]).receivedQueuePackets,1);
  assert.equal(objectRow(mainSchema,report.rows[2])['native.received'],11);
  assert.equal(objectRow(mainSchema,report.rows[3]).receivedQueuePackets,0);
  assert.equal(objectRow(mainSchema,report.rows[3])['native.received'],12);
  assert.equal(rows[4]['shim.matched'],2);assert.equal(rows[4]['shim.sent'],1);
  assert.equal(rows[4]['shim.sendErrors'],1);assert.equal(rows[4]['shim.pendingJobs'],0);
  assert.equal(objectRow(mainSchema,report.rows[4])['native.sent'],777);
  assert.equal(h.timers.size,0);assert.equal(h.sent.length,1);
 });
 h.shim.uninstall();
});

test('hop ON keeps parent rows unchanged, does not inspect queued payloads, and shares each read clock',()=>{
 const h=activeHarness();
 Object.defineProperty(h.state.received,0,{configurable:true,get(){throw Error('receive payload read');}});
 Object.defineProperty(h.shim,'queue',{get(){throw Error('impairment queue scanned');}});
 withTraceGlobals(h,()=>{
  const off=trace(h,{relayHopTrace:false}),on=trace(h);
  assert.equal(off.sample(0,h.net,h.smooth),true);assert.equal(on.sample(0,h.net,h.smooth),true);
  const a=off.finish(),b=on.finish();
  assert.equal(a.schema,'th10-transport-trace/1');assert.equal(b.schema,a.schema);
  assert.deepEqual(b.rowSchema,a.rowSchema);assert.deepEqual(plain(b.rows),plain(a.rows));
  assert.equal(objectRow(mainSchema,b.rows[0]).receivedQueuePackets,1);
  assert.equal(b.relayHop.schema,'th10-relay-hop/1');assert.equal(b.relayHop.enabled,true);
  assert.equal(b.relayHop.complete,true);assertPaired(b);assert.equal(h.timers.size,0);
 });
});

test('known absent relay uses nulls; socket replacement is visible in paired trace rows',()=>{
 const h=activeHarness(),old=h.state.relay;h.at=100;old.emit(new ArrayBuffer(1));
 withTraceGlobals(h,()=>{
  const sample=trace(h);assert.equal(sample.sample(0),true);
  const next=h.socket('player=1&players=2');h.state.relay=next;h.at=120;
  assert.equal(sample.sample(20),true);
  h.state.relay=null;h.at=140;assert.equal(sample.sample(40),true);
  const report=sample.finish(),rows=report.relayHop.rows.map(row=>objectRow(hopSchema,row));
  assert.equal(rows[0].socketId,1);assert.equal(rows[0]['browser.binaryMessagesReceived'],1);
  assert.equal(rows[1].socketId,2);assert.equal(rows[1]['browser.binaryMessagesReceived'],0);
  assert.equal(rows[2].relayAvailable,false);assert.equal(rows[2].socketId,null);
  assert.equal(rows[2]['browser.binaryMessagesReceived'],null);
  assert.equal(rows[2]['browser.lastBinaryReceivePerformanceMs'],null);
  assert.equal(report.complete,true);assertPaired(report);
 });
});

test('the shared interval, attempt cap and finish stop all further diagnostic reads',()=>{
 const h=activeHarness();
 withTraceGlobals(h,()=>{
  const sample=trace(h,{intervalMs:100,maxSamples:2});
  assert.equal(sample.sample(0),true);h.at=120;assert.equal(sample.sample(20),false);
  h.at=200;assert.equal(sample.sample(100),true);
  const beforeReads={...h.reads};h.faults.clock=h.faults.transport=h.faults.hop=true;
  assert.equal(sample.sample(200),false);assert.equal(sample.sample(300),false);
  assert.deepEqual(h.reads,beforeReads);
  const report=sample.finish();
  assert.equal(report.attempts,2);assert.equal(report.dropped,2);assert.equal(report.throttled,1);
  assert.equal(report.valid,true);assert.equal(report.complete,false);
  assert.equal(report.relayHop.valid,true);assert.equal(report.relayHop.complete,false);
  assert.equal(report.rows.length,2);assertPaired(report);
  const calls=report.sampleCalls;assert.equal(sample.sample(400),false);
  assert.equal(report.sampleCalls,calls);assert.deepEqual(h.reads,beforeReads);
  assert.equal(sample.finish(),report);
 });
});

test('the original 2048 default hard limit also bounds relay-hop rows',()=>{
 const h=activeHarness();
 withTraceGlobals(h,()=>{
  assert.throws(()=>trace(h,{maxSamples:2049}),/1..2048/);
  const sample=trace(h);
  for(let i=0;i<2048;++i){h.at=100+20*i;assert.equal(sample.sample(20*i),true);}
  const beforeReads={...h.reads};h.faults.clock=h.faults.transport=h.faults.hop=true;
  assert.equal(sample.sample(40960),false);assert.deepEqual(h.reads,beforeReads);
  const report=sample.finish();assert.equal(report.sampling.hardLimit,2048);
  assert.equal(report.rows.length,2048);assert.equal(report.dropped,1);
  assert.equal(report.valid,true);assert.equal(report.relayHop.complete,false);assertPaired(report);
 });
});

test('missing, disabled, throwing or malformed hop reads invalidate both reports without partial rows',()=>{
 const cases=[
  ['relayHopBrowser',h=>{h.evidence=null;}],
  ['relayHopBrowser',h=>{h.evidence={readRelayHop:()=>null};}],
  ['relayHopBrowser',h=>{h.evidence={readRelayHop(){throw Error('observer read failure');}};}],
  ['relayHopBrowser',h=>{const read=h.evidence.readRelayHop;h.evidence={readRelayHop:()=>({...read(),valid:false})};}],
  ['relayHopBrowser',h=>{const read=h.evidence.readRelayHop;h.evidence={readRelayHop:()=>({...read(),binaryMessagesReceived:null})};}],
  ['relayHopBrowser',h=>{const read=h.evidence.readRelayHop;h.evidence={readRelayHop:()=>({...read(),binaryMessagesReceived:1,lastBinaryReceivePerformanceMs:101})};}],
  ['relayHopBrowser',h=>{const read=h.evidence.readRelayHop;h.evidence={readRelayHop:()=>({...read(),route:'rtc'})};}],
  ['relayHopShim',h=>{h.shim=null;}],
  ['relayHopShim',h=>{h.shim.stats.transportMode='rtc';}],
  ['relayHopShim',h=>{h.shim.stats.sent=undefined;}],
  ['relayHopShim',h=>{h.shim.stats.pendingJobs=-1;}],
  ['relayHopShim',h=>{h.shim.stats.maxTimerOverrunMs=NaN;}],
  ['relayHopShim',h=>{h.shim.stats.queuedBytes=0.5;}],
  ['relayHopShim',h=>{Object.defineProperty(h.shim.stats,'sent',{get(){throw Error('shim getter failure');}});}]
 ];
 for(const [stage,change]of cases){
  const h=activeHarness();change(h);
  withTraceGlobals(h,()=>{
   const sample=trace(h);assert.equal(sample.sample(0),false);
   const report=sample.finish();
   assert.equal(report.valid,false);assert.equal(report.complete,false);
   assert.equal(report.relayHop.valid,false);assert.equal(report.relayHop.complete,false);
   assert.equal(report.readErrors,1);assert.deepEqual(report.firstReadError,{atMs:0,stage});
   assert.equal(report.rows.length,0);assertPaired(report);
  });
 }
});

test('first read failure and earlier good rows survive later failures; invalid time also invalidates the child',()=>{
 const h=activeHarness();
 withTraceGlobals(h,()=>{
  const sample=trace(h);assert.equal(sample.sample(0),true);
  const good=h.shim;h.shim=null;h.at=120;assert.equal(sample.sample(20),false);
  h.shim=good;h.evidence=null;h.at=140;assert.equal(sample.sample(40),false);
  const report=sample.finish();assert.equal(report.rows.length,1);assertPaired(report);
  assert.equal(report.readErrors,2);assert.deepEqual(report.firstReadError,{atMs:20,stage:'relayHopShim'});
  assert.equal(report.valid,false);assert.equal(report.relayHop.valid,false);
 });
 const time=activeHarness();
 withTraceGlobals(time,()=>{
  const sample=trace(time);assert.equal(sample.sample(20),true);
  assert.equal(sample.sample(19),false);const report=sample.finish();
  assert.equal(report.invalidTimes,1);assert.equal(report.firstReadError,null);
  assert.equal(report.valid,false);assert.equal(report.relayHop.valid,false);
  assert.equal(report.relayHop.complete,false);assertPaired(report);
 });
});
