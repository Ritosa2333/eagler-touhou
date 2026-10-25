import assert from 'node:assert/strict';
import {createHash} from 'node:crypto';
import {lstatSync,readFileSync} from 'node:fs';
import {basename,dirname,isAbsolute,resolve} from 'node:path';
import {identityDigest} from '../../../tools/testing/dependency-snapshot.mjs';
import {readSceneReviewArtifact} from './scene-review.mjs';
import {resolveDiagnosticProfile} from './diagnostics.mjs';

const root=resolve(import.meta.dirname,'../../..');
const json=receipt=>JSON.parse(readSceneReviewArtifact(receipt));
function file(row){assert.ok(Number.isSafeInteger(row.bytes)&&row.bytes>=0);assert.match(row.sha256,/^[a-f0-9]{64}$/);}
function option(task,name){const indices=task.command.flatMap((arg,index)=>arg===name?[index]:[]);assert.equal(indices.length,1);
  assert.ok(indices[0]<task.command.length-1);return task.command[indices[0]+1];}

/** Reference immutability is independent of the candidate's comparison verdict. */
function inspectDiagnosticSourceInputs({driver,product,lane}){
  assert.ok(['quick','daily'].includes(lane));
  const profile=resolveDiagnosticProfile(product.game,product.variant);assert.ok(profile?.lanes.includes(lane));
  assert.equal(driver.scope.game,product.game);assert.equal(driver.scope.variant,product.variant);
  assert.equal(driver.identity.stable,true);assert.equal(driver.identity.status,'stable');
  assert.equal(driver.server.shutdown.exited,true);assert.equal(driver.server.shutdown.normal,true);assert.ok(!driver.server.shutdownError);
  assert.notEqual(resolve(driver.identity.pre.path),resolve(driver.identity.post.path));
  const pre=json(driver.identity.pre),post=json(driver.identity.post);
  for(const [snapshot,receipt]of [[pre,driver.identity.pre],[post,driver.identity.post]]){
    assert.equal(identityDigest(snapshot),receipt.fingerprint);assert.equal(snapshot.targetId,receipt.targetId);
    assert.equal(snapshot.game,product.game);assert.equal(snapshot.variant,product.variant);assert.ok(snapshot.lanes.includes(lane));
    assert.equal(snapshot.caseIdentity.profile.game,product.game);assert.equal(snapshot.caseIdentity.profile.variant,product.variant);
    assert.equal(snapshot.caseIdentity.profile.goldenManifest,profile.goldenManifest);
  }
  assert.deepEqual(post,pre,'Reference or execution inputs changed');
  const inventory=pre.caseIdentity.golden;assert.ok(Array.isArray(inventory)&&inventory.length>0);
  assert.equal(new Set(inventory.map(row=>row.path)).size,inventory.length);
  assert.ok(inventory.some(row=>row.path===basename(profile.goldenManifest)));
  for(const row of inventory){assert.ok(row.path&&!isAbsolute(row.path)&&!/^([A-Za-z]:|[\\/])/.test(row.path)&&!row.path.split(/[\\/]/).includes('..'));file(row);}
  const inputs=pre.caseIdentity.files;file(inputs.font);file(inputs.data);
  if(lane==='quick'){
    assert.deepEqual(inputs.quickInputs.map(row=>row.id),profile.quickInputs.map(row=>row.id));inputs.quickInputs.forEach(file);
    assert.equal(new Set(inputs.quickInputs.map(row=>resolve(row.path))).size,profile.quickInputs.length);
    for(const [index,input]of inputs.quickInputs.entries())assert.equal(basename(input.path),profile.quickInputs[index].file);
  }else{
    assert.deepEqual(Object.keys(inputs.dailyReplays).sort(),profile.dailyFixtures.map(row=>row.replayKey).sort());
    Object.values(inputs.dailyReplays).forEach(file);
  }
  const matches=driver.evidence.lanes.filter(row=>row.lane===lane);assert.equal(matches.length,1);const evidence=matches[0];
  assert.equal(evidence.game,product.game);assert.equal(evidence.variant,product.variant);assert.equal(evidence.targetId,pre.targetId);
  const run=json(evidence.runnerArtifact);assert.deepEqual(run,evidence.runnerReport);assert.ok(run.finishedAt);
  assert.equal(run.schema,'eagler-touhou/test-run/1');
  assert.ok(Number.isFinite(Date.parse(run.startedAt))&&Date.parse(run.finishedAt)>=Date.parse(run.startedAt));
  assert.equal(run.game,product.game);assert.equal(run.variant,product.variant);assert.equal(run.lane,lane);
  const captures=lane==='quick'?['quick-capture']:profile.dailyFixtures.map(row=>'capture-'+row.id),gateId=lane+'-gate';
  assert.deepEqual(run.selected,[...captures,gateId]);assert.deepEqual(run.results.map(row=>row.id),run.selected);
  for(const task of run.results){
    assert.equal(resolve(task.cwd),root);assert.equal(task.signal,null);assert.ok(Number.isFinite(Date.parse(task.startedAt)));
    assert.ok(Date.parse(task.startedAt)>=Date.parse(driver.startedAt));assert.ok(Date.parse(task.startedAt)<=Date.parse(run.finishedAt));
    assert.ok(!driver.finishedAt||Date.parse(run.finishedAt)<=Date.parse(driver.finishedAt));
    if(task.id!==gateId){assert.equal(task.status,'passed');assert.equal(task.code,0);
      assert.equal(resolve(task.command[1]),resolve(root,lane==='quick'?profile.quickCapture:profile.dailyCapture));
      if(lane==='quick')assert.equal(option(task,'--fixtures'),String(profile.quickFixtureCount));
      else{const fixture=profile.dailyFixtures.find(row=>'capture-'+row.id===task.id);assert.ok(fixture);
        assert.equal(resolve(option(task,'--replay')),resolve(inputs.dailyReplays[fixture.replayKey].path));}
    }
    else{assert.ok(['passed','failed'].includes(task.status));assert.ok([0,1].includes(task.code));
      assert.equal(task.status,task.code===0?'passed':'failed');
      assert.equal(resolve(task.command[1]),resolve(root,lane==='quick'?profile.quickGate:profile.dailyGate));
      assert.equal(resolve(option(task,'--golden-root')),resolve(root,dirname(profile.goldenManifest)));
      if(lane==='quick')for(const input of inputs.quickInputs)
        assert.equal(resolve(option(task,'--input-root')),dirname(resolve(input.path)));
      else for(const selection of profile.dailyGateReplayOptions)
        assert.equal(resolve(option(task,selection.option)),resolve(inputs.dailyReplays[selection.replayKey].path));
    }
    for(const cleanup of [task.processCleanup,task.cleanupImages].filter(Boolean)){
      assert.deepEqual(cleanup.remaining,[]);assert.ok(['passed','command-failed','not-requested'].includes(cleanup.status));
    }
  }
  return{evidence,proof:{kind:'diagnostic-source-inventory',lane,pre:driver.identity.pre,post:driver.identity.post,
    runner:evidence.runnerArtifact,referenceFiles:inventory.length,
    inputFiles:lane==='quick'?inputs.quickInputs.length:Object.keys(inputs.dailyReplays).length}};
}

export function inspectDiagnosticReferenceIntegrity(request){
  const {evidence,proof}=inspectDiagnosticSourceInputs(request),{product,lane}=request;
  const profile=resolveDiagnosticProfile(product.game,product.variant);
  const gateArtifact=evidence.observation.observation.gate,gate=json(gateArtifact);assert.deepEqual(gate,gateArtifact.report);
  assert.equal(gate.suite,lane);
  const expected=lane==='quick'?profile.quickInputs.map(row=>row.id):profile.dailyFixtures.map(row=>row.id);
  assert.deepEqual(gate.expectedIds,expected);assert.deepEqual(gate.results.map(row=>row.id),expected);
  return{...proof,kind:'diagnostic-reference-immutability',gate:{path:gateArtifact.path,bytes:gateArtifact.bytes,sha256:gateArtifact.sha256}};
}

function loadDiagnosticSource({sourceReport,product,lanes}){
  const driver=json(sourceReport);assert.equal(driver.schema,'eagler-touhou/diagnostic-browser/1');
  assert.ok(driver.finishedAt&&Date.parse(driver.finishedAt)>=Date.parse(driver.startedAt));
  assert.ok(['passed','failed','incomplete'].includes(driver.status));
  const targets=driver.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);
  assert.ok(lanes.length&&new Set(lanes).size===lanes.length&&lanes.every(lane=>['quick','daily'].includes(lane)));
  for(const lane of lanes){
    const evidence=driver.evidence.lanes.find(row=>row.lane===lane);assert.ok(evidence);
    if(!evidence.runnerArtifact){
      const directories=new Set(evidence.runnerReport.results.map(row=>dirname(resolve(row.log))));assert.equal(directories.size,1);
      const path=resolve([...directories][0],'report.json');
      // Old sealed drivers embed the same queue; bind the original queue file now.
      // The source capture is not rewritten and this receipt is part of the review.
      const info=lstatSync(path);assert.ok(info.isFile()&&!info.isSymbolicLink()&&info.size>0&&info.size<=128*1024*1024);
      const bytes=readFileSync(path);assert.equal(bytes.length,info.size);
      evidence.runnerArtifact={path,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')};
    }
  }
  return driver;
}

const sourceRun=driver=>({runId:driver.runId,startedAt:driver.startedAt,finishedAt:driver.finishedAt,status:driver.status,
  runtime:driver.evidence.runtime,data:driver.evidence.manifestData});

/** Capture provenance can be reviewed even when an old gate failed before writing observations. */
export function reviewDiagnosticCaptureSource({sourceReport,product,lane}){
  const driver=loadDiagnosticSource({sourceReport,product,lanes:[lane]});
  return{sourceRun:sourceRun(driver),observation:inspectDiagnosticSourceInputs({driver,product,lane}).proof};
}

/** Reassessment of sealed captures retains their original execution identity. */
export function reviewDiagnosticReference({sourceReport,product}){
  const lanes=['quick','daily'],driver=loadDiagnosticSource({sourceReport,product,lanes});
  return{sourceRun:sourceRun(driver),observations:lanes.map(lane=>inspectDiagnosticReferenceIntegrity({driver,product,lane}))};
}

export function recordDiagnosticReferenceChecks(driver,product){
  const targets=driver.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);
  for(const entry of targets[0].checks.filter(row=>row.id==='VERIFY-04')){
    try{
      const observation=inspectDiagnosticReferenceIntegrity({driver,product,lane:entry.lane});
      Object.assign(entry,{status:'passed',complete:true,scope:'Complete reference and selected input inventory before/after candidate execution',
        observation:{...product,adaptationIds:['VERIFY-04'],observation}});delete entry.error;
    }catch(error){entry.complete=false;entry.error=String(error.message??error);}
  }
}

export function assessDiagnosticReferenceEvidence({driver,entry,product}={}){
  if(!['eagler-touhou/diagnostic-browser/1','eagler-touhou/diagnostic-reference-review/1'].includes(driver?.schema)||entry?.id!=='VERIFY-04')return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'diagnostic-reference-immutability',checks:['VERIFY-04']};
  try{
    assert.ok(driver.finishedAt&&Date.parse(driver.finishedAt)>=Date.parse(driver.startedAt));
    if(driver.schema==='eagler-touhou/diagnostic-reference-review/1'){
      assert.equal(driver.status,'passed');assert.deepEqual(driver.scope,product);assert.equal(driver.newBrowserRun,false);
      assert.equal(entry.status,'passed');assert.equal(entry.complete,true);
      assert.ok(driver.checks.includes(entry));assert.equal(driver.checks.filter(row=>row.id===entry.id).length,1);
      assert.deepEqual(entry.observation,reviewDiagnosticReference({...driver.evidence,product}));result.valid=true;return result;
    }
    const targets=driver.targets.filter(row=>row.game===product.game&&row.variant===product.variant);assert.equal(targets.length,1);
    assert.ok(targets[0].checks.includes(entry));assert.equal(targets[0].checks.filter(row=>row.id===entry.id&&row.lane===entry.lane).length,1);
    assert.equal(entry.status,'passed');assert.equal(entry.complete,true);
    assert.deepEqual(entry.observation,{...product,adaptationIds:['VERIFY-04'],observation:inspectDiagnosticReferenceIntegrity({driver,product,lane:entry.lane})});
    result.valid=true;
  }catch(error){result.reason=error.message;}return result;
}
