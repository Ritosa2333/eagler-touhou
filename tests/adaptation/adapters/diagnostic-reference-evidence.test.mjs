import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtempSync,writeFileSync,readFileSync,rmSync} from 'node:fs';
import {resolve,join,dirname} from 'node:path';
import {tmpdir} from 'node:os';
import {createHash} from 'node:crypto';
import {identityDigest} from '../../../tools/testing/dependency-snapshot.mjs';
import {resolveDiagnosticProfile} from './diagnostics.mjs';
import {inspectDiagnosticReferenceIntegrity,reviewDiagnosticReference,reviewDiagnosticCaptureSource,recordDiagnosticReferenceChecks} from './diagnostic-reference-evidence.mjs';
import {assessCompleteEvidence} from '../complete-evidence.mjs';
import {assessDriverReport} from '../results.mjs';
import {adaptationEnvironment} from '../inputs.mjs';
import {createCaseIdentityReader} from '../identity.mjs';

const root=resolve(import.meta.dirname,'../../..'),product={game:'th08',variant:'normal'};
function fixture(t,lane='quick'){
  const directory=mkdtempSync(join(tmpdir(),'diagnostic-reference-'));
  t.after(()=>{assert.equal(dirname(resolve(directory)),resolve(tmpdir()));rmSync(directory,{recursive:true,force:true});});
  const save=(name,value)=>{const path=join(directory,name),bytes=Buffer.from(JSON.stringify(value));writeFileSync(path,bytes);
    return{path,bytes:bytes.length,sha256:createHash('sha256').update(bytes).digest('hex')};};
  const profile=resolveDiagnosticProfile(product.game,product.variant),targetId='th08-normal',startedAt='2026-10-25T00:00:00.000Z',finishedAt='2026-10-25T00:00:10.000Z';
  const input=name=>({path:join(directory,name),bytes:10,sha256:'a'.repeat(64)});
  const inputs={font:input('font'),data:input('data'),...(lane==='quick'?{quickInputs:profile.quickInputs.map(row=>({...row,...input(row.file)}))}:
    {dailyReplays:Object.fromEntries(profile.dailyFixtures.map(row=>[row.replayKey,input(row.id+'.rpy')]))})};
  const snapshot={...product,targetId,lanes:[lane],caseIdentity:{profile:{...product,goldenManifest:profile.goldenManifest},golden:[{path:'manifest.json',bytes:30,sha256:'b'.repeat(64)}],files:inputs}};
  const pre={...save('pre.json',snapshot),fingerprint:identityDigest(snapshot),targetId},post={...save('post.json',snapshot),fingerprint:identityDigest(snapshot),targetId};
  const captures=lane==='quick'?['quick-capture']:profile.dailyFixtures.map(row=>'capture-'+row.id),gateId=lane+'-gate';
  const selected=[...captures,gateId],run={schema:'eagler-touhou/test-run/1',...product,lane,startedAt,finishedAt:'2026-10-25T00:00:09.000Z',status:'failed',selected,
    results:selected.map(id=>{
      const gate=id===gateId,args=gate?['--golden-root',resolve(root,dirname(profile.goldenManifest))]:lane==='quick'?['--fixtures',String(profile.quickFixtureCount)]:
        ['--replay',inputs.dailyReplays[profile.dailyFixtures.find(row=>'capture-'+row.id===id).replayKey].path];
      if(gate&&lane==='quick')args.push('--input-root',directory);
      if(gate&&lane==='daily')for(const selection of profile.dailyGateReplayOptions)args.push(selection.option,inputs.dailyReplays[selection.replayKey].path);
      return{id,command:[gate?process.execPath:'python',resolve(root,gate?(lane==='quick'?profile.quickGate:profile.dailyGate):(lane==='quick'?profile.quickCapture:profile.dailyCapture)),...args],
        cwd:root,startedAt:'2026-10-25T00:00:01.000Z',durationMs:1,status:gate?'failed':'passed',code:gate?1:0,signal:null};
    })};
  const expected=lane==='quick'?profile.quickInputs.map(row=>row.id):profile.dailyFixtures.map(row=>row.id);
  const gate={suite:lane,status:'DIVERGED',complete:false,expectedIds:expected,results:expected.map(id=>({id,status:'DIVERGED'}))};
  const evidence={...product,lane,targetId,runnerReport:run,runnerArtifact:save('runner.json',run),observation:{observation:{gate:{...save('gate.json',gate),report:gate}}}};
  const entry={id:'VERIFY-04',lane,status:'passed',complete:true},driver={schema:'eagler-touhou/diagnostic-browser/1',startedAt,finishedAt,status:'failed',passed:false,
    scope:{...product,lanes:[lane]},targets:[{...product,id:targetId,checks:[entry]}],identity:{stable:true,status:'stable',pre,post},
    server:{shutdown:{exited:true,normal:true}},evidence:{lanes:[evidence]}};
  const request={driver,product,lane};entry.observation={...product,adaptationIds:['VERIFY-04'],observation:inspectDiagnosticReferenceIntegrity(request)};
  return{...request,entry,evidence,snapshot,save};
}
for(const lane of ['quick','daily'])test(lane+' reference protection can pass independently of actual comparison failure',t=>{
  const f=fixture(t,lane),proof=assessCompleteEvidence(f);assert.equal(proof.valid,true,proof.reason);
  assert.equal(f.driver.status,'failed');assert.equal(f.evidence.runnerReport.status,'failed');
});

test('driver completion records immutable inputs while preserving an unrelated failed comparison',t=>{
  const f=fixture(t);f.entry.status='failed';f.entry.complete=false;f.entry.error='comparison differs';
  const comparison={id:'VERIFY-01',status:'failed',complete:false};f.driver.targets[0].checks.push(comparison);
  recordDiagnosticReferenceChecks(f.driver,product);
  assert.equal(f.entry.complete,true);assert.equal(assessCompleteEvidence(f).valid,true);
  assert.equal(comparison.status,'failed');assert.equal(f.driver.status,'failed');assert.equal(f.entry.error,undefined);
});
for(const [name,change]of Object.entries({
  'changed reference bytes':f=>{const s=structuredClone(f.snapshot);s.caseIdentity.golden[0].sha256='c'.repeat(64);
    f.driver.identity.post={...f.save('post.json',s),fingerprint:identityDigest(s),targetId:s.targetId};},
  'tampered snapshot file':f=>writeFileSync(f.driver.identity.pre.path,'{}'),
  'wrong product':f=>f.driver.scope.game='th10',
  'other variant':f=>f.product={game:'th08',variant:'multiplayer'},
  'missing runner receipt':f=>delete f.evidence.runnerArtifact,
  'tampered task outcome':f=>f.evidence.runnerReport.results[0].status='passed-without-running',
  'changed gate file':f=>writeFileSync(f.evidence.observation.observation.gate.path,'{}'),
  'borrowed target':f=>f.evidence.targetId='other-app',
  'false stable flag':f=>f.driver.identity.stable=false,
  'live server':f=>f.driver.server.shutdown.exited=false,
  'aliased snapshots':f=>f.driver.identity.post=f.driver.identity.pre,
  'unowned claim':f=>f.driver.targets[0].checks=[],
  'modified conclusion':f=>f.entry.observation.observation.referenceFiles=999,
}))test('central reference proof rejects '+name,t=>{const f=fixture(t);change(f);assert.equal(assessCompleteEvidence(f).valid,false);});

test('candidate task failure cannot be disguised by rewriting the sealed runner receipt',t=>{
  const f=fixture(t);f.evidence.runnerReport.results[0].code=1;f.evidence.runnerReport.results[0].status='failed';
  f.evidence.runnerArtifact=f.save('runner.json',f.evidence.runnerReport);
  assert.throws(()=>inspectDiagnosticReferenceIntegrity(f));
});

test('capture provenance remains reviewable when an old gate failed before saving observations',t=>{
  const f=fixture(t);delete f.evidence.observation;
  const sourceReport=f.save('capture-source.json',f.driver);
  const actual=reviewDiagnosticCaptureSource({sourceReport,product,lane:'quick'});
  assert.equal(actual.sourceRun.status,'failed');assert.equal(actual.observation.kind,'diagnostic-source-inventory');
  assert.equal(actual.observation.inputFiles,4);
  assert.throws(()=>inspectDiagnosticReferenceIntegrity(f),'Reference acceptance still requires its actual gate');
});

function reviewFixture(t){
  const quick=fixture(t,'quick'),daily=fixture(t,'daily');
  const driver=quick.driver;driver.scope.lanes=['quick','daily'];driver.evidence.lanes.push(daily.evidence);
  const snapshot={...quick.snapshot,lanes:['quick','daily'],caseIdentity:{profile:quick.snapshot.caseIdentity.profile,golden:quick.snapshot.caseIdentity.golden,
    files:{...quick.snapshot.caseIdentity.files,dailyReplays:daily.snapshot.caseIdentity.files.dailyReplays}}};
  for(const name of ['pre','post'])driver.identity[name]={...quick.save(name+'.json',snapshot),fingerprint:identityDigest(snapshot),targetId:snapshot.targetId};
  for(const [item,save]of [[quick.evidence,quick.save],[daily.evidence,daily.save]]){
    for(const task of item.runnerReport.results)task.log=join(dirname(item.runnerArtifact.path),task.id+'.log');
    save('report.json',item.runnerReport);delete item.runnerArtifact;
  }
  const sourceReport=quick.save('source.json',driver);
  const observation=reviewDiagnosticReference({sourceReport,product}),entry={id:'VERIFY-04',status:'passed',complete:true,observation};
  const review={schema:'eagler-touhou/diagnostic-reference-review/1',runId:'reference-review-fixture',status:'passed',passed:true,scope:product,newBrowserRun:false,
    startedAt:'2026-10-25T00:00:20.000Z',finishedAt:'2026-10-25T00:00:21.000Z',checks:[entry],evidence:{sourceReport}};
  return{driver:review,entry,product,sourceReport,original:driver,save:quick.save};
}

test('offline review reopens both original queues and preserves the failed capture verdict without rewriting it',t=>{
  const f=reviewFixture(t),before=readFileSync(f.sourceReport.path),proof=assessCompleteEvidence(f);
  assert.equal(proof.valid,true,proof.reason);assert.equal(f.entry.observation.sourceRun.status,'failed');
  assert.deepEqual(f.entry.observation.observations.map(row=>row.lane),['quick','daily']);
  assert.equal(f.entry.observation.sourceRun.finishedAt,f.original.finishedAt);
  assert.deepEqual(readFileSync(f.sourceReport.path),before);
  const assessed=assessDriverReport({...product,checksMapped:['VERIFY-04']},{status:'passed',code:0},f.driver);
  assert.equal(assessed.status,'passed',assessed.issues.join('\n'));assert.equal(assessed.checks[0].complete,true);
});

test('offline review rejects a replaced original queue even when the parent retains its old embedded copy',t=>{
  const f=reviewFixture(t),queue=join(dirname(f.entry.observation.observations[0].runner.path),'report.json');
  writeFileSync(queue,'{}');assert.equal(assessCompleteEvidence(f).valid,false);
});

test('offline review cannot upgrade a report that omits a selected lane',t=>{
  const f=reviewFixture(t);f.original.evidence.lanes.pop();
  const sourceReport=f.save('source.json',f.original);
  assert.throws(()=>reviewDiagnosticReference({sourceReport,product}));
});

test('unified reference preflight consumes the sealed report without Runtime, browser or private DATA discovery',async t=>{
  const f=reviewFixture(t),config=f.save('inputs.json',{schema:'eagler-touhou/adaptation-inputs/1',diagnosticReport:'source.json'});
  const environment=await adaptationEnvironment(config.path,{});assert.equal(environment.EAGLER_TEST_DIAGNOSTIC_REPORT,f.sourceReport.path);
  const item={...product,group:'reference-integrity',checks:['VERIFY-04'],task:{command:process.execPath,
    args:['tools/ports/diagnostic-reference-review.mjs','--report='+f.sourceReport.path]}};
  const reader=createCaseIdentityReader({root,environment,inputOnly:true});
  assert.equal((await reader(item)).inputPreflight.status,'passed');
  writeFileSync(f.entry.observation.observations[0].runner.path,'{}');
  await assert.rejects(reader(item));
});
