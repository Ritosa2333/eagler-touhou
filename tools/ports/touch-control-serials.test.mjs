import test from 'node:test';
import assert from 'node:assert/strict';
import {runInNewContext} from 'node:vm';
import {installTouchControlSerialObserver,neutralTouchControlsFromReceipt} from './runtime-desktop-lifecycle.mjs';

test('shared cleanup preserves the selected Runtime serial stream instead of creating new Bomb and Escape edges',()=>{
  const listeners=[],parent={},context={parent,location:{origin:'https://fixture.test'},addEventListener:(_kind,fn)=>listeners.push(fn)};
  const install=runInNewContext(`(${installTouchControlSerialObserver.toString()})`,context);
  const receipt=install({game:'th08',epoch:3});assert.equal(receipt.bombSerial,0);assert.equal(receipt.escapeSerial,0);
  const send=change=>listeners.forEach(listener=>listener({source:parent,origin:context.location.origin,data:{
    protocol:'eagler-touhou/1',game:'th08',epoch:3,command:'touch-controls',bombSerial:7,escapeSerial:2,...change}}));
  send({});assert.equal(receipt.observations,1);
  const neutral=neutralTouchControlsFromReceipt(receipt,{game:'th08',epoch:3});
  assert.equal(neutral.bombSerial,7);assert.equal(neutral.escapeSerial,2);assert.equal(neutral.fireEnabled,false);assert.equal(neutral.focusEnabled,false);
  send({epoch:2,bombSerial:0,escapeSerial:0});send({game:'th10',bombSerial:0});send({protocol:'other',escapeSerial:0});
  send({bombSerial:-1});assert.equal(receipt.bombSerial,7);assert.equal(receipt.escapeSerial,2);assert.equal(receipt.observations,1);
  assert.equal(install({game:'th08',epoch:3}),receipt);assert.equal(listeners.length,1);
  assert.throws(()=>install({game:'th08',epoch:4}),/another Runtime/);
});

test('foreign origin/source and absent serial evidence cannot be used for cleanup',()=>{
  const listeners=[],parent={},context={parent,location:{origin:'https://fixture.test'},addEventListener:(_kind,fn)=>listeners.push(fn)};
  const install=runInNewContext(`(${installTouchControlSerialObserver.toString()})`,context),receipt=install({game:'th08',epoch:1});
  const data={protocol:'eagler-touhou/1',game:'th08',epoch:1,command:'touch-controls',bombSerial:9,escapeSerial:9};
  listeners[0]({source:{},origin:context.location.origin,data});listeners[0]({source:parent,origin:'https://other.test',data});
  assert.equal(receipt.observations,0);
  for(const invalid of [null,{...receipt,epoch:2},{...receipt,bombSerial:undefined}])assert.throws(()=>neutralTouchControlsFromReceipt(invalid,{game:'th08',epoch:1}));
});
