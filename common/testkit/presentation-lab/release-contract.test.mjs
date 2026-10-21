import test from 'node:test';
import assert from 'node:assert/strict';
import {analyzeNormalizedWindow,compareStateEvidence,presentationWindowHealth} from './analyzer.mjs';
import {validateCapabilities,validateTickReceipt} from './contracts.mjs';
import {defineStateAuthority,REQUIRED_STATE_AUTHORITY_DOMAINS,STATE_AUTHORITY_SCHEMA,validateStateAuthority} from './state-authority.mjs';

const authority=(groups=['authoritative'])=>({schema:STATE_AUTHORITY_SCHEMA,domains:REQUIRED_STATE_AUTHORITY_DOMAINS.map(id=>({id,groups,sourceFields:['fixture.state.digest']}))});
const evidence=(digest=1,missingGroups=[])=>({
  coverageVersion:'fixture/state/1',
  groups:[{id:'authoritative',digest,includedFields:['fixture.state.digest']}],
  missingGroups,
  authority:authority(),
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
  const empty={coverageVersion:'fixture/state/1',groups:[],missingGroups:[]};
  assert.equal(compareStateEvidence(empty,[empty]).status,'unknown');
  assert.equal(compareStateEvidence(evidence(),[empty]).status,'unknown');
});

test('state authority requires every shared domain and a source map for every digest group',()=>{
  const complete=evidence();
  assert.equal(compareStateEvidence(complete,[complete]).status,'pass');
  const missingDomain={...complete,authority:{...complete.authority,domains:complete.authority.domains.filter(domain=>domain.id!=='input-transients')}};
  assert.equal(compareStateEvidence(missingDomain,[missingDomain]).status,'unknown');
  const emptySource={...complete,authority:{...complete.authority,domains:complete.authority.domains.map(domain=>domain.id==='practice-state'?{...domain,sourceFields:[]}:domain)}};
  assert.equal(compareStateEvidence(emptySource,[emptySource]).status,'unknown');
  const unmapped={...complete,groups:[...complete.groups,{id:'unmapped',digest:1}]};
  assert.equal(compareStateEvidence(unmapped,[unmapped]).status,'unknown');
  const missingReference={...complete,authority:authority(['absent'])};
  assert.equal(compareStateEvidence(missingReference,[missingReference]).status,'unknown');
  const missingIncludedFields={...complete,groups:[{id:'authoritative',digest:1}]};
  assert.equal(compareStateEvidence(missingIncludedFields,[missingIncludedFields]).status,'unknown');
  const unmappedField={...complete,authority:{...complete.authority,domains:complete.authority.domains.map(domain=>domain.id==='simulation-world'?{...domain,sourceFields:['fixture.state.digest','fixture.state.unmapped']}:domain)}};
  assert.equal(compareStateEvidence(unmappedField,[unmappedField]).status,'unknown');
  const uncoveredIncludedField={...complete,groups:[{id:'authoritative',digest:1,includedFields:['fixture.state.digest','fixture.state.unmapped']}]};
  assert.equal(compareStateEvidence(uncoveredIncludedField,[uncoveredIncludedField]).status,'unknown');
  const fieldWithoutDomain={...complete,authority:{...complete.authority,domains:complete.authority.domains.map(domain=>domain.id==='simulation-world'?{...domain,sourceFields:['fixture.state.digest']}:domain)}};
  fieldWithoutDomain.groups=[{id:'authoritative',digest:1,includedFields:['fixture.state.digest','fixture.state.unmapped']}];
  assert.equal(compareStateEvidence(fieldWithoutDomain,[fieldWithoutDomain]).status,'unknown');
  const drift={...complete,authority:{...complete.authority,domains:complete.authority.domains.map(domain=>domain.id==='replay-state'?{...domain,sourceFields:['other.file.field']}:domain)}};
  assert.equal(compareStateEvidence(complete,[drift]).status,'unknown');
});

test('shared authority builder keeps missing title mappings incomplete',()=>{
  const mappedEvidence={groups:[{id:'world',digest:1,includedFields:['World::tick']}],authority:null};
  const complete=defineStateAuthority({groupFields:{world:['World::tick']},domains:Object.fromEntries(REQUIRED_STATE_AUTHORITY_DOMAINS.map(id=>[id,['world']]))});
  assert.equal(validateStateAuthority({...mappedEvidence,authority:complete}).complete,true);
  const missing=defineStateAuthority({groupFields:{world:['World::tick']},domains:{'simulation-world':['world']}});
  assert.equal(validateStateAuthority({...mappedEvidence,authority:missing}).complete,false);
  const typo=defineStateAuthority({groupFields:{world:['World::tick']},domains:{...Object.fromEntries(REQUIRED_STATE_AUTHORITY_DOMAINS.map(id=>[id,['world']])),unknown:['world']}});
  assert.equal(validateStateAuthority({...mappedEvidence,authority:typo}).complete,false);
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
  assert.equal(clean.valid,true);
  assert.equal(clean.validityStatus,'valid');
  const withoutOptionalSerial=analyzeNormalizedWindow({
    previous:(({referenceDrawSerial,...frame})=>frame)(previous),
    current:(({referenceDrawSerial,...frame})=>frame)(current),
    samples:samples.map(sample=>(({referenceDrawSerial,...frame})=>frame)(sample)),
    stateBefore:evidence(),statesAfter:samples.map(()=>evidence()),
  });
  assert.equal(withoutOptionalSerial.valid,true);
  samples[3]={...frame(11,6,{drawId:20}),alpha:.5};
  const unstable=analyzeNormalizedWindow({previous,current,samples,stateBefore:evidence(),statesAfter:samples.map(()=>evidence())});
  assert.equal(unstable.objects[0].status,'non-idempotent');
});

test('normalized window cannot validate missing, incomplete, or misaligned frame identity',()=>{
  const previous=frame(20,0),current=frame(21,8);
  const samples=[0,.5,.5,1].map(alpha=>({...frame(21,alpha*8),alpha}));
  const analyze=(before=previous,after=current,observed=samples)=>analyzeNormalizedWindow({
    previous:before,current:after,samples:observed,stateBefore:evidence(),statesAfter:observed.map(()=>evidence()),
  });
  const missingTick=samples.map(sample=>({...sample}));delete missingTick[1].simulationTick;
  const missingEpoch=samples.map(sample=>({...sample}));delete missingEpoch[2].sessionEpoch;
  const missingCompleteness=samples.map(sample=>({...sample}));delete missingCompleteness[0].completeness;
  const missingRecords=samples.map(sample=>({...sample}));delete missingRecords[1].records;
  const missingDropCount=samples.map(sample=>({...sample}));delete missingDropCount[2].dropped;
  const incomplete=samples.map(sample=>({...sample}));incomplete[3].completeness='incomplete';
  const wrongEpoch=samples.map(sample=>({...sample}));wrongEpoch[2].sessionEpoch=2;
  const wrongTick=samples.map(sample=>({...sample}));wrongTick[1].simulationTick=20;
  for(const report of [analyze(previous,current,missingTick),analyze(previous,current,missingEpoch),analyze(previous,current,missingCompleteness),analyze(previous,current,missingRecords),analyze(previous,current,missingDropCount),analyze(previous,current,incomplete)]){
    assert.equal(report.valid,false);
    assert.equal(report.validityStatus,'unknown');
  }
  for(const report of [analyze(previous,current,wrongEpoch),analyze(previous,current,wrongTick),analyze(previous,frame(22,8),samples)]){
    assert.equal(report.valid,false);
    assert.equal(report.validityStatus,'invalid');
  }
});

test('previous-only objects remain visible as disappeared and unobserved diagnostics',()=>{
  const previous=frame(5,2),current={...frame(6,2),records:[]};
  const samples=[0,.5,.5,1].map(alpha=>({...frame(6,2),records:[],alpha}));
  const report=analyzeNormalizedWindow({previous,current,samples,stateBefore:evidence(),statesAfter:samples.map(()=>evidence())});
  assert.equal(report.valid,true);
  assert.equal(report.objects.length,1);
  assert.equal(report.objects[0].status,'disappeared');
  assert.equal(report.objects[0].diagnostic,'present-in-previous-absent-in-current');
  assert.equal(report.objects[0].fields[0].status,'unobserved');
  assert.equal(report.counts.disappeared,1);
});

test('workbench health never reports full success for invalid or unknown windows',()=>{
  assert.equal(presentationWindowHealth({valid:true,validityStatus:'valid',purityStatus:'pass'}).status,'ready');
  assert.equal(presentationWindowHealth({valid:false,validityStatus:'invalid',purityStatus:'pass'}).status,'error');
  assert.equal(presentationWindowHealth({valid:false,validityStatus:'unknown',purityStatus:'pass'}).status,'warn');
  assert.equal(presentationWindowHealth({valid:true,purityStatus:'pass'}).status,'warn');
  assert.equal(presentationWindowHealth({purityStatus:'unknown'}).status,'warn');
  assert.equal(presentationWindowHealth({valid:true,validityStatus:'valid',purityStatus:'unknown'}).status,'warn');
});

test('uncertain lifecycle identity never receives an interpolation verdict',()=>{
  const previous=frame(2,0),current=frame(3,10,{confidence:'uncertain'});
  const samples=[0,.5,1].map(alpha=>({...frame(3,alpha*10),alpha}));
  const report=analyzeNormalizedWindow({previous,current,samples,stateBefore:evidence(),statesAfter:samples.map(()=>evidence())});
  assert.equal(report.objects[0].status,'identity-uncertain');
});
