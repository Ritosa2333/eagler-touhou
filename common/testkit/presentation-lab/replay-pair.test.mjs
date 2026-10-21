import test from 'node:test';
import assert from 'node:assert/strict';
import {captureReplayPairRun,compareReplayPairRuns,compareReplayCadenceRuns,verifyReadOnlyCheckpoint} from './replay-pair.mjs';

function fixture({mutateDraw=false,dropTrace=false,events=true,epoch=1}={}) {
  let tick=0,sequence=0,draws=0;
  const rows={events:[],routes:[],uploads:[],errors:[]};
  const controller={freeze(){},step(){
    tick++;
    if(events){const serial=++sequence;rows.events.push({serial,logicalIndex:7,voiceKey:'epoch-'+epoch});
      rows.routes.push({serial:++sequence,eventSerial:serial,logicalIndex:7,direct:true,fallback:false,fallbackStarted:false,pending:false});}
  },preview(){draws++;if(mutateDraw)tick+=2;return {status:'drawn'};}};
  const readCheckpoint=()=>({schema:'eagler-presentation/checkpoint/1',
    state:{complete:true,groups:Array.from({length:17},(_,index)=>({id:index===13?'synchronous-audio-operations':index===14?'host-frame-cadence':`domain-${index}`,digest:tick*100+index}))},
    runtime:{status:Array.from({length:29},(_,index)=>index===0?2:tick),trace:Array.from({length:68},(_,index)=>index===0?tick:index)},
    replay:{available:true,status:[1,tick,tick,tick+10,tick,tick+10,2,0,0,0],trace:{schema:2,nextSequence:tick+1,firstSequence:1,count:tick,capacity:256,latest:Array.from({length:36},(_,index)=>index===0?tick:index===1?1:0)}},
    audio:{auditWords:Array.from({length:25},(_,index)=>index===0?1:tick),sfx:{available:true,enabled:true,sequence,
      counts:{events:rows.events.length,routes:rows.routes.length,uploads:0,errors:0},dropped:{events:0,routes:0,uploads:0,errors:0}}}});
  const readAudioRowsSince=since=>({available:!dropTrace,dropped:{events:0,routes:0,uploads:0,errors:0},
    ...Object.fromEntries(Object.entries(rows).map(([key,value])=>[key,key==='errors'?[]:value.filter(row=>row.serial>since)]))});
  return {epoch,controller,readCheckpoint,readAudioRowsSince,get draws(){return draws;}};
}

const identity={build:{game:'th08',wasm:'a'.repeat(64),sourceDigest:'b'.repeat(64)},data:{bytes:16,sha256:'c'.repeat(64)}};
const replayIdentity={bytes:24,sha256:'d'.repeat(64)};
const scope={game:'th08',variant:'normal'};
const cadenceDifferenceGroups=['host-frame-cadence','synchronous-audio-operations'];

async function capture(f,role,ticks=4) {
  return captureReplayPairRun({controller:f.controller,role,ticks,epoch:f.epoch,identity,replayIdentity,scope,
    readCheckpoint:f.readCheckpoint,readAudioRowsSince:f.readAudioRowsSince,
    yieldEvery:4,onBatch:async()=>{}});
}

function attachCadence(run,rateHz){
 const interval=1000/rateHz,logicalTicks=[];let time=0,last=null,debt=0,sequence=1,callbackIndex=0,ringCount=0,ringNext=0,simulationFrame=0;
 for(const sample of run.samples){
  const sequenceBefore=sequence,callbacks=[];
  while(sequence===sequenceBefore){
   const delta=last===null?0:interval;debt+=delta/1000;
   const tickDue=Math.floor((debt+1e-9)/(1/60))>=1;if(tickDue){debt=Math.max(0,debt-Math.floor((debt+1e-9)/(1/60))*(1/60));sequence++;simulationFrame++;}
   const high=rateHz>60&&callbackIndex>=6;
   const presented=rateHz===60?tickDue:(tickDue||high);
   const before={capacity:512,count:ringCount,next:ringNext};ringCount=Math.min(512,ringCount+1);ringNext=(ringNext+1)%512;
   const receipt={source:'GameHost::frame',timestampMs:time,deltaMs:delta,success:true,replaySequence:sequence,
    timing:{timestampMs:time,deltaMs:delta,simulationFrame,flags:{ready:true,customSpeed:false,tickDue,highRefresh:high,presented}},
    timingRing:{before,after:{capacity:512,count:ringCount,next:ringNext}}};
   callbacks.push(receipt);last=time;time+=interval;callbackIndex++;
   assert.ok(callbackIndex<1000,'synthetic cadence fixture should stay bounded');
  }
  const logicalTick={logicalTick:sample.logicalTick,sequenceBefore,sequenceAfter:sequence,callbacks};
  logicalTicks.push(logicalTick);sample.scheduler=structuredClone(logicalTick);
 }
 run.scheduler={schema:'eagler-presentation/native-frame-callback-cadence/1',source:'GameHost::frame',mode:'normal-fixed-60hz',rateHz,
  requestedTicks:run.samples.length,logicalTicks,summary:{callbackCount:callbackIndex,timingSampleCount:callbackIndex,
   tickDueCount:run.samples.length,presentedCount:logicalTicks.reduce((sum,row)=>sum+row.callbacks.filter(c=>c.timing.flags.presented).length,0),
   highRefreshPresented:logicalTicks.reduce((sum,row)=>sum+row.callbacks.filter(c=>c.timing.flags.highRefresh&&c.timing.flags.presented).length,0),
   betweenTickPresentations:logicalTicks.reduce((sum,row)=>sum+row.callbacks.filter(c=>c.timing.flags.highRefresh&&c.timing.flags.presented&&!c.timing.flags.tickDue).length,0)}};
 return run;
}

test('a fresh Replay pair compares all 17 authority groups, consumer rows, event indexes and route cardinality',async()=>{
  const baseline=await capture(fixture({epoch:1}),'baseline');
  const controlled=await capture(fixture({epoch:2}),'controlled-extra-draw');
  const result=compareReplayPairRuns({baseline,controlledExtraDraw:controlled,requiredTicks:4});
  assert.equal(baseline.readVerification.authorityGroups,17);
  assert.equal(baseline.audioEvents,4);
  assert.equal(controlled.audioRoutes,4);
  assert.equal(result.status,'passed',result.issues.join('; '));
  assert.deepEqual(result.scope,scope);
  assert.ok(result.checks.every(check=>check.evidence.game==='th08'&&check.evidence.variant==='normal'&&
    check.evidence.adaptationIds.length===1&&typeof check.complete==='boolean'));
  assert.equal(result.checks.find(check=>check.id==='DISPLAY-03').complete,true);
  assert.equal(result.checks.find(check=>check.id==='REPLAY-07').complete,true);
  assert.equal(result.checks.find(check=>check.id==='DISPLAY-02').status,'incomplete');
  assert.equal(result.checks.find(check=>check.id==='DISPLAY-07').status,'incomplete');
  assert.equal(result.claims.physicalRefreshVerified,false);
  assert.equal(result.claims.rollbackAudioVerified,false);
});

test('a draw-only mutation fails at the first changed authority checkpoint',async()=>{
  const baseline=await capture(fixture({epoch:1}),'baseline');
  const controlled=await capture(fixture({epoch:2,mutateDraw:true}),'controlled-extra-draw');
  assert.equal(controlled.status,'failed');
  assert.match(controlled.issues.join('; '),/draw changed authoritative/);
  assert.equal(compareReplayPairRuns({baseline,controlledExtraDraw:controlled,requiredTicks:4}).status,'failed');
});

test('missing event trace and missing SFX events remain incomplete',async()=>{
  const missingRows=await capture(fixture({epoch:1,dropTrace:true}),'baseline');
  assert.equal(missingRows.status,'incomplete');
  const silent=await capture(fixture({epoch:2,events:false}),'controlled-extra-draw');
  assert.equal(silent.status,'incomplete');
  const baseline=await capture(fixture({epoch:3}),'baseline');
  assert.equal(compareReplayPairRuns({baseline,controlledExtraDraw:silent,requiredTicks:4}).status,'incomplete');
});

test('pair identity and Runtime epochs cannot be borrowed',async()=>{
  const baseline=await capture(fixture({epoch:1}),'baseline');
  const sameEpoch=await capture(fixture({epoch:1}),'controlled-extra-draw');
  assert.equal(compareReplayPairRuns({baseline,controlledExtraDraw:sameEpoch,requiredTicks:4}).status,'failed');
  const other=await capture(fixture({epoch:2}),'controlled-extra-draw');
  other.replayIdentity.sha256='e'.repeat(64);
  const mismatch=compareReplayPairRuns({baseline,controlledExtraDraw:other,requiredTicks:4});
  assert.equal(mismatch.status,'failed');
  assert.match(mismatch.issues.join('; '),/Replay bytes differ/);
  assert.ok(mismatch.checks.every(check=>check.evidence.adaptationIds?.[0]===check.id));
});

test('VERIFY-07 repeats the actual seventeen-group checkpoint without changing the trace',async()=>{
  const stable=fixture();
  const proof=await verifyReadOnlyCheckpoint({readCheckpoint:stable.readCheckpoint,expectedAuthorityGroups:17});
  assert.equal(proof.status,'passed');
  assert.equal(proof.authorityGroups,17);
  let calls=0;
  const mutating=fixture().readCheckpoint;
  const bad=await verifyReadOnlyCheckpoint({expectedAuthorityGroups:17,readCheckpoint:()=>{
    const value=mutating();value.runtime.trace[0]=calls++;return value;
  }});
  assert.equal(bad.status,'failed');
  assert.ok(bad.issues.some(issue=>issue.includes('Repeated observation reads changed')));
});

test('missing pair identity and scope retain explicit blocked checks instead of a bare status',async()=>{
  const baseline=await capture(fixture({epoch:1}),'baseline');
  const controlled=await capture(fixture({epoch:2}),'controlled-extra-draw');
  const missing=compareReplayPairRuns({baseline,controlledExtraDraw:controlled,requiredTicks:4,scope:{game:'th08',variant:'mp'}});
  assert.equal(missing.status,'failed');
  assert.match(missing.issues.join('; '),/scopes differ/);
  assert.equal(missing.checks.length,7);
  assert.ok(missing.checks.every(check=>check.id&&check.status&&check.complete===false&&
    check.evidence.adaptationIds?.[0]===check.id));
  await assert.rejects(captureReplayPairRun({controller:fixture().controller,role:'baseline',ticks:1,epoch:1,identity,
    replayIdentity,readCheckpoint:()=>({}),readAudioRowsSince:()=>({}),scope:null}),/explicit game and variant scope/);
});

test('a native event with an unrelated route is a real contradiction',async()=>{
  const f=fixture({epoch:1});
  const readAudioRowsSince=f.readAudioRowsSince;
  f.readAudioRowsSince=sequence=>{
    const rows=readAudioRowsSince(sequence);
    if(rows.routes.length)rows.routes[0].logicalIndex++;
    return rows;
  };
  const run=await capture(f,'baseline');
  assert.equal(run.status,'failed');
  assert.match(run.issues.join('; '),/different logical indexes/);
});

test('production callback cadence compares one Replay window at 60, 120 and 144 Hz',async()=>{
 const runs=[];
 for(const [index,hz] of [60,120,144].entries()){
  const role=hz===60?'baseline':'controlled-extra-draw';
  runs.push(attachCadence(await capture(fixture({epoch:index+10}),role),hz));
 }
 const result=compareReplayCadenceRuns({runs,requiredTicks:4,scope,cadenceDifferenceGroups});
 assert.equal(result.status,'passed',result.issues.join('; '));
 assert.deepEqual(result.ratesHz,[60,120,144]);
 assert.equal(result.runs.length,3);
 assert.equal(result.checks.find(row=>row.id==='DISPLAY-01').complete,true);
 assert.equal(result.checks.find(row=>row.id==='REPLAY-07').complete,true);
 assert.equal(result.checks.find(row=>row.id==='REPLAY-07').machineComplete,true);
 assert.equal(result.checks.find(row=>row.id==='AUDIO-06').complete,true);
 assert.equal(result.checks.find(row=>row.id==='AUDIO-06').machineComplete,true);
 assert.equal(result.claims.physicalRefreshVerified,false);
 assert.equal(result.claims.replayNaturalTerminalVerified,false);
});

test('cadence omissions remain incomplete and altered consumer/timing evidence fails',async()=>{
 const runs=[];
 for(const [index,hz] of [60,120,144].entries())
  runs.push(attachCadence(await capture(fixture({epoch:index+20}),hz===60?'baseline':'controlled-extra-draw'),hz));
 assert.equal(compareReplayCadenceRuns({runs:runs.slice(0,2),requiredTicks:4,scope,cadenceDifferenceGroups}).status,'incomplete');
 const altered=structuredClone(runs);altered[1].samples[1].afterStep.runtime.trace[0]+=1;
 const divergent=compareReplayCadenceRuns({runs:altered,requiredTicks:4,scope,cadenceDifferenceGroups});
 assert.equal(divergent.status,'failed');assert.match(divergent.issues.join('; '),/diverged/);
 const badTiming=structuredClone(runs);badTiming[2].scheduler.logicalTicks[0].callbacks[0].timing.flags.tickDue=true;
 const timing=compareReplayCadenceRuns({runs:badTiming,requiredTicks:4,scope,cadenceDifferenceGroups});
 assert.equal(timing.status,'failed');assert.ok(timing.issues.some(issue=>issue.includes('tickDue flag')));
 const noPresent=structuredClone(runs);const tickCallback=noPresent[1].scheduler.logicalTicks.flatMap(row=>row.callbacks)
  .find(receipt=>receipt.timing.flags.tickDue);
 tickCallback.timing.flags.presented=false;
 const missingTickPresent=compareReplayCadenceRuns({runs:noPresent,requiredTicks:4,scope,cadenceDifferenceGroups});
 assert.equal(missingTickPresent.status,'failed');
 assert.ok(missingTickPresent.issues.some(issue=>issue.includes('did not present its fixed logical tick')));
 const borrowed=structuredClone(runs);borrowed[2].replayIdentity.sha256='f'.repeat(64);
 assert.equal(compareReplayCadenceRuns({runs:borrowed,requiredTicks:4,scope,cadenceDifferenceGroups}).status,'failed');
});

test('a high-refresh callback may coincide with a logical tick while other callbacks present between ticks',async()=>{
 const run=attachCadence(await capture(fixture({epoch:50}),'controlled-extra-draw'),120);
 const callbacks=run.scheduler.logicalTicks.flatMap(row=>row.callbacks);
 assert.ok(callbacks.some(receipt=>receipt.timing.flags.highRefresh&&receipt.timing.flags.tickDue&&receipt.timing.flags.presented));
 assert.ok(callbacks.some(receipt=>receipt.timing.flags.highRefresh&&!receipt.timing.flags.tickDue&&receipt.timing.flags.presented));
 const runs=[];
 for(const [index,hz] of [60,120,144].entries())
  runs.push(attachCadence(await capture(fixture({epoch:51+index}),hz===60?'baseline':'controlled-extra-draw'),hz));
 const result=compareReplayCadenceRuns({runs,requiredTicks:4,scope,cadenceDifferenceGroups});
 assert.equal(result.status,'passed',result.issues.join('; '));
 assert.equal(result.issues.some(issue=>issue.includes('not between logical ticks')),false);
});
