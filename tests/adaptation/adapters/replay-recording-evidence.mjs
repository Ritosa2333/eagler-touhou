import assert from 'node:assert/strict';
import {readFileSync} from 'node:fs';
import {createHash} from 'node:crypto';
import {resolveAdaptationAdapter} from './registry.mjs';
import {createReplayRecordingScenes,assessReplayRecordingInputWindow,assessReplayInputIsolation,assessReplayPointerConsumption} from '../../../tools/ports/replay-recording-scenes.mjs';
import {assertReplayVersionRejection} from '../../../tools/ports/replay-version-rejection.mjs';

/** Finite recording proves consumed input; natural playback end has a separate contract. */
export function assertReplayRecordingEvidence(driver,entry,product){
  assert.equal(driver.schema,'eagler-touhou/runtime-desktop-probes/1');
  const selected=driver.targets.filter(target=>target.game===product.game&&target.variant===product.variant);
  assert.equal(selected.length,1,'Replay recording must select one matching product');
  const target=selected[0];assert.ok(target.checks.includes(entry)&&target.checks.filter(row=>row.id===entry.id).length===1);
  assert.equal(entry.status,'passed');
  const e=entry.observation,adapter=resolveAdaptationAdapter(product.game,product.variant);
  assert.ok(adapter?.replay&&adapter.parseReplayFrames,'Register independent Replay input parsing');
  assert.equal(e.schema,'eagler-touhou/replay-recording-run/1');
  assert.equal(e.title.game,product.game);assert.equal(e.title.variant,product.variant);
  assert.equal(e.clock?.mode,'native-manual');
  assert.ok(target.gameplayReadinessByEpoch?.some(value=>value.epoch===e.epoch&&value.ready===true),'Recording has no native gameplay readiness');
  assert.ok(Number.isSafeInteger(e.replay?.bytes)&&e.replay.bytes>0&&e.replay.bytes<=64*1024*1024,'Replay evidence size is invalid');
  const bytes=readFileSync(e.artifacts.replay),sha256=createHash('sha256').update(bytes).digest('hex');
  assert.equal(bytes.length,e.replay.bytes,'Saved Replay size changed');assert.equal(sha256,e.replay.sha256,'Saved Replay bytes changed');
  const plan=createReplayRecordingScenes({semantics:adapter.replay,stage:e.stage,windowTicks:e.windowTicks});
  const scenes=plan.cases.filter(scene=>scene.kind==='recording-input');
  assert.equal(scenes.length,3,'All three input routes need recording evidence');
  const records=scenes.map(scene=>{
    assert.equal(scene.status,'ready','Recording consumer contract is missing');
    const matches=e.cases.filter(record=>record.id===scene.id);assert.equal(matches.length,1,'Missing or duplicate input route');
    const record=matches[0];assert.equal(record.status,'passed');
    assert.ok(record.cleanup?.length&&record.cleanup.every(step=>step.status==='passed'),'Recording input cleanup did not complete');
    return{scene,record};
  });
  const lastFrame=Math.max(...records.flatMap(({record})=>record.samples.map(sample=>sample.recording?.frame??-1)));
  assert.ok(Number.isSafeInteger(lastFrame)&&lastFrame>0&&lastFrame<10000,'Invalid finite recorded frame range');
  const rawFrames=adapter.parseReplayFrames(bytes,{stage:e.stage,expectedSha256:sha256,firstFrame:0,frameCount:lastFrame+1});
  for(const {scene,record} of records){
    const result=assessReplayRecordingInputWindow({samples:record.samples,scene,rawFrames,expectedSha256:sha256});
    assert.equal(result.status,'passed',`${scene.id}: ${result.reason??'native recorder/player mismatch'}`);
  }
  return{e,adapter,bytes,sha256,plan};
}

export function assertReplayIsolationEvidence({e,adapter,bytes,sha256,plan}){
  const scenes=plan.cases.filter(scene=>scene.kind==='playback-live-perturbation');
  assert.ok(scenes.length>=4,'Declare keyboard direction/fire, gamepad and touch isolation probes');
  for(const [kind,covers] of [['keyboard-key','direction'],['keyboard-key','shoot'],['browser-gamepad-bit','shoot'],['touch-fire-control','shoot']])
    assert.ok(scenes.some(scene=>scene.probe.kind===kind&&scene.probe.covers===covers),`Missing ${kind} ${covers} isolation route`);
  const records=scenes.map(scene=>{
    assert.equal(scene.status,'ready');const matches=e.cases.filter(row=>row.id===scene.id);assert.equal(matches.length,1);
    const record=matches[0];assert.equal(record.status,'passed');
    assert.ok(record.cleanup?.length&&record.cleanup.every(step=>step.status==='passed'),'Playback inputs were not cleaned up');
    assert.equal(record.baseline.loadedReplaySha256,sha256);assert.equal(record.perturbed.loadedReplaySha256,sha256);
    return{scene,record};
  });
  const end=1+Math.max(...records.flatMap(({scene,record})=>[...record.baseline.samples,...record.perturbed.samples].map(row=>row.playback.frame+scene.rawIndexOffset)));
  assert.ok(Number.isSafeInteger(end)&&end>0&&end<10000);
  const rawFrames=adapter.parseReplayFrames(bytes,{stage:e.stage,expectedSha256:sha256,firstFrame:0,frameCount:end});
  for(const {scene,record} of records){
    const verdict=assessReplayInputIsolation({baseline:record.baseline.samples,perturbed:record.perturbed.samples,
      scene,rawFrames,expectedSha256:sha256,baselineSha256:record.baseline.loadedReplaySha256,perturbedSha256:record.perturbed.loadedReplaySha256});
    assert.equal(verdict.status,'passed',`${scene.id}: ${verdict.reason??'Live input changed Replay consumption'}`);
  }
}

export function assessRuntimeReplayRecordingEvidence({driver,entry,product}={}){
  if(entry?.id!=='shared-replay-recording-and-isolation')return{available:false,valid:false,kind:null};
  const checks=['REPLAY-03',...(entry.completeChecks?.includes('REPLAY-06')?['REPLAY-06']:[]),...(entry.completeChecks?.includes('REPLAY-02')?['REPLAY-02']:[]),...(entry.completeChecks?.includes('REPLAY-04')?['REPLAY-04']:[])];
  try{const recording=assertReplayRecordingEvidence(driver,entry,product);
    if(checks.includes('REPLAY-06'))assertReplayIsolationEvidence(recording);
    if(checks.includes('REPLAY-02'))assertOriginalReplayExportEvidence(recording);
    if(checks.includes('REPLAY-04'))assertReplayExtensionEvidence(recording);
    return{available:true,valid:true,kind:'native-recorded-consumer',checks};}
  catch(error){return{available:true,valid:false,kind:'native-recorded-consumer',checks,reason:error.message};}
}

export function assertReplayExtensionEvidence({e,adapter,bytes,sha256,plan}){
  const extension={...adapter.parseReplayExtension(bytes,{expectedSha256:sha256}),stage:e.stage};
  const scene=plan.cases.find(scene=>scene.kind==='recording-extension'),record=e.cases.find(row=>row.id===scene.id);
  assert.equal(record.status,'passed');assert.ok(record.cleanup.length>0&&record.cleanup.every(step=>step.status==='passed'));
  const verdict=assessReplayPointerConsumption({samples:record.samples,extension,scene,expectedSha256:sha256});
  assert.equal(verdict.status,'passed',verdict.reason);
  const expected=assertReplayVersionRejection(e.versionRejection,{contract:adapter.replay.pointerExtension.rejection,game:adapter.game,epoch:e.epoch,bytes});
  assert.deepEqual(readFileSync(e.versionRejection.artifact),expected.bytes);
  assert.throws(()=>adapter.parseReplayExtension(expected.bytes,{expectedSha256:expected.mutation.sha256}),/Unsupported.*version/);
}

export function assertOriginalReplayExportEvidence({e,adapter,plan}){
  const exportEvidence=e.originalExport;assert.equal(exportEvidence.status,'passed');
  assert.equal(exportEvidence.finalized.status,'passed');assert.equal(exportEvidence.finalized.path,adapter.replay.finalizer.path);
  const bytes=readFileSync(exportEvidence.artifact),sha256=createHash('sha256').update(bytes).digest('hex');
  assert.equal(bytes.length,exportEvidence.bytes);assert.equal(sha256,exportEvidence.sha256);
  assert.equal(typeof adapter.inspectOriginalReplay,'function');
  const inspection=adapter.inspectOriginalReplay(bytes,{expectedSha256:sha256});assert.deepEqual(exportEvidence.inspection,inspection);
  assert.equal(inspection.format,adapter.replay.format.id);assert.equal(inspection.version,adapter.replay.format.version);
  assert.equal(inspection.extensionBytes,0);assert.equal(inspection.originalBytes,bytes.length);
  const scenes=plan.cases.filter(scene=>scene.kind==='recording-input'&&['keyboard','virtual-gamepad'].some(route=>scene.id===`record-${route}`));
  assert.equal(scenes.length,2);
  assert.ok(scenes.every(scene=>exportEvidence.recordingCases.includes(scene.id)));
  const lastFrame=Math.max(...scenes.flatMap(scene=>e.cases.find(record=>record.id===scene.id).samples.map(sample=>sample.recording.frame)));
  const rawFrames=adapter.parseReplayFrames(bytes,{stage:e.stage,expectedSha256:sha256,firstFrame:0,frameCount:lastFrame+1});
  for(const scene of scenes){
    const record=e.cases.find(record=>record.id===scene.id);
    const verdict=assessReplayRecordingInputWindow({samples:record.samples,scene,rawFrames,expectedSha256:sha256});
    assert.equal(verdict.status,'passed',`${scene.id} original-format export: ${verdict.reason??'native input differs'}`);
  }
}
