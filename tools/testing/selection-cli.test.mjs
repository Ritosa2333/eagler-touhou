import test from 'node:test';
import assert from 'node:assert/strict';
import {spawnSync} from 'node:child_process';
import {mkdtemp,mkdir,writeFile,readFile,rm} from 'node:fs/promises';
import {resolve,dirname} from 'node:path';
import {tmpdir} from 'node:os';

test('invalid suite and missing acceptance inputs invalidate an older passing selection summary',async()=>{
  const temporary=await mkdtemp(resolve(tmpdir(),'test-selection-cli-'));
  try{
    const environment={...process.env};delete environment.EAGLER_TEST_RUNTIME_MANIFEST;
    for(const suite of ['unknown-suite','runtime-browser','invalid-argument']){
      const output=resolve(temporary,suite);await mkdir(output);
      await writeFile(resolve(output,'summary.json'),JSON.stringify({status:'passed',passed:true,runId:'previous-run'}));
      const selection=suite==='invalid-argument'?['--bogus']:['--suite='+suite];
      const child=spawnSync(process.execPath,[resolve(import.meta.dirname,'../../tests/run.mjs'),...selection,`--output=${output}`],
        {env:environment,encoding:'utf8',windowsHide:true,timeout:15000});
      assert.equal(child.status,1,child.stderr);
      const summary=JSON.parse(await readFile(resolve(output,'summary.json')));
      assert.equal(summary.status,'failed');assert.equal(summary.passed,false);assert.notEqual(summary.runId,'previous-run');
      assert.ok(summary.finishedAt);assert.match(summary.failure,suite==='unknown-suite'?/Unknown test suite/:suite==='invalid-argument'?/Unknown test argument/:/EAGLER_TEST_RUNTIME_MANIFEST/);
    }
  }finally{assert.equal(dirname(temporary),resolve(tmpdir()));await rm(temporary,{recursive:true,force:true});}
});
