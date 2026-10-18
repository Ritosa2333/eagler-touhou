// ABI tests call the real browser helper. The small export fixtures supply
// status bytes and acknowledgements; they do not simulate native pacing/Draw.
import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {
 configureNetworkGapTail,readNetworkGapTailStatus,networkGapTailStatusSchema,
 configureNetworkTailPresentation,readNetworkTailPresentationStatus,networkTailPresentationStatusSchema,
} from './network-gap-tail.mjs';

const APP=0x1234;
const apis=[
 {kind:'pacing',read:readNetworkGapTailStatus,configure:configureNetworkGapTail,
  status:'multiplayer_network_gap_tail_status',setter:'multiplayer_network_gap_tail_set_enabled',words:6},
 {kind:'presentation',read:readNetworkTailPresentationStatus,configure:configureNetworkTailPresentation,
  status:'multiplayer_network_tail_presentation_status',setter:'multiplayer_network_tail_presentation_set_enabled',words:3},
];
function fixture(){
 const state={pacing:true,presentation:false,locked:false};
 const calls={pacing:{get:0,set:0},presentation:{get:0,set:0}};
 const pointers={pacing:64,presentation:128};
 const core={memory:new WebAssembly.Memory({initial:1,maximum:4})};
 const values=kind=>kind==='pacing'
  ?[1,Number(state.pacing),Number(state.locked),1.5,1.01,1.5]
  :[1,Number(state.presentation),Number(state.locked)];
 for(const api of apis){
  core[api.status]=app=>{
   assert.equal(app,APP);++calls[api.kind].get;
   new Float64Array(core.memory.buffer,pointers[api.kind],api.words).set(values(api.kind));
   return pointers[api.kind];
  };
  core[api.setter]=(app,enabled)=>{
   assert.equal(app,APP);++calls[api.kind].set;
   if(state.locked||!Number.isInteger(enabled)||enabled<0||enabled>1)return 0;
   state[api.kind]=enabled===1;return 1;
  };
 }
 return {core,state,calls,pointers,values};
}
const cases=[];
const test=(name,run)=>cases.push({name,run});

test('four independent choices keep the other control and ABI fields intact',()=>{
 assert.deepEqual(networkGapTailStatusSchema,['schema','enabled','locked','gapScale','phaseScale','effectiveScale']);
 assert.deepEqual(networkTailPresentationStatusSchema,['schema','enabled','locked']);
 for(const pacing of [false,true])for(const presentation of [false,true]){
  const f=fixture();
  assert.deepEqual(configureNetworkGapTail(f.core,APP,pacing),[1,Number(pacing),0,1.5,1.01,1.5]);
  assert.equal(f.state.presentation,false);
  assert.equal(f.calls.presentation.set,0);
  const before=readNetworkGapTailStatus(f.core,APP);
  assert.deepEqual(configureNetworkTailPresentation(f.core,APP,presentation),[1,Number(presentation),0]);
  assert.deepEqual(readNetworkGapTailStatus(f.core,APP),before);
  assert.deepEqual(f.state,{pacing,presentation,locked:false});
  assert.equal(f.calls.pacing.set,1);
  assert.equal(f.calls.presentation.set,1);
  // Reverse order too: an explicit pacing change cannot overwrite extra Draw.
  configureNetworkGapTail(f.core,APP,!pacing);
  assert.equal(f.state.presentation,presentation);
  assert.equal(f.calls.presentation.set,1);
 }
});

test('default null/undefined is read-only even after lock; returned values are copies',()=>{
 const f=fixture();f.state.locked=true;
 for(const api of apis){
  const first=api.configure(f.core,APP,null);
  assert.deepEqual(first,f.values(api.kind));
  assert.deepEqual(api.configure(f.core,APP,undefined),first);
  assert.equal(f.calls[api.kind].set,0);
  f.state[api.kind]=!f.state[api.kind];
  const next=api.read(f.core,APP);
  assert.notEqual(next[1],first[1]);
  assert.equal(first[1],api.kind==='pacing'?1:0);
 }
});

test('old runtimes allow defaults but reject explicit missing or partial capability',()=>{
 const old={get memory(){throw Error('Absent exports must not read memory');}};
 for(const api of apis){
  assert.equal(api.read(old,APP),null);
  assert.equal(api.configure(old,APP,null),null);
  assert.equal(api.configure(old,APP,undefined),null);
  for(const enabled of [false,true])assert.throws(()=>api.configure(old,APP,enabled),/controls required/);
  for(const missing of [api.status,api.setter]){
   const f=fixture();delete f.core[missing];
   assert.throws(()=>api.configure(f.core,APP,true),/controls required/);
   assert.equal(f.calls[api.kind].set,0);
  }
 }
 // Frozen i exposes the old pacing control but no independent Draw control.
 const legacy=fixture();
 delete legacy.core[apis[1].status];delete legacy.core[apis[1].setter];
 assert.deepEqual(configureNetworkGapTail(legacy.core,APP,false),[1,0,0,1.5,1.01,1.5]);
 assert.equal(configureNetworkTailPresentation(legacy.core,APP,null),null);
 for(const enabled of [false,true])
  assert.throws(()=>configureNetworkTailPresentation(legacy.core,APP,enabled),/Independent.*controls required/);
 assert.equal(legacy.calls.pacing.set,1);
});

test('explicit nonboolean requests are rejected before any native call',()=>{
 for(const api of apis)for(const invalid of [0,1,'on',{},NaN]){
  const f=fixture();
  assert.throws(()=>api.configure(f.core,APP,invalid),/option must be boolean/);
  assert.deepEqual(f.calls[api.kind],{get:0,set:0});
 }
});

test('pointer bounds, alignment and exact end-of-memory range',()=>{
 for(const api of apis){
  const bytes=api.words*8;
  for(const pointer of [0,-8,1,64.5,NaN,Infinity,65536,65536-bytes+8]){
   const f=fixture();f.core[api.status]=()=>pointer;
   assert.throws(()=>api.read(f.core,APP),/status pointer/);
  }
  const f=fixture();f.pointers[api.kind]=f.core.memory.buffer.byteLength-bytes;
  assert.deepEqual(api.read(f.core,APP),f.values(api.kind));
 }
});

test('invalid schema, flags and pacing numbers are rejected rather than coerced',()=>{
 for(const api of apis){
  const changes=[[0,0],[0,2],[0,NaN],[1,2],[1,NaN],[2,-1],[2,Infinity]];
  if(api.kind==='pacing')changes.push([3,0],[4,-1],[5,NaN]);
  for(const [index,value] of changes){
   const f=fixture(),raw=f.values(api.kind);raw[index]=value;
   f.core[api.status]=()=>{
    new Float64Array(f.core.memory.buffer,f.pointers[api.kind],api.words).set(raw);
    return f.pointers[api.kind];
   };
   assert.throws(()=>api.read(f.core,APP),/Invalid network .* status/);
  }
 }
});

test('locked choices, native rejection and false acknowledgements never report setup success',()=>{
 for(const api of apis){
  const locked=fixture();locked.state.locked=true;
  for(const enabled of [false,true])
   assert.throws(()=>api.configure(locked.core,APP,enabled),/before the first real input/);
  assert.equal(locked.calls[api.kind].set,0);
  const rejected=fixture();
  rejected.core[api.setter]=()=>0;
  assert.throws(()=>api.configure(rejected.core,APP,true),/option rejected/);
  assert.equal(rejected.calls[api.kind].get,1);
  const unchanged=fixture();
  unchanged.core[api.setter]=()=>1;
  assert.throws(()=>api.configure(unchanged.core,APP,!unchanged.state[api.kind]),/setup did not take effect/);
  const prematurelyLocked=fixture(),nativeSet=prematurelyLocked.core[api.setter];
  prematurelyLocked.core[api.setter]=(app,enabled)=>{
   const result=nativeSet(app,enabled);prematurelyLocked.state.locked=true;return result;
  };
  assert.throws(()=>api.configure(prematurelyLocked.core,APP,true),/setup did not take effect/);
 }
});

test('status getter memory growth uses the current buffer before and after a setter',()=>{
 for(const api of apis)for(const configure of [false,true]){
  const f=fixture(),oldBuffer=f.core.memory.buffer,nativeGet=f.core[api.status];
  f.core[api.status]=app=>{
   const previousLength=f.core.memory.buffer.byteLength;
   f.core.memory.grow(1);f.pointers[api.kind]=previousLength+64;
   return nativeGet(app);
  };
  const enabled=!f.state[api.kind];
  const result=configure?api.configure(f.core,APP,enabled):api.read(f.core,APP);
  assert.deepEqual(result,f.values(api.kind));
  assert.equal(oldBuffer.byteLength,0);
  assert.equal(f.core.memory.buffer.byteLength,65536*(configure?3:2));
  assert.deepEqual(f.calls[api.kind],{get:configure?2:1,set:configure?1:0});
 }
});

const hash=path=>createHash('sha256').update(readFileSync(new URL(path,import.meta.url))).digest('hex');
const inventory=()=>Object.fromEntries(['./network-gap-tail.mjs','./check-network-tail-options.mjs'].map(path=>[path,hash(path)]));
const sourceFiles=inventory(),results=[];
for(const {name,run} of cases){
 try{run();results.push({name,passed:true});}
 catch(error){results.push({name,passed:false,error:String(error?.stack??error).slice(0,1800)});}
}
const sourceUnchanged=JSON.stringify(inventory())===JSON.stringify(sourceFiles);
const passed=sourceUnchanged&&results.every(result=>result.passed);
console.log(JSON.stringify({schema:'th10-network-tail-options-test/1',passed,
 scope:'real JS helper ABI and option boundaries; no native execution or browser/transport coverage',
 sourceFiles,sourceUnchanged,cases:results.length,results},null,2));
if(!passed)process.exitCode=1;
