import test from 'node:test';import assert from 'node:assert/strict';
import {resolve} from 'node:path';import {pathToFileURL} from 'node:url';
import {recordsFromCapture,commonRoot} from './adapter.mjs';
import {compareCaptures} from './comparison.mjs';
const {compareTraces}=await import(pathToFileURL(resolve(commonRoot,'testkit/replay-verifier/compare.mjs')));
const receipt={ticks:2,complete:true,reason:'replay-finished',provider:'test',replaySha256:'1'.repeat(64),executableSha256:'2'.repeat(64),resourceSha256:'3'.repeat(64)};
const rows=()=>[1,2].map(f=>{const words=Array(36).fill(0);words[0]=1;words[1]=5;words[2]=f;words[3]=f;words[20]=3;words[31]=0x3f800000;return {words};});
test('equal complete original/candidate states pass',async()=>{const r=await compareTraces(recordsFromCapture(receipt,rows()),recordsFromCapture(receipt,rows()));assert.equal(r.status,'PASS');});
test('lost life is a first-tick divergence despite equal input',async()=>{const actual=rows();actual[0].words[20]--;const r=await compareTraces(recordsFromCapture(receipt,rows()),recordsFromCapture(receipt,actual));assert.equal(r.status,'DIVERGED');assert.equal(r.firstDifference.logicalTick,0);});
test('equal bounded prefixes cannot pass',async()=>{const bounded={...receipt,complete:false,reason:'bounded-prefix'};const r=await compareTraces(recordsFromCapture(bounded,rows()),recordsFromCapture(bounded,rows()));assert.equal(r.status,'INCOMPLETE');});
test('missing samples and initial samples are rejected',()=>{const r=rows();r[1].words[2]=3;assert.throws(()=>Array.from(recordsFromCapture(receipt,r)),/Missing/);assert.throws(()=>Array.from(recordsFromCapture({...receipt,ticks:1},r.slice(1))),/first/);});
test('different replay identities are incompatible',async()=>{const r=await compareTraces(recordsFromCapture(receipt,rows()),recordsFromCapture({...receipt,replaySha256:'4'.repeat(64)},rows()));assert.equal(r.status,'INCOMPATIBLE');});

test('mismatched internal resolution is rejected',()=>{const r=rows();r[0].words[31]=0x40000000;assert.throws(()=>recordsFromCapture(receipt,r),/viewport mismatch/);});

test('unequal matching incomplete captures remain incomplete without truncation',async()=>{
 const left={receipt:{...receipt,ticks:1,complete:false,reason:'timeout'},rows:rows().slice(0,1)};
 const right={receipt,rows:rows()};
 const r=await compareCaptures(left,right);
 assert.equal(r.status,'INCOMPLETE');assert.equal(r.comparedTicks,1);
 const changed=rows();changed[0].words[20]--;
 const d=await compareCaptures(left,{receipt,rows:changed});assert.equal(d.status,'DIVERGED');
});
test('different natural completion lengths still diverge',async()=>{
 const r=await compareCaptures({receipt:{...receipt,ticks:1},rows:rows().slice(0,1)},{receipt,rows:rows()});
 assert.equal(r.status,'DIVERGED');
});
test('runtime errors cannot become matching incomplete results',async()=>{
 await assert.rejects(()=>compareCaptures({receipt:{...receipt,errors:['stalled']},rows:rows()},{receipt,rows:rows()}),/exceptions/);
});

test('matching state captures cannot override a recorded score mismatch',()=>{
 assert.throws(()=>recordsFromCapture({...receipt,recordedScoreAudit:{status:'mismatch'}},rows()),/score differs/);
});

test('clock experiments cannot be accepted as ordinary parity evidence',()=>{
 assert.throws(()=>recordsFromCapture({...receipt,diagnosticOverrides:{shotOffset:-12}},rows()),/Diagnostic overrides/);
});

test('special-item count and overlay meter differences are detected before score changes',async()=>{
 for(const index of [33,35]){const actual=rows();actual[0].words[index]+=4;
 const r=await compareTraces(recordsFromCapture(receipt,rows()),recordsFromCapture(receipt,actual));
 assert.equal(r.status,'DIVERGED');assert.equal(r.firstDifference.logicalTick,0);}
});
