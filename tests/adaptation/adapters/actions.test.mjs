import test from 'node:test';
import assert from 'node:assert/strict';
import {readFile} from 'node:fs/promises';
import {resolve} from 'node:path';
import {runInNewContext} from 'node:vm';
import {workspacePath} from '../../../launcher/lib/workspace-layout.mjs';
import {runWasiCpp} from '../../../tools/testing/wasi-cpp.mjs';
import {readTouchActionState,touchActionsRequest} from './actions.mjs';
import {resolveAdaptationAdapter} from './registry.mjs';

const request=touchActionsRequest({game:'th08',variant:'normal'});
const makeWorld=(words,{pointer=64,initial=1,exportName=request.contract.stateExport}={})=>{
  const memory=new WebAssembly.Memory({initial});
  if(pointer+words.length*4<=memory.buffer.byteLength)new Uint32Array(memory.buffer,pointer,words.length).set(words);
  const calls=[];const app={sentinel:'app'};
  const core={memory,[exportName](actual){calls.push(actual);return pointer;}};
  globalThis.__th08Runtime={core,Module:{HEAPU8:new Uint8Array(memory.buffer)},app};
  return{calls,app,memory,core};
};
const full=[1,255,1,9,44,2,15,8,1,3,2,6,0,0,0,0,3,1,1,0];

test('the action snapshot cannot combine a selected export with another Runtime heap',()=>{
  const world=makeWorld(full);
  const context={__th08Runtime:{core:{[request.contract.stateExport]:()=>64},app:world.app},
    HEAPU8:new Uint8Array(world.memory.buffer)};
  try{
    assert.throws(()=>runInNewContext(`(${readTouchActionState.toString()})(request)`,{...context,request}),
      /invalid native snapshot pointer/);
  }finally{delete globalThis.__th08Runtime;}
});

test('the registered action request is serializable and keeps native consumer identity',()=>{
  assert.deepEqual(request,{game:'th08',variant:'normal',contract:{...resolveAdaptationAdapter('th08','normal').actions,
    runtime:resolveAdaptationAdapter('th08','normal').runtime}});
  assert.doesNotThrow(()=>JSON.stringify(request));
  assert.equal(request.contract.sampleBoundary,'after-player-consumer');
  assert.equal(request.contract.stateApp,true);
});

test('the browser reader invokes the native export with the app and decodes a completed consumer snapshot',()=>{
  const world=makeWorld(full);
  try{
    const result=readTouchActionState(request);
    assert.deepEqual(world.calls,[world.app]);
    assert.deepEqual({phase:result.phase,sequence:result.sequence,frame:result.frame,stage:result.stage},
      {phase:'consumer-run',sequence:9,frame:44,stage:2});
    assert.deepEqual(result.consumed,{current:15,previous:8,fire:true,focus:true,bomb:true,escape:true});
    assert.equal(result.focused,true);
    assert.equal(result.activePlayerShots,6);
    assert.equal(result.bomb.active,false);
    assert.deepEqual(result.inventory,{bombsRemaining:3,bombsUsed:1,bombsUsedStage:1});
  }finally{delete globalThis.__th08Runtime;}
});

test('the action reader works after frame.evaluate-style function serialization with no module bindings',()=>{
  const world=makeWorld(full);
  try{
    const serialized=`(${readTouchActionState.toString()})(request)`;
    const result=runInNewContext(serialized,{request,__th08Runtime:globalThis.__th08Runtime});
    assert.equal(result.available,true);
    assert.equal(result.phase,'consumer-run');
    assert.equal(result.consumed.current,15);
    assert.deepEqual(world.calls,[world.app]);
  }finally{delete globalThis.__th08Runtime;}
});

test('paused and unpublished states never reuse stale consumer input',()=>{
  for(const [words,phase] of [
    [[1,0x81,2,0,51,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2],'paused'],
    [[1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],'none'],
  ]){
    makeWorld(words);
    try{
      const result=readTouchActionState(request);
      assert.equal(result.phase,phase);
      assert.equal(result.complete,false);
      assert.equal(result.consumed,null);
      assert.equal(result.bomb,null);
    }finally{delete globalThis.__th08Runtime;}
  }
});

test('the reader rejects wrong validity masks, unknown versions and invalid pointers',()=>{
  for(const words of [
    [...full.slice(0,1),0x1ff,...full.slice(2)],
    [2,...full.slice(1)],
    [...full.slice(0,1),0xfe,...full.slice(2)],
  ]){
    makeWorld(words);
    try{assert.throws(()=>readTouchActionState(request));}finally{delete globalThis.__th08Runtime;}
  }
  const malformed=makeWorld(full,{pointer:131064,initial:2});
  try{assert.throws(()=>readTouchActionState(request),/invalid native snapshot pointer/);}finally{delete globalThis.__th08Runtime;}
});

test('a missing title export is explicit unavailability',()=>{
  makeWorld(full,{exportName:'other_export'});
  try{
    const result=readTouchActionState(request);
    assert.equal(result.available,false);
    assert.match(result.reason,/does not export touch_action_state_v1/);
  }finally{delete globalThis.__th08Runtime;}
});

test('the shared reader accepts uint32 action bits and rejects malformed boolean state',()=>{
  const upperBitRequest=structuredClone(request);
  upperBitRequest.contract.inputMasks.fire=0x80000000;
  const words=[...full];words[6]=0x80000000;
  makeWorld(words);
  try{assert.equal(readTouchActionState(upperBitRequest).consumed.fire,true);}
  finally{delete globalThis.__th08Runtime;}
  for(const offset of [8,12,13]){
    const bad=[...full];bad[offset]=2;makeWorld(bad);
    try{assert.throws(()=>readTouchActionState(request),/non-boolean/);}
    finally{delete globalThis.__th08Runtime;}
  }
});

test('the production C++ snapshot writer packs consumer and paused phases without stale words',async()=>{
  const source=resolve(import.meta.dirname,'../../../tools/ports/fixtures/touch-action-state.cpp');
  await runWasiCpp({source,includes:[workspacePath('th08','web/cpp/game')]});
});
