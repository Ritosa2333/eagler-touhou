import test from 'node:test';
import assert from 'node:assert/strict';
import {readFileSync,mkdtempSync,existsSync,rmSync} from 'node:fs';
import {spawnSync} from 'node:child_process';
import {tmpdir} from 'node:os';
import {join} from 'node:path';
import {fileURLToPath} from 'node:url';
import {runInNewContext} from 'node:vm';
import {assertBeforeDeadline,awaitBeforeDeadline} from './performance-input-proof.mjs';

const driver=new URL('./performance-browser.mjs',import.meta.url),source=readFileSync(driver,'utf8');
const waitSource=source.slice(source.indexOf('async function waitNaturalSaveAfterCopy('),source.indexOf('\nasync function stableProofEvidence('));
function naturalEvents({stack='at http://test/runtime/shell.mjs:347:53',complete=true,fsId=5,error=null}={}){
  const events=[{kind:'native-request',token:5,result:5,entered:60132.2,at:60132.9,stack},
    {kind:'native-poll',token:5,result:1,at:60133},
    {kind:'fs-sync-start',id:5,populate:false,at:60133.2}];
  if(complete)events.push({kind:'fs-sync-complete',id:fsId,populate:false,error,at:60193.4});
  return events;
}
function fixture(events,{saveOnly=true}={}){
  let disposed=false,waitCalls=0;
  const frame={waitForFunction:async(predicate,args,options)=>{
    ++waitCalls;assert.equal(options.polling,100);assert.ok(options.timeout>0);
    const value=runInNewContext('('+predicate.toString()+')(args)',{args,
      __th15Performance:{state:{saveEvents:[{kind:'old-save'},...events]}},
      document:{querySelector:()=>({textContent:''})}});
    if(!value)throw Error('predicate is still waiting for a real natural FS completion');
    return {jsonValue:async()=>value,dispose:async()=>{disposed=true;}};
  }};
  const context={frames:[frame],assert,performance,assertBeforeDeadline,awaitBeforeDeadline,
    saveValidationOnly:saveOnly,saveEvidenceSupersessionTest:true};
  runInNewContext(waitSource,context);
  return {run:()=>context.waitNaturalSaveAfterCopy(0,{metadata:{saveEventCount:1}},performance.now()+1000,[347]),
    disposed:()=>disposed,waitCalls:()=>waitCalls};
}

test('the actual CLI rejects the deliberate supersession option before creating a performance output',()=>{
  const directory=mkdtempSync(join(tmpdir(),'th15-save-evidence-option-'));
  try{
    const output=join(directory,'must-not-exist');
    const result=spawnSync(process.execPath,[fileURLToPath(driver),'--save-evidence-supersession-test','--output='+output],{encoding:'utf8'});
    assert.notEqual(result.status,0);
    assert.match(result.stderr,/--save-evidence-supersession-test requires --save-validation-only/);
    assert.equal(existsSync(output),false);
  }finally{rmSync(directory,{recursive:true,force:true});}
});

test('the actual regression wait requires an existing natural timer native token and its matching FS completion',async()=>{
  const f=fixture(naturalEvents()),result=await f.run();
  assert.equal(result.token,5);assert.equal(result.fsId,5);
  assert.equal(result.filesystemCompletedAt,60193.4);assert.equal(f.disposed(),true);
});

test('a Host save, native completion alone, and an unrelated FS completion do not satisfy the natural-save regression',async()=>{
  for(const events of [naturalEvents({stack:'at command (http://test/runtime/shell.mjs:314:19)'}),
                       naturalEvents({complete:false}),naturalEvents({fsId:4})]){
    await assert.rejects(fixture(events).run(),/still waiting for a real natural FS completion/);
  }
});

test('the actual regression wait propagates real FS failures and cannot run in a performance case',async()=>{
  await assert.rejects(fixture(naturalEvents({error:'real IDB error'})).run(),/observed a save failure/);
  const f=fixture(naturalEvents(),{saveOnly:false});
  await assert.rejects(f.run(),/restricted to save-only validation/);assert.equal(f.waitCalls(),0);
});
