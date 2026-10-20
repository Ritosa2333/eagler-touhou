import test from 'node:test';
import assert from 'node:assert/strict';
import {mkdtemp,readFile,writeFile,rm} from 'node:fs/promises';
import {tmpdir} from 'node:os';
import {resolve} from 'node:path';
import {fileIdentity,identityDigest} from './dependency-snapshot.mjs';
import {runAdaptationVerification} from '../../tests/adaptation/execution.mjs';

const fixture=`import {readFile,mkdir,writeFile} from 'node:fs/promises';
import {resolve} from 'node:path';
const [root,game,out]=process.argv.slice(2), startedAt=new Date().toISOString();
const behavior=JSON.parse(await readFile(resolve(root,game+'.json'),'utf8'));
await writeFile(resolve(root,game+'.marker'),(Number(await readFile(resolve(root,game+'.marker'),'utf8').catch(()=>0))+1).toString());
await mkdir(out,{recursive:true});
if(!behavior.missing)await writeFile(resolve(out,'report.json'),JSON.stringify({schema:'eagler-touhou/runtime-browser/1',runId:game+'-'+startedAt,
startedAt,finishedAt:new Date().toISOString(),status:'passed',passed:true,targets:[{game:behavior.wrongGame?'th20':game,variant:'normal',status:'passed',
checks:[{id:'movement',checks:['INPUT-01'],status:behavior.fail?'failed':'passed',complete:false}]}]}));
process.exitCode=behavior.exit??0;`;

async function setup(t){
  const root=await mkdtemp(resolve(tmpdir(),'adaptation-execution-'));t.after(()=>rm(root,{recursive:true,force:true}));
  await writeFile(resolve(root,'driver.mjs'),fixture);
  for(const game of ['th06','th07'])await writeFile(resolve(root,game+'.json'),'{}');
  const buildPlan=({output})=>({unmappedChecks:[],cases:['th06','th07'].map(game=>({id:game,game,variant:'normal',group:'runtime',checks:['INPUT-01'],
    status:'ready',reportPath:resolve(output,game,'report.json'),requiredInputs:[],blockedInputs:[],
    task:{command:process.execPath,args:[resolve(root,'driver.mjs'),root,game,resolve(output,game)],cwd:root,timeoutMs:30000,resources:[]}}))});
  const createIdentityReader=()=>async item=>({fingerprint:identityDigest({driver:await fileIdentity(resolve(root,'driver.mjs')),
    input:await fileIdentity(resolve(root,item.game+'.json'))})});
  const run=(name,options={})=>runAdaptationVerification({root,output:resolve(root,name),environment:{},...options},
    {buildPlan,createIdentityReader,skipPreparation:true});
  const count=async game=>Number(await readFile(resolve(root,game+'.marker'),'utf8').catch(()=>0));
  return{root,run,count};
}

test('real queue continues independent cases and rejects zero-exit false green',async t=>{
  const {root,run,count}=await setup(t);await writeFile(resolve(root,'th06.json'),'{"fail":true}');
  const report=await run('first');assert.equal(report.status,'failed');
  assert.deepEqual(report.cases.map(item=>item.status),['failed','passed']);assert.equal(await count('th07'),1);
  const queued=JSON.parse(await readFile(resolve(root,'first/processes/report.json'),'utf8'));
  assert.equal(queued.status,'failed');assert.equal(queued.results[0].processStatus,'passed');
});

test('resume retains sealed evidence, failed rerun executes only failures, and corrupted evidence reruns',async t=>{
  const {root,run,count}=await setup(t);await writeFile(resolve(root,'th06.json'),'{"fail":true}');
  const first=await run('first');const second=await run('second',{resume:resolve(root,'first')});
  assert.equal(second.executed,0);assert.equal(second.retained,2);assert.equal(await count('th06'),1);
  const third=await run('third',{rerunFailed:resolve(root,'second')});
  assert.equal(third.executed,1);assert.equal(third.retained,1);assert.equal(await count('th06'),2);assert.equal(await count('th07'),1);
  await writeFile(first.cases[1].report.path,'corrupted');
  const fourth=await run('fourth',{resume:resolve(root,'third')});
  assert.equal(fourth.executed,1);assert.equal(await count('th07'),2);
});

test('changed input reruns its product; unrelated documents preserve cached evidence',async t=>{
  const {root,run,count}=await setup(t);await run('first');await writeFile(resolve(root,'notes.md'),'changed docs');
  const unchanged=await run('second',{resume:resolve(root,'first')});assert.equal(unchanged.executed,0);
  await writeFile(resolve(root,'th06.json'),'{"fail":true}');
  const changed=await run('third',{resume:resolve(root,'second')});assert.equal(changed.executed,1);assert.equal(changed.retained,1);
  assert.equal(await count('th07'),1);assert.equal(changed.cases[0].status,'failed');
});

test('preview never prepares inputs or launches a process, and existing evidence is preserved',async t=>{
  const {root,run,count}=await setup(t);const report=await run('plan',{planOnly:true});
  assert.equal(report.status,'planned');assert.equal(report.passed,false);assert.equal(await count('th06'),0);
  const bytes=await readFile(resolve(root,'plan/summary.json'),'utf8');
  await assert.rejects(run('plan'),/new evidence directory/);assert.equal(await readFile(resolve(root,'plan/summary.json'),'utf8'),bytes);
});

test('selecting an adaptation ID requires complete evidence beyond a passed scene',async t=>{
  const {run}=await setup(t);const report=await run('explicit-check',{checks:['INPUT-01']});
  assert.equal(report.counts.passed,2);assert.equal(report.coverage.complete,false);
  assert.equal(report.status,'incomplete');assert.equal(report.passed,false);
});

test('a tampered parent pass and missing or mismatched child reports cannot be retained',async t=>{
  const {root,run}=await setup(t);await writeFile(resolve(root,'th06.json'),'{"missing":true}');
  await writeFile(resolve(root,'th07.json'),'{"wrongGame":true}');const first=await run('first');
  assert.deepEqual(first.cases.map(item=>item.status),['failed','failed']);
  first.cases[1].status='passed';await writeFile(resolve(root,'first/summary.json'),JSON.stringify(first));
  const next=await run('second',{resume:resolve(root,'first')});assert.equal(next.executed,2);assert.equal(next.status,'failed');
});

test('runtime-selected native judging input changes invalidate retained Replay evidence',async t=>{
  const {root,run,count}=await setup(t);const source=resolve(root,'oracle.cpp');await writeFile(source,'native reference 1');
  const first=await run('first'),item=first.cases[0],driver=JSON.parse(await readFile(item.report.path,'utf8'));
  driver.decisionInputs=[{path:'oracle.cpp',...await fileIdentity(source)}];
  await writeFile(item.report.path,JSON.stringify(driver));item.report={path:item.report.path,...await fileIdentity(item.report.path)};
  await writeFile(resolve(root,'first/summary.json'),JSON.stringify(first));await writeFile(source,'native reference 2');
  const next=await run('second',{resume:resolve(root,'first')});assert.equal(next.executed,1);assert.equal(await count('th06'),2);
});
