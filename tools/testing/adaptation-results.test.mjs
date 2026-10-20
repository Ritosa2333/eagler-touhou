import test from 'node:test';
import assert from 'node:assert/strict';
import {assessDriverReport,adaptationCoverage,summarizeAdaptationCases} from '../../tests/adaptation/results.mjs';

const selected={game:'th06',variant:'normal',checksMapped:['INPUT-01']};
const processResult={status:'passed',code:0,startedAt:'2026-10-19T01:00:00Z'};
const sealed=()=>({schema:'eagler-touhou/runtime-browser/1',runId:'test-run',startedAt:'2026-10-19T01:00:00Z',
  finishedAt:'2026-10-19T01:00:01Z',status:'passed',passed:true,targets:[{game:'th06',variant:'normal',status:'passed',checks:[]}]});

test('zero exit cannot hide a failed assertion, room target or final audit',()=>{
  for(const breakReport of [
    report=>report.targets[0].checks.push({id:'movement',checks:['INPUT-01'],status:'failed',error:'no displacement'}),
    report=>{delete report.targets[0].status;report.targets[0].passed=false;},
    report=>report.targets[0].finalErrorAudit={status:'failed'},
  ]){const report=sealed();breakReport(report);assert.equal(assessDriverReport(selected,processResult,report).status,'failed');}
});

test('sealed reports must match the current invocation, title and variant',()=>{
  for(const breakReport of [report=>delete report.finishedAt,report=>report.targets[0].game='th07',
    report=>report.targets[0].variant='multiplayer',report=>delete report.targets[0].variant,report=>report.startedAt='2026-10-18T01:00:00Z']){
    const report=sealed();breakReport(report);assert.equal(assessDriverReport(selected,processResult,report).status,'failed');
  }
});

test('desktop adaptationIds preserve the actual numbered evidence',()=>{
  const report=sealed();report.targets[0].checks=[{id:'native-four-direction-arrows',adaptationIds:['INPUT-01'],status:'passed',complete:true}];
  const actual=assessDriverReport(selected,processResult,report);
  assert.deepEqual(actual.checks.map(item=>item.id),['INPUT-01']);assert.equal(actual.checks[0].complete,true);
});

test('blocked or unreported execution cannot hide behind a passed outer report',()=>{
  for(const status of ['blocked','not-run',undefined]){
    const report=sealed();report.targets[0].status=status;
    assert.equal(assessDriverReport(selected,processResult,report).status,'incomplete');
  }
  const report=sealed();report.targets[0].scenarios=[{id:'required-scene',status:'blocked',passed:false}];
  assert.equal(assessDriverReport(selected,processResult,report).status,'incomplete');
});

test('incomplete scope, optional NONE observation and scenario success remain distinct',()=>{
  const report=sealed();report.status='incomplete';report.passed=false;
  assert.equal(assessDriverReport(selected,{...processResult,status:'failed',code:1},report).status,'incomplete');
  report.status='passed';report.passed=true;
  report.targets[0].modes=[{mode:'none',status:'recorded',passed:false},{mode:'ogg',status:'observed',passed:true}];
  assert.equal(assessDriverReport(selected,processResult,report).status,'passed');
  report.targets[0].modes[0].status='failed';
  assert.equal(assessDriverReport(selected,processResult,report).status,'failed');
});

test('a completed scenario cannot certify an adaptation ID without complete proof',()=>{
  const cases=[{...selected,id:'a',status:'passed',execution:'retained',checks:[{id:'INPUT-01',status:'passed',complete:false}]}];
  assert.equal(summarizeAdaptationCases(cases).passed,true);
  const coverage=adaptationCoverage(cases);assert.equal(coverage.complete,false);assert.equal(coverage.rows[0].status,'partial');
  assert.equal(coverage.rows[0].evidence[0].execution,'retained');
});
