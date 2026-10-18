import test from 'node:test';
import assert from 'node:assert/strict';
import {EventEmitter} from 'node:events';
import {createBoundedRelaySender} from '../../../../services/relay/src/relay-flow-control.mjs';
import {installRelayTimingObserver} from './relay-timing-observer.mjs';

test('observer preserves real relay bytes, timer delay, cancellation and non-game timers',()=>{
  let serial=0;const timers=new Map(),channel=new EventEmitter(),sent=[];
  channel.send=m=>sent.push(m);
  const target={setTimeout(fn,delay,...args){const id=++serial;timers.set(id,{fn,delay,args});return id;},clearTimeout(id){timers.delete(id);}};
  const oldSet=target.setTimeout,oldClear=target.clearTimeout;
  const uninstall=installRelayTimingObserver(target,channel);
  const send=createBoundedRelaySender({schedule:(...args)=>target.setTimeout(...args),cancel:(...args)=>target.clearTimeout(...args)});
  const socket=new EventEmitter();socket.readyState=1;socket.bufferedAmount=0;socket.messages=[];
  socket.send=(data,options)=>socket.messages.push({bytes:Buffer.from(data),options});
  socket.terminate=()=>assert.fail('observer must not change backpressure');
  channel.emit('message',{kind:'relay-delay-phase',phase:'measure'});
  const input=Buffer.from([1,4,9,16]);assert.equal(send(socket,input,{binary:true,delayMs:35}),true);input.fill(0);
  assert.equal(timers.get(1).delay,35);assert.equal(socket.messages.length,0);
  const first=timers.get(1);timers.delete(1);first.fn(...first.args);
  assert.deepEqual(socket.messages,[{bytes:Buffer.from([1,4,9,16]),options:{binary:true}}]);
  send(socket,Buffer.from([7]),{binary:true,delayMs:40});socket.emit('close');assert.equal(timers.size,0);
  const untouched=()=>{};const id=target.setTimeout(untouched,35);assert.equal(timers.get(id).fn,untouched);
  channel.emit('message',{kind:'relay-delay-snapshot'});
  const report=sent.at(-1);assert.equal(report.rows.length,2);assert.equal(report.pending,0);assert.equal(report.overflow,0);
  assert.equal(report.rows[0].phase,'measure');assert.equal(report.rows[0].delay,35);assert.equal(typeof report.rows[0].fired,'number');
  assert.equal(report.rows[1].fired,null);assert.equal(typeof report.rows[1].cancelled,'number');
  uninstall();assert.equal(target.setTimeout,oldSet);assert.equal(target.clearTimeout,oldClear);
});

test('jitter cannot deliver READY ahead of HELLO on a reliable relay stream',()=>{
  let serial=0;const timers=new Map(),channel=new EventEmitter();channel.send=()=>{};
  const target={setTimeout(fn,delay,...args){const id=++serial;timers.set(id,{fn,delay,args});return id;},clearTimeout(id){timers.delete(id);}};
  const uninstall=installRelayTimingObserver(target,channel);
  const send=createBoundedRelaySender({schedule:(...args)=>target.setTimeout(...args),cancel:(...args)=>target.clearTimeout(...args)});
  const socket=new EventEmitter();socket.readyState=1;socket.bufferedAmount=0;const packets=[];
  socket.send=data=>packets.push(Buffer.from(data).toString());socket.terminate=()=>assert.fail('unexpected backpressure');
  send(socket,Buffer.from('HELLO'),{binary:true,delayMs:40});send(socket,Buffer.from('READY'),{binary:true,delayMs:30});
  timers.get(2).fn();assert.deepEqual(packets,[],'early READY timer waits for original preceding message');
  timers.get(1).fn();assert.deepEqual(packets,['HELLO','READY'],'delivery retains sender order and exact bytes');
  uninstall();
});
