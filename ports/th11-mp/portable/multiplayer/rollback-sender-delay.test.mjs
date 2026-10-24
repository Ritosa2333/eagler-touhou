import test from 'node:test';
import assert from 'node:assert/strict';
import {runInNewContext} from 'node:vm';
import {createSenderDelayQueue} from './rollback-sender-delay.mjs';

// Exercise the serialized browser function without its module environment.
const createQueue=runInNewContext(`(${createSenderDelayQueue.toString()})`);
function fakeClock(){
  let at=0,serial=0;const timers=new Map();
  return {timers,now:()=>at,set:time=>{at=time;},
    setTimer(callback,delay){const id=++serial;timers.set(id,{callback,delay,due:at+delay});return id;},
    clearTimer:id=>timers.delete(id),
    fire(id,time){const timer=timers.get(id);assert(timer,'timer is pending');timers.delete(id);at=time;timer.callback();},
    advance(time){
      for(;;){const next=[...timers].sort((a,b)=>a[1].due-b[1].due||a[0]-b[0])[0];
        if(!next||next[1].due>time)break;this.fire(next[0],next[1].due);}
      at=time;
    },
  };
}
function socket(readyState=1){
  const listeners=new Set();return {readyState,listeners,
    addEventListener(type,fn){assert.equal(type,'close');listeners.add(fn);},
    removeEventListener(type,fn){assert.equal(type,'close');listeners.delete(fn);},
    close(){this.readyState='closed';for(const fn of [...listeners])fn();},
  };
}
function fixture(options={}){
  const clock=fakeClock(),state={queued:0,queuedBytes:0,maxQueued:0,maxQueuedBytes:0,errors:[]};
  const queue=createQueue({state,now:clock.now,setTimer:clock.setTimer,clearTimer:clock.clearTimer,...options});
  const sent=[],log=[];
  function enqueue(peer,id,due,{ordered=true,size=2,send}={}){
    const payload=new Uint8Array(size);if(size)payload[0]=id;
    const entry={id,scheduled:clock.now(),due,requestedDelay:due-clock.now(),actual:null,lateness:null,size};log.push(entry);
    queue.enqueue({socket:peer,payload,entry,ordered,send:send||function(data){assert.equal(this,peer);sent.push({id,peer,at:clock.now(),data});}});
    return entry;
  }
  return {clock,state,queue,sent,log,enqueue};
}
const empty=f=>{assert.equal(f.state.queued,0);assert.equal(f.state.queuedBytes,0);assert.equal(f.clock.timers.size,0);};

test('fractional adjacent deadlines reproduce the old READY/HELLO inversion, FIFO does not',()=>{
  // The former shim had one timer per packet and due[1] = due[0] + .001.
  // Force the observed early callback order; a monotonic due is not a FIFO.
  const old=fakeClock(),wire=[];old.set(6521);
  const hello=old.setTimer(()=>wire.push('HELLO'),52.394339127932);
  old.set(6521.5);const ready=old.setTimer(()=>wire.push('READY'),51.895339127932);
  old.fire(ready,6573.0999999996275);old.fire(hello,6573.299999999814);
  assert.deepEqual(wire,['READY','HELLO']);

  const f=fixture(),peer=socket();f.clock.set(6521);
  const first=f.enqueue(peer,1,6573.394339127932);
  f.clock.set(6521.5);const second=f.enqueue(peer,2,6571.197600314394);
  assert.equal(second.due,first.due);assert.equal(f.clock.timers.size,1);
  f.clock.fire([...f.clock.timers.keys()][0],6573.0999999996275);
  assert.equal(f.sent.length,0);assert.equal(f.state.queued,2);assert.equal(f.clock.timers.size,1);
  f.clock.advance(6575);
  assert.deepEqual(f.sent.map(packet=>packet.id),[1,2]);
  assert(first.actual>=first.due);assert(second.actual>=second.due);empty(f);
  assert.equal(f.log.length,2);assert.equal(peer.listeners.size,0);
});

test('a zero-delay packet cannot overtake an ordered pending packet',()=>{
  const f=fixture(),peer=socket();f.enqueue(peer,1,50);f.clock.set(10);f.enqueue(peer,2,10);
  assert.equal(f.sent.length,0);f.clock.advance(50);
  assert.deepEqual(f.sent.map(packet=>packet.id),[1,2]);empty(f);
  f.enqueue(peer,3,50);assert.deepEqual(f.sent.map(packet=>packet.id),[1,2,3]);empty(f);
});

test('each socket has its own queue and numeric WebSocket or string RTC open states work',()=>{
  const f=fixture(),ws=socket(),rtc=socket('open');f.enqueue(ws,1,100);f.enqueue(rtc,2,10);
  f.clock.advance(10);assert.deepEqual(f.sent.map(packet=>packet.id),[2]);assert.equal(f.state.queued,1);
  f.clock.advance(100);assert.deepEqual(f.sent.map(packet=>packet.id),[2,1]);empty(f);
});

test('ordered RTC preserves FIFO while an unordered lane retains intentional reordering',()=>{
  const f=fixture(),control=socket('open'),inputs=socket('open');
  f.enqueue(control,1,40);f.enqueue(control,2,10);
  const slow=f.enqueue(inputs,3,40,{ordered:false}),fast=f.enqueue(inputs,4,10,{ordered:false});
  f.clock.advance(10);assert.deepEqual(f.sent.map(packet=>packet.id),[4]);
  f.clock.advance(40);assert.deepEqual(f.sent.map(packet=>packet.id),[4,1,2,3]);
  assert.equal(fast.due,10);assert.equal(slow.due,40);empty(f);
});

test('an early unordered timer is rearmed, with one final delivery and log entry',()=>{
  const f=fixture(),peer=socket('open'),entry=f.enqueue(peer,1,10.75,{ordered:false});
  f.clock.fire([...f.clock.timers.keys()][0],10);
  assert.equal(f.sent.length,0);assert.equal(f.state.queued,1);f.clock.advance(12);
  assert.equal(f.sent.length,1);assert.equal(f.log.length,1);assert.equal(entry.actual,11);
  assert.equal(entry.lateness,.25);empty(f);
});

test('socket close cancels its packets once without blocking another socket',()=>{
  const f=fixture(),a=socket(),b=socket('open');f.enqueue(a,1,30);f.enqueue(a,2,40);
  f.enqueue(b,3,10,{ordered:false});const stale=[...f.clock.timers.values()][0].callback;
  a.close();assert.equal(f.state.queued,1);assert.equal(f.state.queuedBytes,2);assert.equal(a.listeners.size,0);
  assert(f.log.slice(0,2).every(entry=>entry.cancelled==='channel closed before due'&&entry.actual===null));
  stale();f.clock.advance(50);assert.deepEqual(f.sent.map(packet=>packet.id),[3]);empty(f);
});

test('global close clears ordered and unordered timers, logs cancellations, and is idempotent',()=>{
  const f=fixture(),a=socket(),b=socket('open');f.enqueue(a,1,20);f.enqueue(a,2,20);
  f.enqueue(b,3,10,{ordered:false});f.enqueue(b,4,15,{ordered:false});
  const stale=[...f.clock.timers.values()].map(timer=>timer.callback);
  assert.equal(f.state.maxQueued,4);assert.equal(f.state.maxQueuedBytes,8);
  f.queue.close();f.queue.close();for(const callback of stale)callback();
  const after=f.enqueue(a,5,0);assert.equal(after.cancelled,'transport injection closed');
  assert.equal(f.log.length,5);assert(f.log.every(entry=>entry.cancelled&&entry.actual===null));
  assert.equal(f.sent.length,0);assert.equal(a.listeners.size+b.listeners.size,0);empty(f);
});

test('a closed socket and a throwing send release counts and retain a terminal log for every packet',()=>{
  const f=fixture(),closed=socket(),throwing=socket();f.enqueue(closed,1,10);closed.readyState=3;
  const failure=f.enqueue(throwing,2,10,{send(){throw Error('native send failed');}});
  const later=f.enqueue(throwing,3,10);f.clock.advance(10);
  assert.equal(f.log[0].cancelled,'channel closed before due');assert.equal(failure.actual,10);
  assert.match(failure.error,/native send failed/);assert.equal(later.actual,10);
  assert.deepEqual(f.sent.map(packet=>packet.id),[3]);assert.equal(f.state.errors.length,1);empty(f);
});

test('the existing 1024 packet cap is atomic and close releases all accepted entries',()=>{
  const f=fixture(),peer=socket();for(let i=0;i<1024;++i)f.enqueue(peer,i,100,{size:1});
  assert.throws(()=>f.enqueue(peer,1024,100,{size:1}),/queue overflow/);
  assert.equal(f.state.queued,1024);assert.equal(f.state.queuedBytes,1024);assert.equal(f.state.maxQueued,1024);
  assert.equal(f.log.length,1025);assert.match(f.log.at(-1).error,/queue overflow/);
  assert.equal(f.state.errors.length,1);f.queue.close();empty(f);
});

test('the existing 4 MiB cap is atomic across sockets, with no unaccounted bytes',()=>{
  const f=fixture(),a=socket(),b=socket();f.enqueue(a,1,100,{size:4*1024*1024});
  assert.throws(()=>f.enqueue(b,2,100,{size:1}),/queue overflow/);
  assert.equal(f.state.queued,1);assert.equal(f.state.queuedBytes,4*1024*1024);
  assert.equal(f.state.maxQueuedBytes,4*1024*1024);assert.equal(b.listeners.size,0);
  f.queue.close();empty(f);assert.equal(f.log.length,2);
});

test('timer scheduling failure cancels the pending packet without a counter or listener leak',()=>{
  const f=fixture({setTimer(){throw Error('timer unavailable');}}),peer=socket();
  assert.throws(()=>f.enqueue(peer,1,10),/timer unavailable/);
  assert.match(f.log[0].error,/timer unavailable/);assert.equal(f.log[0].cancelled,'transport timer failed');
  assert.equal(f.state.errors.length,1);assert.equal(peer.listeners.size,0);empty(f);
});
