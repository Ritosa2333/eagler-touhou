// Fake DOM/RTC/clock contracts only: no browser, socket, WASM or timer is started.
import assert from 'node:assert/strict';
import {EventEmitter} from 'node:events';
import test from 'node:test';
import vm from 'node:vm';
import {installStartupTransportProbe} from './startup-transport-probe.mjs';
import {installRelayTransportProbe} from './relay-transport-probe.mjs';
import {relayRtcTimeoutConfiguration} from './relay-startup-probe.mjs';

function browserFixture(options={}){
 let at=10;
 const context=vm.createContext({URL,performance:{now:()=>++at,timeOrigin:1700000000000},Date:{now:()=>1700000000000+at}});
 vm.runInContext(String.raw`
  globalThis.location={href:'http://127.0.0.1:8123/'};
  class Surface{
   constructor(){this.listeners=new Map();}
   addEventListener(name,fn){const a=this.listeners.get(name)||[];a.push(fn);this.listeners.set(name,a);}
   fire(name,data={}){for(const fn of this.listeners.get(name)||[])fn.call(this,data);this['on'+name]?.call(this,data);}
  }
  class WS extends Surface{
   static OPEN=1;
   constructor(url){super();this.url=url;this.readyState=1;this.sent=[];this.token={native:true};this.newTarget=new.target;}
   send(...args){this.receivedThis=this;this.sent.push(args);if(this.sendError)throw this.sendError;return this.token;}
  }
  class DC extends Surface{
   constructor(label,options={}){super();this.label=label;this.ordered=options.ordered??true;this.maxRetransmits=options.maxRetransmits??null;this.readyState='connecting';}
  }
  class PC extends Surface{
   constructor(config){super();if(config?.constructorError)throw config.constructorError;this.config=config;this.newTarget=new.target;this.connectionState='new';this.iceConnectionState='new';this.iceGatheringState='new';this.signalingState='stable';this.calls=[];}
   invoke(name,args){this.calls.push({name,args,receiver:this});if(this.callError)throw this.callError;return this.nextPromise??Promise.resolve({type:'offer',sdp:'SECRET-SDP'});}
   createOffer(...args){return this.invoke('createOffer',args);}
   createAnswer(...args){return this.invoke('createAnswer',args);}
   setLocalDescription(...args){return this.invoke('setLocalDescription',args);}
   setRemoteDescription(...args){return this.invoke('setRemoteDescription',args);}
   addIceCandidate(...args){return this.invoke('addIceCandidate',args);}
   createDataChannel(...args){this.dcArgs=args;return new DC(...args);}
   close(){this.connectionState='closed';return 37;}
  }
  globalThis.nativeWS=globalThis.WebSocket=WS;globalThis.nativePC=globalThis.RTCPeerConnection=PC;globalThis.DC=DC;
 `,context);
 vm.runInContext('('+installStartupTransportProbe.toString()+')('+JSON.stringify({relayPort:8123,side:0,...options})+');',context);
 return {context,run:source=>vm.runInContext(source,context),snapshot:()=>context.startupTransportProbe.snapshot()};
}

test('startup wrappers preserve constructors, receiver, argument objects and the identical native promise',async()=>{
 const h=browserFixture();
 h.run("globalThis.ws=new WebSocket('ws://127.0.0.1:8123/?player=0&signal=1&member=SECRET-MEMBER');globalThis.pc=new RTCPeerConnection({iceServers:[]});");
 assert.equal(h.run('ws instanceof nativeWS && ws.newTarget===nativeWS && pc instanceof nativePC && pc.newTarget===nativePC'),true);
 assert.equal(h.run("class Derived extends RTCPeerConnection{};const d=new Derived({});d instanceof Derived&&d.newTarget===Derived"),true);
 assert.equal(h.run("globalThis.args={iceRestart:false};globalThis.p=Promise.resolve({type:'offer',sdp:'SECRET-SDP'});pc.nextPromise=p;pc.createOffer(args)===p&&pc.calls.at(-1).receiver===pc&&pc.calls.at(-1).args[0]===args"),true);
 assert.equal(h.run("globalThis.text=JSON.stringify({type:'rtc-ready'});ws.send(text)===ws.token&&ws.sent.at(-1)[0]===text&&ws.receivedThis===ws"),true);
 assert.equal(h.run('pc.close()'),37);
 await Promise.resolve();
 const snapshot=h.snapshot();assert.equal(snapshot.valid,true);assert.equal(snapshot.counts.pcSettled,1);
 assert.ok(snapshot.events.some(e=>e.method==='createOffer'&&e.event==='resolved'));
});

test('rejection and synchronous exceptions keep their original identity, with only the error name observed',async()=>{
 const h=browserFixture();h.run('globalThis.pc=new RTCPeerConnection({});globalThis.failure=new TypeError("SECRET-ERROR-IP");globalThis.p=Promise.reject(failure);pc.nextPromise=p;globalThis.returned=pc.setLocalDescription({type:"offer",sdp:"SECRET-SDP"});');
 assert.equal(h.context.returned,h.context.p);
 await assert.rejects(h.context.returned,error=>error===h.context.failure);
 h.run('pc.callError=failure;');
 assert.throws(()=>h.run('pc.createAnswer()'),error=>error===h.context.failure);
 h.run("globalThis.ws=new WebSocket('ws://127.0.0.1:8123/?player=0&signal=1');ws.sendError=failure;");
 assert.throws(()=>h.run('ws.send(JSON.stringify({type:"rtc-ready"}))'),error=>error===h.context.failure);
 const trace=h.snapshot();assert.equal(trace.valid,true);
 assert.ok(trace.events.some(e=>e.event==='rejected'&&e.errorName==='TypeError'));
 assert.doesNotMatch(JSON.stringify(trace),/SECRET-/);
});

test('text and state observation retains no SDP, candidate text, URL or error message; original handlers coexist',async()=>{
 const h=browserFixture();
 h.run(String.raw`
  globalThis.ws=new WebSocket('ws://127.0.0.1:8123/?player=1&signal=1&member=SECRET-MEMBER');
  globalThis.handler=function(){this.handled=(this.handled||0)+1;};ws.onmessage=handler;
  ws.fire('message',{data:JSON.stringify({type:'signal',from:0,description:{type:'offer',sdp:'SECRET-SDP'},candidate:{candidate:'SECRET-CANDIDATE-IP'}})});
  ws.send(JSON.stringify({type:'rtc-failed',error:'NotSupportedError: SECRET-ERROR-URL'}));
  globalThis.pc=new RTCPeerConnection({});globalThis.description={type:'answer',get sdp(){throw Error('SDP was read');}};
  pc.setRemoteDescription(description);
  pc.iceConnectionState='checking';pc.fire('iceconnectionstatechange');
  globalThis.dc=pc.createDataChannel('th09-input',{ordered:false,maxRetransmits:0});
  dc.readyState='open';dc.fire('open');pc.fire('datachannel',{channel:new DC('th09-control',{ordered:true})});
  pc.fire('icecandidate',{candidate:{candidate:'SECRET-CANDIDATE'}});
  new WebSocket('wss://private.example/SECRET-URL');
 `);
 await Promise.resolve();
 const trace=h.snapshot();assert.equal(trace.valid,true);assert.equal(h.run('ws.onmessage===handler&&ws.handled===1'),true);
 assert.equal(trace.sockets.length,1);assert.equal(trace.counts.ignoredSockets,1);
 assert.equal(trace.channels.length,2);assert.equal(trace.channels[0].kind,'input');assert.equal(trace.channels[0].maxRetransmits,0);
 assert.ok(trace.events.some(e=>e.type==='signal'&&e.descriptionType==='offer'&&e.candidatePresent));
 assert.ok(trace.events.some(e=>e.type==='rtc-failed'&&e.errorName==='NotSupportedError'));
 assert.doesNotMatch(JSON.stringify(trace),/SECRET-|private\.example|"candidate":|"sdp":/i);
});

test('wrapping chain is explicitly verified, stop is bounded and disabled mode changes nothing',()=>{
 const off=browserFixture({enabled:false});assert.equal(off.context.startupTransportProbe,undefined);assert.equal(off.run('WebSocket===nativeWS&&RTCPeerConnection===nativePC'),true);
 const h=browserFixture();h.run('const previous=WebSocket.prototype.send;WebSocket.prototype.send=function(...args){return Reflect.apply(previous,this,args);};');
 assert.equal(h.snapshot().valid,false);
 h.run('startupTransportProbe.expectSendWrapper(WebSocket.prototype.send);');assert.equal(h.snapshot().valid,true);
 const frozen=h.context.startupTransportProbe.snapshot({stop:true});h.run("new WebSocket('ws://127.0.0.1:8123/?signal=1&player=0').send(JSON.stringify({type:'rtc-ready'}));");
 assert.equal(h.context.startupTransportProbe.snapshot({stop:true}),frozen);
 const small=browserFixture({limits:{events:1}});small.run('new RTCPeerConnection({});new RTCPeerConnection({});');
 const trace=small.snapshot();assert.equal(trace.events.length,1);assert.ok(trace.overflow.events>0);assert.equal(trace.valid,false);
});

test('relay startup text records synchronous receive/send causality while E9NP gameplay remains unexercised',()=>{
 let at=0,interval;const token={native:true};
 class Socket extends EventEmitter{static OPEN=1;constructor(){super();this.readyState=1;this.bufferedAmount=0;}send(...args){this.args=args;if(this.error)throw this.error;return token;}}
 Socket.Server=class extends EventEmitter{};
 const probe=installRelayTransportProbe(Socket,{startupTrace:true,rtcTimeoutMs:4500},{now:()=>++at,epoch:()=>1700000000000+at,timeOrigin:1700000000000,repeat:fn=>(interval=fn,{unref(){}}),cancelRepeat:()=>{interval=null;}});
 try{
  const server=new Socket.Server(),a=new Socket(),b=new Socket();
  server.emit('connection',a,{url:'/?room=SECRET-ROOM&signal=1&player=0&member=SECRET-MEMBER'});
  server.emit('connection',b,{url:'/?room=SECRET-ROOM&signal=1&player=1'});
  const text=JSON.stringify({type:'signal',from:0,description:{type:'offer',sdp:'SECRET-SDP'},candidate:{candidate:'SECRET-IP'}});
  a.on('message',()=>assert.equal(b.send(text),token));
  assert.equal(a.emit('message',Buffer.from(text),false),true);assert.equal(b.args[0],text);
  const trace=probe.snapshot();assert.equal(trace.valid,true);assert.equal(trace.coverage.exercised,false);assert.equal(trace.startup.coverage.exercised,true);
  const received=trace.startup.events.find(e=>e.phase==='receive'),sent=trace.startup.events.find(e=>e.phase==='send');
  assert.equal(sent.causeReceiveId,received.id);assert.ok(received.exitedAt>=sent.exitedAt);
  assert.equal(trace.startup.configuration.rtcTimeoutMs,4500);assert.equal(trace.startup.coverage.exactTimerFire,false);
  assert.doesNotMatch(JSON.stringify(trace.startup),/SECRET-/);
  const error=new Error('SECRET-FAILURE');b.error=error;
  assert.throws(()=>b.send(JSON.stringify({type:'rtc-ready'})),value=>value===error);
  const failed=probe.snapshot().startup.events.at(-1);assert.equal(failed.threw,true);assert.equal(failed.thrownErrorName,'Error');
  assert.equal(probe.snapshot().valid,true);
 }finally{probe.dispose();}assert.equal(interval,null);
});

test('constructor entry/return/throw share call IDs before attachment; data-channel synchronous return is bounded',()=>{
 const h=browserFixture();h.run('globalThis.failure=new TypeError("SECRET-CONSTRUCTOR");');
 assert.throws(()=>h.run('new RTCPeerConnection({constructorError:failure})'),error=>error===h.context.failure);
 h.run('globalThis.pc=new RTCPeerConnection({});pc.createDataChannel("th09-input",{ordered:false,maxRetransmits:0});');
 const trace=h.snapshot();assert.equal(trace.valid,true);assert.equal(trace.version,2);assert.equal(trace.coverage.constructorBoundaries,true);
 const enters=trace.events.filter(e=>e.event==='construct-enter');assert.equal(enters.length,2);
 const thrown=trace.events.find(e=>e.event==='construct-threw'),returned=trace.events.find(e=>e.event==='construct-returned');
 assert.equal(thrown.call,enters[0].call);assert.equal(thrown.errorName,'TypeError');assert.ok(thrown.at>enters[0].at);
 assert.equal(returned.call,enters[1].call);assert.ok(returned.at>enters[1].at);
 assert.equal(trace.pcs.length,1);assert.equal(trace.pcs[0].constructorCall,returned.call);
 const created=trace.events.find(e=>e.target==='pc'&&e.event==='created');assert.equal(created.constructorCall,returned.call);assert.ok(created.at>returned.at);
 const dcCall=trace.events.find(e=>e.method==='createDataChannel'&&e.event==='call');
 const dcReturn=trace.events.find(e=>e.method==='createDataChannel'&&e.event==='returned');
 assert.equal(dcCall.call,dcReturn.call);assert.ok(dcReturn.at>dcCall.at);assert.doesNotMatch(JSON.stringify(trace),/SECRET-/);
});

test('RTC timeout metadata matches canonical defaults, aliases and minimum without changing configuration',()=>{
 assert.deepEqual(relayRtcTimeoutConfiguration({}),{rtcTimeoutMs:4500,configuredNames:[]});
 assert.equal(relayRtcTimeoutConfiguration({TH07_DIRECT_TIMEOUT_MS:'200'}).rtcTimeoutMs,1000);
 assert.equal(relayRtcTimeoutConfiguration({EAGLER_NETPLAY_RTC_TIMEOUT_MS:'9000',TH07_RTC_TIMEOUT_MS:'9000'}).rtcTimeoutMs,9000);
 assert.equal(relayRtcTimeoutConfiguration({EAGLER_NETPLAY_RTC_TIMEOUT_MS:'bad'}).rtcTimeoutMs,4500);
 assert.throws(()=>relayRtcTimeoutConfiguration({EAGLER_NETPLAY_RTC_TIMEOUT_MS:'4500',TH07_RTC_TIMEOUT_MS:'4501'}));
});
