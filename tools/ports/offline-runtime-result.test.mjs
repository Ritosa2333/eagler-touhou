import test from 'node:test';
import assert from 'node:assert/strict';
import {offlineRuntimeOutcome} from './offline-runtime-result.mjs';

test('offline base scenario passes while incomplete and blocked adaptation checks remain visible',()=>{
  const result=offlineRuntimeOutcome({scenarioPassed:true,checks:[
    {id:'PACKAGE-02',status:'passed',complete:true},
    {id:'PACKAGE-04',status:'partial',complete:false},
    {id:'PACKAGE-05',status:'passed',complete:true},
    {id:'PACKAGE-03',status:'blocked'},
    {id:'SAVE-06',status:'blocked'},
  ]});
  assert.deepEqual(result,{scenario:{id:'offline-import-reopen',status:'passed',passed:true},
    coverage:{status:'partial',complete:false},status:'passed',passed:true});
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
