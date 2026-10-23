import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,writeFile,readFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {runWithReport} from './cli-report.mjs';

test('new run replaces stale success before initialization and persists its own failure',async()=>{
  const output=await mkdtemp(resolve(tmpdir(),'cli-report-'));
  try{
    await writeFile(resolve(output,'report.json'),JSON.stringify({status:'passed',passed:true,runId:'old'}));
    const failed=await runWithReport({output,schema:'fixture/1'},async report=>{
      const onDisk=JSON.parse(await readFile(resolve(output,'report.json')));
      assert.equal(onDisk.status,'running');assert.equal(onDisk.passed,false);assert.equal(onDisk.runId,report.runId);
      JSON.parse('{invalid');
    });
    assert.equal(failed.status,'failed');assert.equal(failed.passed,false);assert.notEqual(failed.runId,'old');
    assert.ok(failed.finishedAt);assert.match(failed.failure,/SyntaxError/);
    assert.deepEqual(JSON.parse(await readFile(resolve(output,'report.json'))),failed);
  }finally{await rm(output,{recursive:true,force:true});}
});

test('empty completion cannot pass, and explicit incomplete results remain incomplete',async()=>{
  const output=await mkdtemp(resolve(tmpdir(),'cli-report-empty-'));
  try{
    const empty=await runWithReport({output,schema:'fixture/1'},async()=>{});assert.equal(empty.passed,false);
    const incomplete=await runWithReport({output,schema:'fixture/1'},async report=>{report.status='incomplete';});
    assert.equal(incomplete.status,'incomplete');assert.equal(incomplete.passed,false);
    assert.notEqual(empty.runId,incomplete.runId);
  }finally{await rm(output,{recursive:true,force:true});}
});

test('contradictory success fields fail closed and an explicit complete success passes',async()=>{
  const output=await mkdtemp(resolve(tmpdir(),'cli-report-contradiction-'));
  try{
    const result=await runWithReport({output,schema:'fixture/1'},async report=>{report.status='passed';report.passed=false;});
    assert.equal(result.status,'failed');assert.equal(result.passed,false);assert.match(result.failure,/requires passed=true/);
    const valid=await runWithReport({output,schema:'fixture/1'},async report=>{report.status='passed';report.passed=true;});
    assert.equal(valid.passed,true);
  }finally{await rm(output,{recursive:true,force:true});}
});
