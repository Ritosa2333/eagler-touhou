import test from 'node:test';
import assert from 'node:assert/strict';
import {analyzeNormalizedWindow,compareStateEvidence} from './analyzer.mjs';
import {validateCapabilities,validateTickReceipt} from './contracts.mjs';

const evidence=(digest=1,missingGroups=[])=>({
  coverageVersion:'fixture/state/1',
  groups:[{id:'authoritative',digest}],
  missingGroups,
});

function record(value,{drawId=0,confidence='proven'}={}){
  return {
    ownerId:'fixture',objectId:7,generation:3,partId:0,drawId,
    identityConfidence:confidence,lifecycle:{continuous:true},
    coordinateSpace:'screen',fields:[{
      id:'position',value:[value,0],tolerance:.01,
      interpolationPolicy:'continuous',
    }],
  };
}

const frame=(tick,value,options={})=>({
  sessionEpoch:1,simulationTick:tick,referenceDrawSerial:tick,
  completeness:'complete',dropped:0,records:[record(value,options)],
});

test('release capability handshake requires title and native ABI identities',()=>{
  const base={driverApiVersion:1,game:'fixture',adapterVersion:'fixture/1',
    features:{freeze:true,resume:true,step:true,drawOnly:true,references:true,stateEvidence:true,timing:true,capture:true}};
  assert.throws(()=>validateCapabilities(base),/native ABI/);
  assert.equal(validateCapabilities({...base,nativeAbi:'fixture-native/1'}).nativeAbi,'fixture-native/1');
});

test('tick receipts cannot claim progress for blocked or finished states',()=>{
  assert.deepEqual(validateTickReceipt({status:'advanced',advancedTicks:1}),{status:'advanced',advancedTicks:1});
  assert.deepEqual(validateTickReceipt({status:'finished',advancedTicks:0}),{status:'finished',advancedTicks:0});
  assert.throws(()=>validateTickReceipt({status:'finished',advancedTicks:1}),/zero advanced ticks/);
});

test('state purity fails on any changed sample and stays unknown without complete coverage',()=>{
  assert.equal(compareStateEvidence(evidence(),[]).status,'unknown');
  assert.equal(compareStateEvidence(evidence(),[{coverageVersion:'fixture/state/1',groups:evidence().groups}]).status,'unknown');
  assert.equal(compareStateEvidence({groups:evidence().groups,missingGroups:[]},[{groups:evidence().groups,missingGroups:[]}]).status,'unknown');
  assert.equal(compareStateEvidence({coverageVersion:'fixture/state/1',groups:[{id:'authoritative',digest:1},{id:'authoritative',digest:1}],missingGroups:[]},[evidence()]).status,'unknown');
  assert.equal(compareStateEvidence(evidence(),[{...evidence(),groups:[...evidence().groups,{id:'unexpected',digest:1}]}]).status,'unknown');
  assert.equal(compareStateEvidence(evidence(1),[evidence(1),evidence(2),evidence(1)]).status,'fail');
  assert.equal(compareStateEvidence(evidence(1,['audio']),[evidence(1)]).status,'unknown');
  assert.equal(compareStateEvidence(evidence(1),[evidence(1),evidence(1)]).status,'pass');
});

test('normalized identity ignores draw order but repeated alpha must be idempotent',()=>{
  const previous=frame(10,0,{drawId:1}),current=frame(11,10,{drawId:99});
  const samples=[
    {...frame(11,0,{drawId:4}),alpha:0},
    {...frame(11,5,{drawId:8}),alpha:.5},
    {...frame(11,10,{drawId:12}),alpha:1},
    {...frame(11,5,{drawId:16}),alpha:.5},
  ];
  const clean=analyzeNormalizedWindow({previous,current,samples,stateBefore:evidence(),statesAfter:samples.map(()=>evidence())});
  assert.equal(clean.objects.length,1);
  assert.equal(clean.objects[0].status,'interpolated');
  samples[3]={...frame(11,6,{drawId:20}),alpha:.5};
  const unstable=analyzeNormalizedWindow({previous,current,samples,stateBefore:evidence(),statesAfter:samples.map(()=>evidence())});
  assert.equal(unstable.objects[0].status,'non-idempotent');
});

test('uncertain lifecycle identity never receives an interpolation verdict',()=>{
  const previous=frame(2,0),current=frame(3,10,{confidence:'uncertain'});
  const samples=[0,.5,1].map(alpha=>({...frame(3,alpha*10),alpha}));
  const report=analyzeNormalizedWindow({previous,current,samples,stateBefore:evidence(),statesAfter:samples.map(()=>evidence())});
  assert.equal(report.objects[0].status,'identity-uncertain');
});
