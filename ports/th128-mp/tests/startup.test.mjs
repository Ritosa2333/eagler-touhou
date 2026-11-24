import assert from 'node:assert/strict';
import {test} from 'node:test';
import {createMultiplayerRules} from '../../th128/site/runtime/multiplayer-rules.mjs';
import {createMultiplayerStartGate} from '../runtime/startup.mjs';

const rules=(patch={})=>createMultiplayerRules({schema:'th128-multiplayer-rules/1',matchId:'startup-test',epoch:1,
  mode:'parallel-stage',route:'A1-3',seed:34772,coreSHA256:'a'.repeat(64),resourceSHA256:'b'.repeat(64),
  inputDelay:2,rollbackWindow:2,...patch});
const overWire=value=>JSON.parse(JSON.stringify(value));
const gate=(localPlayer,options={})=>createMultiplayerStartGate({localPlayer,rules:rules(),allowedMask:763,...options});

test('both ready offers must be validated and received before game frames unlock',async()=>{
  const [a,b]=await Promise.all([gate(0),gate(1)]);
  assert.equal(a.status().canStart,false);assert.equal(b.status().canStart,false);
  const bAck=await b.receive(overWire(a.offer()),0);
  assert.equal(b.status().canStart,false);
  await assert.rejects(a.receive(overWire(bAck),1),/before remote ready/);
  const aAck=await a.receive(overWire(b.offer()),1);
  assert.equal(a.status().canStart,false);
  await b.receive(overWire(aAck),0);
  assert.equal(b.status().canStart,true);assert.equal(a.status().canStart,false);
  await a.receive(overWire(bAck),1);
  assert.equal(a.status().canStart,true);
  assert.deepEqual(await a.receive(overWire(b.offer()),1),aAck);
});

test('seat, game identity, mask and epoch mismatches cannot ready the round',async()=>{
  const right=await gate(0),same=await gate(1),old=await gate(1,{rules:rules({epoch:2})});
  await assert.rejects(right.receive(same.offer(),0),/remote seat/);
  await assert.rejects(right.receive({...same.offer(),allowedMask:3},1),/capability/);
  await assert.rejects(right.receive(old.offer(),1),/identity/);
  await assert.rejects(right.receive({...same.offer(),rules:{...same.offer().rules,seed:5}},1),/rules mismatch/);
  await assert.rejects(right.receive({...same.offer(),wat:true},1),/fields/);
  assert.equal(right.status().remoteReady,false);assert.equal(right.status().canStart,false);
});
