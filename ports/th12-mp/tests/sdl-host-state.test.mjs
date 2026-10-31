import test from 'node:test';
import assert from 'node:assert/strict';
import {readSdlHostState} from '../src/sdl-host-state.mjs';

function fixture() {
  const strings=new Map([[512,'playing'],[544,'game'],[576,'th12_02']]);
  const core={HEAPU8:new Uint8Array(1024),_th12_host_state_values:()=>8,
    UTF8ToString:p=>strings.get(p),_th12_host_state(){throw Error('JSON used on frame path');}};
  const values=[1,27,512,544,1,59.5,4,0,12,2,3,2,1,1234,7,-32,55296,4.5,2,0,25,14,600,0,0,576,0];
  new Float64Array(core.HEAPU8.buffer,8,27).set(values);
  return {core,values,strings};
}

test('numeric state preserves touch, menu, replay and rendering fields without parsing JSON',()=>{
  const {core}=fixture();
  assert.deepEqual(readSdlHostState(core),{scene:'playing',kind:'game',ready:true,fps:59.5,
    menuSerial:4,menuReady:false,menuAge:12,selection:2,stage:3,character:2,shot:1,cursor:1234,
    instance:'3:7',gestureScope:'3',x:-32,y:55296,fast:4.5,slow:2,paused:false,captureSerial:25,
    drawCalls:14,vertices:600,practiceVertices:0,capturing:false,track:'th12_02',seeking:false});
});

test('each read observes current native values, string content and replacement WASM memory',()=>{
  const {core,values,strings}=fixture(),before=readSdlHostState(core);
  const next=values.slice();next[4]=0;next[10]=4;next[13]=1;next[19]=1;next[24]=1;next[26]=1;
  core.HEAPU8=new Uint8Array(2048);new Float64Array(core.HEAPU8.buffer,8,27).set(next);
  strings.set(512,'pause');strings.set(544,'menu');strings.set(576,'th12_10');
  const after=readSdlHostState(core);
  assert.equal(after.stage,4);assert.equal(after.instance,'4:7');assert.equal(after.scene,'pause');
  assert.equal(after.kind,'menu');assert.equal(after.track,'th12_10');assert.equal(after.ready,false);
  assert.equal(after.paused,true);assert.equal(after.capturing,true);assert.equal(after.seeking,true);
  assert.equal(before.stage,3);assert.equal(before.track,'th12_02');
});

test('older Runtime builds still expose their JSON state',()=>{
  const state={scene:'loading',kind:'menu',stage:6,ready:false};
  assert.deepEqual(readSdlHostState({_th12_host_state:()=>1,UTF8ToString:()=>JSON.stringify(state)}),state);
});

test('malformed native ABI fails before reading out of bounds',()=>{
  const {core}=fixture();
  for(const p of [-8,1,1000]){core._th12_host_state_values=()=>p;assert.throws(()=>readSdlHostState(core),/address/);}
  core._th12_host_state_values=()=>8;
  new Float64Array(core.HEAPU8.buffer,8,27)[0]=2;
  assert.throws(()=>readSdlHostState(core),/ABI/);
});
