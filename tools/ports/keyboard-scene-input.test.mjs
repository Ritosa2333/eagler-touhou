import test from 'node:test';
import assert from 'node:assert/strict';
import {sendOwnedPhysicalKey} from './keyboard-scene-input.mjs';

test('a held virtual key cannot suppress physical down/up or be cleared by physical release',async()=>{
  const heldKeys=new Set(),physical=new Set(),virtual=new Set(['KeyZ']),events=[];
  const io={heldKeys,code:'KeyZ',readBridge:async()=>({active:true,held:physical.has('KeyZ')||virtual.has('KeyZ')}),
    dispatch:async(code,down)=>{
      events.push([code,down]);if(down)physical.add(code);else physical.delete(code);
      return{source:'physical',code,down,transitions:[]};
    }};
  const down=await sendOwnedPhysicalKey({...io,down:true});assert.equal(down.changed,true);
  const up=await sendOwnedPhysicalKey({...io,down:false});assert.equal(up.bridgeHeld,true);assert.equal(up.physicalHeld,false);
  await sendOwnedPhysicalKey({...io,down:true});
  virtual.clear();assert.equal((await io.readBridge()).held,true,'physical owner survives virtual release');
  await sendOwnedPhysicalKey({...io,down:false});assert.equal((await io.readBridge()).held,false);
  assert.deepEqual(events,[['KeyZ',true],['KeyZ',false],['KeyZ',true],['KeyZ',false]]);
});

test('no-op releases do not invent physical events, and a rejected mapper cannot acknowledge a key',async()=>{
  const heldKeys=new Set();let dispatched=0;
  const io={heldKeys,code:'KeyZ',readBridge:async()=>({active:true,held:false}),
    dispatch:async(code,down)=>{dispatched++;return{source:'physical',code,down,transitions:null};}};
  assert.equal((await sendOwnedPhysicalKey({...io,down:false})).changed,false);assert.equal(dispatched,0);
  await assert.rejects(sendOwnedPhysicalKey({...io,down:true}),/did not reach the production mapper/);
  assert.equal(heldKeys.has('KeyZ'),true,'cleanup must know a physical down was sent even when its mapper rejected it');
});
