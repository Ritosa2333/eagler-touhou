import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,writeFileSync,readFileSync,rmSync} from 'node:fs';
import {join} from 'node:path';
import {tmpdir} from 'node:os';
import {createHash} from 'node:crypto';
import {replayFile,withPointerMotion} from '../fixtures/th08-replay-file.mjs';
import {assertReplayRecordingEvidence,assertReplayIsolationEvidence,assertOriginalReplayExportEvidence,assertReplayExtensionEvidence} from './replay-recording-evidence.mjs';
import {resolveAdaptationAdapter} from './registry.mjs';
import {createReplayRecordingScenes} from '../../../tools/ports/replay-recording-scenes.mjs';
import {unsupportedReplayVersion} from '../../../tools/ports/replay-version-rejection.mjs';
import {assessDriverReport} from '../results.mjs';

const product={game:'th08',variant:'normal'};
function extensionFixture(t){
  const f=fixture(t),adapter=resolveAdaptationAdapter('th08','normal'),e=f.entry.observation;
  const plan=createReplayRecordingScenes({semantics:adapter.replay,stage:e.stage,windowTicks:e.windowTicks}),scene=plan.cases.find(row=>row.kind==='recording-extension');
  const bytes=withPointerMotion(readFileSync(f.path),scene.trajectory),sha256=createHash('sha256').update(bytes).digest('hex');writeFileSync(f.path,bytes);
  e.replay={bytes:bytes.length,sha256};
  e.cases.push({id:scene.id,status:'passed',cleanup:[{operation:'release',status:'passed'}],samples:Array.from({length:4},(_,i)=>({sequence:i+1,boundary:scene.boundary,stage:0,
    motion:{playing:true,stage:0,touchCursor:i,touchCount:3,pointCount:i?1:0,point:i?{id:7101,x:scene.trajectory[i-1].x,y:scene.trajectory[i-1].y,released:i===3}:null}}))});
  const contract=adapter.replay.pointerExtension.rejection,{bytes:bad,mutation}=unsupportedReplayVersion(bytes,contract),artifact=join(f.path+'-unsupported.rpyx');writeFileSync(artifact,bad);
  e.versionRejection={status:'passed',mutation,artifact,sent:{command:'write',path:contract.path,protocol:'eagler-touhou/1',game:'th08',epoch:1,request:'version-negative',bytes:Array.from(bad)},
    responses:[{protocol:'eagler-touhou/1',game:'th08',epoch:1,request:'version-negative',ok:false,error:contract.error}],barrier:{game:'th08',epoch:1,request:'still-live',ok:true},
    lists:{before:[],after:[]},originalBefore:Array.from(bytes),originalAfter:Array.from(bytes)};
  return{...f,e,adapter,scene,record:e.cases.at(-1)};
}
test('extended Replay acceptance independently checks applied points and reopens the unknown-version bytes',t=>{
  const f=extensionFixture(t);assertReplayExtensionEvidence(assertReplayRecordingEvidence(f.driver,f.entry,product));
});
for(const [name,mutate] of Object.entries({
  'pointer never applied':f=>f.record.samples[1].motion.point.x=0,
  'pointer up not released':f=>f.record.samples[3].motion.point.released=false,
  'pointer consumption truncated':f=>f.record.samples.pop(),
  'wrong version reply':f=>f.e.versionRejection.responses[0].ok=true,
  'unrelated rejection':f=>f.e.versionRejection.responses[0].error='Error: Invalid path',
  'changed negative file':f=>writeFileSync(f.e.versionRejection.artifact,Buffer.from('changed')),
  'valid recording overwritten':f=>f.e.versionRejection.originalAfter[0]^=1,
}))test(`extended Replay acceptance rejects ${name}`,t=>{const f=extensionFixture(t);mutate(f);assert.throws(()=>assertReplayExtensionEvidence(assertReplayRecordingEvidence(f.driver,f.entry,product)));});
function fixture(t){
  const directory=mkdtempSync(join(tmpdir(),'eagler-recording-proof-'));
  t.after(()=>rmSync(directory,{recursive:true,force:true}));
  const inputFrames=[0,...Array.from({length:3},()=>[0,1,1,0,0]).flat()];
  const bytes=replayFile({inputFrames}),sha256=createHash('sha256').update(bytes).digest('hex');
  const path=join(directory,'recorded.rpy');writeFileSync(path,bytes);
  const cases=['keyboard','virtual-gamepad','touch-controls'].map((route,index)=>({
    id:`record-${route}`,status:'passed',cleanup:[{operation:'release-all',status:'passed'}],
    result:{status:'passed',scope:'ignored-derived-status'},
    samples:Array.from({length:5},(_,i)=>{
      const frame=1+index*5+i;
      return{sequence:i+1,boundary:1,stage:0,controlFrame:frame+30,pollSequence:frame,liveInput:inputFrames[frame],
        recording:{active:true,stage:0,frame,cursor:frame*2,physical:inputFrames[frame]},
        consumer:{sequence:frame,controlFrame:frame+30,stage:0,phase:1,validMask:2,
          current:inputFrames[frame-1],previous:inputFrames[frame-2]??0}};
    }),
  }));
  const entry={id:'shared-replay-recording-and-isolation',status:'passed',complete:true,
    adaptationIds:['REPLAY-02','REPLAY-03','REPLAY-04','REPLAY-06'],observation:{schema:'eagler-touhou/replay-recording-run/1',
      title:product,stage:0,windowTicks:12,epoch:1,clock:{mode:'native-manual'},cases,
      artifacts:{replay:path},replay:{bytes:bytes.length,sha256}}};
  const driver={schema:'eagler-touhou/runtime-desktop-probes/1',runId:'synthetic-recording',
    startedAt:'2026-10-24T00:00:00.000Z',finishedAt:'2026-10-24T00:00:01.000Z',status:'passed',passed:true,
    targets:[{...product,status:'passed',checks:[entry],gameplayReadinessByEpoch:[{epoch:1,ready:true}]}]};
  return{driver,entry,cases,path};
}
test('recording completeness compares all device consumers with independent saved-file bytes and only completes REPLAY-03',t=>{
  const f=fixture(t);assert.doesNotThrow(()=>assertReplayRecordingEvidence(f.driver,f.entry,product));
  const result=assessDriverReport({...product,group:'replay-recording',checksMapped:f.entry.adaptationIds,scenarioIds:[]},
    {status:'passed',code:0},f.driver);
  assert.deepEqual(result.checks.filter(row=>row.complete).map(row=>row.id),['REPLAY-03']);
});
test('original export independently reopens and compares keyboard and pad frames before extensions',t=>{
  const f=fixture(t),recording=assertReplayRecordingEvidence(f.driver,f.entry,product),{e,adapter}=recording;
  e.originalExport={status:'passed',artifact:f.path,sha256:e.replay.sha256,bytes:e.replay.bytes,
    finalized:{status:'passed',path:adapter.replay.finalizer.path},recordingCases:['record-keyboard','record-virtual-gamepad']};
  e.originalExport.inspection=adapter.inspectOriginalReplay(recording.bytes,{expectedSha256:e.replay.sha256});
  assertOriginalReplayExportEvidence(recording);
  e.originalExport.recordingCases.pop();assert.throws(()=>assertOriginalReplayExportEvidence(recording));
  e.originalExport.recordingCases.push('record-virtual-gamepad');
  f.cases[1].samples[2].consumer.current=0;assert.throws(()=>assertOriginalReplayExportEvidence(recording));
});
const mutations={
  'recorder and file agree while player never fires':f=>{f.cases[0].samples[2].consumer.current=0;},
  'player retains Fire after recorded release':f=>{f.cases[1].samples[4].consumer.current=1;},
  'wrong player previous-input edge':f=>{f.cases[2].samples[3].consumer.previous=0;},
  'missing device route':f=>{f.entry.observation.cases.pop();},
  'missing native consumer':f=>{delete f.cases[0].samples[2].consumer;},
  'stale player consumer sequence':f=>{f.cases[1].samples[3].consumer.sequence--;},
  'changed native control frame':f=>{f.cases[0].samples[2].consumer.controlFrame++;},
  'file changed after capture':f=>{writeFileSync(f.path,Buffer.from('changed'));},
  'failed input cleanup':f=>{f.cases[0].cleanup[0].status='incomplete';},
};
for(const [name,mutate] of Object.entries(mutations))test(`recording evidence rejects ${name}`,t=>{
  const f=fixture(t);mutate(f);assert.throws(()=>assertReplayRecordingEvidence(f.driver,f.entry,product));
});

function isolationFixture(t){
  const f=fixture(t),data=assertReplayRecordingEvidence(f.driver,f.entry,product);
  const raw=data.adapter.parseReplayFrames(data.bytes,{stage:0,expectedSha256:data.sha256,firstFrame:0,frameCount:12}).frames;
  for(const scene of data.plan.cases.filter(row=>row.kind==='playback-live-perturbation')){
    const window=perturbed=>Array.from({length:12},(_,index)=>{
      const pressed=perturbed&&index>=3&&index<=4,frame=index+1,controlFrame=(perturbed?2000:1000)+index;
      return{sequence:index+1,boundary:1,stage:0,controlFrame,pollSequence:100+index,
        liveInput:pressed&&!scene.ingress?scene.probe.logicalMask:0,
        playback:{loaded:true,stage:0,frame,cursor:frame*2,current:raw[index].buttons,previous:raw[index-1]?.buttons??0},
        consumer:{phase:1,validMask:2,sequence:(perturbed?500:100)+index,controlFrame,stage:0,current:raw[index].buttons,previous:raw[index-1]?.buttons??0},
        ...(scene.ingress?{ingress:{status:'passed',receiptId:scene.ingress.receipt,
          fields:{[scene.ingress.contextField]:scene.ingress.contextValue,[scene.ingress.activeField]:pressed?1:0}}}:{})};
    });
    data.e.cases.push({id:scene.id,status:'passed',baseline:{samples:window(false),loadedReplaySha256:data.sha256},
      perturbed:{samples:window(true),loadedReplaySha256:data.sha256},cleanup:[{operation:'release-all',status:'passed'}]});
  }
  f.entry.completeChecks=['REPLAY-03','REPLAY-06'];return{...f,data};
}
test('complete Replay isolation reopens the saved file and checks every declared native route',t=>{
  const f=isolationFixture(t);assertReplayIsolationEvidence(f.data);
  const result=assessDriverReport({...product,group:'replay-recording',checksMapped:f.entry.adaptationIds,scenarioIds:[]},{status:'passed',code:0},f.driver);
  assert.deepEqual(result.checks.filter(row=>row.complete).map(row=>row.id),['REPLAY-03','REPLAY-06']);
});
for(const [name,mutate] of Object.entries({
  'missing gamepad route':f=>f.data.e.cases.splice(f.data.e.cases.findIndex(row=>row.id==='playback-live-virtual-gamepad-shoot'),1),
  'touch never reaches native ingress':f=>f.data.e.cases.find(row=>row.id==='playback-live-touch-fire').perturbed.samples.forEach(row=>row.ingress.fields.fire=0),
  'live fire overrides Replay consumer':f=>f.data.e.cases.find(row=>row.id==='playback-live-keyboard-shoot').perturbed.samples[4].consumer.current^=1,
  'different loaded file':f=>f.data.e.cases.at(-1).baseline.loadedReplaySha256='f'.repeat(64),
  'missing release':f=>f.data.e.cases.find(row=>row.id==='playback-live-keyboard-direction').perturbed.samples.forEach((row,index)=>{if(index>=3)row.liveInput=128;}),
  'failed cleanup':f=>f.data.e.cases.at(-1).cleanup[0].status='incomplete',
}))test(`complete Replay isolation rejects ${name}`,t=>{const f=isolationFixture(t);mutate(f);assert.throws(()=>assertReplayIsolationEvidence(f.data));});
