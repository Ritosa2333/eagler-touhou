import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,readFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {runWithReport} from '../testing/cli-report.mjs';
import {DIAGNOSTIC_LANE_CHECKS} from '../../tests/adaptation/adapters/diagnostics.mjs';
import {assessDriverReport} from '../../tests/adaptation/results.mjs';
import {assertDiagnosticCaptureBuild,diagnosticTargetCoverage,finalizeDiagnosticReport,finalizeDiagnosticTarget,plannedDiagnosticChecks,recordDiagnosticLane} from './diagnostic-report.mjs';

test('session input receipts extend but cannot replace any served build field',()=>{
  const served={schema:'lab/1',wasm:'a',toolSources:{capture:'b'},productionBuild:{execution:'c'}};
  const captured={...structuredClone(served),inputs:{data:{sha256:'d'}},runtimeEpoch:1};
  assert.doesNotThrow(()=>assertDiagnosticCaptureBuild(captured,served));
  for(const mutate of [r=>r.wasm='other',r=>r.toolSources.capture='other',r=>delete r.productionBuild]){
    const changed=structuredClone(captured);mutate(changed);
    assert.throws(()=>assertDiagnosticCaptureBuild(changed,served));
  }
});

const lanes=['presentation','quick','daily'];
const primary={presentation:'DISPLAY-03',quick:'VERIFY-01',daily:'VERIFY-02'};

function sampleReport({claimComplete=false}={}){
  const target={id:'runtime-th08-normal',game:'th08',variant:'normal',status:'not-run',passed:false,
    checks:plannedDiagnosticChecks(lanes,DIAGNOSTIC_LANE_CHECKS)};
  const laneReports=lanes.map(lane=>({lane,game:'th08',variant:'normal',status:'passed',scope:`${lane} evidence`,
    observation:{observation:{kind:`${lane}-capture`,artifact:{path:`artifacts/${lane}.json`,bytes:120,sha256:'a'.repeat(64)},
      observedValues:[{id:primary[lane],result:'PASS'}]}}}));
  for(const laneReport of laneReports)recordDiagnosticLane(target,laneReport.lane,laneReport,
    {checkIds:DIAGNOSTIC_LANE_CHECKS[laneReport.lane],primaryCheckId:primary[laneReport.lane]});
  if(!claimComplete)for(const entry of target.checks)entry.complete=false;
  finalizeDiagnosticTarget(target,laneReports,lanes,true);
  return{schema:'eagler-touhou/diagnostic-browser/1',runId:'diagnostic-run',startedAt:'2026-10-21T00:00:00.000Z',
    finishedAt:'2026-10-21T00:01:00.000Z',status:'passed',passed:true,scope:{game:'th08',variant:'normal',lanes},
    targets:[target],coverage:diagnosticTargetCoverage(target)};
}

const testCase={game:'th08',variant:'normal',group:'diagnostic',scenarioIds:['diagnostic-presentation','diagnostic-quick','diagnostic-daily'],
  checksMapped:[...new Set(lanes.flatMap(lane=>DIAGNOSTIC_LANE_CHECKS[lane]))]};
const processResult={code:0,status:'passed'};

test('sealed diagnostic envelopes preserve pending review and real failures independently',async t=>{
  const output=await mkdtemp(join(tmpdir(),'diagnostic-envelope-'));
  t.after(()=>rm(output,{recursive:true,force:true}));
  for(const status of ['incomplete','failed','passed']){
    const report=await runWithReport({output,schema:'eagler-touhou/diagnostic-browser/1'},async report=>{
      const target={id:'th08-normal',game:'th08',variant:'normal',status,passed:status==='passed',checks:[],complete:false};
      report.targets=[target];finalizeDiagnosticReport(report,target);
    });
    assert.equal(report.status,status);assert.equal(report.passed,status==='passed');
    assert.deepEqual(JSON.parse(await readFile(join(output,'report.json'),'utf8')),report);
    assert.ok(report.finishedAt);assert.equal(report.failure,undefined);
  }
  const report=sampleReport();report.targets[0].status='incomplete';report.targets[0].passed=false;
  finalizeDiagnosticReport(report,report.targets[0]);
  const assessed=assessDriverReport(testCase,{code:1,status:'failed'},report);
  assert.equal(assessed.status,'incomplete');assert.deepEqual(assessed.issues,[]);
});

test('structured diagnostic receipt fixtures retain product binding and partial scope',()=>{
  const report=sampleReport(),assessed=assessDriverReport(testCase,processResult,report);
  assert.equal(assessed.status,'passed',assessed.issues.join('; '));
  assert.equal(assessed.checks.filter(check=>check.complete).length,0);
  assert.ok(assessed.checks.every(check=>check.proof?.source==='driver-report'));
  for(const entry of report.targets[0].checks){
    assert.equal(entry.observation.game,'th08');assert.equal(entry.observation.variant,'normal');
    assert.deepEqual(entry.observation.adaptationIds,[entry.id]);
    assert.ok(entry.observation.observation.artifact.sha256);
  }
});

test('changed reference identity fails the sealed diagnostic report even when every lane passed',()=>{
  const report=sampleReport(),target=report.targets[0];
  finalizeDiagnosticTarget(target,lanes.map(lane=>({lane,status:'passed'})),lanes,false);
  finalizeDiagnosticReport(report,target);
  assert.equal(report.status,'failed');assert.equal(report.passed,false);
  const assessed=assessDriverReport(testCase,{code:1,status:'failed'},report);
  assert.equal(assessed.status,'failed');
  assert.ok(assessed.checks.every(check=>check.complete===false));
});

test('a diagnostic receipt fixture cannot claim a complete native capture',()=>{
  const report=sampleReport({claimComplete:true}),assessed=assessDriverReport(testCase,processResult,report);
  assert.equal(assessed.status,'failed');
  assert.ok(assessed.issues.some(issue=>issue.includes('Invalid complete evidence for DISPLAY-03')));
  assert.ok(assessed.checks.every(check=>!check.complete));
});

for(const lane of lanes)test(`driver report assessor rejects a failed ${lane} lane observation`,()=>{
  const report=sampleReport(),target=report.targets[0],id=primary[lane];
  const entry=target.checks.find(check=>check.lane===lane&&check.id===id);
  entry.status='failed';entry.complete=false;target.status='failed';target.passed=false;
  const assessed=assessDriverReport(testCase,{...processResult,code:1},report);
  assert.equal(assessed.status,'failed');
  assert.ok(assessed.issues.some(issue=>issue.includes(`Failed driver check: ${id}`)));
});

test('driver report assessor rejects an omitted planned check',()=>{
  const report=sampleReport();
  report.targets[0].checks=report.targets[0].checks.filter(check=>!(check.lane==='presentation'&&check.id==='DISPLAY-03'));
  const assessed=assessDriverReport(testCase,processResult,report);
  assert.notEqual(assessed.status,'passed');
  assert.ok(assessed.incompleteReasons.some(reason=>reason.includes('DISPLAY-03')));
});

test('driver report assessor rejects an observation bound to another variant',()=>{
  const report=sampleReport();report.targets[0].variant='multiplayer';
  const assessed=assessDriverReport(testCase,processResult,report);
  assert.equal(assessed.status,'failed');
  assert.ok(assessed.issues.some(issue=>issue.includes('another product or selection')));
});
