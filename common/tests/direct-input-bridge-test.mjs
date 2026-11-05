import test from 'node:test';
import assert from 'node:assert/strict';
import {createDirectInputBridge} from '../browser/direct-input-bridge.mjs';
const identity={protocol:'eagler-touhou/1',game:'th11',epoch:23,origin:'https://example.test'};
const message={protocol:identity.protocol,game:identity.game,epoch:identity.epoch,command:'direct-touch',type:'down',id:1,x:.5,y:.5};
test('bridge consumes one validated live input synchronously and never queues it',()=>{
  let calls=0,ready=false;
  const bridge=createDirectInputBridge({...identity,ready:()=>ready,dispatch:m=>{assert.equal(m,message);++calls;return true;}});
  assert(Object.isFrozen(bridge));assert.equal(bridge.schema,'eagler-touhou/direct-input/1');
  assert.equal(bridge.submit(message),false);assert.equal(calls,0);
  ready=true;assert.equal(bridge.submit(message),true);assert.equal(calls,1);
  ready=false;assert.equal(bridge.submit(message),false);assert.equal(calls,1);
});
test('stale identity, RPC and non-input operations stay outside the bridge',()=>{
  let calls=0;const bridge=createDirectInputBridge({...identity,ready:()=>true,dispatch:()=>{++calls;return true;}});
  for(const m of [null,[],{...message,protocol:'other'},{...message,game:'th10'},{...message,epoch:24},
    {...message,request:'rpc'},{...message,command:'launch'},{...message,command:'write'},{...message,command:'sync'}])assert.equal(bridge.submit(m),false);
  assert.equal(calls,0);
});
test('false is untouched and a throwing or unknown result is never converted into fallback',()=>{
  let calls=0;const create=dispatch=>createDirectInputBridge({...identity,ready:()=>true,dispatch});
  assert.equal(create(()=>false).submit(message),false);
  assert.throws(()=>create(()=>{++calls;throw Error('after side effect');}).submit(message),/after side effect/);
  assert.equal(calls,1);
  assert.throws(()=>create(()=>undefined).submit(message),/acknowledgement/);
  assert.throws(()=>create(()=>Promise.resolve(true)).submit(message),/acknowledgement/);
  assert.throws(()=>createDirectInputBridge({...identity,epoch:0,ready:()=>true,dispatch:()=>true}),/identity/);
});
