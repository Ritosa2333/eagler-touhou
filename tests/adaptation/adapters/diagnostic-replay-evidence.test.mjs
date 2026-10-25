import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {fileIdentity} from '../../../tools/testing/dependency-snapshot.mjs';
import {assessCompleteEvidence} from '../complete-evidence.mjs';
import {assessDriverReport,assessDriverReportAsync,readCaseReport} from '../results.mjs';
import {buildAdaptationPlan} from '../catalog.mjs';
import {reviewDiagnosticReplayMethods} from './diagnostic-replay-evidence.mjs';

const product={game:'th08',variant:'normal'},schema='eagler-touhou/diagnostic-replay-review/1';
function forged(){
  const entry={id:'VERIFY-06',status:'passed',complete:true,observation:{lanes:[{methodControls:{passed:true}}]}};
  const driver={schema,runId:'method-claim',...product,scope:product,newGameRun:false,status:'passed',passed:true,
    startedAt:'2026-10-25T00:00:00Z',finishedAt:'2026-10-25T00:00:01Z',checks:[entry],evidence:{}};
  return{driver,entry,product};
}

test('a successful process and claimed method controls cannot certify Replay comparison',async()=>{
  const f=forged(),task={...product,checksMapped:['VERIFY-06']},processResult={status:'passed',code:0};
  const sync=assessCompleteEvidence(f);assert.equal(sync.available,true);assert.equal(sync.valid,false);assert.match(sync.reason,/asynchronous/);
  for(const result of [assessDriverReport(task,processResult,f.driver),await assessDriverReportAsync(task,processResult,f.driver)]){
    assert.equal(result.status,'failed');assert.equal(result.checks[0].complete,false);
  }
});

test('sealed-file aggregation awaits method evidence and rejects an invented source',async t=>{
  const directory=await mkdtemp(join(tmpdir(),'replay-method-'));t.after(()=>rm(directory,{recursive:true,force:true}));
  const path=join(directory,'report.json');await writeFile(path,JSON.stringify(forged().driver));
  const result=await readCaseReport({...product,reportPath:path,checksMapped:['VERIFY-06']},{status:'passed',code:0});
  assert.equal(result.status,'failed');assert.equal(result.checks[0].complete,false);assert.deepEqual(result.report,{path,...await fileIdentity(path)});
});

test('unsupported variants and incomplete source manifests never become method coverage',async t=>{
  await assert.rejects(reviewDiagnosticReplayMethods({sourceReport:{},product:{...product,variant:'multiplayer'}}),/Register/);
  const directory=await mkdtemp(join(tmpdir(),'replay-method-inputs-'));t.after(()=>rm(directory,{recursive:true,force:true}));
  const path=join(directory,'sources.json');await writeFile(path,JSON.stringify({schema:'eagler-touhou/diagnostic-replay-inputs/1',sources:{quick:'source.json'}}));
  await assert.rejects(reviewDiagnosticReplayMethods({sourceReport:{path,...await fileIdentity(path)},product}));
});

test('VERIFY-06 has an independent method owner while Quick and Daily retain game comparisons',()=>{
  const plan=buildAdaptationPlan({games:['th08'],variant:'normal',checks:['VERIFY-06'],environment:{EAGLER_TEST_DIAGNOSTIC_REPORT:'source.json'}});
  const item=plan.cases.find(row=>row.group==='replay-method');assert.ok(item);assert.equal(item.status,'ready');
  assert.deepEqual(item.requiredChecks,['VERIFY-06']);assert.ok(item.task.args.includes('tools/ports/diagnostic-replay-review.mjs'));
  for(const item of plan.cases.filter(row=>row.group==='diagnostic')){
    assert.ok(!item.requiredChecks.includes('VERIFY-06'));assert.ok(item.supportingChecks.includes('VERIFY-06'));
  }
  const unsupported=buildAdaptationPlan({games:['th10'],variant:'normal',groups:['replay-method'],environment:{EAGLER_TEST_DIAGNOSTIC_REPORT:'source.json'}});
  assert.equal(unsupported.cases[0].status,'blocked');assert.match(unsupported.cases[0].reason,/Replay review adapter/);
});
