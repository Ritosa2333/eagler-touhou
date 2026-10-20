import test from 'node:test';
import assert from 'node:assert/strict';
import {offlineRuntimeOutcome} from './offline-runtime-result.mjs';
import {assessDriverReport,adaptationCoverage} from '../../tests/adaptation/results.mjs';

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

test('the base driver result reaches the queue and completes its selected package proof without an update',()=>{
  const checks=[{id:'PACKAGE-02',status:'passed',complete:true},{id:'PACKAGE-03',status:'blocked'}];
  const outcome=offlineRuntimeOutcome({scenarioPassed:true,checks});
  const stamp='2026-10-20T01:00:00Z';
  const driver={schema:'eagler-touhou/offline-runtime-browser/1',runId:'offline-base',startedAt:stamp,finishedAt:stamp,
    scope:{game:'th06',variant:'normal',excluded:['PACKAGE-03']},checks,...outcome};
  const selected={id:'offline-normal-th06',game:'th06',variant:'normal',scenarioIds:['offline-import-reopen'],checksMapped:['PACKAGE-02']};
  const assessed=assessDriverReport(selected,{status:'passed',code:0,startedAt:stamp},driver);
  assert.equal(assessed.status,'passed');
  assert.ok(assessed.scenarios.length>0);
  assert.ok(assessed.scenarios.every(scene=>scene.id==='offline-import-reopen'));
  assert.equal(adaptationCoverage([{...selected,...assessed}]).complete,true);
  assert.equal(driver.checks[1].status,'blocked');
  assert.equal(assessDriverReport({...selected,blockedScenarios:[{id:'package-update-and-user-data-recovery'}]},
    {status:'passed',code:0,startedAt:stamp},driver).status,'incomplete');
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
