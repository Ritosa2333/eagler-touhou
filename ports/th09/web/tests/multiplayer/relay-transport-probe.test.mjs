// Protocol and transport semantics only: fake sockets and a fake clock, no network.
import assert from 'node:assert/strict';
import {EventEmitter} from 'node:events';
import test from 'node:test';
import {installRelayTransportProbe,parseRelayInput} from './relay-transport-probe.mjs';

function packet({sender=0,sequence=0,count=1,targeted=true}={}){
 const payload=Buffer.alloc(44+count*12);
 payload.set([69,57,78,80,4,1,sender,2]);
 payload.writeUInt32LE(17,8);payload.writeUInt32LE(29,12);
 payload.writeUInt32LE(sequence,16);payload.writeUInt32LE(0xffffffff,20);
 payload.writeUInt32LE(sequence,24);payload.writeUInt32LE(0xffffffff,28);
 payload.writeUInt32LE(sequence,32);payload.writeUInt32LE(sequence,36);
 payload.writeInt16LE(-3,40);payload[42]=count;
 for(let i=0;i<count;++i)payload.writeUInt16LE((sequence+i)&511,44+i*12);
 return targeted?Buffer.concat([Buffer.from([0xe7,1-sender]),payload]):payload;
}
function fixture(config={},limits={}){
 let at=0,nextId=1,interval;
 const timers=new Map(),sent=[],token={nativeReturn:true};
 class Socket extends EventEmitter{
  static OPEN=1;
  constructor(){super();this.readyState=1;this.bufferedAmount=0;this.terminated=0;}
  send(...args){if(this.error)throw this.error;sent.push({socket:this,args,at,copy:Buffer.from(args[0])});return token;}
  terminate(){++this.terminated;this.readyState=3;this.emit('close');}
 }
 Socket.Server=class extends EventEmitter{};
 const probe=installRelayTransportProbe(Socket,config,{
  limits,now:()=>at,epoch:()=>1700000000000+at,timeOrigin:1700000000000,
  schedule:(callback,delay)=>{const id=nextId++;timers.set(id,{callback,due:at+delay});return id;},
  cancel:id=>timers.delete(id),repeat:callback=>{interval=callback;return {unref(){}};},cancelRepeat:()=>{interval=null;}
 });
 const server=new Socket.Server(),sockets=[new Socket(),new Socket()];
 for(let side=0;side<2;++side)server.emit('connection',sockets[side],{url:'/?room=fixture&player='+side+'&players=2'});
 function connectForwarding(){
  sockets.forEach((socket,side)=>socket.on('message',(data,binary)=>{
   if(binary)sockets[1-side].send(data[0]===0xe7?data.subarray(2):data,{binary:true});
  }));
 }
 return {probe,server,sockets,sent,timers,token,connectForwarding,
  clock:value=>{at=value;},tick:()=>interval?.(),
  flush(value){at=value;for(const [id,entry] of [...timers].sort((a,b)=>a[1].due-b[1].due))if(entry.due<=at&&timers.delete(id))entry.callback();},
  receive:(side,data)=>sockets[side].emit('message',data,true)
 };
}
const phases=trace=>trace.events.map(row=>trace.columns.phases[row[1]]);

test('exact v4 parser strips targeted envelope, retains unsigned ACK and hashes identical bytes',()=>{
 const a=packet({sender:0,sequence:42}),bare=a.subarray(2),p=parseRelayInput(a),q=parseRelayInput(bare);
 assert.equal(p.kind,'input');assert.equal(p.target,1);
 assert.deepEqual(p.fields,q.fields);assert.equal(p.fields[4],42);
 assert.equal(p.fields[5],0xffffffff);assert.equal(p.fields[7],0xffffffff);
 assert.equal(p.fields[10],-3);assert.equal(p.fields[11],1);assert.equal(p.fields[13],56);assert.equal(p.fields[15],1);
 const malformed=Buffer.from(a);malformed[6]=3;
 assert.equal(parseRelayInput(malformed).kind,'malformed');
 assert.equal(parseRelayInput(a.subarray(0,-1)).kind,'malformed');
 assert.equal(parseRelayInput('signal').kind,'text');
 assert.equal(parseRelayInput(new Uint8Array([1,2,3])).kind,'other-binary');
});
test('observer forwards synchronously with receiver, arguments, return and original exception intact',()=>{
 const h=fixture();
 try{
  const bytes=packet({targeted:false}),options={binary:true},callback=()=>{};
  assert.equal(h.sockets[1].send(bytes,options,callback),h.token);
  assert.equal(h.sent[0].socket,h.sockets[1]);assert.equal(h.sent[0].args[0],bytes);
  assert.equal(h.sent[0].args[1],options);assert.equal(h.sent[0].args[2],callback);assert.equal(h.timers.size,0);
  const originalError=Error('native send failure');h.sockets[1].error=originalError;
  assert.throws(()=>h.sockets[1].send(bytes),error=>error===originalError);
  const trace=h.probe.snapshot();assert.equal(trace.counts.syncThrows,1);
  assert.equal(trace.events.at(-1)[15],1);assert.equal(trace.valid,true);
 }finally{h.probe.dispose();}
});
test('duplicate packet occurrences retain receive -> schedule -> send links',()=>{
 const h=fixture();h.connectForwarding();
 try{
  const p=packet();assert.equal(h.receive(0,p),true);h.clock(1);h.receive(0,p);
  const trace=h.probe.snapshot();
  assert.equal(trace.packets.length,1);assert.deepEqual(phases(trace),['receive','scheduled','send','receive','scheduled','send']);
  for(let offset=0;offset<6;offset+=3){
   assert.equal(trace.events[offset+1][4],trace.events[offset][0]);
   assert.equal(trace.events[offset+2][4],trace.events[offset][0]);
   assert.equal(trace.events[offset+2][5],trace.events[offset+1][5]);
  }
  assert.equal(trace.coverage.exercised,true);assert.equal(trace.coverage.connectionHookExercised,true);
  assert.equal(trace.valid,true);assert.equal(h.sent.length,2);
 }finally{h.probe.dispose();}
});
test('per-sender seeded jitter, relay burst anchor and retained bytes are deterministic',()=>{
 const h=fixture({inject:true,delay:39,jitter:5,burstAt:0,burstFor:500,burstDelay:120});h.connectForwarding();
 try{
  h.receive(0,packet({count:0}));assert.equal(h.probe.snapshot().injection.anchor,null);
  h.clock(100);const input=packet({sequence:1});h.receive(0,input);h.receive(1,packet({sender:1,sequence:1}));
  const before=h.probe.snapshot(),scheduled=before.events.filter(e=>e[1]===1);
  let seed0=(Math.imul(0x915ab,1664525)+1013904223)>>>0;
  const expected0=39+(seed0/0x100000000*2-1)*5;
  seed0=(Math.imul(seed0,1664525)+1013904223)>>>0;
  const seed1=(Math.imul(0x915ac,1664525)+1013904223)>>>0;
  assert.equal(scheduled[0][8],expected0);
  assert.equal(scheduled[1][8],39+(seed0/0x100000000*2-1)*5+120);
  assert.equal(scheduled[2][8],39+(seed1/0x100000000*2-1)*5+120);
  assert.equal(before.injection.anchor.at,100);assert.equal(before.counts.burstScheduled,2);
  const retained=Buffer.from(input.subarray(2));input.fill(255);h.flush(300);
  assert.deepEqual(h.sent[1].copy,retained);assert.equal(h.probe.snapshot().pending.length,0);
  h.clock(600);h.receive(0,packet({sequence:2}));assert.equal(h.probe.snapshot().events.filter(e=>e[1]===1).at(-1)[10],0);
  assert.equal(h.probe.snapshot().valid,true);
 }finally{h.probe.dispose();}
});
test('stop is idempotent, retains pending tail and leaves delayed transport running',()=>{
 const h=fixture({inject:true,delay:20});h.connectForwarding();
 try{
  h.receive(0,packet());const stopped=h.probe.snapshot({stop:true});
  assert.equal(stopped.pending.length,1);assert.equal(stopped.counts.sent,0);assert.equal(stopped.valid,true);
  assert.equal(h.probe.snapshot({stop:true}),stopped);h.flush(30);
  assert.equal(h.sent.length,1);assert.equal(h.probe.snapshot(),stopped);assert.equal(stopped.pending.length,1);
  h.receive(0,packet({sequence:1}));assert.equal(h.timers.size,1);h.flush(60);assert.equal(h.sent.length,2);
 }finally{h.probe.dispose();}
});
test('closed sockets cancel only their queued sends and preserve cancellation records',()=>{
 const h=fixture({inject:true,delay:20});h.connectForwarding();
 try{
  h.receive(0,packet());h.receive(1,packet({sender:1}));
  h.sockets[1].terminate();assert.equal(h.timers.size,1);h.flush(30);
  const trace=h.probe.snapshot();assert.equal(h.sent.length,1);assert.equal(trace.counts.cancelled,1);
  assert.equal(trace.events.find(e=>e[1]===3)[16],1);assert.equal(trace.pending.length,0);assert.equal(trace.valid,true);
 }finally{h.probe.dispose();}
});
test('queue budget and diagnostic overflow are bounded and explicit',()=>{
 const h=fixture({inject:true,delay:100},{pendingMessages:1});h.connectForwarding();
 try{
  h.receive(0,packet());h.receive(0,packet({sequence:1}));
  const trace=h.probe.snapshot();assert.equal(h.sockets[1].terminated,1);assert.equal(h.sockets[0].terminated,0);
  assert.equal(trace.valid,false);assert.equal(trace.counts.budgetFailures,1);assert.equal(trace.pending.length,0);
  assert.equal(trace.errors[0].code,'relay-injection-budget');assert.equal(h.timers.size,0);
 }finally{h.probe.dispose();}
 const q=fixture({}, {packets:1,events:1});q.connectForwarding();
 try{
  q.receive(0,packet());q.receive(0,packet({sequence:1}));
  const trace=q.probe.snapshot();assert.equal(q.sent.length,2);assert.equal(trace.packets.length,1);assert.equal(trace.events.length,1);
  assert.equal(trace.valid,false);assert.ok(trace.overflow.events>0);assert.ok(trace.overflow.packets>0);
 }finally{q.probe.dispose();}
});
test('a connection-only RTC path is a valid empty input trace, not exercised',()=>{
 const h=fixture();
 try{
  h.clock(180);h.tick();const trace=h.probe.snapshot({stop:true});
  assert.equal(trace.valid,true);assert.equal(trace.coverage.exercised,false);assert.equal(trace.coverage.connectionHookExercised,true);
  assert.equal(trace.packets.length,0);assert.equal(trace.events.length,0);assert.deepEqual(trace.timing[0],[180,100,80,180]);
 }finally{h.probe.dispose();}
});


