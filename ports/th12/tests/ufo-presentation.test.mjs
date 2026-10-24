import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {readCoreUfo,ufoTextPoses,ufoTextRenderPriority} from '../src/ufo-presentation.mjs';

const native=JSON.parse(fs.readFileSync(new URL('fixtures/ufo-text-native.json',import.meta.url)));
test('UFO text gates, fields, timer arithmetic and reward flashes match original callbacks',()=>{
  assert.equal(native.originalExeSha256,'99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417');
  for(const record of native.records){
    const input=structuredClone(record.input),before=structuredClone(input);
    const expected=record.calls.map(call=>({text:call.text,font:call.font,
      x:call.position[0],y:call.position[1],z:call.position[2],scaleX:call.scale[0],scaleY:call.scale[1],
      colorARGB:call.colorARGB,alignX:call.alignment[0],alignY:call.alignment[1],drawFlag:call.drawFlag}));
    assert.deepEqual(ufoTextPoses(input),expected,record.name);
    for(const pose of expected)assert.equal(ufoTextRenderPriority(pose),pose.x===512?60:22,record.name);
    assert.deepEqual(input,before,'presentation must not advance '+record.name);
    assert.equal(record.rngUnchanged,true,record.name);
  }
});

function coreRecord(state) {
  const bytes=new TextEncoder().encode(JSON.stringify(state)),heap=new Uint8Array(bytes.length+64);
  heap.set(bytes,16);
  return {HEAPU8:heap,_th12_ufo:()=>16,_th12_ufo_size:()=>bytes.length};
}
test('bounded UFO reader accepts the compact presentation API without reading whole state',()=>{
  const state=structuredClone(native.records[3].input);state.enemy.health=1234;state.enemy.maxHealth=1900;
  const core=coreRecord(state);core._th12_state=()=>{throw Error('whole state must not be read');};
  assert.deepEqual(readCoreUfo(core),state);
  const inactive={...state,enemyId:0,enemy:null};assert.deepEqual(readCoreUfo(coreRecord(inactive)),inactive);
  assert.equal(readCoreUfo({}),null);
  assert.deepEqual(ufoTextPoses(null),[]);
});
test('bounded UFO reader rejects invalid memory extents, malformed JSON and field shapes',()=>{
  const state=structuredClone(native.records[3].input);state.enemy.health=1234;state.enemy.maxHealth=1900;
  for(const [ptr,size] of [[-1,2],[.5,2],[16,4097],[16,1],[Number.MAX_SAFE_INTEGER,2],[16,1.5]]){
    const core=coreRecord(state);core._th12_ufo=()=>ptr;core._th12_ufo_size=()=>size;
    assert.throws(()=>readCoreUfo(core),/UFO/);
  }
  const outside=coreRecord(state);outside._th12_ufo_size=()=>outside.HEAPU8.length;assert.throws(()=>readCoreUfo(outside),/UFO/);
  const invalid=coreRecord(state);invalid.HEAPU8[16]=255;assert.throws(()=>readCoreUfo(invalid));
  for(const value of [[],null,{}, {...state,fill:null},{...state,age:1.5},{...state,enemyId:-1},
    {...state,enemy:{}},{...state,enemy:{...state.enemy,maxHealth:null}}])
    assert.throws(()=>readCoreUfo(coreRecord(value)),/UFO/);
});
