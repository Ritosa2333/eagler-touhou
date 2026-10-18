// ROOT runs: node --test portable/multiplayer/check-websocket-input-impairment.mjs
// These deterministic tests exercise the actual send wrapper with a recording
// socket and controlled deferred timers. They prove injector boundaries, not a
// browser handshake, real relay delivery, TCP behavior or gameplay performance.
import test,{after} from 'node:test';
import assert from 'node:assert/strict';
import {createRequire} from 'node:module';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {runInNewContext} from 'node:vm';

const sourceUrl=new URL('websocket-input-impairment.cjs',import.meta.url);
const realSendBefore=globalThis.WebSocket?.prototype.send;
const {installWebSocketInputImpairment}=createRequire(import.meta.url)('./websocket-input-impairment.cjs');
const sources=['websocket-input-impairment.cjs','check-websocket-input-impairment.mjs',
 '../../third_party/eagler-common/src/netplay/BrowserPeerTransport.cpp',
 '../../third_party/eagler-common/src/netplay/NetplayProtocol.cpp',
 '../../th10_web/cpp/multiplayer/netplay/NetplayProtocolConfig.hpp'];
const inventory=()=>Object.fromEntries(sources.map(path=>[path,
 createHash('sha256').update(readFileSync(new URL(path,import.meta.url))).digest('hex')]));
const before=inventory();
after(()=>assert.deepEqual(inventory(),before,'Injector/test/protocol source changed during validation'));
const INVALID=0xffffffff;

// Independent wire fixtures, following production EncodeInputPacket and
// BrowserPeerTransport's target envelope; no classifier/encoder from the shim.
function packet(frame=0,{count=1,seat=0,players=2,target=null}={}){
 const bytes=new Uint8Array(44+12*count);
 bytes.set([0x45,0x41,0x4e,0x50,4,1,seat,players]);
 const v=new DataView(bytes.buffer);
 v.setUint32(8,0x71456789,true);v.setUint32(12,0x18273645,true);
 v.setUint32(16,frame,true);v.setUint32(20,INVALID,true);
 v.setUint32(24,frame,true);v.setUint32(28,INVALID,true);
 v.setUint32(32,count?frame-count+1:INVALID,true);v.setUint32(36,frame,true);
 bytes[42]=count; // Neutral samples, reserved byte 43 stays zero.
 if(target===null)return bytes;
 const envelope=new Uint8Array(bytes.length+2);envelope.set([0xe7,target]);envelope.set(bytes,2);
 return envelope;
}
const rawBytes=data=>data instanceof ArrayBuffer?new Uint8Array(data):
 ArrayBuffer.isView(data)?new Uint8Array(data.buffer,data.byteOffset,data.byteLength):null;
const deliveredFrames=h=>h.sent.filter(v=>v.bytes&&[0x45,0xe7].includes(v.bytes[0])).map(v=>{
 const o=v.bytes[0]===0xe7?2:0;return new DataView(v.bytes.buffer).getUint32(o+24,true);
});

function harness(options={},install=true){
 let at=0,id=0,state,failTimer=false;
 const timers=new Map(),sent=[];
 class Socket{
  static OPEN=1;
  constructor(){this.readyState=1;this.failSend=false;}
  send(data){
   if(this.failSend)throw Error('intentional original-send failure');
   sent.push({socket:this,data,bytes:rawBytes(data)?.slice()??null,at});
  }
 }
 const socket=new Socket(),original=Socket.prototype.send;
 state={tag:'th10',route:'relay',relay:socket,localPlayer:0,playerCount:2,
  closed:false,failed:false,disconnected:false};
 const env={WebSocket:Socket,transport:()=>state,now:()=>at,
  later(fn,ms){
   if(failTimer){failTimer=false;throw Error('intentional timer-install failure');}
   const key=++id;timers.set(key,{fn,due:at+ms});return key;
  },cancel(key){timers.delete(key);}};
 const api=install?installWebSocketInputImpairment({oneWayMs:50,jitterMs:0,...options},env):null;
 const h={Socket,socket,original,env,api,sent,timers,
  get state(){return state;},set state(value){state=value;},
  get at(){return at;},set at(value){assert.ok(value>=at);at=value;},
  failNextTimer(){failTimer=true;},
  fire(key,time){
   assert.ok(time>=at);at=time;
   const job=timers.get(key);assert.ok(job,'scheduled timer exists');
   timers.delete(key);job.fn();
  },
  advance(time){
   assert.ok(time>=at);
   while(true){
    const next=[...timers].filter(([,job])=>job.due<=time)
     .sort((a,b)=>a[1].due-b[1].due||a[0]-b[0])[0];
    if(!next)break;
    h.fire(next[0],Math.max(at,next[1].due));
   }
   at=time;
  }};
 return h;
}
function accounting(s){
 assert.equal(s.matched,s.broadcastInputs+s.targetedInputs);
 assert.equal(s.matched,s.scheduled+s.dropped+s.overflow+s.scheduleErrors);
 assert.equal(s.scheduled,s.sent+s.canceled+s.sendErrors+s.pendingJobs);
 assert.equal(s.errors,s.sendErrors+s.scheduleErrors);
 assert.equal(s.canceled,s.canceledStaleOrClosed+s.canceledUninstalled);
 assert.ok(s.pendingJobs>=0&&s.pendingJobs<=1024);
 assert.ok(s.queuedBytes>=0&&s.queuedBytes<=s.maxQueuedBytes);
 if(!s.pendingJobs)assert.equal(s.queuedBytes,0);
}

test('import is inert and the browser-global entry installs the same wrapper',t=>{
 assert.equal(globalThis.WebSocket?.prototype.send,realSendBefore);
 const h=harness({},false),realm={ArrayBuffer,Uint8Array,DataView};
 runInNewContext(readFileSync(sourceUrl,'utf8'),realm);
 assert.equal(typeof realm.installWebSocketInputImpairment,'function');
 assert.equal(h.Socket.prototype.send,h.original);
 const api=realm.installWebSocketInputImpairment({oneWayMs:50,jitterMs:0},h.env);
 h.socket.send(packet(7));assert.equal(h.sent.length,0);h.advance(50);
 assert.deepEqual(deliveredFrames(h),[7]);accounting(api.stats);api.uninstall();
 assert.equal(h.Socket.prototype.send,h.original);
 t.diagnostic(JSON.stringify({kind:'websocket-input-impairment-source-inventory',sources:before}));
});

test('whole raw/targeted payloads are snapshotted from ArrayBuffer, offset views and DataView',()=>{
 const h=harness(),inputs=[packet(10),packet(11,{target:1}),packet(12)];
 const backing=new Uint8Array(inputs[1].length+13);backing.fill(0xc7);backing.set(inputs[1],7);
 const view=backing.subarray(7,7+inputs[1].length);
 const backing2=new Uint8Array(inputs[2].length+11);backing2.fill(0xd3);backing2.set(inputs[2],3);
 const dataView=new DataView(backing2.buffer,3,inputs[2].length),expected=inputs.map(v=>v.slice());
 h.socket.send(inputs[0].buffer);h.socket.send(view);h.socket.send(dataView);
 inputs[0].fill(0);backing.fill(0);backing2.fill(0);
 assert.equal(h.sent.length,0);assert.equal(h.api.stats.queuedBytes,expected.reduce((n,b)=>n+b.length,0));
 h.advance(49);assert.equal(h.sent.length,0);h.advance(50);
 assert.deepEqual(h.sent.map(v=>v.bytes),expected);
 assert.deepEqual(h.sent.map(v=>v.at),[50,50,50]);
 assert.equal(h.api.stats.broadcastInputs,2);assert.equal(h.api.stats.targetedInputs,1);
 assert.equal(h.api.stats.minDeliveredDelayMs,50);assert.equal(h.api.stats.maxDeliveredDelayMs,50);
 accounting(h.api.stats);h.api.uninstall();
});

test('text, control, spectator, unrelated sockets and undecided/RTC routes keep original sends',()=>{
 const h=harness(),control=packet(0);control[5]=2;
 const spectator=new Uint8Array([0xe8,...packet(0)]),opaque={opaque:true},text='{"type":"rtc-failed"}';
 for(const data of [text,control,spectator,opaque]){h.socket.send(data);assert.equal(h.sent.at(-1).data,data);}
 const other=new h.Socket(),binary=packet(0);other.send(binary);assert.equal(h.sent.at(-1).socket,other);
 for(const route of [null,'rtc','spectator']){
  h.state.route=route;h.socket.send(binary);assert.equal(h.sent.at(-1).data,binary);
 }
 h.state.route='relay';h.state.tag='th11';h.socket.send(binary);
 assert.equal(h.api.stats.matched,0);assert.equal(h.timers.size,0);
 h.state.tag='th10';h.socket.send(packet(1));assert.equal(h.api.stats.matched,1);
 h.advance(50);assert.equal(deliveredFrames(h).at(-1),1);accounting(h.api.stats);h.api.uninstall();
});

test('malformed envelopes, frame ranges and count/byte boundaries never become matched delay evidence',()=>{
 const h=harness(),cases=[];
 const changed=(offset,value)=>{const b=packet(4);b[offset]=value;return b;};
 const word=(offset,value)=>{const b=packet(4);new DataView(b.buffer).setUint32(offset,value,true);return b;};
 cases.push(packet(4).slice(0,43),packet(4).slice(0,-1),new Uint8Array([...packet(4),0]),
  changed(0,0x54),changed(1,0x42),changed(4,3),changed(5,3),changed(6,1),changed(7,3),
  changed(42,0),changed(43,1),packet(40,{count:33}),
  word(24,INVALID),word(32,INVALID),word(32,3),
  packet(4,{target:0}),packet(4,{target:2}),new Uint8Array([0xe7]),new Uint8Array([0xe7,1]));
 const ack=packet(INVALID,{count:0});new DataView(ack.buffer).setUint32(32,0,true);cases.push(ack);
 const wrap=packet(1,{count:4});new DataView(wrap.buffer).setUint32(32,0xfffffffe,true);cases.push(wrap);
 for(const data of cases){h.socket.send(data);assert.equal(h.sent.at(-1).data,data);}
 assert.equal(h.sent.length,cases.length);assert.equal(h.api.stats.matched,0);
 assert.equal(h.api.stats.spikeStartMs,null);assert.equal(h.timers.size,0);
 accounting(h.api.stats);h.api.uninstall();
});

test('valid count 0/1/32, frame zero/top boundary and every three-seat target retain full bytes',()=>{
 const h=harness();h.state.localPlayer=2;h.state.playerCount=3;
 const inputs=[packet(0,{seat:2,players:3}),packet(31,{count:32,seat:2,players:3,target:0}),
  packet(0xfffffffe,{count:32,seat:2,players:3,target:1}),
  packet(INVALID,{count:0,seat:2,players:3}),packet(300,{count:0,seat:2,players:3,target:0})];
 for(const data of inputs)h.socket.send(data);
 h.advance(50);assert.deepEqual(h.sent.map(v=>v.bytes),inputs);
 assert.equal(h.api.stats.matched,5);assert.equal(h.api.stats.ackOnlyInputs,2);
 assert.equal(h.api.stats.targetedInputs,3);accounting(h.api.stats);h.api.uninstall();
});

test('INVALID_FRAME ACK does not arm a spike; the first qualifying real frame does',()=>{
 const h=harness({spikeFrame:10,spikeMs:5,spikeDelayMs:100});
 h.socket.send(packet(INVALID,{count:0}));assert.equal(h.api.stats.spikeStartMs,null);
 h.at=1;h.socket.send(packet(9));assert.equal(h.api.stats.spikeStartMs,null);
 h.at=2;h.socket.send(packet(10));assert.equal(h.api.stats.spikeStartMs,2);
 assert.equal(h.api.stats.spikeEndMs,7);assert.equal(h.api.stats.spikeDelayed,1);
 h.advance(200);assert.deepEqual(deliveredFrames(h),[INVALID,9,10]);
 assert.equal(h.api.stats.spikeLastDeliveryMs,152);accounting(h.api.stats);h.api.uninstall();
});

test('one Input spike holds later Input in FIFO while Session/text controls remain unchanged',()=>{
 const h=harness({spikeFrame:100,spikeMs:10,spikeDelayMs:100});
 h.socket.send(packet(99));
 h.at=1;h.socket.send(packet(100));
 h.at=20;h.socket.send(packet(101));
 const text='{"type":"peer-disconnected"}',control=packet(0);control[5]=2;
 h.socket.send(text);h.socket.send(control);
 assert.equal(h.sent[0].data,text);assert.equal(h.sent[1].data,control);
 assert.deepEqual(h.sent.map(v=>v.at),[20,20]);
 h.advance(50);assert.equal(h.sent.at(-1).bytes[24],99);
 h.advance(150);assert.equal(h.sent.length,3);
 h.advance(151);assert.deepEqual(h.sent.slice(2).map(v=>new DataView(v.bytes.buffer).getUint32(24,true)),[99,100,101]);
 assert.deepEqual(h.sent.slice(2).map(v=>v.at),[50,151,151]);
 assert.equal(h.api.stats.spikeDelayed,1);assert.equal(h.api.stats.fifoDelayed,1);
 assert.equal(h.api.stats.fifoDelayTotalMs,81);
 assert.equal(h.api.stats.requestedDelayTotalMs,250);assert.equal(h.api.stats.plannedDelayTotalMs,331);
 assert.equal(h.api.stats.timerOverrunTotalMs,0);accounting(h.api.stats);h.api.uninstall();
});

test('seeded jitter stays bounded and reproducible without reordering Input',()=>{
 function sample(seed){
  const h=harness({oneWayMs:50,jitterMs:25,seed});
  for(let f=0;f<12;++f)h.socket.send(packet(f));
  h.advance(100);assert.deepEqual(deliveredFrames(h),Array.from({length:12},(_,i)=>i));
  const times=h.sent.map(v=>v.at);
  assert.ok(times.every(v=>v>=25&&v<=75));assert.ok(times.every((v,i)=>!i||v>=times[i-1]));
  assert.ok(h.api.stats.fifoDelayed>0);accounting(h.api.stats);h.api.uninstall();return times;
 }
 assert.deepEqual(sample(7135),sample(7135));assert.notDeepEqual(sample(7135),sample(7136));
});

test('equal-deadline timers fired in reverse order still cannot reorder accepted messages',()=>{
 const h=harness();for(let f=0;f<3;++f)h.socket.send(packet(f));
 const ids=[...h.timers.keys()];
 h.fire(ids[2],50);h.fire(ids[1],50);assert.equal(h.sent.length,0);
 h.fire(ids[0],50);assert.deepEqual(deliveredFrames(h),[0,1,2]);
 assert.equal(h.api.stats.pendingJobs,0);accounting(h.api.stats);h.api.uninstall();
});

test('actual timer delay is retained separately from requested delay instead of reporting an invented 50ms',()=>{
 const h=harness();h.socket.send(packet(1));
 h.fire([...h.timers.keys()][0],83);
 assert.equal(h.sent[0].at,83);assert.equal(h.api.stats.requestedDelayTotalMs,50);
 assert.equal(h.api.stats.plannedDelayTotalMs,50);assert.equal(h.api.stats.deliveredDelayTotalMs,83);
 assert.equal(h.api.stats.minDeliveredDelayMs,83);assert.equal(h.api.stats.maxTimerOverrunMs,33);
 accounting(h.api.stats);h.api.uninstall();
});

test('dropEvery omits whole matched Input messages only, without compensating or dropping controls',()=>{
 const h=harness({dropEvery:2});
 for(let f=0;f<6;++f){h.socket.send(packet(f,{target:f%2?1:null}));h.socket.send('control '+f);}
 assert.equal(h.sent.length,6);h.advance(50);
 assert.deepEqual(deliveredFrames(h),[0,2,4]);assert.equal(h.api.stats.dropped,3);
 assert.equal(h.api.stats.scheduled,3);assert.equal(h.api.stats.sent,3);
 accounting(h.api.stats);h.api.uninstall();
});

test('queued data never reaches a replaced, closed, failed or reidentified transport',()=>{
 const mutations=[
  h=>{h.state={...h.state};},h=>{h.state.relay=new h.Socket();},
  h=>{h.state.closed=true;},h=>{h.state.failed=true;},h=>{h.state.disconnected=true;},
  h=>{h.state.route='rtc';},h=>{h.state.tag='th11';},
  h=>{h.state.localPlayer=1;},h=>{h.state.playerCount=3;},
  h=>{h.socket.readyState=2;},h=>{h.socket.readyState=3;}
 ];
 for(const change of mutations){
  const h=harness();h.socket.send(packet(8));change(h);h.advance(50);
  assert.equal(h.sent.length,0);assert.equal(h.api.stats.canceledStaleOrClosed,1);
  assert.equal(h.api.stats.sent,0);accounting(h.api.stats);h.api.uninstall();
 }
 const h=harness();h.socket.send(packet(8));const old=h.socket;
 h.at=10;const next=new h.Socket();h.state={...h.state,relay:next};next.send(packet(9));
 h.advance(60);assert.equal(h.sent.length,1);assert.notEqual(h.sent[0].socket,old);
 assert.equal(h.sent[0].socket,next);assert.deepEqual(deliveredFrames(h),[9]);
 accounting(h.api.stats);h.api.uninstall();
});

test('byte and job limits fail synchronously and released capacity can be reused',()=>{
 const h=harness({maxQueuedBytes:112});
 h.socket.send(packet(0));h.socket.send(packet(1));
 assert.equal(h.api.stats.queuedBytes,112);assert.throws(()=>h.socket.send(packet(2)),/capacity/);
 assert.equal(h.api.stats.overflow,1);assert.equal(h.api.stats.scheduled,2);accounting(h.api.stats);
 h.advance(50);h.socket.send(packet(3));h.advance(100);
 assert.deepEqual(deliveredFrames(h),[0,1,3]);assert.equal(h.api.stats.peakQueuedBytes,112);
 accounting(h.api.stats);h.api.uninstall();
 const many=harness();for(let i=0;i<1024;++i)many.socket.send(packet(INVALID,{count:0}));
 assert.throws(()=>many.socket.send(packet(INVALID,{count:0})),/capacity/);
 assert.equal(many.api.stats.pendingJobs,1024);assert.equal(many.api.stats.queuedBytes,1024*44);
 many.api.uninstall();assert.equal(many.timers.size,0);assert.equal(many.sent.length,0);
 assert.equal(many.api.stats.canceledUninstalled,1024);accounting(many.api.stats);
});

test('timer installation and original-send failures are explicit with no falsely sent or stranded bytes',()=>{
 const h=harness();h.failNextTimer();
 assert.throws(()=>h.socket.send(packet(0)),/timer-install/);
 assert.equal(h.api.stats.scheduleErrors,1);assert.equal(h.api.stats.scheduled,0);
 assert.equal(h.api.stats.queuedBytes,0);assert.equal(h.timers.size,0);accounting(h.api.stats);
 h.socket.send(packet(1));h.socket.failSend=true;h.advance(50);
 assert.equal(h.api.stats.sendErrors,1);assert.equal(h.api.stats.sent,0);assert.equal(h.sent.length,0);
 h.socket.failSend=false;h.socket.send(packet(2));h.advance(100);
 assert.deepEqual(deliveredFrames(h),[2]);assert.equal(h.api.stats.errors,2);
 accounting(h.api.stats);h.api.uninstall();
});

test('uninstall is idempotent, cancels pending bytes and respects a later wrapper owner',()=>{
 const h=harness();assert.throws(()=>installWebSocketInputImpairment({},h.env),/already installed/);
 h.socket.send(packet(0));h.socket.send(packet(1));
 const callbacks=[...h.timers.values()].map(job=>job.fn),wrapper=h.Socket.prototype.send;
 const laterWrapper=function(data){return wrapper.call(this,data);};h.Socket.prototype.send=laterWrapper;
 h.api.uninstall();h.api.uninstall();assert.equal(h.Socket.prototype.send,laterWrapper);
 assert.equal(h.api.stats.canceled,2);assert.equal(h.timers.size,0);
 for(const fn of callbacks)fn();assert.equal(h.sent.length,0);
 const input=packet(2);h.socket.send(input);assert.equal(h.sent[0].data,input);
 accounting(h.api.stats);
 const other=harness();other.api.uninstall();assert.equal(other.Socket.prototype.send,other.original);
 const replacement=installWebSocketInputImpairment({oneWayMs:0,jitterMs:0},other.env);
 other.socket.send(packet(3));assert.equal(other.sent.length,0);other.advance(0);
 assert.deepEqual(deliveredFrames(other),[3]);accounting(replacement.stats);replacement.uninstall();
});

test('invalid settings and missing WebSocket fail before changing send or creating timers',()=>{
 const h=harness({},false),invalid=[
  {oneWayMs:NaN},{oneWayMs:-1},{oneWayMs:10001},{jitterMs:Infinity},{jitterMs:-1},
  {dropEvery:-1},{dropEvery:1.5},{maxQueuedBytes:0},{maxQueuedBytes:1048577},
  {spikeFrame:INVALID},{spikeFrame:-1},{spikeMs:Infinity},{spikeDelayMs:-1},
  {seed:0},{seed:INVALID+1},{seed:1.5}
 ];
 for(const options of invalid){
  assert.throws(()=>installWebSocketInputImpairment(options,h.env),/invalid WebSocket/);
  assert.equal(h.Socket.prototype.send,h.original);assert.equal(h.timers.size,0);
 }
 assert.throws(()=>installWebSocketInputImpairment({}, {...h.env,transport:3}),/invalid WebSocket/);
 assert.throws(()=>installWebSocketInputImpairment({}, {...h.env,WebSocket:class {}}),/WebSocket unavailable/);
 assert.equal(h.Socket.prototype.send,h.original);
});
