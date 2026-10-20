import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,readFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {offlineRuntimeOutcome,runOfflineRuntimeReport} from './offline-runtime-result.mjs';
import {assessDriverReport,adaptationCoverage} from '../../tests/adaptation/results.mjs';
import {buildAdaptationPlan} from '../../tests/adaptation/catalog.mjs';

test('offline base scenario passes while incomplete and blocked adaptation checks remain visible',()=>{
  const result=offlineRuntimeOutcome({scenarioPassed:true,checks:[
    {id:'PACKAGE-02',status:'passed',complete:true},
    {id:'PACKAGE-04',status:'partial',complete:false},
    {id:'PACKAGE-05',status:'passed',complete:true},
    {id:'PACKAGE-03',status:'blocked'},
    {id:'SAVE-06',status:'blocked'},
  ]});
  assert.deepEqual(result,{scenario:{id:'offline-import-reopen',status:'passed',passed:true},
    scenarios:[{id:'offline-import-reopen',status:'passed',passed:true}],
    coverage:{status:'partial',complete:false},status:'passed',passed:true});
});

test('the production offline report writer seals the selected catalog scope for the queue',async t=>{
  const output=await mkdtemp(resolve(tmpdir(),'offline-report-'));
  t.after(()=>rm(output,{recursive:true,force:true}));
  const environment={EAGLER_TEST_RUNTIME_MANIFEST:'runtime.json',EAGLER_TEST_PACKAGES:'packages.json',EAGLER_TEST_ARTWORK:'artwork'};
  const options={root:resolve(import.meta.dirname,'../..'),games:['th08'],variant:'normal',groups:['offline'],environment};
  const selected=buildAdaptationPlan({...options,checks:['PACKAGE-02']}).cases[0];
  await runOfflineRuntimeReport({output},async report=>{
    report.scope={game:selected.game,variant:selected.variant,excluded:['PACKAGE-03']};
    report.checks=[{id:'PACKAGE-02',status:'passed',complete:true},{id:'PACKAGE-03',status:'blocked'}];
    // A journey cannot append an unselected scene to the finalized execution scope.
    report.scenarios=[{id:'package-update-and-user-data-recovery',status:'not-run',passed:false}];
    return{scenarioPassed:true};
  });
  const driver=JSON.parse(await readFile(resolve(output,'report.json'),'utf8'));
  assert.ok(driver.runId&&driver.finishedAt);
  assert.deepEqual(driver.scenarios,[{id:'offline-import-reopen',status:'passed',passed:true}]);
  const processResult={status:'passed',code:0,startedAt:driver.startedAt};
  const assessed=assessDriverReport(selected,processResult,driver);
  assert.equal(assessed.status,'passed');
  assert.ok(assessed.scenarios.length>0);
  assert.ok(assessed.scenarios.every(scene=>scene.id==='offline-import-reopen'));
  assert.equal(adaptationCoverage([{...selected,checksMapped:selected.checks,...assessed}]).complete,true);
  assert.equal(driver.checks[1].status,'blocked');
  const withUpdate=buildAdaptationPlan(options).cases[0];
  assert.ok(withUpdate.blockedScenarios.length>0);
  assert.equal(assessDriverReport(withUpdate,processResult,driver).status,'incomplete');
});

test('the sealed offline report rejects a failed selected update and late browser errors',async t=>{
  const root=await mkdtemp(resolve(tmpdir(),'offline-report-failures-'));
  t.after(()=>rm(root,{recursive:true,force:true}));
  for(const kind of ['update-failed','late-error','journey-error']){
    const output=resolve(root,kind);
    await runOfflineRuntimeReport({output},async report=>{
      report.scope={game:'th08',variant:'normal'};
      report.checks=[{id:'PACKAGE-02',status:'passed',complete:true}];
      try{
        if(kind==='journey-error'){report.status='passed';report.passed=true;throw Error('Fixture browser assertion failed');}
        return{scenarioPassed:true,updateScenario:kind==='update-failed'?
          {id:'package-update-and-user-data-recovery',status:'failed',passed:false}:null};
      }finally{if(kind==='late-error')report.finalErrorAudit={status:'failed'};}
    });
    const driver=JSON.parse(await readFile(resolve(output,'report.json'),'utf8'));
    assert.equal(driver.status,'failed',kind);assert.equal(driver.passed,false,kind);assert.ok(driver.finishedAt);
    const assessed=assessDriverReport({game:'th08',variant:'normal'},
      {status:'passed',code:0,startedAt:driver.startedAt},driver);
    assert.equal(assessed.status,'failed',kind);
    if(kind==='late-error'){
      assert.equal(driver.preFinalAuditStatus,'passed');
      assert.ok(driver.scenarios.every(scene=>scene.status==='failed'&&scene.passed===false));
    }
    if(kind==='update-failed')assert.equal(driver.scenarios[1].status,'failed');
    if(kind==='journey-error')assert.match(driver.failure,/Fixture browser assertion failed/);
  }
});

test('selected update failure cannot be hidden by a successful base scene',()=>{
  const updateScenario={id:'package-update-and-user-data-recovery',status:'failed',passed:false};
  const result=offlineRuntimeOutcome({scenarioPassed:true,updateScenario});
  assert.equal(result.passed,false);assert.equal(result.scenarios[1],updateScenario);
});

test('offline behavior failures fail the scenario even when coverage is incomplete',()=>{
  const result=offlineRuntimeOutcome({scenarioPassed:false,checks:[
    {id:'PACKAGE-05',status:'failed',complete:false},{id:'PACKAGE-03',status:'blocked'},
  ]});
  assert.equal(result.scenario.status,'failed');
  assert.equal(result.status,'failed');
  assert.equal(result.passed,false);
  assert.equal(result.coverage.status,'partial');
});
