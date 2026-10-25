import assert from 'node:assert/strict';
import {lstat,readdir} from 'node:fs/promises';
import {resolve,join,dirname,relative} from 'node:path';
import {pathToFileURL} from 'node:url';
import {fileIdentity,dependencySnapshot} from '../../../tools/testing/dependency-snapshot.mjs';
import {runReplaySuite} from '../../../common/testkit/replay-verifier/suite.mjs';
import {readSceneReviewArtifact} from './scene-review.mjs';
import {resolveDiagnosticProfile} from './diagnostics.mjs';
import {reviewDiagnosticCaptureSource} from './diagnostic-reference-evidence.mjs';

const root=resolve(import.meta.dirname,'../../..');
const json=receipt=>JSON.parse(readSceneReviewArtifact(receipt));
function option(task,name){
  const indices=task.command.flatMap((value,index)=>value===name?[index]:[]);assert.equal(indices.length,1);
  assert.ok(indices[0]<task.command.length-1);return task.command[indices[0]+1];
}

async function captureInventory(directory){
  const rows=[];
  async function visit(path){
    const info=await lstat(path);assert.ok(!info.isSymbolicLink(),'Capture inputs cannot be symbolic links');
    if(info.isDirectory())for(const name of (await readdir(path)).sort())await visit(join(path,name));
    else{assert.ok(info.isFile());assert.ok(rows.length<4096,'Capture inventory exceeds its file budget');
      rows.push({path,...await fileIdentity(path)});}
  }
  await visit(resolve(directory));assert.ok(rows.length,'Capture inventory is empty');return rows;
}

/** Fingerprint source files without repeating the full streaming comparison at preflight. */
export async function inspectDiagnosticReplayInputs({sourceReport,product}){
  const profile=resolveDiagnosticProfile(product.game,product.variant);assert.ok(profile?.replayReview,'Register the title Replay review adapter');
  const selection=json(sourceReport);
  const manifest=selection.schema==='eagler-touhou/diagnostic-replay-inputs/1';
  if(manifest)assert.deepEqual(Object.keys(selection.sources).sort(),['daily','quick']);
  const harnessSources=await dependencySnapshot(root,['tests/adaptation/adapters/diagnostic-replay-evidence.mjs',
    profile.replayReview.module,...new Set([...profile.decisionInputs.quick,...profile.decisionInputs.daily])]);
  const lanes=[];
  for(const lane of ['quick','daily']){
    let receipt=sourceReport;
    if(manifest){
      assert.ok(typeof selection.sources[lane]==='string'&&selection.sources[lane].trim());
      const path=resolve(dirname(sourceReport.path),selection.sources[lane]);receipt={path,...await fileIdentity(path)};
    }
    const source=json(receipt),reference=reviewDiagnosticCaptureSource({sourceReport:receipt,product,lane});
    const selected=source.evidence.lanes.filter(row=>row.lane===lane);assert.equal(selected.length,1);
    const captureRoot=selected[0].evidenceRoot,before=await captureInventory(captureRoot);
    const run=json(reference.observation.runner),historical=json(reference.observation.pre).caseIdentity;
    const gate=run.results.find(row=>row.id===lane+'-gate');assert.equal(resolve(option(gate,'--capture-root')),resolve(captureRoot));
    for(const task of run.results.filter(row=>row.id!==lane+'-gate')){
      const fixture=lane==='daily'?profile.dailyFixtures.find(row=>task.id==='capture-'+row.id):null;
      assert.equal(resolve(option(task,'--output')),lane==='quick'?resolve(captureRoot):resolve(captureRoot,fixture.captureFile));
    }
    const inputs=source.evidence.diagnosticInputs;
    const receiptFields=row=>({path:resolve(row.path),bytes:row.bytes,sha256:row.sha256});
    if(lane==='quick')assert.deepEqual(inputs.quickInputs.map(receiptFields),historical.files.quickInputs.map(receiptFields));
    else for(const fixture of profile.dailyFixtures)assert.deepEqual(receiptFields(inputs.dailyReplays[fixture.replayKey]),receiptFields(historical.files.dailyReplays[fixture.replayKey]));
    const goldenRoot=resolve(root,dirname(profile.goldenManifest));
    const goldenBefore=(await captureInventory(goldenRoot)).map(row=>({...row,path:relative(goldenRoot,row.path).replaceAll('\\','/')}));
    const originalGolden=historical.golden;
    assert.deepEqual([...goldenBefore].sort((a,b)=>a.path.localeCompare(b.path,'en')),
      [...originalGolden].sort((a,b)=>a.path.localeCompare(b.path,'en')),'References differ from the original capture inputs');
    lanes.push({lane,sourceReport:receipt,reference,captureRoot,captures:before});
  }
  return{sourceReport,harnessSources,lanes};
}

/** Reopen actual captures and immutable references, preserving game comparison failures. */
export async function reviewDiagnosticReplayMethods(request){
  const input=await inspectDiagnosticReplayInputs(request),{product}=request;
  const profile=resolveDiagnosticProfile(product.game,product.variant);
  const implementation=await import(pathToFileURL(resolve(root,profile.replayReview.module)));
  const createSuite=implementation[profile.replayReview.export];assert.equal(typeof createSuite,'function');
  const lanes=[];
  for(const selected of input.lanes){
    const {lane,captureRoot,sourceReport}=selected,source=json(sourceReport);
    const suite=await createSuite({lane,captureRoot,inputs:source.evidence.diagnosticInputs});
    const comparison={...await runReplaySuite({...suite,methodControls:true}),lane,golden:suite.golden};
    assert.equal(comparison.schema,'eagler/replay-suite-result/v1');assert.equal(comparison.game,product.game);
    assert.equal(comparison.lane,lane);
    const ids=lane==='quick'?profile.quickInputs.map(row=>row.id):profile.dailyFixtures.map(row=>row.id);
    assert.deepEqual(comparison.expectedIds,ids);assert.deepEqual(comparison.results.map(row=>row.id),ids);
    assert.equal(comparison.methodControls?.passed,true,'Real reference method controls failed');
    assert.ok(comparison.methodControls.controls.length>=7,'Method controls omit required differences');
    for(const row of comparison.results){
      assert.ok(['PASS','DIVERGED'].includes(row.status),`${lane}/${row.id} has no valid comparison: ${row.reason??row.status}`);
      assert.ok(['PASS','DIVERGED'].includes(row.comparison?.status),`${lane}/${row.id} lacks a comparison result`);
      assert.ok(row.comparison.comparedTicks>0,`${lane}/${row.id} compared no ticks`);
      assert.ok(!row.terminalValidation||row.terminalValidation.status==='DIVERGED',`${lane}/${row.id} has invalid capture boundaries`);
      const build=row.candidateProvenance?.diagnosticBuild;assert.ok(build,`${lane}/${row.id} lacks captured build identity`);
      for(const key of ['schema','commit','wasm','loader','sourceDigest','toolDigest']){
        assert.ok(source.evidence.labBuild[key]);assert.equal(build[key],source.evidence.labBuild[key],`${lane}/${row.id} used another ${key}`);
      }
      assert.equal(build.wasm,source.evidence.labRuntime.wasmSha256);
    }
    lanes.push({...selected,comparison});
  }
  assert.deepEqual(await inspectDiagnosticReplayInputs(request),input,'Replay inputs or method sources changed during review');
  return{...input,lanes};
}

export function diagnosticReplayClaim(driver,entry){
  return driver?.schema==='eagler-touhou/diagnostic-replay-review/1'&&entry?.id==='VERIFY-06';
}

export async function assessDiagnosticReplayEvidence({driver,entry,product}={}){
  if(!diagnosticReplayClaim(driver,entry))return{available:false,valid:false,kind:null};
  const result={available:true,valid:false,kind:'diagnostic-replay-method',checks:['VERIFY-06']};
  try{
    assert.equal(driver.status,'passed');assert.equal(driver.passed,true);assert.equal(driver.newGameRun,false);
    assert.deepEqual(driver.scope,product);assert.ok(driver.finishedAt&&Date.parse(driver.finishedAt)>=Date.parse(driver.startedAt));
    assert.equal(entry.status,'passed');assert.equal(entry.complete,true);assert.ok(driver.checks.includes(entry));
    assert.equal(driver.checks.filter(row=>row.id===entry.id).length,1);
    assert.deepEqual(entry.observation,await reviewDiagnosticReplayMethods({...driver.evidence,product}));result.valid=true;
  }catch(error){result.reason=error.message;}
  return result;
}
