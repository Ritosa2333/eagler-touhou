import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {mkdtemp,readFile,rm,writeFile} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {basename,join,resolve} from 'node:path';
import test from 'node:test';
import {captureReplayPairRun,compareReplayCadenceRuns,compareReplayPairRuns} from '../../../common/testkit/presentation-lab/replay-pair.mjs';
import {identityDigest} from '../../../tools/testing/dependency-snapshot.mjs';
import {assessCompleteEvidence} from '../complete-evidence.mjs';
import {resolveDiagnosticProfile} from './diagnostics.mjs';
import {assessPresentationEvidence} from './presentation-evidence.mjs';
import {buildPresentationPairedReport} from '../../../tools/ports/presentation-paired-report.mjs';

const hash=value=>createHash('sha256').update(value).digest('hex');
const scope={game:'th08',variant:'normal'};
const profile=resolveDiagnosticProfile(scope.game,scope.variant);
const capability=profile.presentation.pairedReplay;
const machineIds=['DISPLAY-01','DISPLAY-03','DISPLAY-07','REPLAY-07','AUDIO-06','VERIFY-07'];
const humanIds=['DISPLAY-02','DISPLAY-04','DISPLAY-05','DISPLAY-06','DISPLAY-08'];

function replayFixture(epoch,role){
  let tick=0,sequence=0;
  const rows={events:[],routes:[],uploads:[],errors:[]};
  const controller={freeze(){},step(){
    tick++;
    const eventSerial=++sequence;
    rows.events.push({serial:eventSerial,logicalIndex:tick,voiceKey:`epoch-${epoch}`});
    rows.routes.push({serial:++sequence,eventSerial,logicalIndex:tick,direct:true,fallback:false,fallbackStarted:false,pending:false});
  },preview(){return{status:'drawn'};}};
  const readCheckpoint=()=>({schema:'eagler-presentation/checkpoint/1',
    state:{complete:true,groups:Array.from({length:capability.expectedAuthorityGroups},(_,index)=>({
      id:index===13?'synchronous-audio-operations':index===14?'host-frame-cadence':`domain-${index}`,digest:tick*100+index}))},
    runtime:{status:Array.from({length:29},(_,index)=>index===0?2:tick),trace:Array.from({length:68},(_,index)=>index===0?tick:index)},
    replay:{available:true,status:[1,tick,tick,tick+10,tick,tick+10,2,0,0,0],trace:{schema:2,nextSequence:tick+1,
      firstSequence:1,count:tick,capacity:256,latest:Array.from({length:36},(_,index)=>index===0?tick:index===1?1:0)}},
    audio:{auditWords:Array.from({length:25},(_,index)=>index===0?1:tick),sfx:{available:true,enabled:true,sequence,
      counts:{events:rows.events.length,routes:rows.routes.length,uploads:0,errors:0},dropped:{events:0,routes:0,uploads:0,errors:0}}}});
  const readAudioRowsSince=since=>({available:true,dropped:{events:0,routes:0,uploads:0,errors:0},
    ...Object.fromEntries(Object.entries(rows).map(([key,value])=>[key,key==='errors'?[]:value.filter(row=>row.serial>since)]))});
  return{controller,readCheckpoint,readAudioRowsSince,epoch,role};
}

function attachCadence(run,rateHz){
  const interval=1000/rateHz,logicalTicks=[];let time=0,last=null,debt=0,sequence=1,
    callbackIndex=0,ringCount=0,ringNext=0,simulationFrame=0;
  for(const sample of run.samples){
    const sequenceBefore=sequence,callbacks=[];
    while(sequence===sequenceBefore){
      const delta=last===null?0:interval;debt+=delta/1000;
      const tickDue=Math.floor((debt+1e-9)/(1/60))>=1;
      if(tickDue){debt=Math.max(0,debt-Math.floor((debt+1e-9)/(1/60))*(1/60));sequence++;simulationFrame++;}
      const high=rateHz>60&&callbackIndex>=6&&!tickDue,presented=tickDue||high;
      const before={capacity:512,count:ringCount,next:ringNext};ringCount=Math.min(512,ringCount+1);ringNext=(ringNext+1)%512;
      callbacks.push({source:'GameHost::frame',timestampMs:time,deltaMs:delta,success:true,replaySequence:sequence,
        timing:{timestampMs:time,deltaMs:delta,simulationFrame,flags:{ready:true,customSpeed:false,tickDue,highRefresh:high,presented}},
        timingRing:{before,after:{capacity:512,count:ringCount,next:ringNext}}});
      last=time;time+=interval;callbackIndex++;
      assert.ok(callbackIndex<10000,'Synthetic cadence schedule must remain bounded');
    }
    const receipt={logicalTick:sample.logicalTick,sequenceBefore,sequenceAfter:sequence,callbacks};
    logicalTicks.push(receipt);sample.scheduler=structuredClone(receipt);
  }
  run.scheduler={schema:'eagler-presentation/native-frame-callback-cadence/1',source:'GameHost::frame',
    mode:'normal-fixed-60hz',rateHz,requestedTicks:run.samples.length,logicalTicks,
    summary:{callbackCount:callbackIndex,timingSampleCount:callbackIndex,tickDueCount:run.samples.length,
      presentedCount:logicalTicks.reduce((sum,row)=>sum+row.callbacks.filter(callback=>callback.timing.flags.presented).length,0),
      highRefreshPresented:logicalTicks.reduce((sum,row)=>sum+row.callbacks.filter(callback=>callback.timing.flags.highRefresh&&
        callback.timing.flags.presented).length,0),
      betweenTickPresentations:logicalTicks.reduce((sum,row)=>sum+row.callbacks.filter(callback=>callback.timing.flags.highRefresh&&
        callback.timing.flags.presented&&!callback.timing.flags.tickDue).length,0)}};
  return run;
}

function checkRow(id,status,complete,evidence={}){
  return{id,status,complete,machineComplete:complete,evidence:{...scope,adaptationIds:[id],...evidence}};
}

async function identity(path){
  const bytes=await readFile(path);return{path:resolve(path),bytes:bytes.length,sha256:hash(bytes)};
}

async function writeJson(path,value){
  await writeFile(path,JSON.stringify(value,null,2)+'\n');return identity(path);
}

async function evidenceFixture(t){
  const root=await mkdtemp(join(tmpdir(),'paired-presentation-evidence-'));t.after(()=>rm(root,{recursive:true,force:true}));
  const output=join(root,'evidence');await writeFile(join(root,'placeholder'),'');
  const inputPath=join(root,capability.inputFile),inputBytes=Buffer.from('synthetic selected demo replay bytes');
  await writeFile(inputPath,inputBytes);
  const input={...capability,...await identity(inputPath)};
  const data={path:join(root,'th08.dat'),bytes:23,sha256:hash('synthetic selected DATA')};
  const build={game:scope.game,wasm:hash('diagnostic wasm'),sourceDigest:hash('diagnostic source'),toolDigest:hash('diagnostic tools'),
    profile:'presentation-lab',diagnostic:true,instrumented:true,
    productionBuild:{available:true,diagnostic:false,sourceMatchesCurrent:true,binaryMatchesDescriptor:true,wasmSha256:hash('production wasm')}};
  const identityValue={build,data:{bytes:data.bytes,sha256:data.sha256}};
  const runs=[];
  for(let index=0;index<capability.cadencesHz.length;index++){
    const rate=capability.cadencesHz[index],source=replayFixture(index+1,index===0?'baseline':'controlled-extra-draw');
    const run=await captureReplayPairRun({controller:source.controller,role:source.role,ticks:capability.ticks,
      epoch:source.epoch,identity:identityValue,replayIdentity:{bytes:input.bytes,sha256:input.sha256},scope,
      readCheckpoint:source.readCheckpoint,readAudioRowsSince:source.readAudioRowsSince,
      expectedAuthorityGroups:capability.expectedAuthorityGroups,
      drawAlphas:index===0?[]:[0,0.5,1],yieldEvery:capability.ticks,onBatch:async()=>{}});
    run.fixture=capability.fixture;
    run.firstFrame={schema:'th08/first-frame-receipt/1',epoch:run.epoch,event:'first-frame',
      nativePresentCount:4,origin:'http://127.0.0.1:8123'};
    attachCadence(run,rate);runs.push(run);
  }
  const rawPaths={
    baseline:join(output,`paired-replay-${capability.fixture}-baseline.json`),
    controlledExtraDraw:join(output,`paired-replay-${capability.fixture}-controlled-extra-draw.json`),
    cadence144:join(output,`paired-replay-${capability.fixture}-cadence-144hz.json`),
  };
  await (await import('node:fs/promises')).mkdir(output,{recursive:true});
  const runIdentities={};
  for(const [index,key] of ['baseline','controlledExtraDraw','cadence144'].entries())
    runIdentities[key]=await writeJson(rawPaths[key],runs[index]);
  const pairReport=buildPresentationPairedReport({runs,requiredTicks:capability.ticks,scope,
    ratesHz:capability.cadencesHz,expectedAuthorityGroups:capability.expectedAuthorityGroups,
    cadenceDifferenceGroups:capability.cadenceDifferenceGroups,fixture:capability.fixture,input});
  const pairPath=join(output,`paired-replay-${capability.fixture}-result.json`),pair=await writeJson(pairPath,pairReport);
  const humanRows=humanIds.filter(id=>!pairReport.checks.some(row=>row.id===id)).map(id=>checkRow(id,'incomplete',false,{reason:'Human review pending'}));
  const pairedRows=pairReport.checks.filter(row=>machineIds.includes(row.id));
  const acceptanceRows=pairedRows.map(row=>({...row,evidence:{...scope,adaptationIds:[row.id],sources:[row.evidence]}}));
  const acceptanceReport={schema:'eagler-presentation/acceptance/1',scope,status:'incomplete',complete:false,
    checks:[...acceptanceRows,...pairReport.checks.filter(row=>!machineIds.includes(row.id)),...humanRows],
    humanReview:{status:'pending',groups:['continuous-motion','boundary','pause']}};
  const acceptancePath=join(output,'presentation-acceptance.json'),acceptance=await writeJson(acceptancePath,acceptanceReport);
  const snapshot={game:scope.game,variant:scope.variant,lanes:['presentation'],pairedReplay:capability,targetId:'th08-normal',
    target:{id:'th08-normal',game:scope.game,variant:scope.variant},caseIdentity:{profile,
      files:{pairedReplay:{...input}}}};
  const fingerprint=identityDigest(snapshot);
  const pre=await writeJson(join(root,'pre.json'),snapshot),post=await writeJson(join(root,'post.json'),snapshot);
  const snapshotRef=(ref)=>({...ref,fingerprint,targetId:'th08-normal'});
  const entries=machineIds.map(id=>{
    const check=acceptanceRows.find(row=>row.id===id);
    return{id,lane:'presentation-paired',status:check.status,complete:check.complete,machineComplete:check.machineComplete,
      scope:'paired presentation / named Replay window',observation:{game:scope.game,variant:scope.variant,adaptationIds:[id],
        observation:{schema:acceptanceReport.schema,scope,check,humanReview:acceptanceReport.humanReview,
          artifacts:{acceptance,pair,runs:runIdentities,input}}}};
  });
  const target={id:'th08-normal',game:scope.game,variant:scope.variant,status:'passed',checks:entries};
  const driver={schema:'eagler-touhou/diagnostic-browser/1',scope:{...scope,lanes:['presentation'],pairedReplay:{requested:true,
      status:'ready',capability}},selection:{...scope,targetId:target.id},profile:{schema:profile.schema,...scope,files:{}},
    identity:{status:'stable',stable:true,pre:snapshotRef(pre),post:snapshotRef(post)},
    evidence:{...scope,targetId:target.id,manifestData:data,labBuild:build,
      diagnosticInputs:{pairedReplay:input}},server:{status:'ready',url:'http://127.0.0.1:8123/'},targets:[target]};
  return{driver,entries,runs,rawPaths,runIdentities,acceptanceReport,acceptancePath};
}

test('paired machine claims are recomputed from all retained 60/120/144 Hz raw runs',async t=>{
  const fixture=await evidenceFixture(t);
  for(const entry of fixture.entries){
    const result=assessCompleteEvidence({driver:fixture.driver,entry,product:scope});
    if(['AUDIO-06','DISPLAY-07'].includes(entry.id)){
      assert.equal(entry.complete,false);assert.equal(entry.machineComplete,false);assert.equal(result.valid,false);
    }else assert.deepEqual(result,{available:true,valid:true,reason:null,kind:'paired-presentation'},`${entry.id}: ${result.reason}`);
  }
  const forged=structuredClone(fixture.driver),forgedAudio=forged.targets[0].checks.find(row=>row.id==='AUDIO-06');
  forgedAudio.complete=true;forgedAudio.status='passed';forgedAudio.machineComplete=true;
  Object.assign(forgedAudio.observation.observation.check,{complete:true,status:'passed',machineComplete:true});
  assert.equal(assessCompleteEvidence({driver:forged,entry:forgedAudio,product:scope}).valid,false,
    'Event-route summaries cannot certify actual output PCM');
  const review=fixture.acceptanceReport.humanReview;
  assert.equal(review.status,'pending');
  for(const id of humanIds)assert.equal(fixture.acceptanceReport.checks.find(row=>row.id===id).complete,false);
});

test('a complete flag with missing artifacts or a stale raw artifact hash is rejected',async t=>{
  const fixture=await evidenceFixture(t),entry=fixture.entries[0];
  const missing=structuredClone(fixture.driver),missingEntry=missing.targets[0].checks[0];
  delete missingEntry.observation.observation.artifacts.runs.cadence144;
  assert.equal(assessCompleteEvidence({driver:missing,entry:missingEntry,product:scope}).valid,false);
  const bytes=await readFile(fixture.rawPaths.baseline);
  await writeFile(fixture.rawPaths.baseline,Buffer.concat([bytes,Buffer.from(' ')]));
  assert.equal(assessCompleteEvidence({driver:fixture.driver,entry,product:scope}).valid,false);
});

test('raw-run divergence and borrowed Runtime epochs cannot inherit a passed report',async t=>{
  const fixture=await evidenceFixture(t),entry=fixture.entries[0];
  const raw=JSON.parse(await readFile(fixture.rawPaths.baseline,'utf8'));
  raw.samples[0].logicalTick=9;
  const bytes=Buffer.from(JSON.stringify(raw,null,2)+'\n');await writeFile(fixture.rawPaths.baseline,bytes);
  fixture.runIdentities.baseline={path:resolve(fixture.rawPaths.baseline),bytes:bytes.length,sha256:hash(bytes)};
  assert.equal(assessCompleteEvidence({driver:fixture.driver,entry,product:scope}).valid,false);

  const second=await evidenceFixture(t),staleEntry=second.entries[0];
  const borrowed=JSON.parse(await readFile(second.rawPaths.cadence144,'utf8'));
  borrowed.epoch=second.runs[0].epoch;
  const borrowedBytes=Buffer.from(JSON.stringify(borrowed,null,2)+'\n');await writeFile(second.rawPaths.cadence144,borrowedBytes);
  second.runIdentities.cadence144={path:resolve(second.rawPaths.cadence144),bytes:borrowedBytes.length,sha256:hash(borrowedBytes)};
  assert.equal(assessCompleteEvidence({driver:second.driver,entry:staleEntry,product:scope}).valid,false);
});

test('the paired assessor leaves the ordinary DISPLAY-03 capture on its existing assessor',()=>{
  const result=assessPresentationEvidence({driver:{schema:'eagler-touhou/diagnostic-browser/1'},
    entry:{id:'DISPLAY-03',lane:'presentation'},product:scope});
  assert.equal(result.available,false);
});

test('a changed Replay input file cannot inherit its previously recorded source identity',async t=>{
  const fixture=await evidenceFixture(t),entry=fixture.entries[0];
  await writeFile(entry.observation.observation.artifacts.input.path,'different replay input');
  const result=assessCompleteEvidence({driver:fixture.driver,entry,product:scope});
  assert.equal(result.valid,false);assert.match(result.reason,/Selected Replay input bytes differ/);
});

test('legacy lifecycle summaries with fake PNG data cannot certify native boundary evidence',async t=>{
  const fixture=await evidenceFixture(t),base=fixture.entries[0].observation.observation.artifacts;
  const directory=resolve(base.acceptance.path,'..');
  const build={wasm:fixture.runs[0].identity.build.wasm,sourceDigest:fixture.runs[0].identity.build.sourceDigest,
    inputs:{data:fixture.runs[0].identity.data},runtimeEpoch:90};
  const makeReport=(label,scene,tick)=>({schema:capability.boundary.reportSchema,label,tick,valid:true,build,
    scene:{diagnostics:[scene,0,0,0],status:[scene,1,0]},objects:[],disappeared:[],dropped:0,lifecycle:{status:'complete',dropped:0}});
  const title=makeReport('display05-title-before',1,10),game=makeReport('stage1-motion',2,20);
  const report=makeReport('display05-pool-reuse-boundary',2,15);
  const retired={operation:2,owner:5,object:123,generation:1,tick:15},born={...retired,operation:1,generation:2};
  report.disappeared=[{ownerId:5,generation:1,key:'5:123:1:0:0',status:'lifecycle'}];
  report.objects=[{ownerId:5,object:123,generation:2,identityFlags:3,status:'unobserved',fields:[{id:'position',status:'unobserved'}]}];
  const scan={event:{retired,born},report};
  const check={id:'DISPLAY-05',status:'passed',complete:false,machineComplete:true,evidence:{...scope,adaptationIds:['DISPLAY-05']}};
  const raw={schema:'eagler-presentation/display-observations/1',scope,poolReuseCapture:scan,
    lifecycleBefore:{available:true,capacity:8,count:0,dropped:0,events:[]},
    lifecycleAfter:{available:true,capacity:8,count:2,dropped:0,events:[retired,born]},checks:[check]};
  const material=row=>({...row,label:'manual-mark',images:Array.from({length:5},(_,index)=>({alpha:index/4,png:'data:image/png;base64,YQ=='}))});
  const definitions={observations:['presentation-observations.json',raw],scan:['display05-pool-reuse-scan.json',scan],
    capture:['browser-captures.json',{reports:[title,report,game]}],titleMaterial:['material-display05-title-before.json',material(title)],
    poolMaterial:['material-display05-pool-reuse-boundary.json',material(report)]};
  const display={};for(const [key,[name,value]] of Object.entries(definitions))display[key]=await writeJson(join(directory,name),value);
  fixture.acceptanceReport.checks=fixture.acceptanceReport.checks.filter(row=>row.id!=='DISPLAY-05').concat(check);
  const acceptance=await writeJson(fixture.acceptancePath,fixture.acceptanceReport);
  const entry={id:'DISPLAY-05',lane:'presentation-paired',status:'passed',complete:false,machineComplete:true,
    observation:{...scope,adaptationIds:['DISPLAY-05'],observation:{schema:fixture.acceptanceReport.schema,scope,check,
      humanReview:fixture.acceptanceReport.humanReview,artifacts:{...base,acceptance,display}}}};
  fixture.driver.targets[0].checks.push(entry);
  const assess=()=>assessCompleteEvidence({driver:fixture.driver,entry,product:scope});
  assert.equal(assess().valid,false,'Raw native words and real material bytes are required');assert.equal(entry.complete,false);
  game.scene.status[0]=1;
  display.capture=await writeJson(join(directory,definitions.capture[0]),definitions.capture[1]);
  assert.equal(assess().valid,false,'Game supervisor scene must agree with the native gameplay scene');
  game.scene.status[0]=2;
  report.objects[0].fields[0].status='interpolated';
  display.scan=await writeJson(join(directory,definitions.scan[0]),scan);
  display.observations=await writeJson(join(directory,definitions.observations[0]),raw);
  display.capture=await writeJson(join(directory,definitions.capture[0]),definitions.capture[1]);
  assert.equal(assess().valid,false,'A recycled object may not interpolate from the previous generation');
});
