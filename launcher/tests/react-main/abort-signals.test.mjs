import {test} from 'vitest';
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {runInNewContext} from 'node:vm';
import {transformSync} from 'esbuild';
const source=readFileSync(new URL('../../app/services/abort-signals.ts',import.meta.url),'utf8');
const {code}=transformSync(source,{loader:'ts',format:'cjs'}),module={exports:{}};
runInNewContext(code,{module,exports:module.exports,AbortController,Set,Map});
const {linkAbortSignals,waitWithAbort}=module.exports;
function tracked(){
  const controller=new AbortController(),signal=controller.signal,active=new Set(),add=signal.addEventListener.bind(signal),remove=signal.removeEventListener.bind(signal);
  signal.addEventListener=(type,handler,options)=>{if(type==='abort')active.add(handler);add(type,handler,options);};
  signal.removeEventListener=(type,handler)=>{if(type==='abort')active.delete(handler);remove(type,handler);};
  return {controller,signal,active};
}
test('signal linking works without AbortSignal.any and forwards either cancellation reason once',()=>{
  const descriptor=Object.getOwnPropertyDescriptor(AbortSignal,'any');Object.defineProperty(AbortSignal,'any',{configurable:true,value:undefined});
  try{for(const first of [0,1]){
    const sources=[tracked(),tracked()],linked=linkAbortSignals(...sources.map(s=>s.signal)),reason=new Error('cancel '+first);
    let calls=0;linked.signal.addEventListener('abort',()=>calls++);
    sources[first].controller.abort(reason);assert.equal(linked.signal.aborted,true);assert.equal(linked.signal.reason,reason);
    sources[1-first].controller.abort(new Error('later'));assert.equal(linked.signal.reason,reason);assert.equal(calls,1);
    assert.equal(sources[0].active.size+sources[1].active.size,0);linked.dispose();
  }}finally{if(descriptor)Object.defineProperty(AbortSignal,'any',descriptor);else delete AbortSignal.any;}
});
test('pre-aborted inputs preserve input order and install no listeners',()=>{
  const a=tracked(),b=tracked();a.controller.abort('first');b.controller.abort('second');
  const linked=linkAbortSignals(a.signal,b.signal);assert.equal(linked.signal.reason,'first');assert.equal(a.active.size+b.active.size,0);
});
test('disposing a completed operation removes its listeners without cancelling inputs',()=>{
  const a=tracked(),b=tracked(),linked=linkAbortSignals(a.signal,b.signal);assert.equal(a.active.size+b.active.size,2);
  linked.dispose();linked.dispose();assert.equal(a.active.size+b.active.size,0);assert.equal(a.signal.aborted,false);
  a.controller.abort('later');assert.equal(linked.signal.aborted,false);
});
test('absent and duplicate inputs preserve the existing single-signal identity',()=>{
  const a=tracked();assert.equal(linkAbortSignals(undefined).signal,undefined);
  assert.equal(linkAbortSignals(a.signal,undefined,a.signal).signal,a.signal);assert.equal(a.active.size,0);
});

test('cancelled room wait releases its successor without waiting for an unabortable old store read',async()=>{
  const source=tracked(),reason=new Error('room resource version changed');let resolve;
  const underlying=new Promise(done=>resolve=done),waiting=waitWithAbort(underlying,source.signal);
  source.controller.abort(reason);await assert.rejects(waiting,error=>error===reason);
  assert.equal(source.active.size,0);resolve('late');
  assert.equal(await waitWithAbort(Promise.resolve('replacement'),new AbortController().signal),'replacement');
});

test('settled room waits return actual values/errors and release their abort listener',async()=>{
  const source=tracked(),failure=new Error('read failed');
  assert.equal(await waitWithAbort(Promise.resolve(7),source.signal),7);
  await assert.rejects(waitWithAbort(Promise.reject(failure),source.signal),error=>error===failure);
  await Promise.resolve();assert.equal(source.active.size,0);
});
